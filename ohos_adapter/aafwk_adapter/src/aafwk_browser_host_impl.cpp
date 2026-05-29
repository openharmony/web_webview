/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "aafwk_browser_host_impl.h"

#include "nweb_log.h"
#include "graphic_adapter.h"
#include "ibuffer_consumer_listener.h"
#include "iconsumer_surface.h"
#include "nweb_adapter_helper.h"
#include "ipc_types.h"
#include "surface_utils.h"
#include "surface/window.h"
#include "native_window.h"
#include "external_window.h"

#include "../../../ohos_interface/ohos_glue/base/include/ark_web_errno.h"

namespace OHOS::NWeb {

BrowserHost::BrowserHost()
{
    memberFuncMap_[static_cast<uint32_t>(IBrowser::Message::QUERY_RENDER_SURFACE)] =
        [](BrowserHost* that, MessageParcel &data, MessageParcel &reply) {
            return that->HandleQueryRenderSurface(data, reply);
        };
    memberFuncMap_[static_cast<uint32_t>(IBrowser::Message::REPORT_THREAD)] =
        [](BrowserHost* that, MessageParcel &data, MessageParcel &reply) {
            return that->HandleReportThread(data, reply);
        };
    memberFuncMap_[static_cast<uint32_t>(IBrowser::Message::PASS_SURFACE)] =
        [](BrowserHost* that, MessageParcel &data, MessageParcel &reply) {
            return that->HandlePassSurface(data, reply);
        };
    memberFuncMap_[static_cast<uint32_t>(IBrowser::Message::DESTROY_RENDER_SURFACE)] =
        [](BrowserHost* that, MessageParcel &data, MessageParcel &reply) {
            return that->HandleDestroyRenderSurface(data, reply);
        };
}

BrowserHost::~BrowserHost()
{
    memberFuncMap_.clear();
}

int BrowserHost::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel& reply, MessageOption& option)
{
    std::u16string descriptor = BrowserHost::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        WVLOG_E("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto& memberFunc = itFunc->second;
        if (memberFunc) {
            return memberFunc(this, data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int BrowserHost::HandleQueryRenderSurface(MessageParcel &data, MessageParcel &reply)
{
    int surface_id = data.ReadInt32();

    // call child class, get surface void* from kelnel
    // -> aafwk_browser_host_impl.cpp
    sptr<IRemoteObject> surfaceObj = QueryRenderSurface(surface_id);
    reply.WriteRemoteObject(surfaceObj);
    return 0;
}

int BrowserHost::HandleReportThread(MessageParcel &data, MessageParcel &reply)
{
    int status = data.ReadInt32();
    int process_id = data.ReadInt32();
    int thread_id = data.ReadInt32();
    int role = data.ReadInt32();

    // call child class, report thread
    // -> aafwk_browser_host_impl.cpp
    ReportThread(status, process_id, thread_id, role);
    return 0;
}

int BrowserHost::HandlePassSurface(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> surfaceObject = data.ReadRemoteObject();
    sptr<IBufferProducer> bufferProducer = iface_cast<IBufferProducer>(surfaceObject);
    if (bufferProducer == nullptr) {
        WVLOG_E("HandlePass buffer failed.");
        return 0;
    }
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bufferProducer);
    int64_t surface_id = data.ReadInt64();
    PassSurface(surface, surface_id);
    return 0;
}

int BrowserHost::HandleDestroyRenderSurface(MessageParcel &data, MessageParcel &reply)
{
    int32_t surface_id = data.ReadInt32();
    DestroyRenderSurface(surface_id);
    return 0;
}

AafwkBrowserHostImpl::AafwkBrowserHostImpl(std::shared_ptr<AafwkBrowserHostAdapter> adapter)
    : browserHostAdapter_(adapter) {}

sptr<IRemoteObject> AafwkBrowserHostImpl::QueryRenderSurface(int32_t surface_id)
{
    WVLOG_D("browser host impl get request for window id = %{public}d", surface_id);
    if (browserHostAdapter_ == nullptr) {
        return nullptr;
    }
    // send to kernel (Browser)
    bool withRef = true;
    void* window = browserHostAdapter_->GetSurfaceFromKernelWithRef(surface_id);
    if (ArkWebGetErrno() != RESULT_OK) {
        WVLOG_D("retry request for window id = %{public}d", surface_id);
        window = browserHostAdapter_->GetSurfaceFromKernel(surface_id);
        withRef = false;
    }
    if (window) {
        OHNativeWindow* ohNativeWindow = reinterpret_cast<OHNativeWindow*>(window);
        sptr<Surface> surface = ohNativeWindow->surface;
        if (withRef) {
            OH_NativeWindow_NativeObjectUnreference(ohNativeWindow);
        }

        WVLOG_D("browser host impl get request window");
        if (surface != nullptr) {
            return surface->GetProducer()->AsObject();
        }
    }
    WVLOG_E("browser host impl get surface from kernel failed");
    return nullptr;
}

void AafwkBrowserHostImpl::ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role)
{
    ResSchedStatusAdapter resSchedStatus = static_cast<ResSchedStatusAdapter>(status);
    ResSchedRoleAdapter resSchedRole = static_cast<ResSchedRoleAdapter>(role);
    OHOS::NWeb::ResSchedClientAdapter::ReportKeyThread(
        resSchedStatus, process_id, thread_id, resSchedRole);
}

void AafwkBrowserHostImpl::PassSurface(sptr<Surface> surface, int64_t surface_id)
{
    std::unique_lock<std::mutex> map_lock(map_mutex_);
    sptr<Surface> surfaceTmp = surface;
    SurfaceUtils* utils = SurfaceUtils::GetInstance();
    if (!utils) {
        WVLOG_E("get surfaceUtils failed.");
        return;
    }
    if (!surfaceTmp) {
        WVLOG_E("passSurface failed");
        return;
    }
    surface_map_.emplace(surface_id, surfaceTmp);
    utils->Add(surface_id, surfaceTmp);
}

void AafwkBrowserHostImpl::DestroyRenderSurface(int32_t surface_id)
{
    if (browserHostAdapter_ == nullptr) {
        return;
    }
    browserHostAdapter_->DestroySurfaceFromKernel(surface_id);
    WVLOG_D("Destroy render surface id is %{public}d", surface_id);
}
} // namespace OHOS::NWeb
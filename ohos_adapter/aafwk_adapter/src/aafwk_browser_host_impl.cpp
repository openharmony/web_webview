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

#include <charconv>
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
#include "transaction/rs_interfaces.h"
#include "../../../ohos_interface/ohos_glue/base/include/ark_web_errno.h"

namespace OHOS::NWeb {

const std::string DELEGATE_NODE_ID = "delegate_node_id";
const std::string DELEGATE_CONNECT_TO_RENDER = "delegate_connect_to_render";

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
    memberFuncMap_[static_cast<uint32_t>(IBrowser::Message::QUERY_BUFFER_TYPE_LEAK)] =
        [](BrowserHost* that, MessageParcel &data, MessageParcel &reply) {
            return that->HandleQueryBufferTypeLeak(data, reply);
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

int BrowserHost::HandleQueryRenderSurface(MessageParcel& data, MessageParcel& reply)
{
    int surfaceId = data.ReadInt32();
    uint64_t nodeId = 0;

    auto [surfaceObj, rsHandle] = QueryRenderSurface(surfaceId, nodeId);
    if (!reply.WriteRemoteObject(surfaceObj)) {
        WVLOG_E("OH_NativeWindow_WriteToParcel surfaceObj failed");
        return ERR_INVALID_STATE;
    }

    if (!reply.WriteUint64(nodeId)) {
        WVLOG_E("OH_NativeWindow_WriteToParcel nodeId failed");
        return ERR_INVALID_STATE;
    }

    bool hasRSHandle = (rsHandle != nullptr);
    if (!reply.WriteBool(hasRSHandle)) {
        WVLOG_E("OH_NativeWindow_WriteToParcel hasRSHandle failed");
        return ERR_INVALID_STATE;
    }

    if (hasRSHandle) {
        if (!reply.WriteRemoteObject(rsHandle)) {
            WVLOG_E("OH_NativeWindow_WriteToParcel rsHandle failed");
            return ERR_INVALID_STATE;
        }
    }

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

int BrowserHost::HandleQueryBufferTypeLeak(MessageParcel &data, MessageParcel &reply)
{
    int32_t surface_id = data.ReadInt32();
    std::string bufferTypeLeak = QueryBufferTypeLeak(surface_id);
    reply.WriteString(bufferTypeLeak);
    return 0;
}

AafwkBrowserHostImpl::AafwkBrowserHostImpl(std::shared_ptr<AafwkBrowserHostAdapter> adapter)
    : browserHostAdapter_(adapter) {}

std::pair<sptr<IRemoteObject>, sptr<IRemoteObject>> AafwkBrowserHostImpl::QueryRenderSurface(
    int32_t surfaceId, uint64_t& nodeId)
{
    WVLOG_D("browser host impl get request for window id = %{public}d", surfaceId);
    if (browserHostAdapter_ == nullptr) {
        return { nullptr, nullptr };
    }
    // send to kernel (Browser)
    bool withRef = true;
    void* window = browserHostAdapter_->GetSurfaceFromKernelWithRef(surfaceId);
    if (ArkWebGetErrno() != RESULT_OK) {
        WVLOG_W("retry request for window id = %{public}d", surfaceId);
        window = browserHostAdapter_->GetSurfaceFromKernel(surfaceId);
        withRef = false;
    }

    if (window == nullptr) {
        WVLOG_E("browser host impl get surface from kernel failed, window is null");
        return { nullptr, nullptr };
    }

    OHNativeWindow* ohNativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    sptr<Surface> surface = ohNativeWindow->surface;
    if (withRef) {
        OH_NativeWindow_NativeObjectUnreference(ohNativeWindow);
    }

    if (surface == nullptr) {
        WVLOG_E("browser host impl get surface from kernel failed, surface is null");
        return { nullptr, nullptr };
    }

    auto parseUint64 = [](std::string_view str, uint64_t& outVal) -> bool {
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), outVal);
        return (ec == std::errc{}) && (ptr == str.data() + str.size());
    };

    std::string fetchedNodeId = surface->GetUserData(DELEGATE_NODE_ID);
    if (!fetchedNodeId.empty()) {
        if (!parseUint64(fetchedNodeId, nodeId)) {
            WVLOG_E("DelegateDebug Failed to parse node id from string: %s", fetchedNodeId.c_str());
            nodeId = 0;
        }
    }
    std::unique_lock<std::mutex> map_lock(map_mutex_);
    renderSurfaceCache_[surfaceId] = surface;
    std::string fetchedRSHandle = surface->GetUserData(DELEGATE_CONNECT_TO_RENDER);
    sptr<IRemoteObject> rsHandle = nullptr;
    if (!fetchedRSHandle.empty()) {
        uint64_t rawHandle = 0;
        if (parseUint64(fetchedRSHandle, rawHandle) && rawHandle != 0) {
            rsHandle = reinterpret_cast<IRemoteObject*>(rawHandle);
        } else {
            WVLOG_E("DelegateDebug Failed to parse RS handle from string: %s", fetchedRSHandle.c_str());
        }
    }

    if (surface->GetProducer() == nullptr) {
        WVLOG_W("Surface producer is null");
        return { nullptr, nullptr };
    }
    return { surface->GetProducer()->AsObject(), rsHandle };
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
        WVLOG_E("Destroy failed. surfaceId: %{public}d", surface_id);
        return;
    }
    browserHostAdapter_->DestroySurfaceFromKernel(surface_id);
    WVLOG_D("Destroy render surface id is %{public}d", surface_id);
}

std::string AafwkBrowserHostImpl::QueryBufferTypeLeak(int32_t surface_id)
{
    std::unique_lock<std::mutex> map_lock(map_mutex_);
    auto it = renderSurfaceCache_.find(surface_id);
    if (it != renderSurfaceCache_.end() && it->second != nullptr) {
        std::string bufferTypeLeak = it->second->GetBufferTypeLeak();
        renderSurfaceCache_.erase(surface_id);
        return bufferTypeLeak;
    }
    WVLOG_E("browser host impl query buffer type leak failed for window id = %{public}d", surface_id);
    return "";
}

} // namespace OHOS::NWeb
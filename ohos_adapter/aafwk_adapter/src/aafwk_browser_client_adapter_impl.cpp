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
#include "aafwk_browser_client_adapter_impl.h"
#include "aafwk_render_scheduler_impl.h"
#include "surface_utils.h"
#include "surface.h"
#include "ipc_types.h"
#include "surface/window.h"
#include "native_window.h"

namespace OHOS::NWeb {
BrowserClient::BrowserClient(
    const sptr<IRemoteObject> &impl) : IRemoteProxy<IBrowser>(impl) {}

bool BrowserClient::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(BrowserClient::GetDescriptor())) {
        return false;
    }
    return true;
}

sptr<IRemoteObject> BrowserClient::QueryRenderSurface(int32_t surface_id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option; // this should be a sync binder
    if (!WriteInterfaceToken(data)) {
        return nullptr;
    }
    data.WriteInt32(surface_id);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WVLOG_E("Remote is NULL.");
        return nullptr;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IBrowser::Message::QUERY_RENDER_SURFACE),
        data, reply, option);
    if (ret != NO_ERROR) {
        WVLOG_E("SendRequest failed, error code = %{public}d", ret);
    }
    sptr<IRemoteObject> surface = reply.ReadRemoteObject();
    return surface;
}

void BrowserClient::ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option; // this should be a sync binder
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteInt32(status);
    data.WriteInt32(process_id);
    data.WriteInt32(thread_id);
    data.WriteInt32(role);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WVLOG_E("Remote is NULL.");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IBrowser::Message::REPORT_THREAD),
        data, reply, option);
    if (ret != NO_ERROR) {
        WVLOG_E("SendRequest failed, error code = %{public}d", ret);
    }
    return;
}

void BrowserClient::PassSurface(sptr<Surface> surface, int64_t surface_id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option; // this should be a sync binder
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (surface == nullptr) {
        return;
    }
    data.WriteRemoteObject(surface->GetProducer()->AsObject());
    data.WriteInt64(surface_id);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WVLOG_E("Remote is NULL.");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IBrowser::Message::PASS_SURFACE),
        data, reply, option);
    if (ret != NO_ERROR) {
        WVLOG_E("SendRequest failed, error code = %{public}d", ret);
    }
    return;
}

void BrowserClient::DestroyRenderSurface(int32_t surface_id)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option; // this should be a sync binder
    if (!WriteInterfaceToken(data)) {
        return;
    }
    data.WriteInt32(surface_id);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WVLOG_E("Remote is NULL.");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IBrowser::Message::DESTROY_RENDER_SURFACE),
        data, reply, option);
    if (ret != NO_ERROR) {
        WVLOG_E("SendRequest failed, error code = %{public}d", ret);
    }
    return;
}

AafwkBrowserClientAdapterImpl::AafwkBrowserClientAdapterImpl() {}

AafwkBrowserClientAdapterImpl& AafwkBrowserClientAdapterImpl::GetInstance()
{
    static AafwkBrowserClientAdapterImpl instance;
    return instance;
}

void* AafwkBrowserClientAdapterImpl::QueryRenderSurface(int32_t surface_id)
{
    std::unique_lock<std::mutex> window_map_lock(window_map_mutex_);
    if (GetInstance().browserHost_) {
        sptr<IRemoteObject> surfaceObject = GetInstance().browserHost_->QueryRenderSurface(surface_id);
        // get return value
        sptr<IBufferProducer> bufferProducer = iface_cast<IBufferProducer>(surfaceObject);
        sptr<Surface> surface = Surface::CreateSurfaceAsProducer(bufferProducer);
        OHNativeWindow* window = ::CreateNativeWindowFromSurface(&surface);
        if (!window) {
            WVLOG_E("create the native window failed.");
            return nullptr;
        }
        uint64_t usage = BUFFER_USAGE_MEM_DMA;
        NativeWindowHandleOpt(window, SET_USAGE, usage);
        window_map_.emplace(surface_id, window);
        void* newNativeWindow = reinterpret_cast<NWebNativeWindow>(window);
        WVLOG_D("receive surface num = %{public}d", surface_id);
        return newNativeWindow;
    }
    WVLOG_E("browserHost_ is not exist!");
    return nullptr;
}

void AafwkBrowserClientAdapterImpl::ReportThread(
    ResSchedStatusAdapter status, int32_t process_id, int32_t thread_id, ResSchedRoleAdapter role)
{
    int32_t resSchedStatus = static_cast<int>(status);
    int32_t resSchedRole = static_cast<int>(role);
    if (!GetInstance().browserHost_) {
        WVLOG_E("browserHost_ is not exist!");
        return;
    }
    GetInstance().browserHost_->ReportThread(resSchedStatus, process_id, thread_id, resSchedRole);
    WVLOG_D("report thread id = %{public}d", thread_id);
    return;
}

void AafwkBrowserClientAdapterImpl::PassSurface(int64_t surface_id)
{
    if (!GetInstance().browserHost_) {
        WVLOG_E("browserHost_ is not exist!");
        return;
    }
    SurfaceUtils* utils = SurfaceUtils::GetInstance();
    if (!utils) {
        WVLOG_E("get surfaceUtils failed!");
        return;
    }
    sptr<Surface> surface = utils->GetSurface(surface_id);
    if (!surface) {
        WVLOG_E("get surface failed!");
        return;
    }
    GetInstance().browserHost_->PassSurface(surface, surface_id);
    return;
}

void AafwkBrowserClientAdapterImpl::DestroyRenderSurface(int32_t surface_id)
{
    std::unique_lock<std::mutex> window_map_lock(window_map_mutex_);
    if (!GetInstance().browserHost_) {
        WVLOG_E("browserHost_ is not exist!");
        return;
    }

    if (window_map_.find(surface_id) != window_map_.end()) {
        OHNativeWindow* window = window_map_[surface_id];
        ::DestoryNativeWindow(window);
        if (!window) {
            WVLOG_D("window has been destroyed successfully.");
        } else {
            WVLOG_E("failed to destroy the window, there may be some leak.");
        }
        window_map_.erase(surface_id);
    }
    GetInstance().browserHost_->DestroyRenderSurface(surface_id);
    WVLOG_D("Destroy nweb surface id = %{public}d", surface_id);
}
} // namespace OHOS::NWeb
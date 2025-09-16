/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "web_extension_connection_callback_proxy.h"
#include "connection_native_info_parcel.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
WebExtensionConnectionCallbackProxy::WebExtensionConnectionCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IWebExtensionConnectionCallback>(impl) {}

WebExtensionConnectionCallbackProxy::~WebExtensionConnectionCallbackProxy() {}

void WebExtensionConnectionCallbackProxy::OnConnect(ConnectionNativeInfo& info)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(IWebExtensionConnectionCallback::GetDescriptor())) {
        WNMLOG_E("write descriptor failed");
        return;
    }
    sptr<ConnectionNativeInfoParcel> parcel = new (std::nothrow) ConnectionNativeInfoParcel();
    if (parcel == nullptr) {
        WNMLOG_E("new ConnectionNativeInfoParcel failed");
        return;
    }
    parcel->connectionNativeInfo_ = info;
    if (!data.WriteParcelable(parcel)) {
        WNMLOG_E("write ConnectionNativeInfoParcel failed");
        return;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WNMLOG_E("callback is null");
        return;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IWebExtensionConnectionCallback::ON_CONNECT_CODE), data, reply, option);
    if (requestResult != NO_ERROR) {
        WNMLOG_E("send request fail, result: %{public}d", requestResult);
        return;
    }
    WNMLOG_D("SendRequest success");
}

void WebExtensionConnectionCallbackProxy::OnDisconnect(ConnectionNativeInfo& info)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(IWebExtensionConnectionCallback::GetDescriptor())) {
        WNMLOG_E("write descriptor failed");
        return;
    }
    sptr<ConnectionNativeInfoParcel> parcel = new (std::nothrow) ConnectionNativeInfoParcel();
    if (parcel == nullptr) {
        WNMLOG_E("new ConnectionNativeInfoParcel failed");
        return;
    }
    parcel->connectionNativeInfo_ = info;
    if (!data.WriteParcelable(parcel)) {
        WNMLOG_E("write ConnectionNativeInfoParcel failed");
        return;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WNMLOG_E("callback is null");
        return;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IWebExtensionConnectionCallback::ON_DISCONNECT_CODE), data, reply, option);
    if (requestResult != NO_ERROR) {
        WNMLOG_E("send request fail, result: %{public}d", requestResult);
        return;
    }
    WNMLOG_D("SendRequest success");
}

void WebExtensionConnectionCallbackProxy::OnFailed(int32_t errorNum)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(IWebExtensionConnectionCallback::GetDescriptor())) {
        WNMLOG_E("write descriptor failed");
        return;
    }
    if (!data.WriteInt32(errorNum)) {
        WNMLOG_E("write errorNum failed");
        return;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WNMLOG_E("callback is null");
        return;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IWebExtensionConnectionCallback::ON_FAILED_CODE), data, reply, option);
    if (requestResult != NO_ERROR) {
        WNMLOG_E("send request fail, result: %{public}d", requestResult);
        return;
    }
    WNMLOG_D("SendRequest success");
}
} // OHOS::NWeb

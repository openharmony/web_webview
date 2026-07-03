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

#include "web_extension_connection_callback_stub.h"

#include "connection_native_info_parcel.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
int32_t WebExtensionConnectionCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    std::u16string descriptor = data.ReadInterfaceToken();
    if (descriptor != IWebExtensionConnectionCallback::GetDescriptor()) {
        WNMLOG_E("Get unexpect descriptor");
        return -1;
    }
    int32_t msgCode = static_cast<int32_t>(code);
    switch (msgCode) {
        case IWebExtensionConnectionCallback::ON_CONNECT_CODE: {
            sptr<ConnectionNativeInfoParcel> infoParcel = data.ReadParcelable<ConnectionNativeInfoParcel>();
            if (infoParcel == nullptr) {
                WNMLOG_E("read info parcel failed");
                return -1;
            }
            OnConnect(infoParcel->connectionNativeInfo_);
            break;
        }
        case IWebExtensionConnectionCallback::ON_DISCONNECT_CODE: {
            sptr<ConnectionNativeInfoParcel> infoParcel = data.ReadParcelable<ConnectionNativeInfoParcel>();
            if (infoParcel == nullptr) {
                WNMLOG_E("read info parcel failed");
                return -1;
            }
            OnDisconnect(infoParcel->connectionNativeInfo_);
            break;
        }
        case IWebExtensionConnectionCallback::ON_FAILED_CODE: {
            int32_t errorNum;
            if (!data.ReadInt32(errorNum)) {
                WNMLOG_E("read errorNum failed");
                return -1;
            }
            OnFailed(errorNum);
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace OHOS::NWeb

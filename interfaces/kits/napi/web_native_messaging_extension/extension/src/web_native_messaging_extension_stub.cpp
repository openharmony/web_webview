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

#include "web_native_messaging_extension_stub.h"

#include <sys/types.h>
#include <unistd.h>
#include "ipc_types.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {
int32_t WebNativeMessagingExtensionStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    switch (code) {
        case IWebNativeMessagingExtension::CODE_CONNECTNATIVE: {
            WNMEConnectionInfo connInfo;
            WNMEConnectionInfoParcel::Unmarshalling(data, connInfo);
            int32_t errorCode = ConnectNative(connInfo);
            if (errorCode == WNME_SUCCESS) {
                errorCode = getpid();
            }
            reply.WriteInt32(errorCode);
            return ERR_NONE;
        }

        case IWebNativeMessagingExtension::CODE_DISCONNECTNATIVE: {
            WNMEConnectionInfo connInfo;
            WNMEConnectionInfoParcel::Unmarshalling(data, connInfo);
            int32_t errorCode = DisconnectNative(connInfo);
            reply.WriteInt32(errorCode);
            return ERR_NONE;
        }

        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ERR_TRANSACTION_FAILED;
}
} // namespace NWeb
} // namespace OHOS

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

#include "web_native_messaging_extension_proxy.h"

#include "ipc_types.h"
#include "message_parcel.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {

int32_t WebNativeMessagingExtensionProxy::SendRequest(const int32_t code, WNMEConnectionInfo& connection)
{
    WNMLOG_I("WebNativeMessagingExtensionProxy::SendRequest %{public}d", code);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WNMLOG_E("WriteInterfaceToken fail");
        return WNME_IPC_ERROR;
    }

    WNMEConnectionInfoParcel sending;
    sending.conn_info_ = connection;

    if (!sending.Marshalling(data)) {
        WNMLOG_E("Failed to WriteParcelable");
        return WNME_IPC_ERROR;
    }

    int32_t sendResult = Remote()->SendRequest(code, data, reply, option);
    if (sendResult != ERR_NONE) {
        WNMLOG_E("SendRequest failed %{public}d.", sendResult);
        return WNME_IPC_ERROR;
    }

    int32_t errorCode;
    if (!reply.ReadInt32(errorCode)) {
        WNMLOG_E("ReadInt32 failed");
        return WNME_IPC_ERROR;
    }

    return errorCode;
}

int32_t WebNativeMessagingExtensionProxy::ConnectNative(WNMEConnectionInfo& connection)
{
    return SendRequest(IWebNativeMessagingExtension::CODE_CONNECTNATIVE, connection);
}

int32_t WebNativeMessagingExtensionProxy::DisconnectNative(WNMEConnectionInfo& connection)
{
    return SendRequest(IWebNativeMessagingExtension::CODE_DISCONNECTNATIVE, connection);
}
} // namespace NWeb
} // namespace OHOS

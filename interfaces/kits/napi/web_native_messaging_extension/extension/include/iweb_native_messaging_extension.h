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

#ifndef I_WEB_NATIVE_MESSAGING_EXTENSION_H
#define I_WEB_NATIVE_MESSAGING_EXTENSION_H

#include "web_native_messaging_extension_connect_info.h"
#include "iremote_broker.h"

namespace OHOS {
namespace NWeb {
class IWebNativeMessagingExtension : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NWeb.IWEBNATIVEMESSAGINGEXTENSION");

    enum WNMEProxyError {
        WNME_SUCCESS = 0,
        WNME_IPC_ERROR= -1,
        WNME_PARAMS_INVALID = -2,
        WNME_INNER_ERROR = -3,
    };

    /*
     * Remote method code.
     */
    enum {
        CODE_CONNECTNATIVE = 0,
        CODE_DISCONNECTNATIVE = 1,    
    };

    virtual int32_t ConnectNative(WNMEConnectionInfo& connection) = 0;
    virtual int32_t DisconnectNative(WNMEConnectionInfo& connection) = 0;
};    
} // namespace NWeb
} // namespace OHOS
#endif // I_WEB_NATIVE_MESSAGING_EXTENSION_H
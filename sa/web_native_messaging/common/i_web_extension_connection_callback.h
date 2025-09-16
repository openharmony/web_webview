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

#ifndef I_WEB_EXTENSION_CONNECTION_CALLBACK_H
#define I_WEB_EXTENSION_CONNECTION_CALLBACK_H

#include "iremote_broker.h"
#include "web_native_messaging_common.h"

namespace OHOS::NWeb {
class IWebExtensionConnectionCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nweb.IWebExtensionConnectionCallback");
    virtual void OnConnect(ConnectionNativeInfo& info) = 0;
    virtual void OnDisconnect(ConnectionNativeInfo& info) = 0;
    virtual void OnFailed(int32_t errorNum) = 0;

    enum InterfaceCode {
        ON_CONNECT_CODE = 0,
        ON_DISCONNECT_CODE = 1,
        ON_FAILED_CODE = 2,
    };
};
} // namespace OHOS::NWeb
#endif // I_WEB_EXTENSION_CONNECTION_CALLBACK_H

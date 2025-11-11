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

#ifndef WEB_NATIVE_MESSAGING_EXTENSION_PROXY_H
#define WEB_NATIVE_MESSAGING_EXTENSION_PROXY_H

#include <sys/types.h>
#include "iweb_native_messaging_extension.h"
#include "iremote_proxy.h"
#include "web_native_messaging_extension_connect_info.h"

namespace OHOS {
namespace NWeb {

class WebNativeMessagingExtensionProxy : public IRemoteProxy<IWebNativeMessagingExtension> {
public:
    explicit WebNativeMessagingExtensionProxy(const sptr<IRemoteObject> &remote)
        : IRemoteProxy<IWebNativeMessagingExtension>(remote) {}

    virtual ~WebNativeMessagingExtensionProxy() {}

    // return stub process pid when ret > 0.
    int32_t ConnectNative(WNMEConnectionInfo& connection);

    int32_t DisconnectNative(WNMEConnectionInfo& connection);

private:
    static inline BrokerDelegator<WebNativeMessagingExtensionProxy> delegator;
    int32_t SendRequest(const int32_t code, WNMEConnectionInfo& connection);
};
} // namespace NWeb
} // namespace OHOS
#endif // WEB_NATIVE_MESSAGING_EXTENSION_PROXY_H
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
#ifndef NWEB_WEB_EXTENSION_CONNECTION_CALLBACK_PROXY_H
#define NWEB_WEB_EXTENSION_CONNECTION_CALLBACK_PROXY_H

#include "i_web_extension_connection_callback.h"
#include "iremote_proxy.h"
#include "nocopyable.h"

namespace OHOS::NWeb {
class WebExtensionConnectionCallbackProxy : public IRemoteProxy<IWebExtensionConnectionCallback> {
public:
    explicit WebExtensionConnectionCallbackProxy(const sptr<IRemoteObject>& impl);
    ~WebExtensionConnectionCallbackProxy() override;

    void OnConnect(ConnectionNativeInfo& info) override;
    void OnDisconnect(ConnectionNativeInfo& info) override;
    void OnFailed(int32_t errorNum) override;
private:
    static inline BrokerDelegator<WebExtensionConnectionCallbackProxy> delegator_;
};
} // namespace OHOS::NWeb
#endif // NWEB_WEB_EXTENSION_CONNECTION_CALLBACK_PROXY_H

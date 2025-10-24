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
#ifndef WEB_EXTENSION_CONNECTION_CALLBACK_H
#define WEB_EXTENSION_CONNECTION_CALLBACK_H

#include "iremote_stub.h"
#include "nocopyable.h"
#include "web_extension_connection_callback_stub.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
class IExtensionConnectionCallback {
public:
    IExtensionConnectionCallback() {}
    virtual ~IExtensionConnectionCallback() = default;

    virtual void OnExtensionConnect(ConnectionNativeInfo& info) {}
    virtual void OnExtensionDisconnect(ConnectionNativeInfo& info) {}
    virtual void OnExtensionFailed(int32_t connectionId, int32_t errorNum) {}
};

class WebExtensionConnectionCallback : public WebExtensionConnectionCallbackStub {
public:
    explicit WebExtensionConnectionCallback(std::shared_ptr<IExtensionConnectionCallback> jsCallback)
    {
        callback_ = jsCallback;
    }

    ~WebExtensionConnectionCallback() override
    {
        WNMLOG_D("~WebExtensionConnectionCallback");
    }

    void OnConnect(ConnectionNativeInfo& info) override;
    void OnDisconnect(ConnectionNativeInfo& info) override;
    void OnFailed(int32_t errorNum) override;

    int32_t connectionId_ = 0;
private:
    std::shared_ptr<IExtensionConnectionCallback> callback_;
};
} // namespace OHOS::NWeb
#endif //WEB_EXTENSION_CONNECTION_CALLBACK_H

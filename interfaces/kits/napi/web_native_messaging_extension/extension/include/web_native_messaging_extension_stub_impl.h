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

#ifndef WEB_NATIVE_MESSAGING_EXTENSION_STUB_IMPL_H
#define WEB_NATIVE_MESSAGING_EXTENSION_STUB_IMPL_H

#include <cstdint>
#include "web_native_messaging_extension_connect_info.h"
#include "js_web_native_messaging_extension.h"
#include "web_native_messaging_extension_stub.h"

namespace OHOS {
namespace NWeb {
class WebNativeMessagingExtensionStubImpl : public WebNativeMessagingExtensionStub {
public:
    explicit WebNativeMessagingExtensionStubImpl(const std::shared_ptr<WebNativeMessagingExtension>& extension);

    virtual ~WebNativeMessagingExtensionStubImpl();

    virtual int32_t ConnectNative(WNMEConnectionInfo& connection) override;

    virtual int32_t DisconnectNative(WNMEConnectionInfo& connection) override;

private:
    std::weak_ptr<WebNativeMessagingExtension> extension_;
};
} // namespace NWeb
} // namespace OHOS
#endif // WEB_NATIVE_MESSAGING_EXTENSION_STUB_IMPL_H
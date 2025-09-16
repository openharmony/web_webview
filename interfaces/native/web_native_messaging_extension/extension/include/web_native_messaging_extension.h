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

#ifndef WEB_NATIVE_MESSAGING_EXTENSION_H
#define WEB_NATIVE_MESSAGING_EXTENSION_H

#include "web_native_messaging_extension_connect_info.h"
#include "extension_base.h"
#include "web_native_messaging_extension_context.h"

namespace OHOS {
namespace NWeb {
class WebNativeMessagingExtension : public AbilityRuntime::ExtensionBase<WebNativeMessagingExtensionContext> {
public:
    WebNativeMessagingExtension() = default;
    virtual ~WebNativeMessagingExtension() = default;

    static WebNativeMessagingExtension* Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime);

    virtual int32_t ConnectNative(WNMEConnectionInfo& connection);

    virtual int32_t DisconnectNative(WNMEConnectionInfo& connection);
};
} // namespace NWeb
} // namespace OHOS
#endif // WEB_NATIVE_MESSAGING_EXTENSION_H
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

#include "web_native_messaging_extension.h"

#include "js_web_native_messaging_extension.h"
#include "runtime.h"
#include "web_native_messaging_extension_context.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {

WebNativeMessagingExtension* WebNativeMessagingExtension::Create(
    const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    if (!runtime) {
        return new WebNativeMessagingExtension();
    }

    switch (runtime->GetLanguage()) {
        case AbilityRuntime::Runtime::Language::JS:
            return JsWebNativeMessagingExtension::Create(runtime);

        default:
            return new WebNativeMessagingExtension();
    }
}

int32_t WebNativeMessagingExtension::ConnectNative(WNMEConnectionInfo& connection)
{
    return 0;
}

int32_t WebNativeMessagingExtension::DisconnectNative(WNMEConnectionInfo& connection)
{
    return 0;
}

} // namespace NWeb
} // namespace OHOS

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

#include "web_native_messaging_extension_module_loader.h"

#include "web_native_messaging_extension.h"

namespace OHOS {
namespace NWeb {
namespace {
const std::map<std::string, std::string> EXTENSION_PARAMS = {
    {"type", "32"},
    {"name", "WebNativeMessagingExtension"}
};
}

WebNativeMessagingExtensionModuleLoader::WebNativeMessagingExtensionModuleLoader() = default;
WebNativeMessagingExtensionModuleLoader::~WebNativeMessagingExtensionModuleLoader() = default;

AbilityRuntime::Extension *WebNativeMessagingExtensionModuleLoader::Create(
    const std::unique_ptr<AbilityRuntime::Runtime>& runtime) const
{
    return WebNativeMessagingExtension::Create(runtime);
}

std::map<std::string, std::string> WebNativeMessagingExtensionModuleLoader::GetParams()
{
    return EXTENSION_PARAMS;
}

extern "C" __attribute__((visibility("default"))) void *OHOS_EXTENSION_GetExtensionModule()
{
    return &WebNativeMessagingExtensionModuleLoader::GetInstance();
}

} // namespace NWeb
} // namespace OHOS

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

#ifndef JS_WEB_NATIVE_EXTENSION_CONTEXT_H
#define JS_WEB_NATIVE_EXTENSION_CONTEXT_H

#include "napi/native_api.h"
#include "web_native_messaging_extension.h"

namespace OHOS {
namespace NWeb {
napi_value CreateJsWebNativeMessagingExtensionContext(
    napi_env env, std::shared_ptr<WebNativeMessagingExtensionContext> context);
} // namespace NWeb
} // namespace OHOS
#endif // JS_WEB_NATIVE_EXTENSION_CONTEXT_H
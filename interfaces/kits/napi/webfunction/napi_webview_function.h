/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_NAPI_WEBVIEW_FUNCTION_H
#define NWEB_NAPI_WEBVIEW_FUNCTION_H

#include <functional>
#include <unordered_map>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "ohos_adapter_helper.h"
#include "webview_controller.h"
#include "webview_web_inited_callback.h"

namespace OHOS {
namespace NWeb {
napi_value WebFunctionInit(napi_env env, napi_value exports);

napi_value JsOnce(napi_env env, napi_callback_info info);

void RegisterWebInitedCallback(napi_env env, napi_ref callback);
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEBVIEW_FUNCTION_H

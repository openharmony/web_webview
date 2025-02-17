/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NWEB_NAPI_PROXY_CONFIG_H
#define NWEB_NAPI_PROXY_CONFIG_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NWeb {

const std::string PROXY_CONFIG_CLASS_NAME = "ProxyConfig";

class NapiProxyConfig {
public:
    NapiProxyConfig() = default;
    ~NapiProxyConfig() = default;

    static napi_value Init(napi_env env, napi_value exports);
    static napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_InsertBypassRule(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_InsertDirectRule(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_InsertProxyRule(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_BypassHostnamesWithoutPeriod(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_ClearImplicitRules(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_EnableReverseBypass(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetBypassRules(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetProxyRules(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_IsReverseBypassEnabled(napi_env env, napi_callback_info cbinfo);
};
} // namespace NWeb
} // namespace OHOS

#endif  // NWEB_NAPI_PROXY_CONFIG_H
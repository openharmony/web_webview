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

#ifndef NWEB_NAPI_PROXY_RULE_H
#define NWEB_NAPI_PROXY_RULE_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "proxy_rule.h"

namespace OHOS {
namespace NWeb {

const std::string PROXY_RULE_CLASS_NAME = "ProxyRule";
const std::string PROXY_SCHEME_FILTER_ENUM_NAME = "ProxySchemeFilter";

class NapiProxyRule {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_status DefineProperties(napi_env, napi_value *object);
    static void ExportProxyRuleClass(napi_env, napi_value* exportsPointer);
    static void ExportProxySchemeFilterEnum(napi_env, napi_value* exportsPointer);

    NapiProxyRule() = default;
    ~NapiProxyRule() = default;

    static napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetSchemeFilter(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetUrl(napi_env env, napi_callback_info cbinfo);
};
} // namespace NWeb
} // namespace OHOS

#endif  // NWEB_NAPI_PROXY_RULE_H
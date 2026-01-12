/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef NWEB_NAPI_USER_AGENT_BRAND_VERSION_H
#define NWEB_NAPI_USER_AGENT_BRAND_VERSION_H
 
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
 
namespace OHOS {
namespace NWeb {
 
const std::string USER_AGENT_BRAND_VERSION_CLASS_NAME = "UserAgentBrandVersion";
 
class NapiUserAgentBrandVersion {
public:
    NapiUserAgentBrandVersion() = default;
    ~NapiUserAgentBrandVersion() = default;
 
    static napi_value Init(napi_env env, napi_value exports);
    static napi_status DefineProperties(napi_env env, napi_value *object);
    static napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetBrand(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetMajorVersion(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetFullVersion(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetBrand(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetMajorVersion(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetFullVersion(napi_env env, napi_callback_info cbinfo);
};
} // namespace NWeb
} // namespace OHOS
 
#endif  // NWEB_NAPI_USER_AGENT_BRAND_VERSION_H
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
 
#ifndef NWEB_NAPI_USER_AGENT_METADATA_H
#define NWEB_NAPI_USER_AGENT_METADATA_H
 
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
 
namespace OHOS {
namespace NWeb {
 
const std::string USER_AGENT_METADATA_CLASS_NAME = "UserAgentMetadata";
 
class NapiUserAgentMetadata {
public:
    NapiUserAgentMetadata() = default;
    ~NapiUserAgentMetadata() = default;

    static void ExportUserAgentFormFactorEnum(napi_env, napi_value* exports);
    static napi_status DefineProperties(napi_env, napi_value *object);
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetBrandVersionList(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetArchitecture(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetBitness(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetFormFactors(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetFullVersion(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetMobile(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetModel(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetPlatform(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetPlatformVersion(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_SetWow64(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetBrandVersionList(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetArchitecture(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetBitness(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetFormFactors(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetFullVersion(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetMobile(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetModel(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetPlatform(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetPlatformVersion(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetWow64(napi_env env, napi_callback_info cbinfo);
};
} // namespace NWeb
} // namespace OHOS
 
#endif  // NWEB_NAPI_USER_AGENT_METADATA_H
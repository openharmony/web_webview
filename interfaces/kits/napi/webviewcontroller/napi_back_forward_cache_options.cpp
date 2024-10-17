/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "napi_back_forward_cache_options.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <securec.h>

#include "business_error.h"
#include "nweb_log.h"
#include "napi_parse_utils.h"
#include "napi/native_node_api.h"
#include "web_errors.h"

using namespace OHOS::NWebError;

namespace OHOS {
namespace NWeb {

const std::string BACK_FORWARD_CACHE_OPTIONS = "BackForwardCacheOptions";
const std::string BACK_FORWARD_CACHE_SUPPORTED_FEATURES = "BackForwardCacheSupportedFeatures";

napi_value NapiBackForwardCacheOptions::JS_Constructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    return thisVar;
}

napi_value NapiBackForwardCacheSupportedFeatures::JS_Constructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    return thisVar;
}

napi_value NapiBackForwardCacheOptions::Init(napi_env env, napi_value exports)
{
    WVLOG_D("NapiBackForwardCacheOptions::Init");
    napi_value backForwardCacheOptions = nullptr;
    napi_define_class(env, BACK_FORWARD_CACHE_OPTIONS.c_str(),
        BACK_FORWARD_CACHE_OPTIONS.length(),
        JS_Constructor, nullptr, 0, nullptr, &backForwardCacheOptions);
    napi_set_named_property(env, exports, BACK_FORWARD_CACHE_OPTIONS.c_str(),
        backForwardCacheOptions);
    return exports;
}

napi_value NapiBackForwardCacheSupportedFeatures::Init(napi_env env, napi_value exports)
{
    WVLOG_D("NapiBackForwardCacheSupportedFeatures::Init");
    napi_value backForwardCacheSupportedFeatures = nullptr;
    napi_define_class(env, BACK_FORWARD_CACHE_SUPPORTED_FEATURES.c_str(),
        BACK_FORWARD_CACHE_SUPPORTED_FEATURES.length(),
        JS_Constructor, nullptr, 0, nullptr, &backForwardCacheSupportedFeatures);
    napi_set_named_property(env, exports, BACK_FORWARD_CACHE_SUPPORTED_FEATURES.c_str(),
        backForwardCacheSupportedFeatures);
    return exports;
}

}
}
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
 
#include "napi_prefetch_options.h"
 
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
 
const std::string PREFETCH_OPTIONS = "PrefetchOptions";
 
napi_value NapiWebPrefetchOptions::JS_Constructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);
 
    return thisVar;
}
 
napi_value NapiWebPrefetchOptions::Init(napi_env env, napi_value exports)
{
    WVLOG_D("NapiBackForwardCacheOptions::Init");
    napi_value prefetchOptions = nullptr;
    napi_define_class(env, PREFETCH_OPTIONS.c_str(),
        PREFETCH_OPTIONS.length(),
        JS_Constructor, nullptr, 0, nullptr, &prefetchOptions);
    napi_set_named_property(env, exports, PREFETCH_OPTIONS.c_str(),
        prefetchOptions);
    return exports;
}
 
}
}
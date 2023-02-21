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

#include "napi_webview_function.h"

#include <unistd.h>
#include <uv.h>
#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;

std::unordered_map<std::string, std::function<void(napi_env, napi_ref)>> onceType = {
    {"webInited", RegisterWebInitedCallback},
};

napi_value WebFunctionInit(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("once", JsOnce),
    };
    napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties);
    return exports;
}

napi_value JsOnce(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_TWO) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string type = "";
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ONE], &valueType);
    if (!(NapiParseUtils::ParseString(env, argv[0], type)) || (onceType.find(type) == onceType.end()) ||
        (valueType != napi_function)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    napi_ref callback = nullptr;
    napi_create_reference(env, argv[INTEGER_ONE], 1, &callback);
    onceType.find(type)->second(env, callback);

    return result;
}

void RegisterWebInitedCallback(napi_env env, napi_ref callback)
{
    WebInitedCallbackParam *param = new (std::nothrow) WebInitedCallbackParam(env, callback);
    if (param == nullptr) {
        return;
    }
    WebRunInitedCallback *runWebInitedCallbackObj = new (std::nothrow) WebRunInitedCallbackImpl(param);
    if (runWebInitedCallbackObj == nullptr) {
        delete param;
        return;
    }
    OhosAdapterHelper::GetInstance().GetInitWebAdapter()->SetRunWebInitedCallback(std::move(runWebInitedCallbackObj));
}
} // namespace NWeb
} // namespace OHOS

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ani_webview_function.h"

#include <unistd.h>
#include <uv.h>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;

namespace {
const char* WEB_WEBVIEW_NAMESPACE_NAME = "L@ohos/web/webview/webview;";
}

void RegisterWebInitedCallback(ani_env* env, ani_ref callback)
{
    WVLOG_I("enter RegisterWebInitedCallback");
    WebInitedCallbackParam* param = new (std::nothrow) WebInitedCallbackParam(env, callback);
    if (param == nullptr) {
        return;
    }

    WebRunInitedCallback* runWebInitedCallbackObj = new (std::nothrow) WebRunInitedCallbackImpl(param);
    if (runWebInitedCallbackObj == nullptr) {
        delete param;
        return;
    }
    WVLOG_I("success in RegisterWebInitedCallback");
    OhosAdapterHelper::GetInstance().GetInitWebAdapter()->SetRunWebInitedCallback(std::move(runWebInitedCallbackObj));
}

std::unordered_map<std::string, std::function<void(ani_env*, ani_ref)>> onceType = {
    { "webInited", RegisterWebInitedCallback },
};

// type:string
static void JsOnce(ani_env* env, ani_string type, ani_object callback)
{
    WVLOG_I("enter JsOnce");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    std::string argvType = "";
    ani_class functionClass;
    if (env->FindClass("Lstd/core/Function;", &functionClass) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    ani_boolean isFunction;
    env->Object_InstanceOf(callback, functionClass, &isFunction);

    if (!(AniParseUtils::ParseString(env, type, argvType)) || !isFunction ||
        (onceType.find(argvType) == onceType.end())) {
        WVLOG_I("JsOnce TypeError : %{public}s", argvType.c_str());
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    WVLOG_I("JsOnce Type : %{public}s", argvType.c_str());

    auto foundCallback = onceType.find(argvType);
    if (foundCallback != onceType.end()) {
        foundCallback->second(env, callback);
    } else {
        WVLOG_I("error");
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }
    WVLOG_I("exit JsOnce");
}

ani_status StsWebviewFunctionInit(ani_env* env)
{
    WVLOG_I("enter StsWebviewFunctionInit");
    ani_namespace ns;
    if (ANI_OK != env->FindNamespace(WEB_WEBVIEW_NAMESPACE_NAME, &ns)) {
        WVLOG_E("StsWebviewFunctionInit find namespace failed");
        return ANI_ERROR;
    }
    std::array methods = {
        ani_native_function { "once", nullptr, reinterpret_cast<void*>(JsOnce) },
    };
    if (ANI_OK != env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size())) {
        WVLOG_E("StsWebviewFunctionInit bind native function failed");
        return ANI_ERROR;
    }
    WVLOG_I("exit StsWebviewFunctionInit");
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
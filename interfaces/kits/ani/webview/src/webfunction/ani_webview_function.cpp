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
    WVLOG_D("enter RegisterWebInitedCallback");
    WebInitedCallbackParam* param = new (std::nothrow) WebInitedCallbackParam(env, callback);
    if (param == nullptr) {
        return;
    }

    WebRunInitedCallback* runWebInitedCallbackObj = new (std::nothrow) WebRunInitedCallbackImpl(param);
    if (runWebInitedCallbackObj == nullptr) {
        delete param;
        return;
    }
    WVLOG_D("success in RegisterWebInitedCallback");
    OhosAdapterHelper::GetInstance().GetInitWebAdapter()->SetRunWebInitedCallback(std::move(runWebInitedCallbackObj));
}

std::unordered_map<std::string, std::function<void(ani_env*, ani_ref)>> onceType = {
    { "webInited", RegisterWebInitedCallback },
};

// type:string
static void JsOnce(ani_env* env, ani_string type, ani_object callback)
{
    WVLOG_D("enter JsOnce");
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
    if (env->Object_InstanceOf(callback, functionClass, &isFunction) != ANI_OK) {
        WVLOG_E("JsOnce Object_InstanceOf failed");
        return;
    }

    if (!(AniParseUtils::ParseString(env, type, argvType)) || !isFunction ||
        (onceType.find(argvType) == onceType.end())) {
        WVLOG_E("JsOnce argvType(%{public}s) error", argvType.c_str());
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    WVLOG_D("JsOnce Type : %{public}s", argvType.c_str());

    auto foundCallback = onceType.find(argvType);
    if (foundCallback != onceType.end()) {
        foundCallback->second(env, callback);
    } else {
        WVLOG_E("argvType(%{public}s) not found", argvType.c_str());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }
}

ani_status StsWebviewFunctionInit(ani_env* env)
{
    WVLOG_D("enter StsWebviewFunctionInit");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_namespace ns;
    if (env->FindNamespace(WEB_WEBVIEW_NAMESPACE_NAME, &ns) != ANI_OK) {
        WVLOG_E("StsWebviewFunctionInit find namespace failed");
        return ANI_ERROR;
    }
    std::array methods = {
        ani_native_function { "once", nullptr, reinterpret_cast<void*>(JsOnce) },
    };
    if (env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size()) != ANI_OK) {
        WVLOG_E("StsWebviewFunctionInit bind native function failed");
        return ANI_ERROR;
    }
    WVLOG_D("exit StsWebviewFunctionInit");
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
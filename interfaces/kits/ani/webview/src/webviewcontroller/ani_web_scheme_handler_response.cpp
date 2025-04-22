/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <array>
#include <iostream>

#include "webview_controller.h"
#include "web_scheme_handler_response.h"

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME = "L@ohos/web/webview/webview/WebSchemeHandlerResponse;";
}

static void JsSetUrl(ani_env *env, ani_object object, ani_object urlObject)
{
    WVLOG_D("WebSchemeHandlerResponse JsSetUrl.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse *>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(urlObject, &isUndefined) != ANI_OK || isUndefined) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return;
    }
    if (!AniParseUtils::IsString(env, urlObject)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return;
    }
    std::string url;
    if (!AniParseUtils::ParseString(env, urlObject, url)) {
         WVLOG_E("ParseString fail");
        return;
    }
    schemeHandler->SetUrl(url.c_str());
}

static ani_string JsGetUrl(ani_env *env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse JsGetUrl.");
    ani_string url = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return url;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse *>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return url;
    }
    char* result = schemeHandler->GetUrl();
    env->String_NewUTF8(result, strlen(result), &url);
    OH_ArkWeb_ReleaseString(result);
    return url;
}

static void Constructor(ani_env *env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    WebSchemeHandlerResponse* schemeHandler = new (std::nothrow) WebSchemeHandlerResponse(env);
    if (schemeHandler == nullptr) {
        WVLOG_E("new webview controller failed");
        return;
    }
    if (!AniParseUtils::Wrap(env, object, WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME,
                             reinterpret_cast<ani_long>(schemeHandler))) {
        WVLOG_E("WebSchemeHandlerResponse wrap failed");
        delete schemeHandler;
        schemeHandler = nullptr;
    }
}

ani_status StsWebSchemeHandlerResponseInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webSchemeHandlerResponseCls = nullptr;
    ani_status status = env->FindClass(WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME, &webSchemeHandlerResponseCls);
    if (status != ANI_OK || !webSchemeHandlerResponseCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void *>(Constructor) },
        ani_native_function { "setUrl", nullptr, reinterpret_cast<void *>(JsSetUrl) },
        ani_native_function { "getUrl", nullptr, reinterpret_cast<void *>(JsGetUrl) },
    };

    status = env->Class_BindNativeMethods(webSchemeHandlerResponseCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
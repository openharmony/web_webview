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

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "nweb_helper.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_COOKIE_MANAGER_CLASS_NAME = "L@ohos/web/webview/webview/WebCookieManager;";
}

static void SetCookieSyncParseString(ani_env *env, ani_string url, ani_string value, std::string& urlStr, std::string& valueStr)
{
    if (!AniParseUtils::ParseString(env, url, urlStr)) {
        WVLOG_E("Parse url failed.");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return;
    }
    if (!AniParseUtils::ParseString(env, value, valueStr)) {
        WVLOG_E("Parse value failed.");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "value", "string"));
        return;
    }
}

static void JsSetCookieSyncThree(ani_env *env, ani_object aniClass, ani_string url, ani_string value,
    ani_object incognito)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(url, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        WVLOG_E("url is undefined");
        WVLOG_E("is undefined");
        return;
    }
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(value, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        WVLOG_E("value is undefined");
        WVLOG_E("is undefined");
        return;
    }
    std::string urlStr;
    std::string valueStr;
    SetCookieSyncParseString(env, url, value, urlStr, valueStr);
    bool incognitoMode = false;
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(incognito, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        ani_boolean bIncognito = ANI_FALSE;
        if (env->Object_CallMethodByName_Boolean(incognito, "unboxed", nullptr, &bIncognito) != ANI_OK) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "incognito", "boolean"));
            return;
        }
        incognitoMode = static_cast<bool>(bIncognito);
    }
    int isSet = -1;
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookieSync(urlStr, valueStr, incognitoMode , false);
    }
    if (isSet == NWebError::INVALID_URL) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INVALID_URL);
        return;
    } else if (isSet == NWebError::INVALID_COOKIE_VALUE) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INVALID_COOKIE_VALUE);
        return;
    }
}

static void JsSetCookieSync(ani_env *env, ani_object aniClass, ani_string url, ani_string value,
    ani_boolean incognito, ani_boolean includeHttpOnly)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(url, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        WVLOG_E("url is undefined");
        WVLOG_E("is undefined");
        return;
    }
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(value, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        WVLOG_E("value is undefined");
        WVLOG_E("is undefined");
        return;
    }
    std::string urlStr;
    std::string valueStr;
    SetCookieSyncParseString(env, url, value, urlStr, valueStr);
    bool incognitoMode = static_cast<bool>(incognito);
    bool bIncludeHttpOnly = static_cast<bool>(includeHttpOnly);
    int isSet = -1;
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookieSync(urlStr, valueStr, incognitoMode , bIncludeHttpOnly);
    }
    if (isSet == NWebError::INVALID_URL) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INVALID_URL);
        return;
    } else if (isSet == NWebError::INVALID_COOKIE_VALUE) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INVALID_COOKIE_VALUE);
        return;
    }
}

static ani_string JsFetchCookieSync(ani_env *env, ani_object aniClass, ani_string url, ani_object incognito)
{
    WVLOG_D("[COOKIE] JsFetchCookieSync.");
    ani_string result = {};
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(url, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        WVLOG_E("is undefined");
        return result;
    }
    std::string urlStr;
    if (!AniParseUtils::ParseString(env, url, urlStr)) {
        WVLOG_E("Parse url failed.");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return result;
    }

    bool incognitoMode = false;
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(incognito, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        ani_boolean bIncognito;
        if (env->Object_CallMethodByName_Boolean(incognito, "booleanValue", nullptr, &bIncognito) != ANI_OK) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "incognito", "boolean"));
            return result;
        }
        incognitoMode = static_cast<bool>(bIncognito);
    }

    std::string cookieContent = "";
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    bool isValid = true;
    if (cookieManager != nullptr) {
        cookieContent = cookieManager->ReturnCookie(urlStr, isValid, incognitoMode);
    }
    if (cookieContent == "" && !isValid) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INVALID_URL);
        return result;
    }
    env->String_NewUTF8(cookieContent.c_str(), cookieContent.size(), &result);
    return result;
}

static void JsClearAllCookiesSync(ani_env *env, ani_object aniClass, ani_object incognito)
{
    WVLOG_D("[COOKIE] JsClearAllCookiesSync");
    ani_boolean isUndefined = ANI_TRUE;
    bool incognitoMode = false;
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(incognito, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        ani_boolean bIncognito;
        if (env->Object_CallMethodByName_Boolean(incognito, "booleanValue", nullptr, &bIncognito) != ANI_OK) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "incognito", "boolean"));
            return;
        }
        incognitoMode = static_cast<bool>(bIncognito);
    }

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr");
        return;
    }
    cookieManager->DeleteCookieEntirely(nullptr, incognitoMode);
}

static void JsSaveCookieSync(ani_env *env, ani_object aniClass)
{
    WVLOG_D("[COOKIE] JsSaveCookieSync");
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr");
        return;
    }
    cookieManager->Store();
}

ani_status StsWebCookieManagerInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webCookieManagerCls = nullptr;
    ani_status status = env->FindClass(WEB_COOKIE_MANAGER_CLASS_NAME, &webCookieManagerCls);
    if (status != ANI_OK || !webCookieManagerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_COOKIE_MANAGER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "configCookieSyncInternal", nullptr, reinterpret_cast<void *>(JsSetCookieSyncThree) },
        ani_native_function { "configCookieSync", nullptr, reinterpret_cast<void *>(JsSetCookieSync) },
        ani_native_function { "fetchCookieSync", nullptr, reinterpret_cast<void *>(JsFetchCookieSync) },
        ani_native_function { "saveCookieSync", nullptr, reinterpret_cast<void *>(JsSaveCookieSync) },
        ani_native_function { "clearAllCookiesSync", nullptr, reinterpret_cast<void *>(JsClearAllCookiesSync) },
    };

    status = env->Class_BindNativeMethods(webCookieManagerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }

    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
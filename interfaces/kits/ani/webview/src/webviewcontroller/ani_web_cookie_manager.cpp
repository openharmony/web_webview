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
#include "ani_web_cookie_manager.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "nweb_helper.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_COOKIE_MANAGER_CLASS_NAME = "@ohos.web.webview.webview.WebCookieManager";
}

const std::string TASK_ID = "configCookieAsync";
constexpr int32_t PARAM_NUM = 1;
constexpr long RESULT_OK = 1;

static void ClearSessionCookieSync(ani_env *env, ani_object aniClass)
{
    WVLOG_D("[COOKIE] ClearSessionCookieSync");
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr");
        return;
    }
    cookieManager->DeleteSessionCookies(nullptr);
}

static ani_boolean JsExistCookie(ani_env *env, ani_object aniClass, ani_object incognito)
{
    WVLOG_D("[COOKIE] JsExistCookie");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    ani_boolean isUndefined = ANI_TRUE;
    bool incognitoMode = false;
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(incognito, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        ani_boolean bIncognito;
        if (env->Object_CallMethodByName_Boolean(incognito, "toBoolean", nullptr, &bIncognito) != ANI_OK) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "incognito", "boolean"));
            return ANI_FALSE;
        }
        incognitoMode = static_cast<bool>(bIncognito);
    }

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr");
        return ANI_FALSE;
    }
    return static_cast<ani_boolean>(cookieManager->ExistCookies(incognitoMode));
}

static ani_boolean JsIsCookieAllowed(ani_env *env, ani_object aniClass)
{
    WVLOG_D("[COOKIE] JsIsCookieAllowed");
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr");
        return ANI_FALSE;
    }
    return static_cast<ani_boolean>(cookieManager->IsAcceptCookieAllowed());
}

static void JsSetCookieSyncParseString(ani_env *env, ani_string url, ani_string value, std::string& urlStr,
    std::string& valueStr)
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
    JsSetCookieSyncParseString(env, url, value, urlStr, valueStr);
    bool incognitoMode = false;
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(incognito, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        ani_boolean bIncognito = ANI_FALSE;
        if (env->Object_CallMethodByName_Boolean(incognito, "toBoolean", nullptr, &bIncognito) != ANI_OK) {
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
        isSet = cookieManager->SetCookieSync(urlStr, valueStr, incognitoMode, false);
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
    JsSetCookieSyncParseString(env, url, value, urlStr, valueStr);
    bool incognitoMode = static_cast<bool>(incognito);
    bool bIncludeHttpOnly = static_cast<bool>(includeHttpOnly);
    int isSet = -1;
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookieSync(urlStr, valueStr, incognitoMode, bIncludeHttpOnly);
    }
    if (isSet == NWebError::INVALID_URL) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INVALID_URL);
        return;
    } else if (isSet == NWebError::INVALID_COOKIE_VALUE) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INVALID_COOKIE_VALUE);
        return;
    }
}

NWebConfigCookieCallbackImpl::NWebConfigCookieCallbackImpl(ani_env* env, ani_ref callback, ani_resolver resolver)
    :vm_(nullptr), callback_(nullptr), resolver_(resolver)
{
    WVLOG_D("enter NWebConfigCookieCallbackImpl::NWebConfigCookieCallbackImpl");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->GetVM(&vm_);
    if (callback) {
        env->GlobalReference_Create(callback, &callback_);
    }
}

NWebConfigCookieCallbackImpl::~NWebConfigCookieCallbackImpl()
{
    WVLOG_D("enter NWebConfigCookieCallbackImpl::~NWebConfigCookieCallbackImpl");
    ani_env* env = GetEnv();
    if (env && callback_) {
        env->GlobalReference_Delete(callback_);
    }
}

void UvAfterWorkCbPromise(std::shared_ptr<NWebConfigCookieCallbackImpl> cookieObj, long result)
{
    if (!cookieObj) {
        WVLOG_E("cookieObj is null");
        return;
    }
    ani_resolver resolver = cookieObj->GetResolver();
    if (!resolver) {
        WVLOG_E("resolver is null");
        return;
    }
    ani_env* env = cookieObj->GetEnv();
    if (!env) {
        WVLOG_E("env is null");
        return;
    }
    ani_ref resultRef = nullptr;
    ani_status status = ANI_OK;
    if (result != RESULT_OK) {
        resultRef = AniBusinessError::CreateError(env, result);
        if ((status = env->PromiseResolver_Reject(resolver, reinterpret_cast<ani_error>(resultRef))) != ANI_OK) {
            WVLOG_E("promise reject error status = %{public}d", status);
        }
    } else {
        env->GetNull(&resultRef);
        if ((status = env->PromiseResolver_Resolve(resolver, resultRef)) != ANI_OK) {
            WVLOG_E("promise reject error status = %{public}d", status);
        }
    }
}

void UvJsCallbackThreadWoker(std::shared_ptr<NWebConfigCookieCallbackImpl> cookieObj, long result)
{
    WVLOG_D("in NWebConfigCookieCallbackImpl::OnReceiveValue result = %{public}ld", result);
    if (!cookieObj) {
        WVLOG_E("cookieObj is null");
        return;
    }
    ani_env* env = cookieObj->GetEnv();
    if (!env) {
        WVLOG_E("env is null");
        return;
    }
    if (cookieObj->GetCallBack()) {
        ani_ref resultRef = nullptr;
        ani_status status = ANI_OK;
        ani_ref callbackResult = nullptr;
        if (result != RESULT_OK) {
            resultRef = AniBusinessError::CreateError(env, result);
        } else {
            if ((status = env->GetNull(&resultRef)) != ANI_OK) {
                WVLOG_E("resultRef is null");
                return;
            }
        }
        if ((status = env->FunctionalObject_Call(static_cast<ani_fn_object>(cookieObj->GetCallBack()),
            PARAM_NUM, &resultRef, &callbackResult)) != ANI_OK) {
            WVLOG_E("callback function execute error status = %{public}d", status);
        }
    }
    if (cookieObj->GetResolver()) {
        UvAfterWorkCbPromise(cookieObj, result);
    }
}

void NWebConfigCookieCallbackImpl::OnReceiveValue(long result)
{
    WVLOG_D("in NWebConfigCookieCallbackImpl::OnReceiveValue result = %{public}ld", result);
    std::shared_ptr<NWebConfigCookieCallbackImpl> sharedThis = shared_from_this();

    if (!mainHandler_) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            return;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }

    if (!mainHandler_) {
        WVLOG_E("mainHandler_ is null.");
        return;
    }

    auto task = [sharedThis, result] () { UvJsCallbackThreadWoker(sharedThis, result); };
    bool postResult = false;
    postResult = mainHandler_->PostTask(std::move(task), TASK_ID, 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
    if (postResult) {
        WVLOG_D("PostTask success");
    } else {
        WVLOG_E("PostTask failed");
    }
}

static ani_object JsSetCookiePromise(ani_env *env, ani_object aniClass, ani_string url, ani_string value,
    ani_boolean incognito, ani_boolean includeHttpOnly)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::string urlStr;
    std::string valueStr;
    JsSetCookieSyncParseString(env, url, value, urlStr, valueStr);
    bool tempIncognito = static_cast<bool>(incognito);
    bool tempIncludeHttpOnly = static_cast<bool>(includeHttpOnly);
    ani_object promise;
    ani_resolver resolver;
    if (env->Promise_New(&resolver, &promise) != ANI_OK) {
        WVLOG_E("create promise object error");
        return nullptr;
    }
    if (promise && resolver) {
        std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
            OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
        if (cookieManager == nullptr) {
            ani_ref jsResult = nullptr;
            env->GetUndefined(&jsResult);
            if (env ->PromiseResolver_Reject(resolver, static_cast<ani_error>(jsResult)) != ANI_OK) {
                WVLOG_E("promise reject error");
                return nullptr;
            }
        } else {
            auto callbackImpl = std::make_shared<OHOS::NWeb::NWebConfigCookieCallbackImpl>(env, nullptr, resolver);
            cookieManager->SetCookieAsync(urlStr, valueStr, tempIncognito, tempIncludeHttpOnly, callbackImpl);
        }
    }
    return promise;
}

static void JsSetCookieCallback(ani_env *env, ani_object aniClass, ani_string url, ani_string value,
    ani_object callback)
{
    if (!env) {
        WVLOG_E("env is null");
        return;
    }
    if (!AniParseUtils::IsFunction(env, callback)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
        return;
    }
    std::string urlStr;
    std::string valueStr;
    JsSetCookieSyncParseString(env, url, value, urlStr, valueStr);
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is null");
        ani_status status = ANI_OK;
        ani_ref jsResult = nullptr;
        ani_ref jsCallback = nullptr;
        if ((status = env->GetNull(&jsResult)) != ANI_OK) {
            WVLOG_E("jsResult is null");
            return;
        }
        env->GlobalReference_Create(callback, &jsCallback);
        if (jsCallback) {
            ani_ref callbackResult = nullptr;
            if ((status = env->FunctionalObject_Call(static_cast<ani_fn_object>(jsCallback),
                PARAM_NUM, &jsResult, &callbackResult)) != ANI_OK) {
                WVLOG_E("callback function execute error status = %{public}d", status);
                }
        }
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebConfigCookieCallbackImpl>(env, callback, nullptr);
        cookieManager->SetCookieAsync(urlStr, valueStr, false, false, callbackImpl);
    }
}

static void JsPutAcceptCookieEnabled(ani_env *env, ani_object aniClass, ani_boolean accept)
{
    WVLOG_D("[COOKIE] JsPutAcceptCookieEnabled");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptCookieEnabled(accept);
    }
    return;
}

static void JsPutAcceptThirdPartyCookieEnabled(ani_env* env, ani_object aniClass, ani_boolean accept)
{
    WVLOG_D("[COOKIE] JsPutAcceptThirdPartyCookieEnabled");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptThirdPartyCookieEnabled(accept);
    }
    return;
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
        if (env->Object_CallMethodByName_Boolean(incognito, "toBoolean", ":z", &bIncognito) != ANI_OK) {
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

static ani_boolean JsIsThirdPartyCookieAllowed(ani_env *env, ani_object aniClass)
{
    WVLOG_D("[COOKIE] JsIsThirdPartyCookieAllowed");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        return static_cast<ani_boolean>(cookieManager->IsThirdPartyCookieAllowed());
    } else {
        WVLOG_E("cookieManager is nullptr");
        return ANI_FALSE;
    }
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
        if (env->Object_CallMethodByName_Boolean(incognito, "toBoolean", nullptr, &bIncognito) != ANI_OK) {
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

static void JsSetLazyInitializeWebEngine(ani_env* env, ani_object aniClass, ani_boolean lazy)
{
    WVLOG_I("[COOKIE] JsSetLazyInitializeWebEngine Lazy: %d.", lazy);
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    bool lazyInit = static_cast<bool>(lazy);
    OHOS::NWeb::NWebHelper::Instance().SetLazyInitializeWebEngine(lazyInit);
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
        ani_native_function { "clearSessionCookieSync", nullptr, reinterpret_cast<void *>(ClearSessionCookieSync) },
        ani_native_function { "existCookie", nullptr, reinterpret_cast<void *>(JsExistCookie) },
        ani_native_function { "isCookieAllowed", nullptr, reinterpret_cast<void *>(JsIsCookieAllowed) },
        ani_native_function { "configCookieSyncInternal", nullptr, reinterpret_cast<void *>(JsSetCookieSyncThree) },
        ani_native_function { "configCookieSync", "C{std.core.String}C{std.core.String}zz:",
                              reinterpret_cast<void *>(JsSetCookieSync) },
        ani_native_function { "configCookiePromise", nullptr, reinterpret_cast<void *>(JsSetCookiePromise)},
        ani_native_function { "configCookieCallback", nullptr, reinterpret_cast<void *>(JsSetCookieCallback)},
        ani_native_function { "putAcceptCookieEnabled", nullptr, reinterpret_cast<void *>(JsPutAcceptCookieEnabled) },
        ani_native_function {"putAcceptThirdPartyCookieEnabled", nullptr,
            reinterpret_cast<void *>(JsPutAcceptThirdPartyCookieEnabled) },
        ani_native_function { "isThirdPartyCookieAllowed",
            nullptr, reinterpret_cast<void *>(JsIsThirdPartyCookieAllowed) },
        ani_native_function { "fetchCookieSync", nullptr, reinterpret_cast<void *>(JsFetchCookieSync) },
        ani_native_function { "saveCookieSync", nullptr, reinterpret_cast<void *>(JsSaveCookieSync) },
        ani_native_function { "clearAllCookiesSync", nullptr, reinterpret_cast<void *>(JsClearAllCookiesSync) },
        ani_native_function {
            "setLazyInitializeWebEngine", nullptr, reinterpret_cast<void*>(JsSetLazyInitializeWebEngine) },
    };

    status = env->Class_BindStaticNativeMethods(webCookieManagerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindStaticNativeMethods failed status: %{public}d", status);
    }

    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
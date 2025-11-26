/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_web_cookie_manager.h"

#include <cstdint>
#include <uv.h>
#include <vector>

#include "business_error.h"
#include "nweb_napi_scope.h"
#include "napi/native_common.h"
#include "nweb_cookie_manager.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "napi_parse_utils.h"
#include "web_errors.h"
#include "securec.h"

namespace OHOS {
namespace NWeb {
napi_value NapiWebCookieManager::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getCookie", NapiWebCookieManager::JsGetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("fetchCookieSync", NapiWebCookieManager::JsGetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("fetchCookie", NapiWebCookieManager::JsFetchCookieAsync),
        DECLARE_NAPI_STATIC_FUNCTION("setCookie", NapiWebCookieManager::JsSetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("configCookieSync", NapiWebCookieManager::JsSetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("configCookie", NapiWebCookieManager::JsConfigCookieAsync),
        DECLARE_NAPI_STATIC_FUNCTION("isCookieAllowed", NapiWebCookieManager::JsIsCookieAllowed),
        DECLARE_NAPI_STATIC_FUNCTION("putAcceptCookieEnabled", NapiWebCookieManager::JsPutAcceptCookieEnabled),
        DECLARE_NAPI_STATIC_FUNCTION("isThirdPartyCookieAllowed",
                                     NapiWebCookieManager::JsIsThirdPartyCookieAllowed),
        DECLARE_NAPI_STATIC_FUNCTION("putAcceptThirdPartyCookieEnabled",
                                     NapiWebCookieManager::JsPutAcceptThirdPartyCookieEnabled),
        DECLARE_NAPI_STATIC_FUNCTION("existCookie", NapiWebCookieManager::JsExistCookie),
        DECLARE_NAPI_STATIC_FUNCTION("deleteEntireCookie", NapiWebCookieManager::JsDeleteEntireCookie),
        DECLARE_NAPI_STATIC_FUNCTION("clearAllCookiesSync", NapiWebCookieManager::JsDeleteEntireCookie),
        DECLARE_NAPI_STATIC_FUNCTION("clearAllCookies", NapiWebCookieManager::JsClearAllCookiesAsync),
        DECLARE_NAPI_STATIC_FUNCTION("deleteSessionCookie", NapiWebCookieManager::JsDeleteSessionCookie),
        DECLARE_NAPI_STATIC_FUNCTION("clearSessionCookieSync", NapiWebCookieManager::JsDeleteSessionCookie),
        DECLARE_NAPI_STATIC_FUNCTION("clearSessionCookie", NapiWebCookieManager::JsClearSessionCookieAsync),
        DECLARE_NAPI_STATIC_FUNCTION("saveCookieAsync", NapiWebCookieManager::JsSaveCookieAsync),
        DECLARE_NAPI_STATIC_FUNCTION("saveCookieSync", NapiWebCookieManager::JsSaveCookieSync),
        DECLARE_NAPI_STATIC_FUNCTION("fetchAllCookies", NapiWebCookieManager::JsFetchAllCookies),
    };
    napi_value constructor = nullptr;

    napi_define_class(env, WEB_COOKIE_MANAGER_CLASS_NAME.c_str(), WEB_COOKIE_MANAGER_CLASS_NAME.length(),
        JsConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "NapiWebCookieManager define js class failed");
    napi_status status = napi_set_named_property(env, exports, "WebCookieManager", constructor);
    NAPI_ASSERT(env, status == napi_ok, "NapiWebCookieManager set property failed");
    return exports;
}

napi_value NapiWebCookieManager::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;

    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    return thisVar;
}

bool NapiWebCookieManager::GetStringPara(napi_env env, napi_value argv, std::string& outValue)
{
    constexpr int32_t MAX_STRING_LENGTH = 40960;
    size_t bufferSize = 0;
    napi_valuetype valueType = napi_null;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_string) {
        return false;
    }
    napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
    if (bufferSize > MAX_STRING_LENGTH) {
        return false;
    }
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv, stringValue, bufferSize + 1, &jsStringLength);
    if (jsStringLength != bufferSize) {
        return false;
    }
    outValue = stringValue;
    return true;
}

bool NapiWebCookieManager::GetBooleanPara(napi_env env, napi_value argv, bool& outValue)
{
    napi_valuetype valueType = napi_null;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_boolean) {
        return false;
    }

    bool boolValue;
    napi_get_value_bool(env, argv, &boolValue);
    outValue = boolValue;
    return true;
}

void NapiCallBackNullptr(napi_env env, napi_ref jsCallback)
{
    napi_value jsResult = nullptr;
    napi_get_null(env, &jsResult);
    napi_value callback = nullptr;
    napi_get_reference_value(env, jsCallback, &callback);
    napi_value callbackResult = nullptr;
    napi_call_function(env, nullptr, callback, INTEGER_ONE, &jsResult, &callbackResult);
}

void FetchCookieAsyncCallback(napi_env env, napi_ref jsCallback, std::string url)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        NapiCallBackNullptr(env, jsCallback);
        napi_delete_reference(env, jsCallback);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebFetchCookieCallbackImpl>(env, jsCallback, nullptr);
        cookieManager->GetCookieAsync(url, false, callbackImpl);
    }
}

void FetchCookieAsyncPromise(napi_env env, napi_deferred deferred, std::string url, bool incognitoMode)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        napi_value jsResult = nullptr;
        napi_get_undefined(env, &jsResult);
        napi_reject_deferred(env, deferred, jsResult);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebFetchCookieCallbackImpl>(env, nullptr, deferred);
        cookieManager->GetCookieAsync(url, incognitoMode, callbackImpl);
    }
}

napi_value NapiWebCookieManager::JsFetchCookieAsync(napi_env env, napi_callback_info info)
{
    size_t argc = INTEGER_TWO;
    size_t argcCallback = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = {0};
    napi_value retValue = nullptr;

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE && argc != INTEGER_TWO) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "one", "two"));
        return nullptr;
    }

    std::string url;
    if (!GetStringPara(env, argv[INTEGER_ZERO], url)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    bool incognitoMode = false;

    if (argc == INTEGER_TWO) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[argcCallback - 1], &valueType);

        if (valueType == napi_function) {
            napi_ref jsCallback = nullptr;
            napi_create_reference(env, argv[argcCallback - 1], INTEGER_ONE, &jsCallback);
            if (jsCallback) {
                FetchCookieAsyncCallback(env, jsCallback, url);
            }
            return result;
        }

        if (!GetBooleanPara(env, argv[INTEGER_ONE], incognitoMode)) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYEPS_ERROR));
            return nullptr;
        }
    }

    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);
    if (promise && deferred) {
        FetchCookieAsyncPromise(env, deferred, url, incognitoMode);
    }
    return promise;
}

napi_value NapiWebCookieManager::JsGetCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_TWO;
    size_t argcForOld = INTEGER_ONE;
    napi_value argv[INTEGER_TWO] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_TWO && argc != argcForOld) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "one", "two"));
        return nullptr;
    }

    std::string url;
    if (!GetStringPara(env, argv[INTEGER_ZERO], url)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return nullptr;
    }

    bool incognitoMode = false;
    if (argc == INTEGER_TWO && !GetBooleanPara(env, argv[INTEGER_ONE], incognitoMode)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "incognito", "boolean"));
        return nullptr;
    }

    napi_value result = nullptr;
    std::string cookieContent = "";

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    bool isValid = true;
    if (cookieManager != nullptr) {
        cookieContent = cookieManager->ReturnCookie(url, isValid, incognitoMode);
    }

    if (cookieContent == "" && !isValid) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::INVALID_URL);
        return nullptr;
    }
    napi_create_string_utf8(env, cookieContent.c_str(), cookieContent.length(), &result);
    return result;
}

napi_value NapiWebCookieManager::JsSetCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_FOUR;
    napi_value argv[INTEGER_FOUR] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc > INTEGER_FOUR || argc < INTEGER_TWO) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_THREE, "two", "three", "four"));
        return nullptr;
    }

    std::string url;
    std::string value;
    if (!GetStringPara(env, argv[INTEGER_ZERO], url)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return nullptr;
    }
    if (!GetStringPara(env, argv[INTEGER_ONE], value)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "value", "string"));
        return nullptr;
    }

    bool incognitoMode = false;
    if (argc > INTEGER_TWO && !GetBooleanPara(env, argv[INTEGER_TWO], incognitoMode)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "incognito", "boolean"));
        return nullptr;
    }

    bool includeHttpOnly = false;
    if (argc == INTEGER_FOUR && !GetBooleanPara(env, argv[INTEGER_THREE], includeHttpOnly)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "includeHttpOnly", "boolean"));
        return nullptr;
    }

    napi_value result = nullptr;
    int isSet = -1;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookieSync(url, value, incognitoMode, includeHttpOnly);
    }
    if (isSet == NWebError::INVALID_URL) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::INVALID_URL);
        return nullptr;
    } else if (isSet == NWebError::INVALID_COOKIE_VALUE) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::INVALID_COOKIE_VALUE);
        return nullptr;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

void ConfigCookieAsyncCallback(napi_env env, napi_ref jsCallback, std::string url, std::string value)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        NapiCallBackNullptr(env, jsCallback);
        napi_delete_reference(env, jsCallback);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebConfigCookieCallbackImpl>(env, jsCallback, nullptr);
        cookieManager->SetCookieAsync(url, value, false, false, callbackImpl);
    }
}

napi_value ConfigCookieAsyncPromise(
    napi_env env, std::string url, std::string value, bool incognitoMode, bool includeHttpOnly)
{
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);
    if (promise && deferred) {
        std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
            OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
        if (cookieManager == nullptr) {
            napi_value jsResult = nullptr;
            napi_get_undefined(env, &jsResult);
            napi_reject_deferred(env, deferred, jsResult);
        } else {
            auto callbackImpl = std::make_shared<OHOS::NWeb::NWebConfigCookieCallbackImpl>(env, nullptr, deferred);
            cookieManager->SetCookieAsync(url, value, incognitoMode, includeHttpOnly, callbackImpl);
        }
    }
    return promise;
}

bool NapiWebCookieManager::GetStringParaAndEmitError(napi_env env, napi_value argv, 
                                                     const std::string& parav, 
                                                     std::string& value)
{
    if (!GetStringPara(env, argv, value)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, parav.c_str(), "string"));
        return false;
    }
    return true;
}

bool NapiWebCookieManager::GetBooleanParaAndEmitError(napi_env env, napi_value argv, 
                                                      const std::string& parav, 
                                                      bool& value)
{
    if (!GetBooleanPara(env, argv, value)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, parav.c_str(), "boolean"));
        return false;
    }
    return true;
}

napi_value NapiWebCookieManager::JsConfigCookieAsync(napi_env env, napi_callback_info info)
{
    size_t argc = INTEGER_FOUR;
    size_t argcCallback = INTEGER_THREE;
    napi_value argv[INTEGER_FOUR] = { 0 };
    napi_value retValue = nullptr;

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc > INTEGER_FOUR || argc < INTEGER_TWO) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_THREE, "two", "three", "four"));
        return nullptr;
    }

    std::string url;
    std::string value;
    if (!GetStringParaAndEmitError(env, argv[INTEGER_ZERO], "url", url)
        || !GetStringParaAndEmitError(env, argv[INTEGER_ONE], "value", value)) {
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
            return nullptr;
        }
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], INTEGER_ONE, &jsCallback);

        if (jsCallback) {
            ConfigCookieAsyncCallback(env, jsCallback, url, value);
        }
        return result;
    }

    bool incognitoMode = false;
    bool includeHttpOnly = false;

    if (argc == INTEGER_FOUR) {
        if (!GetBooleanParaAndEmitError(env, argv[INTEGER_TWO], "incognito", incognitoMode)
            || !GetBooleanParaAndEmitError(env, argv[INTEGER_THREE], "includeHttpOnly", includeHttpOnly)) {
            return nullptr;
        }
    }

    return ConfigCookieAsyncPromise(env, url, value, incognitoMode, includeHttpOnly);
}

napi_value NapiWebCookieManager::JsIsCookieAllowed(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool accept = true;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsAcceptCookieAllowed();
    }
    NAPI_CALL(env, napi_get_boolean(env, accept, &result));
    return result;
}

napi_value NapiWebCookieManager::JsPutAcceptCookieEnabled(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    bool accept;
    if (!GetBooleanPara(env, argv[INTEGER_ZERO], accept)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "accept", "booleane"));
        return nullptr;
    }

    napi_value result = nullptr;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptCookieEnabled(accept);
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebCookieManager::JsIsThirdPartyCookieAllowed(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool accept = true;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsThirdPartyCookieAllowed();
    }
    NAPI_CALL(env, napi_get_boolean(env, accept, &result));
    return result;
}

napi_value NapiWebCookieManager::JsPutAcceptThirdPartyCookieEnabled(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    bool accept;
    if (!GetBooleanPara(env, argv[INTEGER_ZERO], accept)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "accept", "boolean"));
        return nullptr;
    }

    napi_value result = nullptr;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptThirdPartyCookieEnabled(accept);
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebCookieManager::JsExistCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcForOld = INTEGER_ZERO;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE && argc != argcForOld) {
        return nullptr;
    }

    bool incognitoMode = false;
    if (argc == INTEGER_ONE && !GetBooleanPara(env, argv[INTEGER_ZERO], incognitoMode)) {
        return nullptr;
    }

    napi_value result = nullptr;
    bool exist = true;
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        exist = cookieManager->ExistCookies(incognitoMode);
    }
    NAPI_CALL(env, napi_get_boolean(env, exist, &result));
    return result;
}

void ClearAllCookiesAsyncCallback(napi_env env, napi_ref jsCallback)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        NapiCallBackNullptr(env, jsCallback);
        napi_delete_reference(env, jsCallback);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebCookieCallbackImpl>(env, jsCallback, nullptr);
        cookieManager->DeleteCookieEntirely(callbackImpl, false);
    }
}

void ClearAllCookiesAsyncPromise(napi_env env, napi_deferred deferred)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        napi_value jsResult = nullptr;
        napi_get_undefined(env, &jsResult);
        napi_reject_deferred(env, deferred, jsResult);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebCookieCallbackImpl>(env, nullptr, deferred);
        cookieManager->DeleteCookieEntirely(callbackImpl, false);
    }
}

napi_value NapiWebCookieManager::JsClearAllCookiesAsync(napi_env env, napi_callback_info info)
{
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_ZERO;
    size_t argcCallback = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};
    napi_value thisVar = nullptr;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != argcPromise && argc != argcCallback) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "zero", "one"));
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
            return nullptr;
        }
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], INTEGER_ONE, &jsCallback);

        if (jsCallback) {
            ClearAllCookiesAsyncCallback(env, jsCallback);
        }
        return result;
    }

    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);
    if (promise && deferred) {
        ClearAllCookiesAsyncPromise(env, deferred);
    }
    return promise;
}

napi_value NapiWebCookieManager::JsDeleteEntireCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcForOld = INTEGER_ZERO;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE && argc != argcForOld) {
        return nullptr;
    }

    bool incognitoMode = false;
    if (argc == INTEGER_ONE && !GetBooleanPara(env, argv[INTEGER_ZERO], incognitoMode)) {
        return nullptr;
    }
    napi_value result = nullptr;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->DeleteCookieEntirely(nullptr, incognitoMode);
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

void ClearSessionCookieAsyncCallback(napi_env env, napi_ref jsCallback)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        NapiCallBackNullptr(env, jsCallback);
        napi_delete_reference(env, jsCallback);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebCookieCallbackImpl>(env, jsCallback, nullptr);
        cookieManager->DeleteSessionCookies(callbackImpl);
    }
}

void ClearSessionCookieAsyncPromise(napi_env env, napi_deferred deferred)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        napi_value jsResult = nullptr;
        napi_get_undefined(env, &jsResult);
        napi_reject_deferred(env, deferred, jsResult);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebCookieCallbackImpl>(env, nullptr, deferred);
        cookieManager->DeleteSessionCookies(callbackImpl);
    }
}

napi_value NapiWebCookieManager::JsClearSessionCookieAsync(napi_env env, napi_callback_info info)
{
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_ZERO;
    size_t argcCallback = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};
    napi_value thisVar = nullptr;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != argcPromise && argc != argcCallback) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "zero", "one"));
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
            return nullptr;
        }
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], INTEGER_ONE, &jsCallback);

        if (jsCallback) {
            ClearSessionCookieAsyncCallback(env, jsCallback);
        }
        return result;
    }

    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);
    if (promise && deferred) {
        ClearSessionCookieAsyncPromise(env, deferred);
    }
    return promise;
}

napi_value NapiWebCookieManager::JsDeleteSessionCookie(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->DeleteSessionCookies(nullptr);
    }
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiWebCookieManager::JsSaveCookieSync(napi_env env, napi_callback_info info)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr)");
        napi_value result = nullptr;
        napi_get_undefined(env, &result);
        return result;
    }

    cookieManager->Store();
    napi_value result = nullptr;
    return result;
}

void SaveCookieAsyncCallback(napi_env env, napi_ref jsCallback)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        napi_value jsResult = nullptr;
        napi_get_null(env, &jsResult);
        napi_value callback = nullptr;
        napi_get_reference_value(env, jsCallback, &callback);
        napi_value callbackResult = nullptr;
        napi_call_function(env, nullptr, callback, INTEGER_ONE, &jsResult, &callbackResult);
        napi_delete_reference(env, jsCallback);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebSaveCookieCallbackImpl>(env, jsCallback, nullptr);
        cookieManager->Store(callbackImpl);
    }
}

void SaveCookieAsyncPromise(napi_env env, napi_deferred deferred)
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        napi_value jsResult = nullptr;
        napi_get_undefined(env, &jsResult);
        napi_reject_deferred(env, deferred, jsResult);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebSaveCookieCallbackImpl>(env, nullptr, deferred);
        cookieManager->Store(callbackImpl);
    }
}

napi_value NapiWebCookieManager::JsSaveCookieAsync(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;

    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_ZERO;
    size_t argcCallback = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != argcPromise && argc != argcCallback) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "zero", "one"));
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
            return nullptr;
        }
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], INTEGER_ONE, &jsCallback);

        if (jsCallback) {
            SaveCookieAsyncCallback(env, jsCallback);
        }
        return result;
    }

    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);
    if (promise && deferred) {
        SaveCookieAsyncPromise(env, deferred);
    }
    return promise;
}

void NWebSaveCookieCallbackImpl::UvJsCallbackThreadWoker(uv_work_t *work, int status)
{
    if (work == nullptr) {
        WVLOG_E("uv work is null");
        return;
    }
    NapiWebCookieManager::WebCookieManagerParam *data =
        reinterpret_cast<NapiWebCookieManager::WebCookieManagerParam*>(work->data);
    if (data == nullptr) {
        WVLOG_E("WebCookieManagerParam is null");
        delete work;
        work = nullptr;
        return;
    }
    NApiScope scope(data->env_);
    if (!scope.IsVaild()) {
        return;
    }

    if (data->callback_) {
        napi_value result[INTEGER_ONE] = {0};
        napi_get_null(data->env_, &result[INTEGER_ZERO]);

        napi_value onSaveCookieFunc = nullptr;
        napi_get_reference_value(data->env_, data->callback_, &onSaveCookieFunc);
        napi_value callbackResult = nullptr;
        napi_call_function(data->env_, nullptr, onSaveCookieFunc,
            INTEGER_ONE, &result[INTEGER_ZERO], &callbackResult);
        napi_delete_reference(data->env_, data->callback_);
    } else if (data->deferred_) {
        napi_value jsResult = nullptr;
        napi_get_undefined(data->env_, &jsResult);
        napi_resolve_deferred(data->env_, data->deferred_, jsResult);
    }

    delete data;
    data = nullptr;
    delete work;
    work = nullptr;
}

void NWebSaveCookieCallbackImpl::OnReceiveValue(bool result)
{
    WVLOG_D("save cookie received result, result = %{public}d", result);
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        WVLOG_E("get uv event loop failed");
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("new uv work failed");
        return;
    }
    NapiWebCookieManager::WebCookieManagerParam *param =
        new (std::nothrow) NapiWebCookieManager::WebCookieManagerParam();
    if (param == nullptr) {
        WVLOG_E("new WebCookieManagerParam failed");
        delete work;
        return;
    }
    param->env_ = env_;
    param->callback_ = callback_;
    param->deferred_ = deferred_;

    work->data = reinterpret_cast<void*>(param);
    int ret = uv_queue_work_with_qos_internal(
        loop, work, [](uv_work_t* work) {}, UvJsCallbackThreadWoker, uv_qos_user_initiated,
        "WebviewNWebSaveCookieCallbackImpl");
    if (ret != 0) {
        if (param != nullptr) {
            delete param;
            param = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}

void NapiWebCookieManager::ExecuteGetCookies(napi_env env, void *data)
{
    GetCookiesParam *param = reinterpret_cast<GetCookiesParam *>(data);
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        param->status = napi_generic_failure;
        return;
    }
    std::vector<std::shared_ptr<NWebCookie>> cookies = cookieManager->GetAllCookies(param->incognitoMode);
    for (auto& cookie : cookies) {
        NapiWebHttpCookie napiCookie;
        napiCookie.samesitePolicy = cookie->GetSamesitePolicy();
        napiCookie.expiresDate = cookie->GetExpiresDate();
        napiCookie.name = cookie->GetName();
        napiCookie.isSessionCookie = cookie->GetIsSessionCookie();
        napiCookie.value = cookie->GetValue();
        napiCookie.path = cookie->GetPath();
        napiCookie.isHttpOnly = cookie->GetIsHttpOnly();
        napiCookie.isSecure = cookie->GetIsSecure();
        napiCookie.domain = cookie->GetDomain();
        param->cookies.push_back(napiCookie);
    }
    param->status = param->cookies.empty() ? napi_generic_failure : napi_ok;
}
 
void NapiWebCookieManager::GetNapiWebHttpCookieForResult(napi_env env,
    const std::vector<NapiWebHttpCookie> &cookies, napi_value result)
{
    int32_t index = 0;
    for (auto& cookie : cookies) {
        napi_value napiWebHttpCookie = nullptr;
        napi_create_object(env, &napiWebHttpCookie);
 
        napi_value samesitePolicy = nullptr;
        napi_create_int32(env, static_cast<uint32_t>(cookie.samesitePolicy), &samesitePolicy);
        napi_set_named_property(env, napiWebHttpCookie, "samesitePolicy", samesitePolicy);
 
        napi_value expiresDate = nullptr;
        napi_create_string_utf8(env, cookie.expiresDate.c_str(), NAPI_AUTO_LENGTH, &expiresDate);
        napi_set_named_property(env, napiWebHttpCookie, "expiresDate", expiresDate);
 
        napi_value name = nullptr;
        napi_create_string_utf8(env, cookie.name.c_str(), NAPI_AUTO_LENGTH, &name);
        napi_set_named_property(env, napiWebHttpCookie, "name", name);
 
        napi_value isSessionCookie = nullptr;
        napi_get_boolean(env, static_cast<bool>(cookie.isSessionCookie), &isSessionCookie);
        napi_set_named_property(env, napiWebHttpCookie, "isSessionCookie", isSessionCookie);
 
        napi_value value = nullptr;
        napi_create_string_utf8(env, cookie.value.c_str(), NAPI_AUTO_LENGTH, &value);
        napi_set_named_property(env, napiWebHttpCookie, "value", value);
 
        napi_value path = nullptr;
        napi_create_string_utf8(env, cookie.path.c_str(), NAPI_AUTO_LENGTH, &path);
        napi_set_named_property(env, napiWebHttpCookie, "path", path);
 
        napi_value isHttpOnly = nullptr;
        napi_get_boolean(env, static_cast<bool>(cookie.isHttpOnly), &isHttpOnly);
        napi_set_named_property(env, napiWebHttpCookie, "isHttpOnly", isHttpOnly);
 
        napi_value isSecure = nullptr;
        napi_get_boolean(env, static_cast<bool>(cookie.isSecure), &isSecure);
        napi_set_named_property(env, napiWebHttpCookie, "isSecure", isSecure);
 
        napi_value domain = nullptr;
        napi_create_string_utf8(env, cookie.domain.c_str(), NAPI_AUTO_LENGTH, &domain);
        napi_set_named_property(env, napiWebHttpCookie, "domain", domain);
 
        napi_set_element(env, result, index, napiWebHttpCookie);
        index++;
    }
}
 
void NapiWebCookieManager::GetCookiesPromiseComplete(napi_env env, napi_status status, void *data)
{
    GetCookiesParam* param = static_cast<GetCookiesParam*>(data);
    NApiScope scope(env);
    if (!scope.IsVaild()) {
        return;
    }

    napi_value setResult = nullptr;
    napi_create_array(env, &setResult);
    GetNapiWebHttpCookieForResult(env, param->cookies, setResult);
    if (param->status == napi_ok) {
        napi_resolve_deferred(env, param->deferred, setResult);
    }
    napi_delete_async_work(env, param->asyncWork);
    delete param;
}
 
napi_value NapiWebCookieManager::JsFetchAllCookies(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        return nullptr;
    }
 
    bool incognitoMode = false;
    if (!GetBooleanPara(env, argv[INTEGER_ZERO], incognitoMode)) {
        return nullptr;
    }
 
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);
 
    GetCookiesParam *param = new (std::nothrow) GetCookiesParam {
        .env = env,
        .asyncWork = nullptr,
        .deferred = deferred,
        .incognitoMode = incognitoMode,
    };
    if (param == nullptr) {
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, ExecuteGetCookies,
        GetCookiesPromiseComplete, static_cast<void *>(param), &param->asyncWork));
    NAPI_CALL(env, napi_queue_async_work_with_qos(env, param->asyncWork, napi_qos_user_initiated));
    return promise;
}

void NWebFetchCookieCallbackImpl::UvJsCallbackThreadWoker(uv_work_t *work, int status)
{
    if (work == nullptr) {
        WVLOG_E("NWebFetchCookieCallbackImpl uv work is null");
        return;
    }
    NapiWebCookieManager::WebFetchCookieManagerParam *data =
        reinterpret_cast<NapiWebCookieManager::WebFetchCookieManagerParam*>(work->data);
    if (data == nullptr) {
        WVLOG_E("NWebFetchCookieCallbackImpl WebFetchCookieManagerParam is null");
        delete work;
        work = nullptr;
        return;
    }
    NApiScope scope(data->env_);
    if (!scope.IsVaild()) {
        return;
    }

    napi_value result[INTEGER_TWO] = { 0 };
    if (data->callback_) {
        napi_get_null(data->env_, &result[INTEGER_ONE]);
        if (data->result_.c_str() == std::to_string(NWebError::INVALID_URL)) {
            result[INTEGER_ZERO] = NWebError::BusinessError::CreateError(data->env_, NWebError::INVALID_URL);
        } else {
            napi_get_undefined(data->env_, &result[INTEGER_ZERO]);
            napi_create_string_utf8(data->env_, data->result_.c_str(), NAPI_AUTO_LENGTH, &result[INTEGER_ONE]);
        }

        napi_value onGetCookieFunc = nullptr;
        napi_get_reference_value(data->env_, data->callback_, &onGetCookieFunc);

        napi_value args[INTEGER_TWO] = {result[INTEGER_ZERO], result[INTEGER_ONE]};
        napi_value callbackResult = nullptr;
        napi_call_function(data->env_, nullptr, onGetCookieFunc, INTEGER_TWO, &args[INTEGER_ZERO], &callbackResult);
        napi_delete_reference(data->env_, data->callback_);
    } else if (data->deferred_) {
        result[INTEGER_ZERO] = NWebError::BusinessError::CreateError(data->env_, NWebError::INVALID_URL);
        napi_create_string_utf8(data->env_, data->result_.c_str(), NAPI_AUTO_LENGTH, &result[INTEGER_ONE]);
        napi_value args[INTEGER_TWO] = {result[INTEGER_ZERO], result[INTEGER_ONE]};
        if (data->result_.c_str() == std::to_string(NWebError::INVALID_URL)) {
            napi_reject_deferred(data->env_, data->deferred_, args[INTEGER_ZERO]);
        } else {
            napi_resolve_deferred(data->env_, data->deferred_, args[INTEGER_ONE]);
        }
    }

    delete data;
    data = nullptr;
    delete work;
    work = nullptr;
}

void NWebFetchCookieCallbackImpl::OnReceiveValue(const std::string &result)
{
    WVLOG_D("NWebFetchCookieCallbackImpl received result");
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        WVLOG_E("get uv event loop failed");
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("new uv work failed");
        return;
    }
    NapiWebCookieManager::WebFetchCookieManagerParam *param =
        new (std::nothrow) NapiWebCookieManager::WebFetchCookieManagerParam();
    if (param == nullptr) {
        WVLOG_E("new WebFetchCookieManagerParam failed");
        delete work;
        return;
    }
    param->env_ = env_;
    param->callback_ = callback_;
    param->deferred_ = deferred_;
    param->result_ = result;

    work->data = reinterpret_cast<void*>(param);
    int ret = uv_queue_work_with_qos_internal(
        loop, work, [](uv_work_t* work) {}, UvJsCallbackThreadWoker, uv_qos_user_initiated,
        "WebviewNWebFetchCookieCallbackImpl");
    if (ret != 0) {
        if (param != nullptr) {
            delete param;
            param = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}

void NWebCookieCallbackImpl::UvJsCallbackThreadWoker(uv_work_t *work, int status)
{
    if (work == nullptr) {
        WVLOG_E("NWebCookieCallbackImpl uv work is null");
        return;
    }
    NapiWebCookieManager::WebCookieManagerParam *data =
        reinterpret_cast<NapiWebCookieManager::WebCookieManagerParam*>(work->data);
    if (data == nullptr) {
        WVLOG_E("NWebCookieCallbackImpl WebCookieManagerParam is null");
        delete work;
        work = nullptr;
        return;
    }
    NApiScope scope(data->env_);
    if (!scope.IsVaild()) {
        return;
    }

    if (data->callback_) {
        napi_value result[INTEGER_ONE] = {0};
        napi_get_null(data->env_, &result[INTEGER_ZERO]);

        napi_value onCookieFunc = nullptr;
        napi_get_reference_value(data->env_, data->callback_, &onCookieFunc);
        napi_value callbackResult = nullptr;
        napi_call_function(data->env_, nullptr, onCookieFunc,
            INTEGER_ONE, &result[INTEGER_ZERO], &callbackResult);
        napi_delete_reference(data->env_, data->callback_);
    } else if (data->deferred_) {
        napi_value jsResult = nullptr;
        napi_get_undefined(data->env_, &jsResult);
        napi_resolve_deferred(data->env_, data->deferred_, jsResult);
    }

    delete data;
    data = nullptr;
    delete work;
    work = nullptr;
}

void NWebCookieCallbackImpl::OnReceiveValue(bool result)
{
    WVLOG_D("NWebCookieCallbackImpl received result, result = %{public}d", result);
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        WVLOG_E("NWebCookieCallbackImpl get uv event loop failed");
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("NWebCookieCallbackImpl new uv work failed");
        return;
    }
    NapiWebCookieManager::WebCookieManagerParam *param =
        new (std::nothrow) NapiWebCookieManager::WebCookieManagerParam();
    if (param == nullptr) {
        WVLOG_E("NWebCookieCallbackImpl new WebCookieManagerParam failed");
        delete work;
        return;
    }
    param->env_ = env_;
    param->callback_ = callback_;
    param->deferred_ = deferred_;

    work->data = reinterpret_cast<void*>(param);
    int ret = uv_queue_work_with_qos_internal(
        loop, work, [](uv_work_t* work) {}, UvJsCallbackThreadWoker, uv_qos_user_initiated,
        "WebviewNWebCookieCallbackImpl");
    if (ret != 0) {
        if (param != nullptr) {
            delete param;
            param = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}

void NWebConfigCookieCallbackImpl::UvJsCallbackThreadWoker(uv_work_t *work, int status)
{
    if (work == nullptr) {
        WVLOG_E("NWebConfigCookieCallbackImpl uv work is null");
        return;
    }
    NapiWebCookieManager::WebConfigCookieManagerParam *data =
        reinterpret_cast<NapiWebCookieManager::WebConfigCookieManagerParam*>(work->data);
    if (data == nullptr) {
        WVLOG_E("NWebConfigCookieCallbackImpl WebConfigCookieManagerParam is null");
        delete work;
        work = nullptr;
        return;
    }
    NApiScope scope(data->env_);
    if (!scope.IsVaild()) {
        return;
    }

    if (data->callback_) {
        napi_value result[INTEGER_ONE] = {0};
        if (data->result_ != 1) {
            result[INTEGER_ZERO] = NWebError::BusinessError::CreateError(data->env_, data->result_);
        } else {
            napi_get_null(data->env_, &result[INTEGER_ZERO]);
        }

        napi_value onGetCookieFunc = nullptr;
        napi_get_reference_value(data->env_, data->callback_, &onGetCookieFunc);

        napi_value callbackResult = nullptr;
        napi_call_function(data->env_, nullptr, onGetCookieFunc,
            INTEGER_ONE, &result[INTEGER_ZERO], &callbackResult);
        napi_delete_reference(data->env_, data->callback_);
    } else if (data->deferred_) {
        napi_value result[INTEGER_ONE] = {0};
        if (data->result_ != 1) {
            result[INTEGER_ZERO] = NWebError::BusinessError::CreateError(data->env_, data->result_);
            napi_reject_deferred(data->env_, data->deferred_, result[INTEGER_ZERO]);
        } else {
            napi_get_null(data->env_, &result[INTEGER_ZERO]);
            napi_resolve_deferred(data->env_, data->deferred_, result[INTEGER_ZERO]);
        }
    }

    delete data;
    data = nullptr;
    delete work;
    work = nullptr;
}

void NWebConfigCookieCallbackImpl::OnReceiveValue(long result)
{
    WVLOG_D("NWebFetchCookieCallbackImpl received result");
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        WVLOG_E("get uv event loop failed");
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("new uv work failed");
        return;
    }
    NapiWebCookieManager::WebConfigCookieManagerParam *param =
        new (std::nothrow) NapiWebCookieManager::WebConfigCookieManagerParam();
    if (param == nullptr) {
        WVLOG_E("new WebConfigCookieManagerParam failed");
        delete work;
        return;
    }
    param->env_ = env_;
    param->callback_ = callback_;
    param->deferred_ = deferred_;
    param->result_ = result;

    work->data = reinterpret_cast<void*>(param);
    int ret = uv_queue_work_with_qos_internal(
        loop, work, [](uv_work_t* work) {}, UvJsCallbackThreadWoker, uv_qos_user_initiated,
        "WebviewNWebConfigCookieCallbackImpl");
    if (ret != 0) {
        if (param != nullptr) {
            delete param;
            param = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}

} // namespace NWeb
} // namespace OHOS

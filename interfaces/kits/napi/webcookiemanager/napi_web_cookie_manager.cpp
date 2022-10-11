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
#include <vector>

#include "business_error.h"
#include "napi/native_common.h"
#include "nweb_cookie_manager.h"
#include "nweb_helper.h"
#include "nweb_save_cookie_callback.h"
#include "web_errors.h"
#include "securec.h"

namespace OHOS {
namespace NWeb {
napi_value NapiWebCookieManager::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getCookie", NapiWebCookieManager::JsGetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("setCookie", NapiWebCookieManager::JsSetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("isCookieAllowed", NapiWebCookieManager::JsIsCookieAllowed),
        DECLARE_NAPI_STATIC_FUNCTION("putAcceptCookieEnabled", NapiWebCookieManager::JsPutAcceptCookieEnabled),
        DECLARE_NAPI_STATIC_FUNCTION("isThirdPartyCookieAllowed",
                                     NapiWebCookieManager::JsIsThirdPartyCookieAllowed),
        DECLARE_NAPI_STATIC_FUNCTION("putAcceptThirdPartyCookieEnabled",
                                     NapiWebCookieManager::JsPutAcceptThirdPartyCookieEnabled),
        DECLARE_NAPI_STATIC_FUNCTION("existCookie", NapiWebCookieManager::JsExistCookie),
        DECLARE_NAPI_STATIC_FUNCTION("deleteEntireCookie", NapiWebCookieManager::JsDeleteEntireCookie),
        DECLARE_NAPI_STATIC_FUNCTION("deleteSessionCookie", NapiWebCookieManager::JsDeleteSessionCookie),
        DECLARE_NAPI_STATIC_FUNCTION("saveCookieAsync", NapiWebCookieManager::JsSaveCookieAsync),
        DECLARE_NAPI_STATIC_FUNCTION("saveCookieSync", NapiWebCookieManager::JsSaveCookieSync),
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

    size_t argc = 2;
    napi_value argv[2] = { 0 };
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

napi_value NapiWebCookieManager::JsGetCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    napi_value argv[1] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != 1) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string url;
    if (!GetStringPara(env, argv[0], url)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_value result = nullptr;
    std::string cookieContent = "";

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieContent = cookieManager->ReturnCookie(url);
    }

    if (cookieContent == "") {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::INVALID_URL);
        return nullptr;
    }
    napi_create_string_utf8(env, cookieContent.c_str(), cookieContent.length(), &result);
    return result;
}

napi_value NapiWebCookieManager::JsSetCookie(napi_env env, napi_callback_info info)
{
    constexpr int32_t SETCOOKIE_PARA_NUM = 2;
    napi_value retValue = nullptr;
    size_t argc = SETCOOKIE_PARA_NUM;
    napi_value argv[2] = { 0, 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != SETCOOKIE_PARA_NUM) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string url;
    std::string value;
    if (!GetStringPara(env, argv[0], url)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }
    if (!GetStringPara(env, argv[1], value)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_value result = nullptr;
    int isSet = -1;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookie(url, value);
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

napi_value NapiWebCookieManager::JsIsCookieAllowed(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool accept = true;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsAcceptCookieAllowed();
    }
    NAPI_CALL(env, napi_get_boolean(env, accept, &result));
    return result;
}

napi_value NapiWebCookieManager::JsPutAcceptCookieEnabled(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    napi_value argv[1] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != 1) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    bool accept;
    if (!GetBooleanPara(env, argv[0], accept)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_value result = nullptr;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
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

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsThirdPartyCookieAllowed();
    }
    NAPI_CALL(env, napi_get_boolean(env, accept, &result));
    return result;
}

napi_value NapiWebCookieManager::JsPutAcceptThirdPartyCookieEnabled(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    napi_value argv[1] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != 1) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    bool accept;
    if (!GetBooleanPara(env, argv[0], accept)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_value result = nullptr;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptThirdPartyCookieEnabled(accept);
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebCookieManager::JsExistCookie(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool exist = true;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        exist = cookieManager->ExistCookies();
    }
    NAPI_CALL(env, napi_get_boolean(env, exist, &result));
    return result;
}

napi_value NapiWebCookieManager::JsDeleteEntireCookie(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->DeleteCookieEntirely(nullptr);
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebCookieManager::JsDeleteSessionCookie(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->DeleteSessionCookies(nullptr);
    }
    napi_get_undefined(env, &result);
    return result;
}

void SaveCookieAsyncCallback(napi_env env, napi_ref jsCallback)
{
    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        napi_value callback = nullptr;
        napi_value jsResult = nullptr;
        napi_value callbackResult = nullptr;

        napi_get_reference_value(env, jsCallback, &callback);
        napi_get_boolean(env, false, &jsResult);
        napi_call_function(env, nullptr, callback, 1, &jsResult, &callbackResult);
        napi_delete_reference(env, jsCallback);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebSaveCookieCallback>();
        if (callbackImpl) {
            callbackImpl->SetCallBack([env, jCallback = std::move(jsCallback)](bool result) {
                if (!env) {
                    return;
                }
                napi_value callback = nullptr;
                napi_get_reference_value(env, jCallback, &callback);

                napi_value jsResult = nullptr;
                napi_value callbackResult = nullptr;
                napi_get_boolean(env, result, &jsResult);
                napi_call_function(env, nullptr, callback, 1, &jsResult, &callbackResult);
                napi_delete_reference(env, jCallback);
            });
        } else {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        }
        cookieManager->Store(callbackImpl);
    }
}

void SaveCookieAsyncPromise(napi_env env, napi_deferred deferred)
{
    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        napi_value jsResult = nullptr;
        napi_get_boolean(env, false, &jsResult);
        napi_reject_deferred(env, deferred, jsResult);
    } else {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebSaveCookieCallback>();
        if (callbackImpl) {
            callbackImpl->SetCallBack([env, deferred](bool result) {
                if (!env) {
                    return;
                }
                napi_value jsResult = nullptr;
                napi_get_boolean(env, result, &jsResult);
                napi_resolve_deferred(env, deferred, jsResult);
            });
        }
        cookieManager->Store(callbackImpl);
    }
}

napi_value NapiWebCookieManager::JsSaveCookieAsync(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;

    size_t argc = 1;
    size_t argcPromise = 0;
    size_t argcCallback = 1;
    napi_value argv[1] = {0};

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != argcPromise && argc != argcCallback) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
            return nullptr;
        }
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], 1, &jsCallback);

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

napi_value NapiWebCookieManager::JsSaveCookieSync(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->Store();
    }

    napi_get_undefined(env, &result);
    return result;
}
} // namespace NWeb
} // namespace OHOS

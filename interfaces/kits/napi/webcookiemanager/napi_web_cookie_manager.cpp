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

napi_value NapiWebCookieManager::JsGetCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string url;
    if (!GetStringPara(env, argv[INTEGER_ZERO], url)) {
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
    napi_value retValue = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_TWO) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string url;
    std::string value;
    if (!GetStringPara(env, argv[INTEGER_ZERO], url)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }
    if (!GetStringPara(env, argv[INTEGER_ONE], value)) {
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
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    bool accept;
    if (!GetBooleanPara(env, argv[INTEGER_ZERO], accept)) {
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
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    bool accept;
    if (!GetBooleanPara(env, argv[INTEGER_ZERO], accept)) {
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
    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
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
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(data->env_, &scope);
    if (scope == nullptr) {
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

    napi_close_handle_scope(data->env_, scope);
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
    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvJsCallbackThreadWoker);
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
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

#ifndef NWEB_NAPI_WEB_COOKIE_MANAGER_H
#define NWEB_NAPI_WEB_COOKIE_MANAGER_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nweb_value_callback.h"

namespace OHOS {
namespace NWeb {
const std::string WEB_COOKIE_MANAGER_CLASS_NAME = "WebCookieManager";

struct NapiWebHttpCookie {
    int samesitePolicy;
    std::string expiresDate;
    std::string name;
    bool isSessionCookie;
    std::string value;
    std::string path;
    bool isHttpOnly;
    bool isSecure;
    std::string domain;
};
 
struct GetCookiesParam {
    std::vector<NapiWebHttpCookie> cookies;
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_status status;
    bool incognitoMode;
    int errCode;
};

class NapiWebCookieManager {
public:
    NapiWebCookieManager() {}

    ~NapiWebCookieManager() = default;

    struct WebCookieManagerParam {
        napi_env env_;
        napi_ref callback_;
        napi_deferred deferred_;
    };

    struct WebFetchCookieManagerParam {
        napi_env env_;
        napi_ref callback_;
        napi_deferred deferred_;
        std::string result_;
    };

    struct WebConfigCookieManagerParam {
        napi_env env_;
        napi_ref callback_;
        napi_deferred deferred_;
        long result_;
    };

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static bool GetStringPara(napi_env env, napi_value argv, std::string& outValue);

    static bool GetBooleanPara(napi_env env, napi_value argv, bool& outValue);

    static napi_value JsGetCookie(napi_env env, napi_callback_info info);

    static napi_value JsSetCookie(napi_env env, napi_callback_info info);

    static napi_value JsIsCookieAllowed(napi_env env, napi_callback_info info);

    static napi_value JsPutAcceptCookieEnabled(napi_env env, napi_callback_info info);

    static napi_value JsIsThirdPartyCookieAllowed(napi_env env, napi_callback_info info);

    static napi_value JsPutAcceptThirdPartyCookieEnabled(napi_env env, napi_callback_info info);

    static napi_value JsExistCookie(napi_env env, napi_callback_info info);

    static napi_value JsDeleteEntireCookie(napi_env env, napi_callback_info info);

    static napi_value JsDeleteSessionCookie(napi_env env, napi_callback_info info);

    static napi_value JsSaveCookieAsync(napi_env env, napi_callback_info info);

    static napi_value JsSaveCookieSync(napi_env env, napi_callback_info info);

    static napi_value JsFetchCookieAsync(napi_env env, napi_callback_info info);

    static bool GetStringParaAndEmitError(napi_env env, napi_value argv,
                                          const std::string& parav, std::string& value);

    static bool GetBooleanParaAndEmitError(napi_env env, napi_value argv,
                                           const std::string& parav, bool& value);

    static napi_value JsConfigCookieAsync(napi_env env, napi_callback_info info);

    static napi_value JsClearAllCookiesAsync(napi_env env, napi_callback_info info);

    static napi_value JsClearSessionCookieAsync(napi_env env, napi_callback_info info);

    static napi_value JsDeleteAllIncongitoCookies(napi_env env, napi_callback_info info);

    static napi_value JsGetIncognitoCookie(napi_env env, napi_callback_info info);

    static napi_value JsHasIncognitoCookie(napi_env env, napi_callback_info info);

    static napi_value JsSetIncognitoCookie(napi_env env, napi_callback_info info);

    static void ExecuteGetCookies(napi_env env, void *data);
 
    static void GetNapiWebHttpCookieForResult(napi_env env,
        const std::vector<NapiWebHttpCookie> &info, napi_value result);
 
    static void GetCookiesPromiseComplete(napi_env env, napi_status status, void *data);
 
    static napi_value JsFetchAllCookies(napi_env env, napi_callback_info info);
};

class NWebSaveCookieCallbackImpl : public NWebBoolValueCallback {
public:
    NWebSaveCookieCallbackImpl(napi_env env, napi_ref callback, napi_deferred deferred)
        : env_(env), callback_(callback), deferred_(deferred) {}
    ~NWebSaveCookieCallbackImpl() = default;

    void OnReceiveValue(bool result) override;

    static void UvJsCallbackThreadWoker(uv_work_t *work, int status);
private:
    napi_env env_;
    napi_ref callback_;
    napi_deferred deferred_;
};

class NWebCookieCallbackImpl : public NWebBoolValueCallback {
public:
    NWebCookieCallbackImpl(napi_env env, napi_ref callback, napi_deferred deferred)
        : env_(env), callback_(callback), deferred_(deferred) {}
    ~NWebCookieCallbackImpl() = default;

    void OnReceiveValue(bool result) override;

    static void UvJsCallbackThreadWoker(uv_work_t *work, int status);
private:
    napi_env env_;
    napi_ref callback_;
    napi_deferred deferred_;
};

class NWebFetchCookieCallbackImpl : public NWebStringValueCallback {
public:
    NWebFetchCookieCallbackImpl(napi_env env, napi_ref callback, napi_deferred deferred)
        : env_(env), callback_(callback), deferred_(deferred) {}
    ~NWebFetchCookieCallbackImpl() = default;

    void OnReceiveValue(const std::string &result) override;

    static void UvJsCallbackThreadWoker(uv_work_t *work, int status);
private:
    napi_env env_;
    napi_ref callback_;
    napi_deferred deferred_;
};

class NWebConfigCookieCallbackImpl : public NWebLongValueCallback {
public:
    NWebConfigCookieCallbackImpl(napi_env env, napi_ref callback, napi_deferred deferred)
        : env_(env), callback_(callback), deferred_(deferred) {}
    ~NWebConfigCookieCallbackImpl() = default;

    void OnReceiveValue(long result) override;

    static void UvJsCallbackThreadWoker(uv_work_t *work, int status);
private:
    napi_env env_;
    napi_ref callback_;
    napi_deferred deferred_;
};

} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEB_COOKIE_MANAGER_H
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

class NapiWebCookieManager {
public:
    NapiWebCookieManager() {}

    ~NapiWebCookieManager() = default;

    struct WebCookieManagerParam {
        napi_env env_;
        napi_ref callback_;
        napi_deferred deferred_;
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
};

class NWebSaveCookieCallbackImpl : public OHOS::NWeb::NWebValueCallback<bool> {
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
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEB_COOKIE_MANAGER_H
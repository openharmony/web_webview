/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_COOKIE_MANAGER_WRAPPER_H_
#define ARK_WEB_COOKIE_MANAGER_WRAPPER_H_
#pragma once

#include "include/nweb_cookie_manager.h"
#include "ohos_nweb/include/ark_web_cookie_manager.h"

namespace OHOS::ArkWeb {

class ArkWebCookieManagerWrapper : public OHOS::NWeb::NWebCookieManager {
public:
    ArkWebCookieManagerWrapper(ArkWebRefPtr<ArkWebCookieManager> ark_web_cookie_manager);
    ~ArkWebCookieManagerWrapper() = default;

    /**
     * @brief Ensures all cookies currently accessible through the ReturnCookie
     *        API are written to persistent storage.
     *
     * @param true if store cookie success else return false.
     */
    bool Store() override;

    /**
     * @brief Ensures all cookies currently accessible through the ReturnCookie
     *        API are written to persistent storage.
     *
     * @param callback a callback to be executed when cookies has Stored.
     */
    void Store(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) override;

    /**
     * @brief Sets a single cookie (key-value pair) for the given URL sync.
     *
     * @param url the URL for which the cookie is to be set.
     * @param value the cookie as a string, using the format of the 'Set-Cookie'
     *        HTTP response header.
     * @param incognito_mode true if web is in the incognito mode, flase
     *        otherwise.
     *
     * @return 0 if set cookie success else return error id.
     */
    int SetCookie(const std::string& url, const std::string& value, bool incognito_mode) override;

    /**
     * @brief Sets a single cookie (key-value pair) for the given URL.
     *
     * @param url the URL for which the cookie is to be set.
     * @param value the cookie as a string, using the format of the 'Set-Cookie'
     *        HTTP response header.
     * @param callback a callback to be executed when the cookie has been set.
     */
    void SetCookie(const std::string& url, const std::string& value,
        std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) override;

    /**
     * @brief Gets whether there are stored cookies.
     *
     * @param incognito_mode true if web is in the incognito mode, flase
     *        otherwise.
     *
     * @return true if there are stored cookies else false.
     */
    bool ExistCookies(bool incognito_mode) override;

    /**
     * @brief Gets whether there are stored cookies.
     *
     * @param callback a callback to be executed when the cookie has checked.
     */
    void ExistCookies(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) override;

    /**
     * @brief Gets all the cookies for the given URL. This is sync method
     *
     * @param url the URL for which the cookies are requested.
     * @param incognito_mode true if web is in the incognito mode, flase
     *        otherwise.
     *
     * @return the cookie value for given URL.
     */
    std::string ReturnCookie(const std::string& url, bool& is_valid, bool incognito_mode) override;

    /**
     * @brief Gets all the cookies for the given URL.
     *
     * @param url the URL for which the cookies are requested.
     * @param callback a callback which is executed when the cookies have been
     *        gotten.
     */
    void ReturnCookie(const std::string& url, std::shared_ptr<OHOS::NWeb::NWebStringValueCallback> callback) override;

    /**
     * @brief Configs a single cookie (key-value pair) for the given URL.
     *
     * @param url the URL for which the cookie is to be set.
     * @param value the cookie as a string, using the format of the 'Set-Cookie'
     *        HTTP response header.
     * @param callback a callback to be executed when the cookie has been set.
     */
    void ConfigCookie(const std::string& url, const std::string& value,
        std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback) override;

    /**
     * @brief Removes all session cookies, which are cookies without an expiration
     *        date.
     *
     * @param callback a callback to be executed when all session cookies has
     *        removed.
     */
    void DeleteSessionCookies(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) override;

    /**
     * @brief Removes all cookies.
     *
     * @param callback a callback to be executed when all cookies has removed.
     * @param incognito_mode true if web is in the incognito mode, flase
     *        otherwise.
     */
    void DeleteCookieEntirely(
        std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback, bool incognito_mode) override;

    /**
     * @brief Get whether the instance can send and accept cookies.
     *
     * @return true if the instance send and accept cookies.
     */
    bool IsAcceptCookieAllowed() override;

    /**
     * @brief Sets whether the instance should send and accept cookies. By default
     *        this is set to be true and the nweb accepts cookies.
     *
     * @param accept whether the instance should send and accept cookies.
     */
    void PutAcceptCookieEnabled(bool accept) override;

    /**
     * @brief Get whether the instance allows setting cookies of third parties
     *
     * @return true if the instance allows the setting of third-party cookies.
     */
    bool IsThirdPartyCookieAllowed() override;

    /**
     * @brief Get whether instances can send and accept cookies for file scheme
     *        URLs.
     *
     * @return true if instances send and accept cookies for file scheme URLs.
     */
    bool IsFileURLSchemeCookiesAllowed() override;

    /**
     * @brief Set whether the instance allows setting cookies of third parties. By
     *        default, this value is set to be false. Nweb does not allow the
     *        setting of third-party cookies.
     *
     * @param accept whether the instance allows the setting of third-party
     *        cookies.
     */
    void PutAcceptThirdPartyCookieEnabled(bool accept) override;

    /**
     * @brief Sets whether the instance should send and accept cookies for file
     *        scheme URLs.
     *
     * @param allow whether the instance should send and accept cookies for file
     *        scheme URLs.
     */
    void PutAcceptFileURLSchemeCookiesEnabled(bool allow) override;

    /**
     * @brief Gets all the cookies for the given URL. This is sync method
     *
     * @param url URL to which the cookie to be obtained belongs. A complete URL is recommended.
     * @param incognito True indicates that the memory cookies of the webview in privacy mode are obtained,
     *                  and false indicates that cookies in non-privacy mode are obtained.
     * @param includeHttpOnly If true HTTP-only cookies will also be included in the cookieValue.
     * @param cookieValue Get the cookie value corresponding to the URL.
     * @return the cookie value for given URL.
     */
    std::string ReturnCookieWithHttpOnly(
        const std::string& url, bool& is_valid, bool incognito_mode, bool includeHttpOnly) override;

    /**
     * @brief Sets a single cookie (key-value pair) for the given URL sync.
     *
     * @param url Specifies the URL to which the cookie belongs. A complete URL is recommended.
     * @param cookieValue The value of the cookie to be set.
     * @param incognito True indicates that cookies of the corresponding URL are set in privacy mode,
     *                  and false indicates that cookies of the corresponding URL are set in non-privacy mode.
     * @param includeHttpOnly If true, HTTP-only cookies can also be overwritten.
     * @return 0 if set cookie success else return error id.
     */
    int SetCookieWithHttpOnly(
        const std::string& url, const std::string& value, bool incognito_mode, bool includeHttpOnly) override;

    void GetCookieAsync(const std::string& url, bool incognitoMode,
        std::shared_ptr<OHOS::NWeb::NWebStringValueCallback> callback) override;

    int SetCookieSync(
        const std::string& url, const std::string& value, bool incognitoMode, bool includeHttpOnly) override;

    void SetCookieAsync(const std::string& url, const std::string& value, bool incognitoMode, bool includeHttpOnly,
        std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback) override;

    std::vector<std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies>> GetAllCookieAsync(bool incognitoMode) override;

private:
    ArkWebRefPtr<ArkWebCookieManager> ark_web_cookie_manager_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_COOKIE_MANAGER_WRAPPER_H_

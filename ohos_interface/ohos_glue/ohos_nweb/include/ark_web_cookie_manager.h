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

#ifndef ARK_WEB_COOKIE_MANAGER_H_
#define ARK_WEB_COOKIE_MANAGER_H_
#pragma once

#include "ohos_nweb/include/ark_web_bool_value_callback.h"
#include "ohos_nweb/include/ark_web_long_value_callback.h"
#include "ohos_nweb/include/ark_web_string_value_callback.h"
#include "ohos_nweb/include/ark_web_cookie_vector.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebCookieManager : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Ensures all cookies currently accessible through the ReturnCookie
     *        API are written to persistent storage.
     *
     * @param true if store cookie success else return false.
     */
    /*--ark web()--*/
    virtual bool Store() = 0;

    /**
     * @brief Ensures all cookies currently accessible through the ReturnCookie
     *        API are written to persistent storage.
     *
     * @param callback a callback to be executed when cookies has Stored.
     */
    /*--ark web()--*/
    virtual void Store(ArkWebRefPtr<ArkWebBoolValueCallback> callback) = 0;

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
    /*--ark web()--*/
    virtual int SetCookie(const ArkWebString& url, const ArkWebString& value, bool incognito_mode) = 0;

    /**
     * @brief Sets a single cookie (key-value pair) for the given URL.
     *
     * @param url the URL for which the cookie is to be set.
     * @param value the cookie as a string, using the format of the 'Set-Cookie'
     *        HTTP response header.
     * @param callback a callback to be executed when the cookie has been set.
     */
    /*--ark web()--*/
    virtual void SetCookie(
        const ArkWebString& url, const ArkWebString& value, ArkWebRefPtr<ArkWebBoolValueCallback> callback) = 0;

    /**
     * @brief Gets whether there are stored cookies.
     *
     * @param incognito_mode true if web is in the incognito mode, flase
     *        otherwise.
     *
     * @return true if there are stored cookies else false.
     */
    /*--ark web()--*/
    virtual bool ExistCookies(bool incognito_mode) = 0;

    /**
     * @brief Gets whether there are stored cookies.
     *
     * @param callback a callback to be executed when the cookie has checked.
     */
    /*--ark web()--*/
    virtual void ExistCookies(ArkWebRefPtr<ArkWebBoolValueCallback> callback) = 0;

    /**
     * @brief Gets all the cookies for the given URL. This is sync method
     *
     * @param url the URL for which the cookies are requested.
     * @param incognito_mode true if web is in the incognito mode, flase
     *        otherwise.
     *
     * @return the cookie value for given URL.
     */
    /*--ark web()--*/
    virtual ArkWebString ReturnCookie(const ArkWebString& url, bool& is_valid, bool incognito_mode) = 0;

    /**
     * @brief Gets all the cookies for the given URL.
     *
     * @param url the URL for which the cookies are requested.
     * @param callback a callback which is executed when the cookies have been
     *        gotten.
     */
    /*--ark web()--*/
    virtual void ReturnCookie(const ArkWebString& url, ArkWebRefPtr<ArkWebStringValueCallback> callback) = 0;

    /**
     * @brief Configs a single cookie (key-value pair) for the given URL.
     *
     * @param url the URL for which the cookie is to be set.
     * @param value the cookie as a string, using the format of the 'Set-Cookie'
     *        HTTP response header.
     * @param callback a callback to be executed when the cookie has been set.
     */
    /*--ark web()--*/
    virtual void ConfigCookie(
        const ArkWebString& url, const ArkWebString& value, ArkWebRefPtr<ArkWebLongValueCallback> callback) = 0;

    /**
     * @brief Removes all session cookies, which are cookies without an expiration
     *        date.
     *
     * @param callback a callback to be executed when all session cookies has
     *        removed.
     */
    /*--ark web()--*/
    virtual void DeleteSessionCookies(ArkWebRefPtr<ArkWebBoolValueCallback> callback) = 0;

    /**
     * @brief Removes all cookies.
     *
     * @param callback a callback to be executed when all cookies has removed.
     * @param incognito_mode true if web is in the incognito mode, flase
     *        otherwise.
     */
    /*--ark web()--*/
    virtual void DeleteCookieEntirely(ArkWebRefPtr<ArkWebBoolValueCallback> callback, bool incognito_mode) = 0;

    /**
     * @brief Get whether the instance can send and accept cookies.
     *
     * @return true if the instance send and accept cookies.
     */
    /*--ark web()--*/
    virtual bool IsAcceptCookieAllowed() = 0;

    /**
     * @brief Sets whether the instance should send and accept cookies. By default
     *        this is set to be true and the nweb accepts cookies.
     *
     * @param accept whether the instance should send and accept cookies.
     */
    /*--ark web()--*/
    virtual void PutAcceptCookieEnabled(bool accept) = 0;

    /**
     * @brief Get whether the instance allows setting cookies of third parties
     *
     * @return true if the instance allows the setting of third-party cookies.
     */
    /*--ark web()--*/
    virtual bool IsThirdPartyCookieAllowed() = 0;

    /**
     * @brief Get whether instances can send and accept cookies for file scheme
     *        URLs.
     *
     * @return true if instances send and accept cookies for file scheme URLs.
     */
    /*--ark web()--*/
    virtual bool IsFileURLSchemeCookiesAllowed() = 0;

    /**
     * @brief Set whether the instance allows setting cookies of third parties. By
     *        default, this value is set to be false. Nweb does not allow the
     *        setting of third-party cookies.
     *
     * @param accept whether the instance allows the setting of third-party
     *        cookies.
     */
    /*--ark web()--*/
    virtual void PutAcceptThirdPartyCookieEnabled(bool accept) = 0;

    /**
     * @brief Sets whether the instance should send and accept cookies for file
     *        scheme URLs.
     *
     * @param allow whether the instance should send and accept cookies for file
     *        scheme URLs.
     */
    /*--ark web()--*/
    virtual void PutAcceptFileURLSchemeCookiesEnabled(bool allow) = 0;

    /**
     * @Description: Gets all the cookies for the given URL async.
     *
     * @Input url: the URL for which the cookies are requested.
     * @Input incognitoMode: true if web is in the incognito mode, false otherwise.
     * @Input callback: a callback which is executed when the cookies have been gotten.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual void GetCookieAsync(
        const ArkWebString& url, bool incognitoMode, ArkWebRefPtr<ArkWebStringValueCallback> callback) = 0;

    /**
     * @Description: Sets a single cookie (key-value pair) for the given URL sync.
     *
     * @Input url: the URL for which the cookie is to be set.
     * @Input value: the cookie as a string, using the format of the 'Set-Cookie' HTTP response header.
     * @Input incognitoMode: true if web is in the incognito mode, false otherwise.
     * @Input includeHttpOnly: If true, HTTP-only cookies can also be overwritten.
     * @return: 0 if set cookie success else return error id.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual int SetCookieSync(
        const ArkWebString& url, const ArkWebString& value, bool incognitoMode, bool includeHttpOnly) = 0;

    /**
     * @Description: Sets a single cookie (key-value pair) for the given URL async.
     *
     * @Input url: the URL for which the cookie is to be set.
     * @Input value: the cookie as a string, using the format of the 'Set-Cookie' HTTP response header.
     * @Input incognitoMode: true if web is in the incognito mode, false otherwise.
     * @Input includeHttpOnly: If true, HTTP-only cookies can also be overwritten.
     * @Input callback: a callback to be executed when the cookie has been set.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual void SetCookieAsync(const ArkWebString& url, const ArkWebString& value, bool incognitoMode,
        bool includeHttpOnly, ArkWebRefPtr<ArkWebLongValueCallback> callback) = 0;

    /**
     * @Description: Fetches all stored cookies asynchronously.
     *
     * @Input incognitoMode: set true to gets all cookies in incognito context, false otherwise.
     * @return: nullptr if get all cookies failed else return value.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual ArkWebCookieVector GetAllCookies(bool incognitoMode) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_COOKIE_MANAGER_H_

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

#ifndef ARK_WEB_COOKIE_MANAGER_COOKIES_WRAPPER_H_
#define ARK_WEB_COOKIE_MANAGER_COOKIES_WRAPPER_H_
#pragma once

#include "include/nweb_cookie_manager.h"
#include "ohos_nweb/include/ark_web_cookie_manager_cookies.h"

namespace OHOS::ArkWeb {

class ArkWebCookieManagerCookiesWrapper : public OHOS::NWeb::NWebCookieManagerCookies {
public:
    ArkWebCookieManagerCookiesWrapper(ArkWebRefPtr<ArkWebCookieManagerCookies> ark_web_cookie_manager_cookies);
    ~ArkWebCookieManagerCookiesWrapper() = default;

    void SetSamesitePolicy(int samesitePolicy) override;
    void SetExpiresDate(const std::string& expiresDate) override;
    void SetName(const std::string& name) override;
    void SetIsSessionCookie(bool isSessionCookie) override;
    void SetValue(const std::string& value) override;
    void SetPath(const std::string& path) override;
    void SetIsHttpOnly(bool isHttpOnly) override;
    void SetIsSecure(bool isSecure) override;
    void SetDomain(const std::string& domain) override;

    int GetSamesitePolicy() override;
    std::string GetExpiresDate() override;
    std::string GetName() override;
    bool GetIsSessionCookie() override;
    std::string GetValue() override;
    std::string GetPath() override;
    bool GetIsHttpOnly() override;
    bool GetIsSecure() override;
    std::string GetDomain() override;

private:
    ArkWebRefPtr<ArkWebCookieManagerCookies> ark_web_cookie_manager_cookies_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_COOKIE_MANAGER_COOKIES_WRAPPER_H_

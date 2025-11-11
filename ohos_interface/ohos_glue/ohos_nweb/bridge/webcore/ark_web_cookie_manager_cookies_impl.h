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

#ifndef ARK_WEB_COOKIE_MANAGER_COOKIES_IMPL_H_
#define ARK_WEB_COOKIE_MANAGER_COOKIES_IMPL_H_
#pragma once

#include "include/nweb_cookie_manager.h"
#include "ohos_nweb/include/ark_web_cookie_manager_cookies.h"

namespace OHOS::ArkWeb {

class ArkWebCookieManagerCookiesImpl : public ArkWebCookieManagerCookies {
    IMPLEMENT_REFCOUNTING(ArkWebCookieManagerCookies);

public:
    ArkWebCookieManagerCookiesImpl(std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies> nweb_cookie_manager_cookies);
    ~ArkWebCookieManagerCookiesImpl() = default;

    void SetSamesitePolicy(int samesitePolicy) override;
    void SetExpiresDate(const ArkWebString& expiresDate) override;
    void SetName(const ArkWebString& name) override;
    void SetIsSessionCookie(bool isSessionCookie) override;
    void SetValue(const ArkWebString& value) override;
    void SetPath(const ArkWebString& path) override;
    void SetIsHttpOnly(bool isHttpOnly) override;
    void SetIsSecure(bool isSecure) override;
    void SetDomain(const ArkWebString& domain) override;

    int GetSamesitePolicy() override;
    ArkWebString GetExpiresDate() override;
    ArkWebString GetName() override;
    bool GetIsSessionCookie() override;
    ArkWebString GetValue() override;
    ArkWebString GetPath() override;
    bool GetIsHttpOnly() override;
    bool GetIsSecure() override;
    ArkWebString GetDomain() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies> nweb_cookie_manager_cookies_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_COOKIE_MANAGER_COOKIES_IMPL_H_

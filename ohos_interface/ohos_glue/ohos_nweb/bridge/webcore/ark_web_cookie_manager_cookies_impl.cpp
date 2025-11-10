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
 
#include "ohos_nweb/bridge/ark_web_cookie_manager_cookies_impl.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebCookieManagerCookiesImpl::ArkWebCookieManagerCookiesImpl(
    std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies> nweb_cookie_manager_cookies)
    : nweb_cookie_manager_cookies_(nweb_cookie_manager_cookies)
{}
 
void ArkWebCookieManagerCookiesImpl::SetSamesitePolicy(int samesitePolicy)
{
    nweb_cookie_manager_cookies_->SetSamesitePolicy(samesitePolicy);
}
 
void ArkWebCookieManagerCookiesImpl::SetExpiresDate(const ArkWebString& expiresDate)
{
    nweb_cookie_manager_cookies_->SetExpiresDate(ArkWebStringStructToClass(expiresDate));
}
 
void ArkWebCookieManagerCookiesImpl::SetName(const ArkWebString& name)
{
    nweb_cookie_manager_cookies_->SetName(ArkWebStringStructToClass(name));
}
 
void ArkWebCookieManagerCookiesImpl::SetIsSessionCookie(bool isSessionCookie)
{
    nweb_cookie_manager_cookies_->SetIsSessionCookie(isSessionCookie);
}
 
void ArkWebCookieManagerCookiesImpl::SetValue(const ArkWebString& value)
{
    nweb_cookie_manager_cookies_->SetValue(ArkWebStringStructToClass(value));
}
 
void ArkWebCookieManagerCookiesImpl::SetPath(const ArkWebString& path)
{
    nweb_cookie_manager_cookies_->SetPath(ArkWebStringStructToClass(path));
}
 
void ArkWebCookieManagerCookiesImpl::SetIsHttpOnly(bool isHttpOnly)
{
    nweb_cookie_manager_cookies_->SetIsHttpOnly(isHttpOnly);
}
 
void ArkWebCookieManagerCookiesImpl::SetIsSecure(bool isSecure)
{
    nweb_cookie_manager_cookies_->SetIsSecure(isSecure);
}
 
void ArkWebCookieManagerCookiesImpl::SetDomain(const ArkWebString& domain)
{
    nweb_cookie_manager_cookies_->SetDomain(ArkWebStringStructToClass(domain));
}
 
int ArkWebCookieManagerCookiesImpl::GetSamesitePolicy()
{
    return nweb_cookie_manager_cookies_->GetSamesitePolicy();
}
 
ArkWebString ArkWebCookieManagerCookiesImpl::GetExpiresDate()
{
    return ArkWebStringClassToStruct(nweb_cookie_manager_cookies_->GetExpiresDate());
}
 
ArkWebString ArkWebCookieManagerCookiesImpl::GetName()
{
    return ArkWebStringClassToStruct(nweb_cookie_manager_cookies_->GetName());
}
 
bool ArkWebCookieManagerCookiesImpl::GetIsSessionCookie()
{
    return nweb_cookie_manager_cookies_->GetIsSessionCookie();
}
 
ArkWebString ArkWebCookieManagerCookiesImpl::GetValue()
{
    return ArkWebStringClassToStruct(nweb_cookie_manager_cookies_->GetValue());
}
 
ArkWebString ArkWebCookieManagerCookiesImpl::GetPath()
{
    return ArkWebStringClassToStruct(nweb_cookie_manager_cookies_->GetPath());
}
 
bool ArkWebCookieManagerCookiesImpl::GetIsHttpOnly()
{
    return nweb_cookie_manager_cookies_->GetIsHttpOnly();
}
 
bool ArkWebCookieManagerCookiesImpl::GetIsSecure()
{
    return nweb_cookie_manager_cookies_->GetIsSecure();
}
 
ArkWebString ArkWebCookieManagerCookiesImpl::GetDomain()
{
    return ArkWebStringClassToStruct(nweb_cookie_manager_cookies_->GetDomain());
}
 
} // namespace OHOS::ArkWeb

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
 
#include "ohos_nweb/bridge/ark_web_cookie_manager_cookies_wrapper.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebCookieManagerCookiesWrapper::ArkWebCookieManagerCookiesWrapper(
    ArkWebRefPtr<ArkWebCookieManagerCookies> ark_web_cookie_manager_cookies)
    : ark_web_cookie_manager_cookies_(ark_web_cookie_manager_cookies)
{}
 
void ArkWebCookieManagerCookiesWrapper::SetSamesitePolicy(int samesitePolicy)
{
    ark_web_cookie_manager_cookies_->SetSamesitePolicy(samesitePolicy);
}
 
void ArkWebCookieManagerCookiesWrapper::SetExpiresDate(const std::string& expiresDate)
{
    ArkWebString stExpiresDate = ArkWebStringClassToStruct(expiresDate);
 
    ark_web_cookie_manager_cookies_->SetExpiresDate(stExpiresDate);
 
    ArkWebStringStructRelease(stExpiresDate);
}
 
void ArkWebCookieManagerCookiesWrapper::SetName(const std::string& name)
{
    ArkWebString stName = ArkWebStringClassToStruct(name);
 
    ark_web_cookie_manager_cookies_->SetName(stName);
 
    ArkWebStringStructRelease(stName);
}
 
void ArkWebCookieManagerCookiesWrapper::SetIsSessionCookie(bool isSessionCookie)
{
    ark_web_cookie_manager_cookies_->SetIsSessionCookie(isSessionCookie);
}
 
void ArkWebCookieManagerCookiesWrapper::SetValue(const std::string& value)
{
    ArkWebString stValue = ArkWebStringClassToStruct(value);
 
    ark_web_cookie_manager_cookies_->SetValue(stValue);
 
    ArkWebStringStructRelease(stValue);
}
 
void ArkWebCookieManagerCookiesWrapper::SetPath(const std::string& path)
{
    ArkWebString stPath = ArkWebStringClassToStruct(path);
 
    ark_web_cookie_manager_cookies_->SetPath(stPath);
 
    ArkWebStringStructRelease(stPath);
}
 
void ArkWebCookieManagerCookiesWrapper::SetIsHttpOnly(bool isHttpOnly)
{
    ark_web_cookie_manager_cookies_->SetIsHttpOnly(isHttpOnly);
}
 
void ArkWebCookieManagerCookiesWrapper::SetIsSecure(bool isSecure)
{
    ark_web_cookie_manager_cookies_->SetIsSecure(isSecure);
}
 
void ArkWebCookieManagerCookiesWrapper::SetDomain(const std::string& domain)
{
    ArkWebString stDomain = ArkWebStringClassToStruct(domain);
 
    ark_web_cookie_manager_cookies_->SetDomain(stDomain);
 
    ArkWebStringStructRelease(stDomain);
}
 
int ArkWebCookieManagerCookiesWrapper::GetSamesitePolicy()
{
    return ark_web_cookie_manager_cookies_->GetSamesitePolicy();
}
 
std::string ArkWebCookieManagerCookiesWrapper::GetExpiresDate()
{
    ArkWebString stExpiresDate = ark_web_cookie_manager_cookies_->GetExpiresDate();
 
    std::string objExpiresDate = ArkWebStringStructToClass(stExpiresDate);
    ArkWebStringStructRelease(stExpiresDate);
    return objExpiresDate;
}
 
std::string ArkWebCookieManagerCookiesWrapper::GetName()
{
    ArkWebString stName = ark_web_cookie_manager_cookies_->GetName();
 
    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}
 
bool ArkWebCookieManagerCookiesWrapper::GetIsSessionCookie()
{
    return ark_web_cookie_manager_cookies_->GetIsSessionCookie();
}
 
std::string ArkWebCookieManagerCookiesWrapper::GetValue()
{
    ArkWebString stValue = ark_web_cookie_manager_cookies_->GetValue();
 
    std::string objValue = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return objValue;
}
 
std::string ArkWebCookieManagerCookiesWrapper::GetPath()
{
    ArkWebString stPath = ark_web_cookie_manager_cookies_->GetPath();
 
    std::string objPath = ArkWebStringStructToClass(stPath);
    ArkWebStringStructRelease(stPath);
    return objPath;
}
 
bool ArkWebCookieManagerCookiesWrapper::GetIsHttpOnly()
{
    return ark_web_cookie_manager_cookies_->GetIsHttpOnly();
}
 
bool ArkWebCookieManagerCookiesWrapper::GetIsSecure()
{
    return ark_web_cookie_manager_cookies_->GetIsSecure();
}
 
std::string ArkWebCookieManagerCookiesWrapper::GetDomain()
{
    ArkWebString stDomain = ark_web_cookie_manager_cookies_->GetDomain();
 
    std::string objDomain = ArkWebStringStructToClass(stDomain);
    ArkWebStringStructRelease(stDomain);
    return objDomain;
}
 
} // namespace OHOS::ArkWeb

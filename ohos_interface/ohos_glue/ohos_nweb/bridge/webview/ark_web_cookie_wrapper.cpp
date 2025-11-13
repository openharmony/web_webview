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
 
#include "ohos_nweb/bridge/ark_web_cookie_wrapper.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebCookieWrapper::ArkWebCookieWrapper(
    ArkWebRefPtr<ArkWebCookie> ark_web_cookie)
    : ark_web_cookie_(ark_web_cookie)
{}
 
int ArkWebCookieWrapper::GetSamesitePolicy()
{
    return ark_web_cookie_->GetSamesitePolicy();
}
 
std::string ArkWebCookieWrapper::GetExpiresDate()
{
    ArkWebString stExpiresDate = ark_web_cookie_->GetExpiresDate();
 
    std::string objExpiresDate = ArkWebStringStructToClass(stExpiresDate);
    ArkWebStringStructRelease(stExpiresDate);
    return objExpiresDate;
}
 
std::string ArkWebCookieWrapper::GetName()
{
    ArkWebString stName = ark_web_cookie_->GetName();
 
    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}
 
bool ArkWebCookieWrapper::GetIsSessionCookie()
{
    return ark_web_cookie_->GetIsSessionCookie();
}
 
std::string ArkWebCookieWrapper::GetValue()
{
    ArkWebString stValue = ark_web_cookie_->GetValue();
 
    std::string objValue = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return objValue;
}
 
std::string ArkWebCookieWrapper::GetPath()
{
    ArkWebString stPath = ark_web_cookie_->GetPath();
 
    std::string objPath = ArkWebStringStructToClass(stPath);
    ArkWebStringStructRelease(stPath);
    return objPath;
}
 
bool ArkWebCookieWrapper::GetIsHttpOnly()
{
    return ark_web_cookie_->GetIsHttpOnly();
}
 
bool ArkWebCookieWrapper::GetIsSecure()
{
    return ark_web_cookie_->GetIsSecure();
}
 
std::string ArkWebCookieWrapper::GetDomain()
{
    ArkWebString stDomain = ark_web_cookie_->GetDomain();
 
    std::string objDomain = ArkWebStringStructToClass(stDomain);
    ArkWebStringStructRelease(stDomain);
    return objDomain;
}
 
} // namespace OHOS::ArkWeb

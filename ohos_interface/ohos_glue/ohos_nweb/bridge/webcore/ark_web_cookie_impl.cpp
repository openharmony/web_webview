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
 
#include "ohos_nweb/bridge/ark_web_cookie_impl.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebCookieImpl::ArkWebCookieImpl(
    std::shared_ptr<OHOS::NWeb::NWebCookie> nweb_cookie)
    : nweb_cookie_(nweb_cookie)
{}
 
int ArkWebCookieImpl::GetSamesitePolicy()
{
    return nweb_cookie_->GetSamesitePolicy();
}
 
ArkWebString ArkWebCookieImpl::GetExpiresDate()
{
    return ArkWebStringClassToStruct(nweb_cookie_->GetExpiresDate());
}
 
ArkWebString ArkWebCookieImpl::GetName()
{
    return ArkWebStringClassToStruct(nweb_cookie_->GetName());
}
 
bool ArkWebCookieImpl::GetIsSessionCookie()
{
    return nweb_cookie_->GetIsSessionCookie();
}
 
ArkWebString ArkWebCookieImpl::GetValue()
{
    return ArkWebStringClassToStruct(nweb_cookie_->GetValue());
}
 
ArkWebString ArkWebCookieImpl::GetPath()
{
    return ArkWebStringClassToStruct(nweb_cookie_->GetPath());
}
 
bool ArkWebCookieImpl::GetIsHttpOnly()
{
    return nweb_cookie_->GetIsHttpOnly();
}
 
bool ArkWebCookieImpl::GetIsSecure()
{
    return nweb_cookie_->GetIsSecure();
}
 
ArkWebString ArkWebCookieImpl::GetDomain()
{
    return ArkWebStringClassToStruct(nweb_cookie_->GetDomain());
}
 
} // namespace OHOS::ArkWeb

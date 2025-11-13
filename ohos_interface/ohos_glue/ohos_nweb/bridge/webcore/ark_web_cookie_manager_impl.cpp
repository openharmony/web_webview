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

#include "ohos_nweb/bridge/ark_web_cookie_manager_impl.h"

#include "ohos_nweb/bridge/ark_web_bool_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_long_value_callback_wrapper.h"
#include "ohos_nweb/bridge/ark_web_string_value_callback_wrapper.h"
#include "ohos_nweb/cpptoc/ark_web_cookie_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCookieManagerImpl::ArkWebCookieManagerImpl(std::shared_ptr<OHOS::NWeb::NWebCookieManager> nweb_cookie_manager)
    : nweb_cookie_manager_(nweb_cookie_manager)
{}

bool ArkWebCookieManagerImpl::Store()
{
    return nweb_cookie_manager_->Store();
}

void ArkWebCookieManagerImpl::Store(ArkWebRefPtr<ArkWebBoolValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->Store(nullptr);
        return;
    }

    nweb_cookie_manager_->Store(std::make_shared<ArkWebBoolValueCallbackWrapper>(callback));
}

int ArkWebCookieManagerImpl::SetCookie(const ArkWebString& url, const ArkWebString& value, bool incognito_mode)
{
    return nweb_cookie_manager_->SetCookie(
        ArkWebStringStructToClass(url), ArkWebStringStructToClass(value), incognito_mode);
}

void ArkWebCookieManagerImpl::SetCookie(
    const ArkWebString& url, const ArkWebString& value, ArkWebRefPtr<ArkWebBoolValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->SetCookie(ArkWebStringStructToClass(url), ArkWebStringStructToClass(value), nullptr);
        return;
    }

    nweb_cookie_manager_->SetCookie(ArkWebStringStructToClass(url), ArkWebStringStructToClass(value),
        std::make_shared<ArkWebBoolValueCallbackWrapper>(callback));
}

bool ArkWebCookieManagerImpl::ExistCookies(bool incognito_mode)
{
    return nweb_cookie_manager_->ExistCookies(incognito_mode);
}

void ArkWebCookieManagerImpl::ExistCookies(ArkWebRefPtr<ArkWebBoolValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->ExistCookies(nullptr);
        return;
    }

    nweb_cookie_manager_->ExistCookies(std::make_shared<ArkWebBoolValueCallbackWrapper>(callback));
}

ArkWebString ArkWebCookieManagerImpl::ReturnCookie(const ArkWebString& url, bool& is_valid, bool incognito_mode)
{
    return ArkWebStringClassToStruct(
        nweb_cookie_manager_->ReturnCookie(ArkWebStringStructToClass(url), is_valid, incognito_mode));
}

void ArkWebCookieManagerImpl::ReturnCookie(const ArkWebString& url, ArkWebRefPtr<ArkWebStringValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->ReturnCookie(ArkWebStringStructToClass(url), nullptr);
        return;
    }

    nweb_cookie_manager_->ReturnCookie(
        ArkWebStringStructToClass(url), std::make_shared<ArkWebStringValueCallbackWrapper>(callback));
}

void ArkWebCookieManagerImpl::ConfigCookie(
    const ArkWebString& url, const ArkWebString& value, ArkWebRefPtr<ArkWebLongValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->ConfigCookie(ArkWebStringStructToClass(url), ArkWebStringStructToClass(value), nullptr);
        return;
    }

    nweb_cookie_manager_->ConfigCookie(ArkWebStringStructToClass(url), ArkWebStringStructToClass(value),
        std::make_shared<ArkWebLongValueCallbackWrapper>(callback));
}

void ArkWebCookieManagerImpl::DeleteSessionCookies(ArkWebRefPtr<ArkWebBoolValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->DeleteSessionCookies(nullptr);
        return;
    }

    nweb_cookie_manager_->DeleteSessionCookies(std::make_shared<ArkWebBoolValueCallbackWrapper>(callback));
}

void ArkWebCookieManagerImpl::DeleteCookieEntirely(ArkWebRefPtr<ArkWebBoolValueCallback> callback, bool incognito_mode)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->DeleteCookieEntirely(nullptr, incognito_mode);
        return;
    }

    nweb_cookie_manager_->DeleteCookieEntirely(
        std::make_shared<ArkWebBoolValueCallbackWrapper>(callback), incognito_mode);
}

bool ArkWebCookieManagerImpl::IsAcceptCookieAllowed()
{
    return nweb_cookie_manager_->IsAcceptCookieAllowed();
}

void ArkWebCookieManagerImpl::PutAcceptCookieEnabled(bool accept)
{
    nweb_cookie_manager_->PutAcceptCookieEnabled(accept);
}

bool ArkWebCookieManagerImpl::IsThirdPartyCookieAllowed()
{
    return nweb_cookie_manager_->IsThirdPartyCookieAllowed();
}

bool ArkWebCookieManagerImpl::IsFileURLSchemeCookiesAllowed()
{
    return nweb_cookie_manager_->IsFileURLSchemeCookiesAllowed();
}

void ArkWebCookieManagerImpl::PutAcceptThirdPartyCookieEnabled(bool accept)
{
    nweb_cookie_manager_->PutAcceptThirdPartyCookieEnabled(accept);
}

void ArkWebCookieManagerImpl::PutAcceptFileURLSchemeCookiesEnabled(bool allow)
{
    nweb_cookie_manager_->PutAcceptFileURLSchemeCookiesEnabled(allow);
}

void ArkWebCookieManagerImpl::GetCookieAsync(
    const ArkWebString& url, bool incognitoMode, ArkWebRefPtr<ArkWebStringValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->GetCookieAsync(ArkWebStringStructToClass(url), incognitoMode, nullptr);
        return;
    }

    nweb_cookie_manager_->GetCookieAsync(
        ArkWebStringStructToClass(url), incognitoMode, std::make_shared<ArkWebStringValueCallbackWrapper>(callback));
}

int ArkWebCookieManagerImpl::SetCookieSync(
    const ArkWebString& url, const ArkWebString& value, bool incognitoMode, bool includeHttpOnly)
{
    return nweb_cookie_manager_->SetCookieSync(
        ArkWebStringStructToClass(url), ArkWebStringStructToClass(value), incognitoMode, includeHttpOnly);
}

void ArkWebCookieManagerImpl::SetCookieAsync(const ArkWebString& url, const ArkWebString& value, bool incognitoMode,
    bool includeHttpOnly, ArkWebRefPtr<ArkWebLongValueCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_cookie_manager_->SetCookieAsync(
            ArkWebStringStructToClass(url), ArkWebStringStructToClass(value), incognitoMode, includeHttpOnly, nullptr);
        return;
    }

    nweb_cookie_manager_->SetCookieAsync(ArkWebStringStructToClass(url), ArkWebStringStructToClass(value),
        incognitoMode, includeHttpOnly, std::make_shared<ArkWebLongValueCallbackWrapper>(callback));
}

ArkWebCookieVector ArkWebCookieManagerImpl::GetAllCookie(bool incognitoMode)
{
    return ArkWebCookieVectorClassToStruct(nweb_cookie_manager_->GetAllCookie(incognitoMode));
}

} // namespace OHOS::ArkWeb

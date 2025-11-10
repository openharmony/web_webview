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

#include "ohos_nweb/bridge/ark_web_cookie_manager_wrapper.h"

#include "ohos_nweb/bridge/ark_web_bool_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_long_value_callback_impl.h"
#include "ohos_nweb/bridge/ark_web_string_value_callback_impl.h"
#include "ohos_nweb/ctocpp/ark_web_cookie_manager_cookies_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCookieManagerWrapper::ArkWebCookieManagerWrapper(ArkWebRefPtr<ArkWebCookieManager> ark_web_cookie_manager)
    : ark_web_cookie_manager_(ark_web_cookie_manager)
{}

bool ArkWebCookieManagerWrapper::Store()
{
    return ark_web_cookie_manager_->Store();
}

void ArkWebCookieManagerWrapper::Store(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->Store(nullptr);
        return;
    }

    ark_web_cookie_manager_->Store(new ArkWebBoolValueCallbackImpl(callback));
}

int ArkWebCookieManagerWrapper::SetCookie(const std::string& url, const std::string& value, bool incognito_mode)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stValue = ArkWebStringClassToStruct(value);

    int code = ark_web_cookie_manager_->SetCookie(stUrl, stValue, incognito_mode);

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stValue);
    return code;
}

void ArkWebCookieManagerWrapper::SetCookie(
    const std::string& url, const std::string& value, std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stValue = ArkWebStringClassToStruct(value);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->SetCookie(stUrl, stValue, nullptr);
    } else {
        ark_web_cookie_manager_->SetCookie(stUrl, stValue, new ArkWebBoolValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stValue);
}

bool ArkWebCookieManagerWrapper::ExistCookies(bool incognito_mode)
{
    return ark_web_cookie_manager_->ExistCookies(incognito_mode);
}

void ArkWebCookieManagerWrapper::ExistCookies(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->ExistCookies(nullptr);
        return;
    }

    ark_web_cookie_manager_->ExistCookies(new ArkWebBoolValueCallbackImpl(callback));
}

std::string ArkWebCookieManagerWrapper::ReturnCookie(const std::string& url, bool& is_valid, bool incognito_mode)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ArkWebString stCookie = ark_web_cookie_manager_->ReturnCookie(stUrl, is_valid, incognito_mode);

    std::string objCookie = ArkWebStringStructToClass(stCookie);
    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stCookie);
    return objCookie;
}

void ArkWebCookieManagerWrapper::ReturnCookie(
    const std::string& url, std::shared_ptr<OHOS::NWeb::NWebStringValueCallback> callback)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->ReturnCookie(stUrl, nullptr);
    } else {
        ark_web_cookie_manager_->ReturnCookie(stUrl, new ArkWebStringValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stUrl);
}

void ArkWebCookieManagerWrapper::ConfigCookie(
    const std::string& url, const std::string& value, std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stValue = ArkWebStringClassToStruct(value);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->ConfigCookie(stUrl, stValue, nullptr);
    } else {
        ark_web_cookie_manager_->ConfigCookie(stUrl, stValue, new ArkWebLongValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stValue);
}

void ArkWebCookieManagerWrapper::DeleteSessionCookies(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->DeleteSessionCookies(nullptr);
        return;
    }

    ark_web_cookie_manager_->DeleteSessionCookies(new ArkWebBoolValueCallbackImpl(callback));
}

void ArkWebCookieManagerWrapper::DeleteCookieEntirely(
    std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback, bool incognito_mode)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->DeleteCookieEntirely(nullptr, incognito_mode);
        return;
    }

    ark_web_cookie_manager_->DeleteCookieEntirely(new ArkWebBoolValueCallbackImpl(callback), incognito_mode);
}

bool ArkWebCookieManagerWrapper::IsAcceptCookieAllowed()
{
    return ark_web_cookie_manager_->IsAcceptCookieAllowed();
}

void ArkWebCookieManagerWrapper::PutAcceptCookieEnabled(bool accept)
{
    ark_web_cookie_manager_->PutAcceptCookieEnabled(accept);
}

bool ArkWebCookieManagerWrapper::IsThirdPartyCookieAllowed()
{
    return ark_web_cookie_manager_->IsThirdPartyCookieAllowed();
}

bool ArkWebCookieManagerWrapper::IsFileURLSchemeCookiesAllowed()
{
    return ark_web_cookie_manager_->IsFileURLSchemeCookiesAllowed();
}

void ArkWebCookieManagerWrapper::PutAcceptThirdPartyCookieEnabled(bool accept)
{
    ark_web_cookie_manager_->PutAcceptThirdPartyCookieEnabled(accept);
}

void ArkWebCookieManagerWrapper::PutAcceptFileURLSchemeCookiesEnabled(bool allow)
{
    ark_web_cookie_manager_->PutAcceptFileURLSchemeCookiesEnabled(allow);
}

std::string ArkWebCookieManagerWrapper::ReturnCookieWithHttpOnly(
    const std::string& url, bool& is_valid, bool incognito_mode, bool includeHttpOnly)
{
    return "";
}

int ArkWebCookieManagerWrapper::SetCookieWithHttpOnly(
    const std::string& url, const std::string& value, bool incognito_mode, bool includeHttpOnly)
{
    return 0;
}

void ArkWebCookieManagerWrapper::GetCookieAsync(
    const std::string& url, bool incognitoMode, std::shared_ptr<OHOS::NWeb::NWebStringValueCallback> callback)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->GetCookieAsync(stUrl, incognitoMode, nullptr);
    } else {
        ark_web_cookie_manager_->GetCookieAsync(stUrl, incognitoMode, new ArkWebStringValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stUrl);
}

int ArkWebCookieManagerWrapper::SetCookieSync(
    const std::string& url, const std::string& value, bool incognitoMode, bool includeHttpOnly)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stValue = ArkWebStringClassToStruct(value);

    int code = ark_web_cookie_manager_->SetCookieSync(stUrl, stValue, incognitoMode, includeHttpOnly);

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stValue);
    return code;
}

void ArkWebCookieManagerWrapper::SetCookieAsync(const std::string& url, const std::string& value, bool incognitoMode,
    bool includeHttpOnly, std::shared_ptr<OHOS::NWeb::NWebLongValueCallback> callback)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stValue = ArkWebStringClassToStruct(value);

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_cookie_manager_->SetCookieAsync(stUrl, stValue, incognitoMode, includeHttpOnly, nullptr);
    } else {
        ark_web_cookie_manager_->SetCookieAsync(
            stUrl, stValue, incognitoMode, includeHttpOnly, new ArkWebLongValueCallbackImpl(callback));
    }

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stValue);
}

std::vector<std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies>> ArkWebCookieManagerWrapper::GetAllCookieAsync(bool incognitoMode)
{
    return ArkWebCookieManagerCookiesVectorStructToClass(ark_web_cookie_manager_->GetAllCookieAsync(incognitoMode));
}

} // namespace OHOS::ArkWeb

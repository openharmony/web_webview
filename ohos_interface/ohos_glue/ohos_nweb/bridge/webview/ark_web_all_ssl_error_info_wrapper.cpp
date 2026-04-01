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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_all_ssl_error_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAllSslErrorInfoWrapper::ArkWebAllSslErrorInfoWrapper(
    ArkWebRefPtr<ArkWebAllSslErrorInfo> ark_web_all_ssl_error_info)
    : ark_web_all_ssl_error_info_(ark_web_all_ssl_error_info)
{}

OHOS::NWeb::SslError ArkWebAllSslErrorInfoWrapper::GetError()
{
    return static_cast<OHOS::NWeb::SslError>(ark_web_all_ssl_error_info_->GetError());
}

std::string ArkWebAllSslErrorInfoWrapper::GetUrl()
{
    auto stValue = ark_web_all_ssl_error_info_->GetUrl();
    auto value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}

std::string ArkWebAllSslErrorInfoWrapper::GetOriginalUrl()
{
    auto stValue = ark_web_all_ssl_error_info_->GetOriginalUrl();
    auto value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}

std::string ArkWebAllSslErrorInfoWrapper::GetReferrer()
{
    auto stValue = ark_web_all_ssl_error_info_->GetReferrer();
    auto value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}

bool ArkWebAllSslErrorInfoWrapper::GetIsFatalError()
{
    return ark_web_all_ssl_error_info_->GetIsFatalError();
}

bool ArkWebAllSslErrorInfoWrapper::GetIsMainFrame()
{
    return ark_web_all_ssl_error_info_->GetIsMainFrame();
}

std::vector<std::string> ArkWebAllSslErrorInfoWrapper::GetCertChainData()
{
    auto stValue = ark_web_all_ssl_error_info_->GetCertChainData();
    auto value = ArkWebStringVectorStructToClass(stValue);
    ArkWebStringVectorStructRelease(stValue);
    return value;
}

} // namespace OHOS::ArkWeb

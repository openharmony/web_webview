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

#include "ohos_nweb/bridge/ark_web_all_ssl_error_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAllSslErrorInfoImpl::ArkWebAllSslErrorInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebAllSslErrorInfo> nweb_all_ssl_error_info)
    : nweb_all_ssl_error_info_(nweb_all_ssl_error_info)
{}

OHOS::NWeb::SslError ArkWebAllSslErrorInfoImpl::GetError()
{
    return static_cast<int32_t>(nweb_all_ssl_error_info_->GetError());
}

std::string ArkWebAllSslErrorInfoImpl::GetUrl()
{
    return ArkWebStringClassToStruct(nweb_all_ssl_error_info_->GetUrl());
}

std::string ArkWebAllSslErrorInfoImpl::GetOriginalUrl()
{
    return ArkWebStringClassToStruct(nweb_all_ssl_error_info_->GetOriginalUrl());
}

std::string ArkWebAllSslErrorInfoImpl::GetReferrer()
{
    return ArkWebStringClassToStruct(nweb_all_ssl_error_info_->GetReferrer());
}

bool ArkWebAllSslErrorInfoImpl::GetIsFatalError()
{
    return nweb_all_ssl_error_info_->GetIsFatalError();
}

bool ArkWebAllSslErrorInfoImpl::GetIsMainFrame()
{
    return nweb_all_ssl_error_info_->GetIsMainFrame();
}

std::vector<std::string> ArkWebAllSslErrorInfoImpl::GetCertChainData()
{
    return ArkWebStringVectorClassToStruct(nweb_all_ssl_error_info_->GetCertChainData());
}

} // namespace OHOS::ArkWeb

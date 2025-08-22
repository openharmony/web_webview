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

#include "ohos_nweb/bridge/ark_web_adsblock_manager_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAdsBlockManagerWrapper::ArkWebAdsBlockManagerWrapper(ArkWebRefPtr<ArkWebAdsBlockManager> ark_web_adsblock_manager)
    : ark_web_adsblock_manager_(ark_web_adsblock_manager)
{}

void ArkWebAdsBlockManagerWrapper::SetAdsBlockRules(const std::string& rulesFile, bool replace)
{
    ArkWebString stRulesFile = ArkWebStringClassToStruct(rulesFile);

    ark_web_adsblock_manager_->SetAdsBlockRules(stRulesFile, replace);

    ArkWebStringStructRelease(stRulesFile);
}

void ArkWebAdsBlockManagerWrapper::AddAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes)
{
    ArkWebStringVector stDomainSuffixes = ArkWebStringVectorClassToStruct(domainSuffixes);

    ark_web_adsblock_manager_->AddAdsBlockDisallowedList(stDomainSuffixes);

    ArkWebStringVectorStructRelease(stDomainSuffixes);
}

void ArkWebAdsBlockManagerWrapper::RemoveAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes)
{
    ArkWebStringVector stDomainSuffixes = ArkWebStringVectorClassToStruct(domainSuffixes);

    ark_web_adsblock_manager_->RemoveAdsBlockDisallowedList(stDomainSuffixes);

    ArkWebStringVectorStructRelease(stDomainSuffixes);
}

void ArkWebAdsBlockManagerWrapper::ClearAdsBlockDisallowedList()
{
    ark_web_adsblock_manager_->ClearAdsBlockDisallowedList();
}

void ArkWebAdsBlockManagerWrapper::AddAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes)
{
    ArkWebStringVector stDomainSuffixes = ArkWebStringVectorClassToStruct(domainSuffixes);

    ark_web_adsblock_manager_->AddAdsBlockAllowedList(stDomainSuffixes);

    ArkWebStringVectorStructRelease(stDomainSuffixes);
}

void ArkWebAdsBlockManagerWrapper::RemoveAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes)
{
    ArkWebStringVector stDomainSuffixes = ArkWebStringVectorClassToStruct(domainSuffixes);

    ark_web_adsblock_manager_->RemoveAdsBlockAllowedList(stDomainSuffixes);

    ArkWebStringVectorStructRelease(stDomainSuffixes);
}

void ArkWebAdsBlockManagerWrapper::ClearAdsBlockAllowedList()
{
    ark_web_adsblock_manager_->ClearAdsBlockAllowedList();
}

} // namespace OHOS::ArkWeb

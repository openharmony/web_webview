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

#include "ohos_nweb/bridge/ark_web_adsblock_manager_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAdsBlockManagerImpl::ArkWebAdsBlockManagerImpl(
    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> nweb_adsblock_manager)
    : nweb_adsblock_manager_(nweb_adsblock_manager)
{}

void ArkWebAdsBlockManagerImpl::SetAdsBlockRules(const ArkWebString &url, bool replace)
{
  nweb_adsblock_manager_->SetAdsBlockRules(ArkWebStringStructToClass(url), replace);
}

void ArkWebAdsBlockManagerImpl::AddAdsBlockDisallowedList(const ArkWebStringVector &domainSuffixes)
{
  nweb_adsblock_manager_->AddAdsBlockDisallowedList(ArkWebStringVectorStructToClass(domainSuffixes));
}

void ArkWebAdsBlockManagerImpl::RemoveAdsBlockDisallowedList(const ArkWebStringVector &domainSuffixes)
{
  nweb_adsblock_manager_->RemoveAdsBlockDisallowedList(ArkWebStringVectorStructToClass(domainSuffixes));
}

void ArkWebAdsBlockManagerImpl::ClearAdsBlockDisallowedList()
{
  nweb_adsblock_manager_->ClearAdsBlockDisallowedList();
}

void ArkWebAdsBlockManagerImpl::AddAdsBlockAllowedList(const ArkWebStringVector &domainSuffixes)
{
  nweb_adsblock_manager_->AddAdsBlockAllowedList(ArkWebStringVectorStructToClass(domainSuffixes));
}

void ArkWebAdsBlockManagerImpl::RemoveAdsBlockAllowedList(const ArkWebStringVector &domainSuffixes)
{
  nweb_adsblock_manager_->RemoveAdsBlockAllowedList(ArkWebStringVectorStructToClass(domainSuffixes));
}

void ArkWebAdsBlockManagerImpl::ClearAdsBlockAllowedList() {
  nweb_adsblock_manager_->ClearAdsBlockAllowedList();
}

} // namespace OHOS::ArkWeb

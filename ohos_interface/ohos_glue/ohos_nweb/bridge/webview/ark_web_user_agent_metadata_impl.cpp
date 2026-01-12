/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_user_agent_metadata_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebUserAgentMetadataImpl::ArkWebUserAgentMetadataImpl(
    std::shared_ptr<OHOS::NWeb::NWebUserAgentMetadata> nweb_user_agent_metadata)
    : nweb_user_agent_metadata_(nweb_user_agent_metadata)
{}

void ArkWebUserAgentMetadataImpl::SetBrandVersionList(
    const ArkWebStringVector& brand, const ArkWebStringVector& majorVersion, const ArkWebStringVector& fullVersion)
{
    nweb_user_agent_metadata_->SetBrandVersionList(ArkWebStringVectorStructToClass(brand),
        ArkWebStringVectorStructToClass(majorVersion), ArkWebStringVectorStructToClass(fullVersion));
}

void ArkWebUserAgentMetadataImpl::SetArchitecture(const ArkWebString& arch)
{
    nweb_user_agent_metadata_->SetArchitecture(ArkWebStringStructToClass(arch));
}

void ArkWebUserAgentMetadataImpl::SetBitness(const ArkWebString& bitness)
{
    nweb_user_agent_metadata_->SetBitness(ArkWebStringStructToClass(bitness));
}

void ArkWebUserAgentMetadataImpl::SetFormFactors(const ArkWebStringVector& formFactors)
{
    nweb_user_agent_metadata_->SetFormFactors(ArkWebStringVectorStructToClass(formFactors));
}

void ArkWebUserAgentMetadataImpl::SetFullVersion(const ArkWebString& fullVersion)
{
    nweb_user_agent_metadata_->SetFullVersion(ArkWebStringStructToClass(fullVersion));
}

void ArkWebUserAgentMetadataImpl::SetMobile(const bool isMobile)
{
    nweb_user_agent_metadata_->SetMobile(isMobile);
}

void ArkWebUserAgentMetadataImpl::SetModel(const ArkWebString& model)
{
    nweb_user_agent_metadata_->SetModel(ArkWebStringStructToClass(model));
}

void ArkWebUserAgentMetadataImpl::SetPlatform(const ArkWebString& platform)
{
    nweb_user_agent_metadata_->SetPlatform(ArkWebStringStructToClass(platform));
}

void ArkWebUserAgentMetadataImpl::SetPlatformVersion(const ArkWebString& platformVersion)
{
    nweb_user_agent_metadata_->SetPlatformVersion(ArkWebStringStructToClass(platformVersion));
}

void ArkWebUserAgentMetadataImpl::SetWow64(const bool isWow64)
{
    nweb_user_agent_metadata_->SetWow64(isWow64);
}

void ArkWebUserAgentMetadataImpl::GetBrandVersionList(
    ArkWebStringVector& brand, ArkWebStringVector& majorVersion, ArkWebStringVector& fullVersion)
{
    std::vector<std::string> strBrand;
    std::vector<std::string> strMajorVersion;
    std::vector<std::string> strFullVersion;
    nweb_user_agent_metadata_->GetBrandVersionList(strBrand, strMajorVersion, strFullVersion);
    brand = ArkWebStringVectorClassToStruct(strBrand);
    majorVersion = ArkWebStringVectorClassToStruct(strMajorVersion);
    fullVersion = ArkWebStringVectorClassToStruct(strFullVersion);
}

ArkWebString ArkWebUserAgentMetadataImpl::GetArchitecture()
{
    return ArkWebStringClassToStruct(nweb_user_agent_metadata_->GetArchitecture());
}

ArkWebString ArkWebUserAgentMetadataImpl::GetBitness()
{
    return ArkWebStringClassToStruct(nweb_user_agent_metadata_->GetBitness());
}

ArkWebStringVector ArkWebUserAgentMetadataImpl::GetFormFactors()
{
    return ArkWebStringVectorClassToStruct(nweb_user_agent_metadata_->GetFormFactors());
}

ArkWebString ArkWebUserAgentMetadataImpl::GetFullVersion()
{
    return ArkWebStringClassToStruct(nweb_user_agent_metadata_->GetFullVersion());
}

bool ArkWebUserAgentMetadataImpl::GetMobile()
{
    return nweb_user_agent_metadata_->GetMobile();
}

ArkWebString ArkWebUserAgentMetadataImpl::GetModel()
{
    return ArkWebStringClassToStruct(nweb_user_agent_metadata_->GetModel());
}

ArkWebString ArkWebUserAgentMetadataImpl::GetPlatform()
{
    return ArkWebStringClassToStruct(nweb_user_agent_metadata_->GetPlatform());
}

ArkWebString ArkWebUserAgentMetadataImpl::GetPlatformVersion()
{
    return ArkWebStringClassToStruct(nweb_user_agent_metadata_->GetPlatformVersion());
}

bool ArkWebUserAgentMetadataImpl::GetWow64()
{
    return nweb_user_agent_metadata_->GetWow64();
}
} // namespace OHOS::ArkWeb

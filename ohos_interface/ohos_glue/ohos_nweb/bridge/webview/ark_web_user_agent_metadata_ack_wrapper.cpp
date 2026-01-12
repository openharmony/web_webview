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
 
#include "base/include/ark_web_types.h"
#include "ohos_nweb/bridge/ark_web_user_agent_metadata_ack_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebUserAgentMetadataAckWrapper::ArkWebUserAgentMetadataAckWrapper(
    ArkWebRefPtr<ArkWebUserAgentMetadataAck> ark_web_user_agent_metadata)
    : ark_web_user_agent_metadata_(ark_web_user_agent_metadata)
{}

void ArkWebUserAgentMetadataAckWrapper::SetBrandVersionList(const std::vector<std::string>& brand,
    const std::vector<std::string>& majorVersion, const std::vector<std::string>& fullVersion)
{
    ArkWebStringVector stBrand = ArkWebStringVectorClassToStruct(brand);
    ArkWebStringVector stMajorVersion = ArkWebStringVectorClassToStruct(majorVersion);
    ArkWebStringVector stFullVersion = ArkWebStringVectorClassToStruct(fullVersion);

    ark_web_user_agent_metadata_->SetBrandVersionList(stBrand, stMajorVersion, stFullVersion);
 
    ArkWebStringVectorStructRelease(stBrand);
    ArkWebStringVectorStructRelease(stMajorVersion);
    ArkWebStringVectorStructRelease(stFullVersion);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetArchitecture(const std::string& arch)
{
    ArkWebString stValue = ArkWebStringClassToStruct(arch);
 
    ark_web_user_agent_metadata_->SetArchitecture(stValue);
 
    ArkWebStringStructRelease(stValue);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetBitness(const std::string& bitness)
{
    ArkWebString stValue = ArkWebStringClassToStruct(bitness);
 
    ark_web_user_agent_metadata_->SetBitness(stValue);
 
    ArkWebStringStructRelease(stValue);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetFormFactors(const std::vector<std::string>& formFactors)
{
    ArkWebStringVector stValue = ArkWebStringVectorClassToStruct(formFactors);
 
    ark_web_user_agent_metadata_->SetFormFactors(stValue);
 
    ArkWebStringVectorStructRelease(stValue);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetFullVersion(const std::string& fullVersion)
{
    ArkWebString stValue = ArkWebStringClassToStruct(fullVersion);
 
    ark_web_user_agent_metadata_->SetFullVersion(stValue);
 
    ArkWebStringStructRelease(stValue);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetMobile(const bool isMobile)
{
    ark_web_user_agent_metadata_->SetMobile(isMobile);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetModel(const std::string& model)
{
    ArkWebString stValue = ArkWebStringClassToStruct(model);
 
    ark_web_user_agent_metadata_->SetModel(stValue);
 
    ArkWebStringStructRelease(stValue);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetPlatform(const std::string& platform)
{
    ArkWebString stValue = ArkWebStringClassToStruct(platform);
 
    ark_web_user_agent_metadata_->SetPlatform(stValue);
 
    ArkWebStringStructRelease(stValue);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetPlatformVersion(const std::string& platformVersion)
{
    ArkWebString stValue = ArkWebStringClassToStruct(platformVersion);
 
    ark_web_user_agent_metadata_->SetPlatformVersion(stValue);
 
    ArkWebStringStructRelease(stValue);
}
 
void ArkWebUserAgentMetadataAckWrapper::SetWow64(const bool isWow64)
{
    ark_web_user_agent_metadata_->SetWow64(isWow64);
}

void ArkWebUserAgentMetadataAckWrapper::GetBrandVersionList(
    std::vector<std::string>& brand, std::vector<std::string>& majorVersion, std::vector<std::string>& fullVersion)
{
    ArkWebStringVector stBrand;
    ArkWebStringVector stMajorVersion;
    ArkWebStringVector stFullVersion;

    ark_web_user_agent_metadata_->GetBrandVersionList(stBrand, stMajorVersion, stFullVersion);
    brand = ArkWebStringVectorStructToClass(stBrand);
    majorVersion = ArkWebStringVectorStructToClass(stMajorVersion);
    fullVersion = ArkWebStringVectorStructToClass(stFullVersion);

    ArkWebStringVectorStructRelease(stBrand);
    ArkWebStringVectorStructRelease(stMajorVersion);
    ArkWebStringVectorStructRelease(stFullVersion);
}

std::string ArkWebUserAgentMetadataAckWrapper::GetArchitecture()
{
    ArkWebString stValue = ark_web_user_agent_metadata_->GetArchitecture();
 
    std::string value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}
 
std::string ArkWebUserAgentMetadataAckWrapper::GetBitness()
{
    ArkWebString stValue = ark_web_user_agent_metadata_->GetBitness();
 
    std::string value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}
 
std::vector<std::string> ArkWebUserAgentMetadataAckWrapper::GetFormFactors()
{
    auto stValue = ark_web_user_agent_metadata_->GetFormFactors();
    auto value = ArkWebStringVectorStructToClass(stValue);
    ArkWebStringVectorStructRelease(stValue);
    return value;
}
 
std::string ArkWebUserAgentMetadataAckWrapper::GetFullVersion()
{
    ArkWebString stValue = ark_web_user_agent_metadata_->GetFullVersion();
 
    std::string value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}
 
bool ArkWebUserAgentMetadataAckWrapper::GetMobile()
{
    return ark_web_user_agent_metadata_->GetMobile();
}
 
std::string ArkWebUserAgentMetadataAckWrapper::GetModel()
{
    ArkWebString stValue = ark_web_user_agent_metadata_->GetModel();
 
    std::string value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}
 
std::string ArkWebUserAgentMetadataAckWrapper::GetPlatform()
{
    ArkWebString stValue = ark_web_user_agent_metadata_->GetPlatform();
 
    std::string value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}
 
std::string ArkWebUserAgentMetadataAckWrapper::GetPlatformVersion()
{
    ArkWebString stValue = ark_web_user_agent_metadata_->GetPlatformVersion();
 
    std::string value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}
 
bool ArkWebUserAgentMetadataAckWrapper::GetWow64()
{
    return ark_web_user_agent_metadata_->GetWow64();
}
} // namespace OHOS::ArkWeb
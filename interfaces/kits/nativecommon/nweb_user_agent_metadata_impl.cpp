/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nweb_user_agent_metadata_impl.h"

namespace OHOS::NWeb {

NWebUserAgentMetadataImpl::NWebUserAgentMetadataImpl() {}

NWebUserAgentMetadataImpl::NWebUserAgentMetadataImpl(const std::vector<std::string>& brand,
    const std::vector<std::string>& majorVersion, const std::vector<std::string>& brandFullVersion,
    const UserAgentMetadataInfo& info)
    : brand_(brand), majorVersion_(majorVersion), brandFullVersion_(brandFullVersion), arch_(info.arch),
      bitness_(info.bitness), formFactors_(info.formFactors), fullVersion_(info.fullVersion), isMobile_(info.isMobile),
      model_(info.model), platform_(info.platform), platformVersion_(info.platformVersion), isWow64_(info.isWow64)
{}

void NWebUserAgentMetadataImpl::SetBrandVersionList(const std::vector<std::string>& brand,
    const std::vector<std::string>& majorVersion, const std::vector<std::string>& brandFullVersion)
{
    if (brand.size() != majorVersion.size() || brand.size() != brandFullVersion.size() ||
        majorVersion.size() != brandFullVersion.size()) {
        return;
    }
    brand_ = brand;
    majorVersion_ = majorVersion;
    brandFullVersion_ = brandFullVersion;
}

void NWebUserAgentMetadataImpl::SetArchitecture(const std::string& arch)
{
    arch_ = arch;
}

void NWebUserAgentMetadataImpl::SetBitness(const std::string& bitness)
{
    bitness_ = bitness;
}

void NWebUserAgentMetadataImpl::SetFormFactors(const std::vector<std::string>& formFactors)
{
    formFactors_.clear();
    std::set<std::string> formFactorSet { "Automotive", "Desktop", "Mobile", "EInk", "Tablet", "Watch", "XR" };
    for (auto& it : formFactors) {
        if (formFactorSet.find(it) != formFactorSet.end()) {
            formFactors_.push_back(it);
        }
    }
}

void NWebUserAgentMetadataImpl::SetFullVersion(const std::string& fullVersion)
{
    fullVersion_ = fullVersion;
}

void NWebUserAgentMetadataImpl::SetMobile(const bool isMobile)
{
    isMobile_ = isMobile;
}

void NWebUserAgentMetadataImpl::SetModel(const std::string& model)
{
    model_ = model;
}

void NWebUserAgentMetadataImpl::SetPlatform(const std::string& platform)
{
    platform_ = platform;
}

void NWebUserAgentMetadataImpl::SetPlatformVersion(const std::string& platformVersion)
{
    platformVersion_ = platformVersion;
}

void NWebUserAgentMetadataImpl::SetWow64(const bool isWow64)
{
    isWow64_ = isWow64;
}

void NWebUserAgentMetadataImpl::GetBrandVersionList(
    std::vector<std::string>& brand, std::vector<std::string>& majorVersion, std::vector<std::string>& brandFullVersion)
{
    brand = brand_;
    majorVersion = majorVersion_;
    brandFullVersion = brandFullVersion_;
}

std::string NWebUserAgentMetadataImpl::GetArchitecture()
{
    return arch_;
}

std::string NWebUserAgentMetadataImpl::GetBitness()
{
    return bitness_;
}
std::vector<std::string> NWebUserAgentMetadataImpl::GetFormFactors()
{
    return formFactors_;
}

std::string NWebUserAgentMetadataImpl::GetFullVersion()
{
    return fullVersion_;
}

bool NWebUserAgentMetadataImpl::GetMobile()
{
    return isMobile_;
}

std::string NWebUserAgentMetadataImpl::GetModel()
{
    return model_;
}

std::string NWebUserAgentMetadataImpl::GetPlatform()
{
    return platform_;
}

std::string NWebUserAgentMetadataImpl::GetPlatformVersion()
{
    return platformVersion_;
}

bool NWebUserAgentMetadataImpl::GetWow64()
{
    return isWow64_;
}

void NWebUserAgentMetadataImpl::SetBrandVersionListPtr(
    const std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>>& brandVersionList)
{
    brand_.clear();
    majorVersion_.clear();
    brandFullVersion_.clear();
    for (auto it : brandVersionList) {
        brand_.push_back(it->GetBrand());
        majorVersion_.push_back(it->GetMajorVersion());
        brandFullVersion_.push_back(it->GetFullVersion());
    }
}

std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>> NWebUserAgentMetadataImpl::GetBrandVersionListPtr()
{
    std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>> versions;
    for (size_t i = 0; i < brand_.size(); i++) {
        auto ptrBrandVersion = std::make_shared<OHOS::NWeb::NWebUserAgentBrandVersionImpl>(
            brand_[i], majorVersion_[i], brandFullVersion_[i]);
        versions.push_back(ptrBrandVersion);
    }
    return versions;
}

} // namespace OHOS::NWeb

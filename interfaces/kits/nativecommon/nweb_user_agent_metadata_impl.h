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

#ifndef NWEB_WEB_USER_AGENT_METADATA_IMPL_H
#define NWEB_WEB_USER_AGENT_METADATA_IMPL_H

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "nweb_user_agent_brand_version_impl.h"
#include "nweb_user_agent_metadata.h"

namespace OHOS::NWeb {
typedef struct UserAgentMetadataInfo {
    std::string arch;
    std::string bitness;
    std::vector<std::string> formFactors;
    std::string fullVersion;
    bool isMobile;
    std::string model;
    std::string platform;
    std::string platformVersion;
    bool isWow64;
} UserAgentMetadataInfo;

class NWebUserAgentMetadataImpl : public NWebUserAgentMetadata {
public:
    NWebUserAgentMetadataImpl();
    NWebUserAgentMetadataImpl(const std::vector<std::string>& brand, const std::vector<std::string>& majorVersion,
        const std::vector<std::string>& brandFullVersion, const UserAgentMetadataInfo& info);
    ~NWebUserAgentMetadataImpl() = default;

    void SetBrandVersionList(const std::vector<std::string>& brand, const std::vector<std::string>& majorVersion,
        const std::vector<std::string>& brandFullVersion) override;

    void SetArchitecture(const std::string& arch) override;

    void SetBitness(const std::string& bitness) override;

    void SetFormFactors(const std::vector<std::string>& formFactors) override;

    void SetFullVersion(const std::string& fullVersion) override;

    void SetMobile(const bool isMobile) override;

    void SetModel(const std::string& model) override;

    void SetPlatform(const std::string& platform) override;

    void SetPlatformVersion(const std::string& platformVersion) override;

    void SetWow64(const bool isWow64) override;

    void GetBrandVersionList(std::vector<std::string>& brand, std::vector<std::string>& majorVersion,
        std::vector<std::string>& brandFullVersion) override;

    std::string GetArchitecture() override;

    std::string GetBitness() override;

    std::vector<std::string> GetFormFactors() override;

    std::string GetFullVersion() override;

    bool GetMobile() override;

    std::string GetModel() override;

    std::string GetPlatform() override;

    std::string GetPlatformVersion() override;

    bool GetWow64() override;

    void SetBrandVersionListPtr(const std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>>& brandVersionList);

    std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>> GetBrandVersionListPtr();

private:
    std::vector<std::string> brand_;
    std::vector<std::string> majorVersion_;
    std::vector<std::string> brandFullVersion_;
    std::string arch_;
    std::string bitness_;
    std::vector<std::string> formFactors_;
    std::string fullVersion_;
    bool isMobile_;
    std::string model_;
    std::string platform_;
    std::string platformVersion_;
    bool isWow64_;
};
} // namespace OHOS::NWeb

#endif // NWEB_WEB_USER_AGENT_METADATA_IMPL_H

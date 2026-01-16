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

#ifndef NWEB_WEB_USER_AGENT_METADATA_H
#define NWEB_WEB_USER_AGENT_METADATA_H

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebUserAgentMetadata {
public:
    NWebUserAgentMetadata() = default;
    virtual ~NWebUserAgentMetadata() = default;

    virtual void SetBrandVersionList(const std::vector<std::string>& brand,
        const std::vector<std::string>& majorVersion, const std::vector<std::string>& fullVersion) = 0;
    virtual void SetArchitecture(const std::string& arch) = 0;
    virtual void SetBitness(const std::string& bitness) = 0;
    virtual void SetFormFactors(const std::vector<std::string>& formFactors) = 0;
    virtual void SetFullVersion(const std::string& fullVersion) = 0;
    virtual void SetMobile(const bool isMobile) = 0;
    virtual void SetModel(const std::string& model) = 0;
    virtual void SetPlatform(const std::string& platform) = 0;
    virtual void SetPlatformVersion(const std::string& platformVersion) = 0;
    virtual void SetWow64(const bool isWow64) = 0;
    virtual void GetBrandVersionList(std::vector<std::string>& brand, std::vector<std::string>& majorVersion,
        std::vector<std::string>& fullVersion) = 0;
    virtual std::string GetArchitecture() = 0;
    virtual std::string GetBitness() = 0;
    virtual std::vector<std::string> GetFormFactors() = 0;
    virtual std::string GetFullVersion() = 0;
    virtual bool GetMobile() = 0;
    virtual std::string GetModel() = 0;
    virtual std::string GetPlatform() = 0;
    virtual std::string GetPlatformVersion() = 0;
    virtual bool GetWow64() = 0;
};
} // namespace OHOS::NWeb

#endif // NWEB_WEB_USER_AGENT_METADATA_H

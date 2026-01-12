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

#ifndef NWEB_WEB_USER_AGENT_BRAND_VERSION_IMPL_H
#define NWEB_WEB_USER_AGENT_BRAND_VERSION_IMPL_H

#include <memory>
#include <string>
#include <vector>

#include "nweb_user_agent_brand_version.h"

namespace OHOS::NWeb {

class NWebUserAgentBrandVersionImpl : public NWebUserAgentBrandVersion {
public:
    NWebUserAgentBrandVersionImpl();
    NWebUserAgentBrandVersionImpl(
        const std::string& brand, const std::string& majorVersion, const std::string& fullVersion);
    ~NWebUserAgentBrandVersionImpl() = default;

    void SetBrand(const std::string& brand) override;

    void SetMajorVersion(const std::string& majorVersion) override;

    void SetFullVersion(const std::string& fullVersion) override;

    std::string GetBrand() override;

    std::string GetMajorVersion() override;

    std::string GetFullVersion() override;

private:
    std::string brand_;
    std::string majorVersion_;
    std::string fullVersion_;
};
} // namespace OHOS::NWeb

#endif // NWEB_WEB_USER_AGENT_BRAND_VERSION_IMPL_H

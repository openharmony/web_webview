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

#ifndef ARK_WEB_USER_AGENT_METADATA_IMPL_H_
#define ARK_WEB_USER_AGENT_METADATA_IMPL_H_
#pragma once

#include "include/nweb_user_agent_metadata.h"
#include "ohos_nweb/include/ark_web_user_agent_metadata.h"

namespace OHOS::ArkWeb {

class ArkWebUserAgentMetadataImpl : public ArkWebUserAgentMetadata {
    IMPLEMENT_REFCOUNTING(ArkWebUserAgentMetadata);

public:
    ArkWebUserAgentMetadataImpl(std::shared_ptr<OHOS::NWeb::NWebUserAgentMetadata> nweb_user_agent_metadata);
    ~ArkWebUserAgentMetadataImpl() = default;

    void SetBrandVersionList(const ArkWebStringVector& brand, const ArkWebStringVector& majorVersion,
        const ArkWebStringVector& fullVersion) override;

    void SetArchitecture(const ArkWebString& arch) override;

    void SetBitness(const ArkWebString& bitness) override;

    void SetFormFactors(const ArkWebStringVector& formFactors) override;

    void SetFullVersion(const ArkWebString& fullVersion) override;

    void SetMobile(const bool isMobile) override;

    void SetModel(const ArkWebString& model) override;

    void SetPlatform(const ArkWebString& platform) override;

    void SetPlatformVersion(const ArkWebString& platformVersion) override;

    void SetWow64(const bool isWow64) override;

    void GetBrandVersionList(
        ArkWebStringVector& brand, ArkWebStringVector& majorVersion, ArkWebStringVector& fullVersion) override;

    ArkWebString GetArchitecture() override;

    ArkWebString GetBitness() override;

    ArkWebStringVector GetFormFactors() override;

    ArkWebString GetFullVersion() override;

    bool GetMobile() override;

    ArkWebString GetModel() override;

    ArkWebString GetPlatform() override;

    ArkWebString GetPlatformVersion() override;

    bool GetWow64() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebUserAgentMetadata> nweb_user_agent_metadata_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_USER_AGENT_METADATA_IMPL_H_

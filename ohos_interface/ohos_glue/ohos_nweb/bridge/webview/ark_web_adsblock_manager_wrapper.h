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

#ifndef ARK_WEB_ADSBLOCK_MANAGER_WRAPPER_H_
#define ARK_WEB_ADSBLOCK_MANAGER_WRAPPER_H_
#pragma once

#include "include/nweb_adsblock_manager.h"
#include "ohos_nweb/include/ark_web_adsblock_manager.h"

namespace OHOS::ArkWeb {

class ArkWebAdsBlockManagerWrapper : public OHOS::NWeb::NWebAdsBlockManager {
public:
    ArkWebAdsBlockManagerWrapper(ArkWebRefPtr<ArkWebAdsBlockManager> ark_web_adsblock_manager);
    ~ArkWebAdsBlockManagerWrapper() = default;

    /**
     * @brief Set Ads Block ruleset file, containing easylist rules.
     *
     * @param rulesFile absolute easylist file path contains app customized ads block rules.
     * @param replace replace internal rules or not;
     */
    void SetAdsBlockRules(const std::string& rulesFile, bool replace) override;

    /**
     * @brief Add items to Ads Block Disallowed list.
     *
     * @param domainSuffix list of domains suffix. if web page url matches someone in the list,
     * Ads Block will be disallowed for the web page.
     */
    void AddAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes) override;

    /**
     * @brief Remove items from Ads Block disallowed list.
     *
     * @param domainSuffix : list of domains suffix needed be removed from disallow list
     */
    void RemoveAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes) override;

    /**
     * @brief Clear Ads Block disallowed list.
     *
     */
    void ClearAdsBlockDisallowedList() override;

    /**
     * @brief Add items to Ads Block Allowed list.
     * By default, ads block is allowed for all pages unless they are added to the
     * disallow list. The priority of allowlist is higher than the disallowlist. It is
     * used to re-enable ads block on the page that matches disallow list.
     *
     * @param domainSuffix list of domains suffix, if web page url matches someone in the list,
     * Ads Block will be allowed for the web page.
     */
    void AddAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes) override;

    /**
     * @brief Remove items from Ads Block allowed list.
     *
     * @param domainSuffix : list of domains suffix needed be removed from allow list
     */
    void RemoveAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes) override;

    /**
     * @brief Clear Ads Block allow list.
     *
     */
    void ClearAdsBlockAllowedList() override;

private:
    ArkWebRefPtr<ArkWebAdsBlockManager> ark_web_adsblock_manager_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ADSBLOCK_MANAGER_WRAPPER_H_

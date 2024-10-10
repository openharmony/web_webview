/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_ADSBLOCK_MANAGER_H
#define NWEB_ADSBLOCK_MANAGER_H

#include <memory>
#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebAdsBlockManager {
public:
    NWebAdsBlockManager() = default;

    virtual ~NWebAdsBlockManager() = default;

    /**
     * @brief Set Ads Block ruleset file, containing easylist rules.
     *
     * @param rulesFile absolute easylist file path contains app customized ads block rules.
     * @param replace replace internal rules or not;
     */
    virtual void SetAdsBlockRules(const std::string& rulesFile, bool replace) = 0;

    /**
     * @brief Add items to Ads Block Disallowed list.
     *
     * @param domainSuffix list of domains suffix. if web page url matches someone in the list,
     * Ads Block will be disallowed for the web page.
     */
    virtual void AddAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes) = 0;

    /**
     * @brief Remove items from Ads Block disallowed list.
     *
     * @param domainSuffix : list of domains suffix needed be removed from disallow list
     */
    virtual void RemoveAdsBlockDisallowedList(const std::vector<std::string>& domainSuffixes) = 0;

    /**
     * @brief Clear Ads Block disallowed list.
     *
     */
    virtual void ClearAdsBlockDisallowedList() = 0;

    /**
     * @brief Add items to Ads Block Allowed list.
     * By default, ads block is allowed for all pages unless they are added to the
     * disallow list. The priority of allowlist is higher than the disallowlist. It is
     * used to re-enable ads block on the page that matches disallow list.
     *
     * @param domainSuffix list of domains suffix, if web page url matches someone in the list,
     * Ads Block will be allowed for the web page.
     */
    virtual void AddAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes) = 0;

    /**
     * @brief Remove items from Ads Block allowed list.
     *
     * @param domainSuffix : list of domains suffix needed be removed from allow list
     */
    virtual void RemoveAdsBlockAllowedList(const std::vector<std::string>& domainSuffixes) = 0;

    /**
     * @brief Clear Ads Block allow list.
     *
     */
    virtual void ClearAdsBlockAllowedList() = 0;
};

} // namespace OHOS::NWeb

#endif // NWEB_ADSBLOCK_MANAGER_H

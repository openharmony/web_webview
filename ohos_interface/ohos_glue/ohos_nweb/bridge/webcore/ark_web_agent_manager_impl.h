/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_AGENT_MANAGER_IMPL_H_
#define ARK_WEB_AGENT_MANAGER_IMPL_H_
#pragma once

#include "include/nweb_agent_manager.h"
#include "ohos_nweb/include/ark_web_agent_manager.h"

namespace OHOS::ArkWeb {

class ArkWebAgentManagerImpl : public ArkWebAgentManager {
    IMPLEMENT_REFCOUNTING(ArkWebAgentManagerImpl);

public:
    ArkWebAgentManagerImpl(std::shared_ptr<OHOS::NWeb::NWebAgentManager> nweb_agent_manager);
    ~ArkWebAgentManagerImpl() = default;

    /**
     * @brief Enable or disable agent feature.
     *
     * @param enabled true to enable agent, false to disable.
     */
    void SetAgentEnabled(bool enabled) override;

    /**
     * @brief Check if agent feature is enabled.
     *
     * @return true if agent is enabled, false otherwise.
     */
    bool IsAgentEnabled() override;

    /**
     * @brief Set the config for content change detection.
     *
     * @param min_report_time The minimum report interval.
     * @param text_content_ratio The text content change ratio.
     */
    void SetContentChangeDetectionConfig(int32_t min_report_time, float text_content_ratio) override;

    /**
     * @brief Enable or disable highlight when in agent.
     *
     * @param enabled true to enable highlight, false to disable.
     */
    void SetAgentNeedHighlight(bool enabled) override;

    /**
     * @brief Request web dom json string
     *
     * @param callback request callabck
     */
    void RequestWebDomJsonString(ArkWebRefPtr<ArkWebMessageValueCallback> callback) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebAgentManager> nweb_agent_manager_;
};

}  // namespace OHOS::ArkWeb

#endif // ARK_WEB_AGENT_MANAGER_IMPL_H_

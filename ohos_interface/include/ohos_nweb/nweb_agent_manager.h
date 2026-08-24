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

#ifndef NWEB_AGENT_MANAGER_H
#define NWEB_AGENT_MANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "nweb_export.h"
#include "nweb_value_callback.h"

namespace OHOS::NWeb {

/**
 * @brief Enumeration for DOM extraction mode.
 */
enum class NWebDomExtractionMode : int32_t {
    /** Extract the full DOM tree */
    FULL_TREE = 0,
    /** Extract only viewport-visible DOM nodes */
    VIEWPORT_ONLY = 1,
};

class OHOS_NWEB_EXPORT NWebAgentManager {
public:
    NWebAgentManager() = default;

    virtual ~NWebAgentManager() = default;

    /**
     * @brief Enable or disable agent feature.
     *
     * @param enabled true to enable agent, false to disable.
     */
    virtual void SetAgentEnabled(bool enabled) {}

    /**
     * @brief Check if agent feature is enabled.
     *
     * @return true if agent is enabled, false otherwise.
     */
    virtual bool IsAgentEnabled()
    {
        return false;
    }

    /**
     * @brief Set the config for content change detection.
     *
     * @param min_report_time The minimum report interval.
     * @param text_content_ratio The text content change ratio.
     */
    virtual void SetContentChangeDetectionConfig(int32_t min_report_time, float text_content_ratio) {}

    /**
     * @brief Set the config for content change detection.
     *
     * @param reportDomTree Whether DOM extraction is enabled.
     */
    virtual void SetDomExtractionConfig(bool reportDomTree) {}

    /**
     * @brief Enable or disable highlight when in agent.
     *
     * @param enabled true to enable highlight, false to disable.
     */
    virtual void SetAgentNeedHighlight(bool enabled) {}

    /**
     * @brief Request web dom json string
     *
     * @param callback request callabck
     */
    virtual void RequestWebDomJsonString(std::shared_ptr<NWebMessageValueCallback> callback) {}

    /**
     * @brief Request page scene query for input controls.
     *
     * @param ruleJson JSON string serving both as query config (passed to JS __psQuery())
     *                 and as observer callback routing identifier (passed to onDomReady(result, ruleJson)).
     * @param ruleId Rule identifier (observer).
     * @param nodeTypes Node type tags to observe. Empty = query only, non-empty = query + observer.
     * @param callback Result callback for immediate query result.
     */
    virtual void RequestPageSceneQuery(
        const std::string& ruleJson,
        const std::string& ruleId,
        const std::vector<std::string>& nodeTypes,
        std::shared_ptr<NWebMessageValueCallback> callback) {}

    /**
     * @brief Request web dom json string with extraction mode
     *
     * @param callback request callback
     * @param mode DOM extraction mode, 0 for full tree (default), 1 for viewport only.
     *             see NWebDomExtractionMode for details.
     */
    virtual void RequestWebDomJsonStringWithOptions(
        std::shared_ptr<NWebMessageValueCallback> callback,
        int32_t mode) {}
};
}  // namespace OHOS::NWeb

#endif // NWEB_AGENT_MANAGER_H

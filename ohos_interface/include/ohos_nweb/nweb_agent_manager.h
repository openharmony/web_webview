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

#include "nweb_export.h"
#include "nweb_value_callback.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebAgentManager {
public:
    NWebAgentManager() = default;

    virtual ~NWebAgentManager() = default;

    /**
     * @brief Enable or disable agent feature.
     *
     * @param enabled true to enable agent, false to disable.
     */
    virtual void SetAgentEnabled(bool enabled) = 0;

    /**
     * @brief Check if agent feature is enabled.
     *
     * @return true if agent is enabled, false otherwise.
     */
    virtual bool IsAgentEnabled() = 0;

    /**
     * @brief Set the config for content change detection.
     *
     * @param min_report_time The minimum report interval.
     * @param text_content_ratio The text content change ratio.
     */
    virtual void SetContentChangeDetectionConfig(int32_t min_report_time, float text_content_ratio) = 0;
};
}  // namespace OHOS::NWeb

#endif // NWEB_AGENT_MANAGER_H

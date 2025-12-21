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

#ifndef ARK_WEB_AGENT_MANAGER_H_
#define ARK_WEB_AGENT_MANAGER_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebAgentManager : public virtual ArkWebBaseRefCounted {
public:

    /**
     * @brief Enable or disable agent feature.
     *
     * @param enabled true to enable agent, false to disable.
     */
    /*--ark web()--*/
    virtual void SetAgentEnabled(bool enabled) = 0;

    /**
     * @brief Check if agent feature is enabled.
     *
     * @return true if agent is enabled, false otherwise.
     */
    /*--ark web()--*/
    virtual bool IsAgentEnabled() = 0;

    /**
     * @brief Set the config for content change detection.
     *
     * @param min_report_time The minimum report interval.
     * @param text_content_ratio The text content change ratio.
     */
    /*--ark web()--*/
    virtual void SetContentChangeDetectionConfig(int32_t min_report_time, float text_content_ratio) = 0;

    /**
     * @brief Enable or disable highlight when in agent.
     *
     * @param enabled true to enable highlight, false to disable.
     */
    /*--ark web()--*/
    virtual void SetAgentNeedHighlight(bool enabled) {}
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_AGENT_MANAGER_H_

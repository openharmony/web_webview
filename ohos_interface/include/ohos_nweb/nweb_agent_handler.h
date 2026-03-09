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

#ifndef NWEB_AGENT_HANDLER_H
#define NWEB_AGENT_HANDLER_H

#include "nweb_export.h"
#include "nweb_value_callback.h"
#include <string>

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebAgentHandler {
public:
    enum AISessionType { TRANSLATOR = 0, LANGUAGE_DETECTOR, SUMMARIZER, WRITER, REWRITER, PROMPT, PROOFREADER };

    enum AISessionResultType { SUCCESS = 0, FAILURE, RUNNING };

    NWebAgentHandler() = default;

    virtual ~NWebAgentHandler() = default;

    /**
     * @brief Report a JSON event from nweb agent.
     *
     * @param json The JSON string describing the event.
     */
    virtual void ReportEventJson(const std::string& json) {}

    /**
     * @brief Called when creating AI session.
     *
     * @param type The type enum of the AI session.
     * @param id The ID string of the AI session.
     * @param params The JSON string describing the params.
     * @param callback The callback object notifying the result.
     */
    virtual void OnCreateAISession(AISessionType type, const std::string& id, const std::string& params,
        std::shared_ptr<NWebStringVectorValueCallback> callback) {}

    /**
     * @brief Called when executing AI action.
     *
     * @param type The type enum of the AI session.
     * @param id The ID string of the AI session.
     * @param params The JSON string describing the params.
     * @param callback The callback object notifying the result.
     */
    virtual void OnExecuteAIAction(AISessionType type, const std::string& id, const std::string& params,
        std::shared_ptr<NWebStringVectorValueCallback> callback) {}

    /**
     * @brief Called when destroying AI session.
     *
     * @param type The type enum of the AI session.
     * @param id The ID string of the AI session.
     */
    virtual void OnDestroyAISession(AISessionType type, const std::string& id) {}
};

} // namespace OHOS::NWeb

#endif // NWEB_AGENT_HANDLER_H

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

#ifndef ARK_WEB_AGENT_HANDLER_H_
#define ARK_WEB_AGENT_HANDLER_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"
#include <string>
#include "ohos_nweb/include/ark_web_string_vector_value_callback.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebAgentHandler : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Report a JSON event from nweb agent.
     *
     * @param json The JSON string describing the event.
     */
    /*--ark web()--*/
    virtual void ReportEventJson(const ArkWebString& json) = 0;

    /**
     * @brief Called when creating AI session.
     *
     * @param type The type enum of the AI session.
     * @param id The ID string of the AI session.
     * @param params The JSON string describing the params.
     * @param callback The callback object notifying the result.
     */
    /*--ark web()--*/
    virtual void OnCreateAISession(int type, const ArkWebString& id, const ArkWebString& params,
        ArkWebRefPtr<ArkWebStringVectorValueCallback> callback) = 0;

    /**
     * @brief Called when executing AI action.
     *
     * @param type The type enum of the AI session.
     * @param id The ID string of the AI session.
     * @param params The JSON string describing the params.
     * @param callback The callback object notifying the result.
     */
    /*--ark web()--*/
    virtual void OnExecuteAIAction(int type, const ArkWebString& id, const ArkWebString& params,
        ArkWebRefPtr<ArkWebStringVectorValueCallback> callback) = 0;

    /**
     * @brief Called when destroying AI session.
     *
     * @param type The type enum of the AI session.
     * @param id The ID string of the AI session.
     */
    /*--ark web()--*/
    virtual void OnDestroyAISession(int type, const ArkWebString& id) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_AGENT_HANDLER_H_

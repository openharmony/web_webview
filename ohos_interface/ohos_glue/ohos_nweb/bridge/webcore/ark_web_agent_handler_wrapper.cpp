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

#include "ohos_nweb/bridge/ark_web_agent_handler_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "ohos_nweb/bridge/ark_web_string_vector_value_callback_impl.h"

namespace OHOS::ArkWeb {

ArkWebAgentHandlerWrapper::ArkWebAgentHandlerWrapper(ArkWebRefPtr<ArkWebAgentHandler> ark_web_agent_handler)
    : ark_web_agent_handler_(ark_web_agent_handler)
{}

void ArkWebAgentHandlerWrapper::ReportEventJson(const std::string& json)
{
    ArkWebString stJson = ArkWebStringClassToStruct(json);

    if (ark_web_agent_handler_) {
        ark_web_agent_handler_->ReportEventJson(stJson);
    }

    ArkWebStringStructRelease(stJson);
}

void ArkWebAgentHandlerWrapper::OnCreateAISession(AISessionType type, const std::string& id, const std::string& params,
    std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> callback)
{
    ArkWebString stId = ArkWebStringClassToStruct(id);
    ArkWebString stParams = ArkWebStringClassToStruct(params);
    ArkWebRefPtr<ArkWebStringVectorValueCallback> ark_web_callback = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_callback = new ArkWebStringVectorValueCallbackImpl(callback);
    }

    if (ark_web_agent_handler_) {
        ark_web_agent_handler_->OnCreateAISession(static_cast<int>(type), stId, stParams, ark_web_callback);
    }

    ArkWebStringStructRelease(stId);
    ArkWebStringStructRelease(stParams);
}

void ArkWebAgentHandlerWrapper::OnExecuteAIAction(AISessionType type, const std::string& id, const std::string& params,
    std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> callback)
{
    ArkWebString stId = ArkWebStringClassToStruct(id);
    ArkWebString stParams = ArkWebStringClassToStruct(params);
    ArkWebRefPtr<ArkWebStringVectorValueCallback> ark_web_callback = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_callback = new ArkWebStringVectorValueCallbackImpl(callback);
    }

    if (ark_web_agent_handler_) {
        ark_web_agent_handler_->OnExecuteAIAction(static_cast<int>(type), stId, stParams, ark_web_callback);
    }

    ArkWebStringStructRelease(stId);
    ArkWebStringStructRelease(stParams);
}

void ArkWebAgentHandlerWrapper::OnDestroyAISession(AISessionType type, const std::string& id)
{
    ArkWebString stId = ArkWebStringClassToStruct(id);

    if (ark_web_agent_handler_) {
        ark_web_agent_handler_->OnDestroyAISession(static_cast<int>(type), stId);
    }

    ArkWebStringStructRelease(stId);
}

} // namespace OHOS::ArkWeb

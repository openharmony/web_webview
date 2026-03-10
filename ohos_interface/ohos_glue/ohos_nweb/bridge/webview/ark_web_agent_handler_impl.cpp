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

#include "ohos_nweb/bridge/ark_web_agent_handler_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "ohos_nweb/bridge/ark_web_string_vector_value_callback_wrapper.h"

namespace OHOS::ArkWeb {

ArkWebAgentHandlerImpl::ArkWebAgentHandlerImpl(std::shared_ptr<OHOS::NWeb::NWebAgentHandler> nweb_agent_handler)
    : nweb_agent_handler_(nweb_agent_handler)
{}

void ArkWebAgentHandlerImpl::ReportEventJson(const ArkWebString& json)
{
    nweb_agent_handler_->ReportEventJson(ArkWebStringStructToClass(json));
}

void ArkWebAgentHandlerImpl::OnCreateAISession(int type, const ArkWebString& id, const ArkWebString& params,
    ArkWebRefPtr<ArkWebStringVectorValueCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> nweb_callback = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_callback = std::make_shared<ArkWebStringVectorValueCallbackWrapper>(callback);
    }
    nweb_agent_handler_->OnCreateAISession(
        static_cast<OHOS::NWeb::NWebAgentHandler::AISessionType>(type),
        ArkWebStringStructToClass(id),
        ArkWebStringStructToClass(params),
        nweb_callback);
}

void ArkWebAgentHandlerImpl::OnExecuteAIAction(int type, const ArkWebString& id, const ArkWebString& params,
    ArkWebRefPtr<ArkWebStringVectorValueCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> nweb_callback = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_callback = std::make_shared<ArkWebStringVectorValueCallbackWrapper>(callback);
    }
    nweb_agent_handler_->OnExecuteAIAction(
        static_cast<OHOS::NWeb::NWebAgentHandler::AISessionType>(type),
        ArkWebStringStructToClass(id),
        ArkWebStringStructToClass(params),
        nweb_callback);
}

void ArkWebAgentHandlerImpl::OnDestroyAISession(int type, const ArkWebString& id)
{
    nweb_agent_handler_->OnDestroyAISession(
        static_cast<OHOS::NWeb::NWebAgentHandler::AISessionType>(type),
        ArkWebStringStructToClass(id));
}

} // namespace OHOS::ArkWeb

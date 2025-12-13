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

namespace OHOS::ArkWeb {

ArkWebAgentHandlerWrapper::ArkWebAgentHandlerWrapper(ArkWebRefPtr<ArkWebAgentHandler> ark_web_agent_handler)
    : ark_web_agent_handler_(ark_web_agent_handler)
{}

void ArkWebAgentHandlerWrapper::ReportEventJson(const std::string& json)
{
    ArkWebString stJson = ArkWebStringClassToStruct(json);

    ark_web_agent_handler_->ReportEventJson(stJson);

    ArkWebStringStructRelease(stJson);
}

} // namespace OHOS::ArkWeb

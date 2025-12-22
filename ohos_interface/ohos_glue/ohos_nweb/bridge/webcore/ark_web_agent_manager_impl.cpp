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

#include "ohos_nweb/bridge/ark_web_agent_manager_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAgentManagerImpl::ArkWebAgentManagerImpl(
    std::shared_ptr<OHOS::NWeb::NWebAgentManager> nweb_agent_manager)
    : nweb_agent_manager_(nweb_agent_manager)
{}

void ArkWebAgentManagerImpl::SetAgentEnabled(bool enabled)
{
    if (nweb_agent_manager_) {
        nweb_agent_manager_->SetAgentEnabled(enabled);
    }
}

bool ArkWebAgentManagerImpl::IsAgentEnabled()
{
    return nweb_agent_manager_ ? nweb_agent_manager_->IsAgentEnabled() : false;
}

void ArkWebAgentManagerImpl::SetContentChangeDetectionConfig(int32_t min_report_time, float text_content_ratio)
{
    if (nweb_agent_manager_) {
        nweb_agent_manager_->SetContentChangeDetectionConfig(min_report_time, text_content_ratio);
    }
}

void ArkWebAgentManagerImpl::SetAgentNeedHighlight(bool enabled)
{
    if (nweb_agent_manager_) {
        nweb_agent_manager_->SetAgentNeedHighlight(enabled);
    }
}

}  // namespace OHOS::ArkWeb

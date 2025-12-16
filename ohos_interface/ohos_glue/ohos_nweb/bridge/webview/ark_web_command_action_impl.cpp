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

#include "ohos_nweb/bridge/ark_web_command_action_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCommandActionImpl::ArkWebCommandActionImpl(std::shared_ptr<OHOS::NWeb::NWebCommandAction> action)
    : action_(action)
{}

ArkWebString ArkWebCommandActionImpl::GetEventType()
{
    return ArkWebStringClassToStruct(action_->GetEventType());
}

ArkWebString ArkWebCommandActionImpl::GetXPath()
{
    return ArkWebStringClassToStruct(action_->GetXPath());
}

int32_t ArkWebCommandActionImpl::GetDuration()
{
    return action_->GetDuration();
}

ArkWebString ArkWebCommandActionImpl::GetAlign()
{
    return ArkWebStringClassToStruct(action_->GetAlign());
}

int32_t ArkWebCommandActionImpl::GetOffset()
{
    return action_->GetOffset();
}
}  // namespace OHOS::ArkWeb
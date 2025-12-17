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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_command_action_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCommandActionWrapper::ArkWebCommandActionWrapper(ArkWebRefPtr<ArkWebCommandAction> action)
    : action_(action)
{}

std::string ArkWebCommandActionWrapper::GetEventType()
{
    ArkWebString eventTypeArkString = action_->GetEventType();
    std::string eventTypeString = ArkWebStringStructToClass(eventTypeArkString);
    ArkWebStringStructRelease(eventTypeArkString);
    return eventTypeString;
}

std::string ArkWebCommandActionWrapper::GetXPath()
{
    ArkWebString xPathArkString = action_->GetXPath();
    std::string xPathString = ArkWebStringStructToClass(xPathArkString);
    ArkWebStringStructRelease(xPathArkString);
    return xPathString;
}

int32_t ArkWebCommandActionWrapper::GetDuration()
{
    return action_->GetDuration();
}

std::string ArkWebCommandActionWrapper::GetAlign()
{
    ArkWebString alignWebString = action_->GetAlign();
    std::string alignString = ArkWebStringStructToClass(alignWebString);
    ArkWebStringStructRelease(alignWebString);
    return alignString;
}

int32_t ArkWebCommandActionWrapper::GetOffset()
{
    return action_->GetOffset();
}
}  // namespace OHOS::ArkWeb
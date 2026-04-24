/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_command_action_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCommandActionInfoImpl::ArkWebCommandActionInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebCommandActionInfo> info)
    : info_(info)
{}

ArkWebString ArkWebCommandActionInfoImpl::GetEventType()
{
    return ArkWebStringClassToStruct(info_->GetEventType());
}

ArkWebString ArkWebCommandActionInfoImpl::GetInputValue()
{
    return ArkWebStringClassToStruct(info_->GetInputValue());
}

ArkWebString ArkWebCommandActionInfoImpl::GetXPath()
{
    return ArkWebStringClassToStruct(info_->GetXPath());
}

ArkWebStringVector ArkWebCommandActionInfoImpl::GetOptionValues()
{
    return ArkWebStringVectorClassToStruct(info_->GetOptionValues());
}

ArkWebInt32Vector ArkWebCommandActionInfoImpl::GetOptionIndexes()
{
    return ArkWebBasicVectorClassToStruct<int, ArkWebInt32Vector>(info_->GetOptionIndexes());
}

double ArkWebCommandActionInfoImpl::GetX()
{
    return info_->GetX();
}

double ArkWebCommandActionInfoImpl::GetY()
{
    return info_->GetY();
}

double ArkWebCommandActionInfoImpl::GetDistanceX()
{
    return info_->GetDistanceX();
}

double ArkWebCommandActionInfoImpl::GetDistanceY()
{
    return info_->GetDistanceY();
}

float ArkWebCommandActionInfoImpl::GetScale()
{
    return info_->GetScale();
}

int32_t ArkWebCommandActionInfoImpl::GetDuration()
{
    return info_->GetDuration();
}

int32_t ArkWebCommandActionInfoImpl::GetTapCount()
{
    return info_->GetTapCount();
}

int32_t ArkWebCommandActionInfoImpl::GetSpeed()
{
    return info_->GetSpeed();
}

}  // namespace OHOS::ArkWeb

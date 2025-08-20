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

#include "ohos_nweb/bridge/ark_web_stylus_touch_point_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebStylusTouchPointInfoWrapper::ArkWebStylusTouchPointInfoWrapper(
    ArkWebRefPtr<ArkWebStylusTouchPointInfo> ark_web_stylus_touch_point_info)
    : ark_web_stylus_touch_point_info_(ark_web_stylus_touch_point_info)
{}

int32_t ArkWebStylusTouchPointInfoWrapper::GetId()
{
    return ark_web_stylus_touch_point_info_->GetId();
}

double ArkWebStylusTouchPointInfoWrapper::GetX()
{
    return ark_web_stylus_touch_point_info_->GetX();
}

double ArkWebStylusTouchPointInfoWrapper::GetY()
{
    return ark_web_stylus_touch_point_info_->GetY();
}

float ArkWebStylusTouchPointInfoWrapper::GetForce()
{
    return ark_web_stylus_touch_point_info_->GetForce();
}

float ArkWebStylusTouchPointInfoWrapper::GetTiltX()
{
    return ark_web_stylus_touch_point_info_->GetTiltX();
}

float ArkWebStylusTouchPointInfoWrapper::GetTiltY()
{
    return ark_web_stylus_touch_point_info_->GetTiltY();
}

float ArkWebStylusTouchPointInfoWrapper::GetRollAngle()
{
    return ark_web_stylus_touch_point_info_->GetRollAngle();
}

int32_t ArkWebStylusTouchPointInfoWrapper::GetWidth()
{
    return ark_web_stylus_touch_point_info_->GetWidth();
}

int32_t ArkWebStylusTouchPointInfoWrapper::GetHeight()
{
    return ark_web_stylus_touch_point_info_->GetHeight();
}

SourceTool ArkWebStylusTouchPointInfoWrapper::GetSourceTool()
{
    return static_cast<SourceTool>(ark_web_stylus_touch_point_info_->GetSourceTool());
}

} // namespace OHOS::ArkWeb

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

#include "ohos_nweb/bridge/ark_web_stylus_touch_point_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebStylusTouchPointInfoImpl::ArkWebStylusTouchPointInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebStylusTouchPointInfo> nweb_stylus_touch_point_info)
    : nweb_stylus_touch_point_info_(nweb_stylus_touch_point_info)
{}

int32_t ArkWebStylusTouchPointInfoImpl::GetId()
{
    return nweb_stylus_touch_point_info_->GetId();
}

double ArkWebStylusTouchPointInfoImpl::GetX()
{
    return nweb_stylus_touch_point_info_->GetX();
}

double ArkWebStylusTouchPointInfoImpl::GetY()
{
    return nweb_stylus_touch_point_info_->GetY();
}

float ArkWebStylusTouchPointInfoImpl::GetForce()
{
    return nweb_stylus_touch_point_info_->GetForce();
}

float ArkWebStylusTouchPointInfoImpl::GetTiltX()
{
    return nweb_stylus_touch_point_info_->GetTiltX();
}

float ArkWebStylusTouchPointInfoImpl::GetTiltY()
{
    return nweb_stylus_touch_point_info_->GetTiltY();
}

float ArkWebStylusTouchPointInfoImpl::GetRollAngle()
{
    return nweb_stylus_touch_point_info_->GetRollAngle();
}

int32_t ArkWebStylusTouchPointInfoImpl::GetWidth()
{
    return nweb_stylus_touch_point_info_->GetWidth();
}

int32_t ArkWebStylusTouchPointInfoImpl::GetHeight()
{
    return nweb_stylus_touch_point_info_->GetHeight();
}

int32_t ArkWebStylusTouchPointInfoImpl::GetSourceTool()
{
    return static_cast<int32_t>(nweb_stylus_touch_point_info_->GetSourceTool());
}

} // namespace OHOS::ArkWeb

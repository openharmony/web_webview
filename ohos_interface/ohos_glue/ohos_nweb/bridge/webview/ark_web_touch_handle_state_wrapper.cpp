/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_touch_handle_state_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebTouchHandleStateWrapper::ArkWebTouchHandleStateWrapper(
    ArkWebRefPtr<ArkWebTouchHandleState> ark_web_touch_handle_state)
    : ark_web_touch_handle_state_(ark_web_touch_handle_state)
{}

int32_t ArkWebTouchHandleStateWrapper::GetX()
{
    return ark_web_touch_handle_state_->GetX();
}

int32_t ArkWebTouchHandleStateWrapper::GetY()
{
    return ark_web_touch_handle_state_->GetY();
}

bool ArkWebTouchHandleStateWrapper::IsEnable()
{
    return ark_web_touch_handle_state_->IsEnable();
}

float ArkWebTouchHandleStateWrapper::GetAlpha()
{
    return ark_web_touch_handle_state_->GetAlpha();
}

float ArkWebTouchHandleStateWrapper::GetEdgeHeight()
{
    return ark_web_touch_handle_state_->GetEdgeHeight();
}

int32_t ArkWebTouchHandleStateWrapper::GetViewPortX()
{
    return ark_web_touch_handle_state_->GetViewPortX();
}

int32_t ArkWebTouchHandleStateWrapper::GetViewPortY()
{
    return ark_web_touch_handle_state_->GetViewPortY();
}

int32_t ArkWebTouchHandleStateWrapper::GetTouchHandleId()
{
    return ark_web_touch_handle_state_->GetTouchHandleId();
}

ArkWebTouchHandleType ArkWebTouchHandleStateWrapper::GetTouchHandleType()
{
    return static_cast<ArkWebTouchHandleType>(ark_web_touch_handle_state_->GetTouchHandleType());
}

bool ArkWebTouchHandleStateWrapper::IsDragging()
{
    return ark_web_touch_handle_state_->IsDragging();
}

} // namespace OHOS::ArkWeb

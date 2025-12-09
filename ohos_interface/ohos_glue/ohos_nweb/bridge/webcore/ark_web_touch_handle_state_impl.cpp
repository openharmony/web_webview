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

#include "ohos_nweb/bridge/ark_web_touch_handle_state_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebTouchHandleType = OHOS::NWeb::NWebTouchHandleState::TouchHandleType;

ArkWebTouchHandleStateImpl::ArkWebTouchHandleStateImpl(
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> nweb_touch_handle_state)
    : nweb_touch_handle_state_(nweb_touch_handle_state)
{}

int32_t ArkWebTouchHandleStateImpl::GetX()
{
    return nweb_touch_handle_state_->GetX();
}

int32_t ArkWebTouchHandleStateImpl::GetY()
{
    return nweb_touch_handle_state_->GetY();
}

bool ArkWebTouchHandleStateImpl::IsEnable()
{
    return nweb_touch_handle_state_->IsEnable();
}

float ArkWebTouchHandleStateImpl::GetAlpha()
{
    return nweb_touch_handle_state_->GetAlpha();
}

float ArkWebTouchHandleStateImpl::GetEdgeHeight()
{
    return nweb_touch_handle_state_->GetEdgeHeight();
}

int32_t ArkWebTouchHandleStateImpl::GetViewPortX()
{
    return nweb_touch_handle_state_->GetViewPortX();
}

int32_t ArkWebTouchHandleStateImpl::GetViewPortY()
{
    return nweb_touch_handle_state_->GetViewPortY();
}

int32_t ArkWebTouchHandleStateImpl::GetTouchHandleId()
{
    return nweb_touch_handle_state_->GetTouchHandleId();
}

int ArkWebTouchHandleStateImpl::GetTouchHandleType()
{
    return static_cast<int>(nweb_touch_handle_state_->GetTouchHandleType());
}

bool ArkWebTouchHandleStateImpl::IsDragging()
{
    return nweb_touch_handle_state_->IsDragging();
}

} // namespace OHOS::ArkWeb

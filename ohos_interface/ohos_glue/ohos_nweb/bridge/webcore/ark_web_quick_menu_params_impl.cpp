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

#include "ohos_nweb/bridge/ark_web_quick_menu_params_impl.h"

#include "ohos_nweb/bridge/ark_web_touch_handle_state_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebTouchHandleType = OHOS::NWeb::NWebTouchHandleState::TouchHandleType;

ArkWebQuickMenuParamsImpl::ArkWebQuickMenuParamsImpl(
    std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> nweb_quick_menu_params)
    : nweb_quick_menu_params_(nweb_quick_menu_params)
{}

int32_t ArkWebQuickMenuParamsImpl::GetXCoord()
{
    return nweb_quick_menu_params_->GetXCoord();
}

int32_t ArkWebQuickMenuParamsImpl::GetYCoord()
{
    return nweb_quick_menu_params_->GetYCoord();
}

int32_t ArkWebQuickMenuParamsImpl::GetWidth()
{
    return nweb_quick_menu_params_->GetWidth();
}

int32_t ArkWebQuickMenuParamsImpl::GetHeight()
{
    return nweb_quick_menu_params_->GetHeight();
}

int32_t ArkWebQuickMenuParamsImpl::GetEditStateFlags()
{
    return nweb_quick_menu_params_->GetEditStateFlags();
}

int32_t ArkWebQuickMenuParamsImpl::GetSelectX()
{
    return nweb_quick_menu_params_->GetSelectX();
}

int32_t ArkWebQuickMenuParamsImpl::GetSelectY()
{
    return nweb_quick_menu_params_->GetSelectY();
}

int32_t ArkWebQuickMenuParamsImpl::GetSelectWidth()
{
    return nweb_quick_menu_params_->GetSelectWidth();
}

int32_t ArkWebQuickMenuParamsImpl::GetSelectXHeight()
{
    return nweb_quick_menu_params_->GetSelectXHeight();
}

ArkWebRefPtr<ArkWebTouchHandleState> ArkWebQuickMenuParamsImpl::GetTouchHandleState(int type)
{
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> nweb_touch_handle_state =
        nweb_quick_menu_params_->GetTouchHandleState(static_cast<ArkWebTouchHandleType>(type));
    if (CHECK_SHARED_PTR_IS_NULL(nweb_touch_handle_state)) {
        return nullptr;
    }

    return new ArkWebTouchHandleStateImpl(nweb_touch_handle_state);
}

bool ArkWebQuickMenuParamsImpl::GetIsLongPressActived()
{
    return nweb_quick_menu_params_->GetIsLongPressActived();
}
} // namespace OHOS::ArkWeb

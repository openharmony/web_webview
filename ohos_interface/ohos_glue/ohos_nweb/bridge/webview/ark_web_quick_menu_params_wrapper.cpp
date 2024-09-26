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

#include "ohos_nweb/bridge/ark_web_quick_menu_params_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebQuickMenuParamsWrapper::ArkWebQuickMenuParamsWrapper(
    ArkWebRefPtr<ArkWebQuickMenuParams> ark_web_quick_menu_params)
    : ark_web_quick_menu_params_(ark_web_quick_menu_params)
{}

int32_t ArkWebQuickMenuParamsWrapper::GetXCoord()
{
    return ark_web_quick_menu_params_->GetXCoord();
}

int32_t ArkWebQuickMenuParamsWrapper::GetYCoord()
{
    return ark_web_quick_menu_params_->GetYCoord();
}

int32_t ArkWebQuickMenuParamsWrapper::GetWidth()
{
    return ark_web_quick_menu_params_->GetWidth();
}

int32_t ArkWebQuickMenuParamsWrapper::GetHeight()
{
    return ark_web_quick_menu_params_->GetHeight();
}

int32_t ArkWebQuickMenuParamsWrapper::GetEditStateFlags()
{
    return ark_web_quick_menu_params_->GetEditStateFlags();
}

int32_t ArkWebQuickMenuParamsWrapper::GetSelectX()
{
    return ark_web_quick_menu_params_->GetSelectX();
}

int32_t ArkWebQuickMenuParamsWrapper::GetSelectY()
{
    return ark_web_quick_menu_params_->GetSelectY();
}

int32_t ArkWebQuickMenuParamsWrapper::GetSelectWidth()
{
    return ark_web_quick_menu_params_->GetSelectWidth();
}

int32_t ArkWebQuickMenuParamsWrapper::GetSelectXHeight()
{
    return ark_web_quick_menu_params_->GetSelectXHeight();
}

std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> ArkWebQuickMenuParamsWrapper::GetTouchHandleState(
    ArkWebTouchHandleType type)
{
    ArkWebRefPtr<ArkWebTouchHandleState> ark_web_touch_handle_state =
        ark_web_quick_menu_params_->GetTouchHandleState(static_cast<int>(type));
    if (CHECK_REF_PTR_IS_NULL(ark_web_touch_handle_state)) {
        return nullptr;
    }

    return std::make_shared<ArkWebTouchHandleStateWrapper>(ark_web_touch_handle_state);
}

bool ArkWebQuickMenuParamsWrapper::GetIsLongPressActived()
{
    return ark_web_quick_menu_params_->GetIsLongPressActived();
}
} // namespace OHOS::ArkWeb

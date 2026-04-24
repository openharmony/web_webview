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

#include "ohos_nweb/bridge/ark_web_command_action_manager_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

#include "ohos_nweb/bridge/ark_web_command_action_info_wrapper.h"

namespace OHOS::ArkWeb {

ArkWebCommandActionManagerImpl::ArkWebCommandActionManagerImpl(
    std::shared_ptr<OHOS::NWeb::NWebCommandActionManager> nweb_command_action_manager)
    : nweb_command_action_manager_(nweb_command_action_manager)
{}

int32_t ArkWebCommandActionManagerImpl::HandleInputCommand(ArkWebRefPtr<ArkWebCommandActionInfo> info)
{
    if (CHECK_REF_PTR_IS_NULL(info)) {
        return nweb_command_action_manager_->HandleInputCommand(nullptr);
    }
    return nweb_command_action_manager_->HandleInputCommand(
        std::make_shared<ArkWebCommandActionInfoWrapper>(info));
}

int32_t ArkWebCommandActionManagerImpl::HandleSelectCommand(ArkWebRefPtr<ArkWebCommandActionInfo> info)
{
    if (CHECK_REF_PTR_IS_NULL(info)) {
        return nweb_command_action_manager_->HandleSelectCommand(nullptr);
    }
    return nweb_command_action_manager_->HandleSelectCommand(
        std::make_shared<ArkWebCommandActionInfoWrapper>(info));
}

int32_t ArkWebCommandActionManagerImpl::HandleGestureCommand(ArkWebRefPtr<ArkWebCommandActionInfo> info)
{
    if (CHECK_REF_PTR_IS_NULL(info)) {
        return nweb_command_action_manager_->HandleGestureCommand(nullptr);
    }
    return nweb_command_action_manager_->HandleGestureCommand(
        std::make_shared<ArkWebCommandActionInfoWrapper>(info));
}

}  // namespace OHOS::ArkWeb

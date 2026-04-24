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

#include "ohos_nweb/bridge/ark_web_command_action_manager_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

#include "ohos_nweb/bridge/ark_web_command_action_info_impl.h"

namespace OHOS::ArkWeb {

ArkWebCommandActionManagerWrapper::ArkWebCommandActionManagerWrapper(
    ArkWebRefPtr<ArkWebCommandActionManager> ark_web_command_action_manager)
    : ark_web_command_action_manager_(ark_web_command_action_manager)
{}

int32_t ArkWebCommandActionManagerWrapper::HandleInputCommand(
    std::shared_ptr<OHOS::NWeb::NWebCommandActionInfo> info)
{
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return ark_web_command_action_manager_->HandleInputCommand(nullptr);
    }
    return ark_web_command_action_manager_->HandleInputCommand(new ArkWebCommandActionInfoImpl(info));
}

int32_t ArkWebCommandActionManagerWrapper::HandleSelectCommand(
    std::shared_ptr<OHOS::NWeb::NWebCommandActionInfo> info)
{
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return ark_web_command_action_manager_->HandleSelectCommand(nullptr);
    }
    return ark_web_command_action_manager_->HandleSelectCommand(new ArkWebCommandActionInfoImpl(info));
}

}  // namespace OHOS::ArkWeb

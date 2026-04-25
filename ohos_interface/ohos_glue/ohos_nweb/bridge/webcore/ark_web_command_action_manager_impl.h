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

#ifndef ARK_WEB_COMMAND_ACTION_MANAGER_IMPL_H_
#define ARK_WEB_COMMAND_ACTION_MANAGER_IMPL_H_
#pragma once

#include "include/nweb_command_action_manager.h"
#include "ohos_nweb/include/ark_web_command_action_manager.h"

namespace OHOS::ArkWeb {

class ArkWebCommandActionManagerImpl : public ArkWebCommandActionManager {
    IMPLEMENT_REFCOUNTING(ArkWebCommandActionManagerImpl);

public:
    ArkWebCommandActionManagerImpl(std::shared_ptr<OHOS::NWeb::NWebCommandActionManager> nweb_command_action_manager);
    ~ArkWebCommandActionManagerImpl() = default;

    /**
     * @brief Handle input command action.
     *
     * @param info The command action info for input events.
     * @return The result of command. 0 on success, negative error code on failure.
     */
    int32_t HandleInputCommand(ArkWebRefPtr<ArkWebCommandActionInfo> info) override;

    /**
     * @brief Handle select command action.
     *
     * @param info The command action info for select events.
     * @return The result of command. 0 on success, negative error code on failure.
     */
    int32_t HandleSelectCommand(ArkWebRefPtr<ArkWebCommandActionInfo> info) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebCommandActionManager> nweb_command_action_manager_;
};

}  // namespace OHOS::ArkWeb

#endif  // ARK_WEB_COMMAND_ACTION_MANAGER_IMPL_H_

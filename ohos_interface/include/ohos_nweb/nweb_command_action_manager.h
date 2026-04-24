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

#ifndef NWEB_COMMAND_ACTION_MANAGER_H
#define NWEB_COMMAND_ACTION_MANAGER_H

#include <memory>

#include "nweb_command_action_info.h"
#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebCommandActionManager {
public:
    NWebCommandActionManager() = default;
    virtual ~NWebCommandActionManager() = default;

    virtual int32_t HandleInputCommand(std::shared_ptr<NWebCommandActionInfo> info) = 0;
    virtual int32_t HandleSelectCommand(std::shared_ptr<NWebCommandActionInfo> info) = 0;
    virtual int32_t HandleGestureCommand(std::shared_ptr<NWebCommandActionInfo> action) = 0;
};

}  // namespace OHOS::NWeb

#endif  // NWEB_COMMAND_ACTION_MANAGER_H

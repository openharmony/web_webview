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

#ifndef ARK_WEB_COMMAND_ACTION_WRAPPER_H_
#define ARK_WEB_COMMAND_ACTION_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_command_action.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebCommandActionWrapper : public OHOS::NWeb::NWebCommandAction {
public:
    ArkWebCommandActionWrapper(ArkWebRefPtr<ArkWebCommandAction> action);
    ~ArkWebCommandActionWrapper() = default;

    std::string GetEventType() override;

    std::string GetXPath() override;

    int32_t GetDuration() override;

    std::string GetAlign() override;

    int32_t GetOffset() override;

private:
    ArkWebRefPtr<ArkWebCommandAction> action_;
};
}  // namespace OHOS::ArkWeb

#endif  // ARK_WEB_COMMAND_ACTION_WRAPPER_H_
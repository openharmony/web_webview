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

#ifndef ARK_WEB_COMMAND_ACTION_IMPL_H_
#define ARK_WEB_COMMAND_ACTION_IMPL_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_command_action.h"

namespace OHOS::ArkWeb {

class ArkWebCommandActionImpl : public ArkWebCommandAction {
    IMPLEMENT_REFCOUNTING(ArkWebCommandActionImpl);

public:
    ArkWebCommandActionImpl(std::shared_ptr<OHOS::NWeb::NWebCommandAction> action);
    ~ArkWebCommandActionImpl() = default;

    ArkWebString GetEventType() override;

    ArkWebString GetXPath() override;

    int32_t GetDuration() override;

    ArkWebString GetAlign() override;

    int32_t GetOffset() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebCommandAction> action_;
};

}  // namespace OHOS::ArkWeb

#endif  // ARK_WEB_COMMAND_ACTION_IMPL_H_

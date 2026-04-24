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

#ifndef ARK_WEB_COMMAND_ACTION_INFO_IMPL_H_
#define ARK_WEB_COMMAND_ACTION_INFO_IMPL_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_command_action_info.h"

namespace OHOS::ArkWeb {

class ArkWebCommandActionInfoImpl : public ArkWebCommandActionInfo {
    IMPLEMENT_REFCOUNTING(ArkWebCommandActionInfoImpl);

public:
    explicit ArkWebCommandActionInfoImpl(std::shared_ptr<OHOS::NWeb::NWebCommandActionInfo> info);
    ~ArkWebCommandActionInfoImpl() = default;

    ArkWebString GetEventType() override;
    ArkWebString GetInputValue() override;
    ArkWebString GetXPath() override;
    ArkWebStringVector GetOptionValues() override;
    ArkWebInt32Vector GetOptionIndexes() override;

    double GetX() override;
    double GetY() override;
    double GetDistanceX() override;
    double GetDistanceY() override;
    float GetScale() override;
    int32_t GetDuration() override;
    int32_t GetTapCount() override;
    int32_t GetSpeed() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebCommandActionInfo> info_;
};

}  // namespace OHOS::ArkWeb

#endif  // ARK_WEB_COMMAND_ACTION_INFO_IMPL_H_

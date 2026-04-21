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

#ifndef ARK_WEB_COMMAND_ACTION_INFO_WRAPPER_H_
#define ARK_WEB_COMMAND_ACTION_INFO_WRAPPER_H_
#pragma once

#include <cstdint>

#include "include/nweb_command_action_info.h"
#include "ohos_nweb/include/ark_web_command_action_info.h"

namespace OHOS::ArkWeb {

class ArkWebCommandActionInfoWrapper : public OHOS::NWeb::NWebCommandActionInfo {
public:
    explicit ArkWebCommandActionInfoWrapper(ArkWebRefPtr<ArkWebCommandActionInfo> info);
    ~ArkWebCommandActionInfoWrapper() = default;

    std::string GetEventType() const override;
    std::string GetXPath() const override;
    std::string GetInputValue() const override;
    std::vector<std::string> GetOptionValues() const override;
    std::vector<int32_t> GetOptionIndexes() const override;

private:
    ArkWebRefPtr<ArkWebCommandActionInfo> info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_COMMAND_ACTION_INFO_WRAPPER_H_

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

#ifndef NWEB_COMMAND_ACTION_INFO_H
#define NWEB_COMMAND_ACTION_INFO_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "nweb_export.h"

namespace OHOS::NWeb {

/**
 * @brief Interface for DOM command action information.
 * Carries parsed parameters from ace_engine to webview for execution.
 */
class OHOS_NWEB_EXPORT NWebCommandActionInfo {
public:
    virtual ~NWebCommandActionInfo() = default;

    virtual std::string GetEventType() const
    {
        return "";
    }
    virtual std::string GetXPath() const
    {
        return "";
    }

    // Input getter
    virtual std::string GetInputValue() const
    {
        return "";
    }

    // Select getters
    virtual std::vector<std::string> GetOptionValues() const
    {
        return {};
    }

    virtual std::vector<int32_t> GetOptionIndexes() const
    {
        return {};
    }
};

} // namespace OHOS::NWeb

#endif // NWEB_COMMAND_ACTION_INFO_H

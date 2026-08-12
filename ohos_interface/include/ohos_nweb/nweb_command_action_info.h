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
 * @brief Enum for auto-fill mode.
 */
enum class AutoFillMode : int32_t {
    Unspecified = -1,  // item-level unspecified, fallback to defaultMode
    Overwrite = 0,
    Insert = 1,
};
 
inline AutoFillMode StringToAutoFillMode(const std::string& modeStr)
{
    if (modeStr == "insert") {
        return AutoFillMode::Insert;
    }
    return AutoFillMode::Overwrite;
}
 
inline std::string AutoFillModeToString(AutoFillMode mode)
{
    switch (mode) {
        case AutoFillMode::Insert:
            return "insert";
        case AutoFillMode::Overwrite:
        default:
            return "overwrite";
    }
}
 
/**
 * @brief Item class for inputAutoFill batch filling.
 * Abstract base class — concrete instances created via AutoFillItemImpl.
 */
class OHOS_NWEB_EXPORT AutoFillItem {
public:
    AutoFillItem() = default;
    virtual ~AutoFillItem() = default;
 
    virtual std::string GetXPath() const = 0;
    virtual std::string GetContent() const = 0;
    virtual int32_t GetIndex() const = 0;
    virtual AutoFillMode GetMode() const = 0;
};

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

    virtual double GetX() const
    {
        return 0.0;
    }

    virtual double GetY() const
    {
        return 0.0;
    }

    virtual double GetDistanceX() const
    {
        return 0.0;
    }

    virtual double GetDistanceY() const
    {
        return 0.0;
    }

    virtual float GetScale() const
    {
        return 1.0f;
    }

    virtual int32_t GetDuration() const
    {
        return 0;
    }

    virtual int32_t GetTapCount() const
    {
        return 1;
    }

    virtual int32_t GetSpeed() const
    {
        return 0;
    }

    // inputAutoFill batch fill getters
    virtual std::vector<std::shared_ptr<AutoFillItem>> GetAutoFillItems() const
    {
        return {};
    }

    virtual AutoFillMode GetDefaultMode() const
    {
        return AutoFillMode::Unspecified;
    }
};

} // namespace OHOS::NWeb

#endif // NWEB_COMMAND_ACTION_INFO_H

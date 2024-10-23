/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NWEB_ACCESSIBILITY_NODE_INFO_H
#define NWEB_ACCESSIBILITY_NODE_INFO_H

#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebAccessibilityNodeInfo {
public:
    virtual ~NWebAccessibilityNodeInfo() = default;

    virtual std::string GetHint() = 0;

    virtual std::string GetError() = 0;

    virtual int32_t GetRectX() = 0;

    virtual int32_t GetRectY() = 0;

    virtual void SetPageId(int32_t pageId) = 0;

    virtual int32_t GetPageId() = 0;

    virtual std::vector<uint32_t> GetActions() = 0;

    virtual std::string GetContent() = 0;

    virtual std::vector<int64_t> GetChildIds() = 0;

    virtual void SetParentId(int64_t parentId) = 0;

    virtual int64_t GetParentId() = 0;

    virtual bool GetIsHeading() = 0;

    virtual bool GetIsChecked() = 0;

    virtual bool GetIsEnabled() = 0;

    virtual bool GetIsFocused() = 0;

    virtual int32_t GetRectWidth() = 0;

    virtual int32_t GetRectHeight() = 0;

    virtual bool GetIsVisible() = 0;

    virtual bool GetIsHinting() = 0;

    virtual bool GetIsEditable() = 0;

    virtual bool GetIsSelected() = 0;

    virtual size_t GetItemCounts() = 0;

    virtual int32_t GetLiveRegion() = 0;

    virtual bool GetIsPassword() = 0;

    virtual bool GetIsCheckable() = 0;

    virtual bool GetIsClickable() = 0;

    virtual bool GetIsFocusable() = 0;

    virtual bool GetIsScrollable() = 0;

    virtual bool GetIsDeletable() = 0;

    virtual int64_t GetAccessibilityId() = 0;

    virtual bool GetIsPopupSupported() = 0;

    virtual bool GetIsContentInvalid() = 0;

    virtual int32_t GetSelectionEnd() = 0;

    virtual int32_t GetSelectionStart() = 0;

    virtual float GetRangeInfoMin() = 0;

    virtual float GetRangeInfoMax() = 0;

    virtual float GetRangeInfoCurrent() = 0;

    virtual int32_t GetInputType() = 0;

    virtual std::string GetComponentType() = 0;

    virtual std::string GetDescriptionInfo() = 0;

    virtual int32_t GetGridRows() = 0;

    virtual int32_t GetGridItemRow() = 0;

    virtual int32_t GetGridColumns() = 0;

    virtual int32_t GetGridItemColumn() = 0;

    virtual int32_t GetGridItemRowSpan() = 0;

    virtual int32_t GetGridSelectedMode() = 0;

    virtual int32_t GetGridItemColumnSpan() = 0;

    virtual bool GetIsAccessibilityFocus() = 0;

    virtual bool GetIsPluralLineSupported() = 0;

    virtual bool GetIsAccessibilityGroup()
    {
        return false;
    }

    virtual std::string GetAccessibilityLevel()
    {
        return std::string();
    }

    virtual std::string GetAccessibilityDescription()
    {
        return std::string();
    }

    virtual std::string GetAccessibilityText()
    {
        return std::string();
    }
};

} // namespace OHOS::NWeb

#endif // NWEB_ACCESSIBILITY_NODE_INFO_H
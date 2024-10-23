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

#ifndef ARK_WEB_ACCESSIBILITY_NODE_INFO_H_
#define ARK_WEB_ACCESSIBILITY_NODE_INFO_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebAccessibilityNodeInfo : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebString GetHint() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetError() = 0;

    /*--ark web()--*/
    virtual int32_t GetRectX() = 0;

    /*--ark web()--*/
    virtual int32_t GetRectY() = 0;

    /*--ark web()--*/
    virtual void SetPageId(int32_t page_id) = 0;

    /*--ark web()--*/
    virtual int32_t GetPageId() = 0;

    /*--ark web()--*/
    virtual ArkWebUint32Vector GetActions() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetContent() = 0;

    /*--ark web()--*/
    virtual ArkWebInt64Vector GetChildIds() = 0;

    /*--ark web()--*/
    virtual void SetParentId(int64_t parentId_id) = 0;

    /*--ark web()--*/
    virtual int64_t GetParentId() = 0;

    /*--ark web()--*/
    virtual bool GetIsHeading() = 0;

    /*--ark web()--*/
    virtual bool GetIsChecked() = 0;

    /*--ark web()--*/
    virtual bool GetIsEnabled() = 0;

    /*--ark web()--*/
    virtual bool GetIsFocused() = 0;

    /*--ark web()--*/
    virtual int32_t GetRectWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetRectHeight() = 0;

    /*--ark web()--*/
    virtual bool GetIsVisible() = 0;

    /*--ark web()--*/
    virtual bool GetIsHinting() = 0;

    /*--ark web()--*/
    virtual bool GetIsEditable() = 0;

    /*--ark web()--*/
    virtual bool GetIsSelected() = 0;

    /*--ark web()--*/
    virtual size_t GetItemCounts() = 0;

    /*--ark web()--*/
    virtual int32_t GetLiveRegion() = 0;

    /*--ark web()--*/
    virtual bool GetIsPassword() = 0;

    /*--ark web()--*/
    virtual bool GetIsCheckable() = 0;

    /*--ark web()--*/
    virtual bool GetIsClickable() = 0;

    /*--ark web()--*/
    virtual bool GetIsFocusable() = 0;

    /*--ark web()--*/
    virtual bool GetIsScrollable() = 0;

    /*--ark web()--*/
    virtual bool GetIsDeletable() = 0;

    /*--ark web()--*/
    virtual int64_t GetAccessibilityId() = 0;

    /*--ark web()--*/
    virtual bool GetIsPopupSupported() = 0;

    /*--ark web()--*/
    virtual bool GetIsContentInvalid() = 0;

    /*--ark web()--*/
    virtual int32_t GetSelectionEnd() = 0;

    /*--ark web()--*/
    virtual int32_t GetSelectionStart() = 0;

    /*--ark web()--*/
    virtual float GetRangeInfoMin() = 0;

    /*--ark web()--*/
    virtual float GetRangeInfoMax() = 0;

    /*--ark web()--*/
    virtual float GetRangeInfoCurrent() = 0;

    /*--ark web()--*/
    virtual int32_t GetInputType() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetComponentType() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetDescriptionInfo() = 0;

    /*--ark web()--*/
    virtual int32_t GetGridRows() = 0;

    /*--ark web()--*/
    virtual int32_t GetGridItemRow() = 0;

    /*--ark web()--*/
    virtual int32_t GetGridColumns() = 0;

    /*--ark web()--*/
    virtual int32_t GetGridItemColumn() = 0;

    /*--ark web()--*/
    virtual int32_t GetGridItemRowSpan() = 0;

    /*--ark web()--*/
    virtual int32_t GetGridSelectedMode() = 0;

    /*--ark web()--*/
    virtual int32_t GetGridItemColumnSpan() = 0;

    /*--ark web()--*/
    virtual bool GetIsAccessibilityFocus() = 0;

    /*--ark web()--*/
    virtual bool GetIsPluralLineSupported() = 0;

    /**
     * @Description: Get whether the acccessibility node is an accessibility group.
     * @Return: Whether the acccessibility node is an accessibility group.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual bool GetIsAccessibilityGroup() = 0;

    /**
     * @Description: Get the acccessibility level of the accessibility node.
     * @Return: The acccessibility level of the accessibility node.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual ArkWebString GetAccessibilityLevel() = 0;

    /**
     * @Description: Get the acccessibility description of the accessibility node.
     * @Return: The acccessibility description of the accessibility node.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual ArkWebString GetAccessibilityDescription() = 0;

    /**
     * @Description: Get the acccessibility text of the accessibility node.
     * @Return: The acccessibility text of the accessibility node.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual ArkWebString GetAccessibilityText() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ACCESSIBILITY_NODE_INFO_H_

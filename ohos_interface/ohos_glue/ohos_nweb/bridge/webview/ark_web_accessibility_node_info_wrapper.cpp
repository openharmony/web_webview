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

#include "ohos_nweb/bridge/ark_web_accessibility_node_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAccessibilityNodeInfoWrapper::ArkWebAccessibilityNodeInfoWrapper(
    ArkWebRefPtr<ArkWebAccessibilityNodeInfo> ark_web_accessibility_node_info)
    : ark_web_accessibility_node_info_(ark_web_accessibility_node_info)
{}

std::string ArkWebAccessibilityNodeInfoWrapper::GetHint()
{
    ArkWebString stHint = ark_web_accessibility_node_info_->GetHint();

    std::string objHint = ArkWebStringStructToClass(stHint);
    ArkWebStringStructRelease(stHint);
    return objHint;
}

std::string ArkWebAccessibilityNodeInfoWrapper::GetError()
{
    ArkWebString stError = ark_web_accessibility_node_info_->GetError();

    std::string objError = ArkWebStringStructToClass(stError);
    ArkWebStringStructRelease(stError);
    return objError;
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetRectX()
{
    return ark_web_accessibility_node_info_->GetRectX();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetRectY()
{
    return ark_web_accessibility_node_info_->GetRectY();
}

void ArkWebAccessibilityNodeInfoWrapper::SetPageId(int32_t page_id)
{
    ark_web_accessibility_node_info_->SetPageId(page_id);
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetPageId()
{
    return ark_web_accessibility_node_info_->GetPageId();
}

std::vector<uint32_t> ArkWebAccessibilityNodeInfoWrapper::GetActions()
{
    ArkWebUint32Vector stActions = ark_web_accessibility_node_info_->GetActions();

    std::vector<uint32_t> objActions = ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(stActions);
    ArkWebBasicVectorStructRelease<ArkWebUint32Vector>(stActions);
    return objActions;
}

std::string ArkWebAccessibilityNodeInfoWrapper::GetContent()
{
    ArkWebString stContent = ark_web_accessibility_node_info_->GetContent();

    std::string objContent = ArkWebStringStructToClass(stContent);
    ArkWebStringStructRelease(stContent);
    return objContent;
}

std::vector<int64_t> ArkWebAccessibilityNodeInfoWrapper::GetChildIds()
{
    ArkWebInt64Vector stChildIds = ark_web_accessibility_node_info_->GetChildIds();

    std::vector<int64_t> objChildIds = ArkWebBasicVectorStructToClass<int64_t, ArkWebInt64Vector>(stChildIds);
    ArkWebBasicVectorStructRelease<ArkWebInt64Vector>(stChildIds);
    return objChildIds;
}

void ArkWebAccessibilityNodeInfoWrapper::SetParentId(int64_t parent_id)
{
    ark_web_accessibility_node_info_->SetParentId(parent_id);
}

int64_t ArkWebAccessibilityNodeInfoWrapper::GetParentId()
{
    return ark_web_accessibility_node_info_->GetParentId();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsHeading()
{
    return ark_web_accessibility_node_info_->GetIsHeading();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsChecked()
{
    return ark_web_accessibility_node_info_->GetIsChecked();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsEnabled()
{
    return ark_web_accessibility_node_info_->GetIsEnabled();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsFocused()
{
    return ark_web_accessibility_node_info_->GetIsFocused();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetRectWidth()
{
    return ark_web_accessibility_node_info_->GetRectWidth();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetRectHeight()
{
    return ark_web_accessibility_node_info_->GetRectHeight();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsVisible()
{
    return ark_web_accessibility_node_info_->GetIsVisible();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsHinting()
{
    return ark_web_accessibility_node_info_->GetIsHinting();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsEditable()
{
    return ark_web_accessibility_node_info_->GetIsEditable();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsSelected()
{
    return ark_web_accessibility_node_info_->GetIsSelected();
}

size_t ArkWebAccessibilityNodeInfoWrapper::GetItemCounts()
{
    return ark_web_accessibility_node_info_->GetItemCounts();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetLiveRegion()
{
    return ark_web_accessibility_node_info_->GetLiveRegion();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsPassword()
{
    return ark_web_accessibility_node_info_->GetIsPassword();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsCheckable()
{
    return ark_web_accessibility_node_info_->GetIsCheckable();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsClickable()
{
    return ark_web_accessibility_node_info_->GetIsClickable();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsFocusable()
{
    return ark_web_accessibility_node_info_->GetIsFocusable();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsScrollable()
{
    return ark_web_accessibility_node_info_->GetIsScrollable();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsDeletable()
{
    return ark_web_accessibility_node_info_->GetIsDeletable();
}

int64_t ArkWebAccessibilityNodeInfoWrapper::GetAccessibilityId()
{
    return ark_web_accessibility_node_info_->GetAccessibilityId();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsPopupSupported()
{
    return ark_web_accessibility_node_info_->GetIsPopupSupported();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsContentInvalid()
{
    return ark_web_accessibility_node_info_->GetIsContentInvalid();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetSelectionEnd()
{
    return ark_web_accessibility_node_info_->GetSelectionEnd();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetSelectionStart()
{
    return ark_web_accessibility_node_info_->GetSelectionStart();
}

float ArkWebAccessibilityNodeInfoWrapper::GetRangeInfoMin()
{
    return ark_web_accessibility_node_info_->GetRangeInfoMin();
}

float ArkWebAccessibilityNodeInfoWrapper::GetRangeInfoMax()
{
    return ark_web_accessibility_node_info_->GetRangeInfoMax();
}

float ArkWebAccessibilityNodeInfoWrapper::GetRangeInfoCurrent()
{
    return ark_web_accessibility_node_info_->GetRangeInfoCurrent();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetInputType()
{
    return ark_web_accessibility_node_info_->GetInputType();
}

std::string ArkWebAccessibilityNodeInfoWrapper::GetComponentType()
{
    ArkWebString stComponentType = ark_web_accessibility_node_info_->GetComponentType();

    std::string objComponentType = ArkWebStringStructToClass(stComponentType);
    ArkWebStringStructRelease(stComponentType);
    return objComponentType;
}

std::string ArkWebAccessibilityNodeInfoWrapper::GetDescriptionInfo()
{
    ArkWebString stDescriptionInfo = ark_web_accessibility_node_info_->GetDescriptionInfo();

    std::string objDescriptionInfo = ArkWebStringStructToClass(stDescriptionInfo);
    ArkWebStringStructRelease(stDescriptionInfo);
    return objDescriptionInfo;
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetGridRows()
{
    return ark_web_accessibility_node_info_->GetGridRows();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetGridItemRow()
{
    return ark_web_accessibility_node_info_->GetGridItemRow();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetGridColumns()
{
    return ark_web_accessibility_node_info_->GetGridColumns();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetGridItemColumn()
{
    return ark_web_accessibility_node_info_->GetGridItemColumn();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetGridItemRowSpan()
{
    return ark_web_accessibility_node_info_->GetGridItemRowSpan();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetGridSelectedMode()
{
    return ark_web_accessibility_node_info_->GetGridSelectedMode();
}

int32_t ArkWebAccessibilityNodeInfoWrapper::GetGridItemColumnSpan()
{
    return ark_web_accessibility_node_info_->GetGridItemColumnSpan();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsAccessibilityFocus()
{
    return ark_web_accessibility_node_info_->GetIsAccessibilityFocus();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsPluralLineSupported()
{
    return ark_web_accessibility_node_info_->GetIsPluralLineSupported();
}

bool ArkWebAccessibilityNodeInfoWrapper::GetIsAccessibilityGroup()
{
    return ark_web_accessibility_node_info_->GetIsAccessibilityGroup();
}

std::string ArkWebAccessibilityNodeInfoWrapper::GetAccessibilityLevel()
{
    ArkWebString stAccessibilityLevel = ark_web_accessibility_node_info_->GetAccessibilityLevel();

    std::string objAccessibilityLevel = ArkWebStringStructToClass(stAccessibilityLevel);
    ArkWebStringStructRelease(stAccessibilityLevel);
    return objAccessibilityLevel;
}

std::string ArkWebAccessibilityNodeInfoWrapper::GetAccessibilityDescription()
{
    ArkWebString stAccessibilityDescription = ark_web_accessibility_node_info_->GetAccessibilityDescription();

    std::string objAccessibilityDescription = ArkWebStringStructToClass(stAccessibilityDescription);
    ArkWebStringStructRelease(stAccessibilityDescription);
    return objAccessibilityDescription;
}

std::string ArkWebAccessibilityNodeInfoWrapper::GetAccessibilityText()
{
    ArkWebString stAccessibilityText = ark_web_accessibility_node_info_->GetAccessibilityText();

    std::string objAccessibilityText = ArkWebStringStructToClass(stAccessibilityText);
    ArkWebStringStructRelease(stAccessibilityText);
    return objAccessibilityText;
}

} // namespace OHOS::ArkWeb

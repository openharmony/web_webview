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

#include "ohos_nweb/bridge/ark_web_accessibility_node_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebAccessibilityNodeInfoImpl::ArkWebAccessibilityNodeInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> nweb_accessibility_node_info)
    : nweb_accessibility_node_info_(nweb_accessibility_node_info)
{}

ArkWebString ArkWebAccessibilityNodeInfoImpl::GetHint()
{
    return ArkWebStringClassToStruct(nweb_accessibility_node_info_->GetHint());
}

ArkWebString ArkWebAccessibilityNodeInfoImpl::GetError()
{
    return ArkWebStringClassToStruct(nweb_accessibility_node_info_->GetError());
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetRectX()
{
    return nweb_accessibility_node_info_->GetRectX();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetRectY()
{
    return nweb_accessibility_node_info_->GetRectY();
}

void ArkWebAccessibilityNodeInfoImpl::SetPageId(int32_t page_id)
{
    nweb_accessibility_node_info_->SetPageId(page_id);
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetPageId()
{
    return nweb_accessibility_node_info_->GetPageId();
}

ArkWebUint32Vector ArkWebAccessibilityNodeInfoImpl::GetActions()
{
    return ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(nweb_accessibility_node_info_->GetActions());
}

ArkWebString ArkWebAccessibilityNodeInfoImpl::GetContent()
{
    return ArkWebStringClassToStruct(nweb_accessibility_node_info_->GetContent());
}

ArkWebInt64Vector ArkWebAccessibilityNodeInfoImpl::GetChildIds()
{
    return ArkWebBasicVectorClassToStruct<int64_t, ArkWebInt64Vector>(nweb_accessibility_node_info_->GetChildIds());
}

void ArkWebAccessibilityNodeInfoImpl::SetParentId(int64_t parent_id)
{
    nweb_accessibility_node_info_->SetParentId(parent_id);
}

int64_t ArkWebAccessibilityNodeInfoImpl::GetParentId()
{
    return nweb_accessibility_node_info_->GetParentId();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsHeading()
{
    return nweb_accessibility_node_info_->GetIsHeading();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsChecked()
{
    return nweb_accessibility_node_info_->GetIsChecked();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsEnabled()
{
    return nweb_accessibility_node_info_->GetIsEnabled();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsFocused()
{
    return nweb_accessibility_node_info_->GetIsFocused();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetRectWidth()
{
    return nweb_accessibility_node_info_->GetRectWidth();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetRectHeight()
{
    return nweb_accessibility_node_info_->GetRectHeight();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsVisible()
{
    return nweb_accessibility_node_info_->GetIsVisible();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsHinting()
{
    return nweb_accessibility_node_info_->GetIsHinting();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsEditable()
{
    return nweb_accessibility_node_info_->GetIsEditable();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsSelected()
{
    return nweb_accessibility_node_info_->GetIsSelected();
}

size_t ArkWebAccessibilityNodeInfoImpl::GetItemCounts()
{
    return nweb_accessibility_node_info_->GetItemCounts();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetLiveRegion()
{
    return nweb_accessibility_node_info_->GetLiveRegion();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsPassword()
{
    return nweb_accessibility_node_info_->GetIsPassword();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsCheckable()
{
    return nweb_accessibility_node_info_->GetIsCheckable();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsClickable()
{
    return nweb_accessibility_node_info_->GetIsClickable();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsFocusable()
{
    return nweb_accessibility_node_info_->GetIsFocusable();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsScrollable()
{
    return nweb_accessibility_node_info_->GetIsScrollable();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsDeletable()
{
    return nweb_accessibility_node_info_->GetIsDeletable();
}

int64_t ArkWebAccessibilityNodeInfoImpl::GetAccessibilityId()
{
    return nweb_accessibility_node_info_->GetAccessibilityId();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsPopupSupported()
{
    return nweb_accessibility_node_info_->GetIsPopupSupported();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsContentInvalid()
{
    return nweb_accessibility_node_info_->GetIsContentInvalid();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetSelectionEnd()
{
    return nweb_accessibility_node_info_->GetSelectionEnd();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetSelectionStart()
{
    return nweb_accessibility_node_info_->GetSelectionStart();
}

float ArkWebAccessibilityNodeInfoImpl::GetRangeInfoMin()
{
    return nweb_accessibility_node_info_->GetRangeInfoMin();
}

float ArkWebAccessibilityNodeInfoImpl::GetRangeInfoMax()
{
    return nweb_accessibility_node_info_->GetRangeInfoMax();
}

float ArkWebAccessibilityNodeInfoImpl::GetRangeInfoCurrent()
{
    return nweb_accessibility_node_info_->GetRangeInfoCurrent();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetInputType()
{
    return nweb_accessibility_node_info_->GetInputType();
}

ArkWebString ArkWebAccessibilityNodeInfoImpl::GetComponentType()
{
    return ArkWebStringClassToStruct(nweb_accessibility_node_info_->GetComponentType());
}

ArkWebString ArkWebAccessibilityNodeInfoImpl::GetDescriptionInfo()
{
    return ArkWebStringClassToStruct(nweb_accessibility_node_info_->GetDescriptionInfo());
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetGridRows()
{
    return nweb_accessibility_node_info_->GetGridRows();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetGridItemRow()
{
    return nweb_accessibility_node_info_->GetGridItemRow();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetGridColumns()
{
    return nweb_accessibility_node_info_->GetGridColumns();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetGridItemColumn()
{
    return nweb_accessibility_node_info_->GetGridItemColumn();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetGridItemRowSpan()
{
    return nweb_accessibility_node_info_->GetGridItemRowSpan();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetGridSelectedMode()
{
    return nweb_accessibility_node_info_->GetGridSelectedMode();
}

int32_t ArkWebAccessibilityNodeInfoImpl::GetGridItemColumnSpan()
{
    return nweb_accessibility_node_info_->GetGridItemColumnSpan();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsAccessibilityFocus()
{
    return nweb_accessibility_node_info_->GetIsAccessibilityFocus();
}

bool ArkWebAccessibilityNodeInfoImpl::GetIsPluralLineSupported()
{
    return nweb_accessibility_node_info_->GetIsPluralLineSupported();
}

ArkWebString ArkWebAccessibilityNodeInfoImpl::GetAccessibilityLevel()
{
    return ArkWebStringClassToStruct(nweb_accessibility_node_info_->GetAccessibilityLevel());
}
} // namespace OHOS::ArkWeb

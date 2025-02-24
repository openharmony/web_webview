/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "display_manager_adapter_impl.h"

#include <cmath>
#include "display_info.h"
#include "nweb_log.h"
#include "syspara/parameters.h"
#include "oh_display_manager.h"
#include "oh_display_info.h"

using namespace OHOS::Rosen;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
constexpr float EPS = 0.0001f;

DisplayListenerAdapterImpl::DisplayListenerAdapterImpl(
    std::shared_ptr<DisplayListenerAdapter> listener) : listener_(listener) {}

void DisplayListenerAdapterImpl::OnCreate(DisplayId id)
{
    if (listener_ != nullptr) {
        listener_->OnCreate(id);
    }
}

void DisplayListenerAdapterImpl::OnDestroy(DisplayId id)
{
    if (listener_ != nullptr) {
        listener_->OnDestroy(id);
    }
}

void DisplayListenerAdapterImpl::OnChange(DisplayId id)
{
    if (CheckOnlyRefreshRateDecreased(id)) {
        return;
    }
    if (listener_ != nullptr) {
        listener_->OnChange(id);
    }
}

bool DisplayListenerAdapterImpl::CheckOnlyRefreshRateDecreased(DisplayId id)
{
    if (id != DisplayManager::GetInstance().GetDefaultDisplayId()) {
        return false;
    }
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    if (!displayPtr) {
        return false;
    }
    auto displayInfo = displayPtr->GetDisplayInfo();
    if (!displayInfo) {
        return false;
    }
    auto nwebDisplayInfo =  ConvertDisplayInfo(*displayInfo);
    if (nwebDisplayInfo == cachedDisplayedInfo_ &&
        nwebDisplayInfo.refreshRate_ != cachedDisplayedInfo_.refreshRate_) {
        WVLOG_D("refresh rate change is intercepted, previous refresh rate: %{public}u, after: %{public}u",
            cachedDisplayedInfo_.refreshRate_, nwebDisplayInfo.refreshRate_);
        cachedDisplayedInfo_ = nwebDisplayInfo;
        return true;
    }
    cachedDisplayedInfo_ = nwebDisplayInfo;
    return false;
}

OHOS::NWeb::DisplayInfo DisplayListenerAdapterImpl::ConvertDisplayInfo(
    const OHOS::Rosen::DisplayInfo& info)
{
    OHOS::NWeb::DisplayInfo displayInfo = OHOS::NWeb::DisplayInfo{};
    displayInfo.width_ = info.GetWidth();
    displayInfo.height_ = info.GetHeight();
    displayInfo.refreshRate_ = info.GetRefreshRate();
    displayInfo.virtualPixelRatio_ = info.GetVirtualPixelRatio();
    displayInfo.xDpi_ = info.GetXDpi();
    displayInfo.yDpi_ = info.GetYDpi();
    displayInfo.rotationType_ = info.GetRotation();
    displayInfo.orientationType_ = info.GetOrientation();
    displayInfo.displayOrientation_ = info.GetDisplayOrientation();
    return displayInfo;
}

FoldStatusListenerAdapterImpl::FoldStatusListenerAdapterImpl(
    std::shared_ptr<FoldStatusListenerAdapter> listener) : listener_(listener) {}

OHOS::NWeb::FoldStatus FoldStatusListenerAdapterImpl::ConvertFoldStatus(
    NativeDisplayManager_FoldDisplayMode displayMode)
{
    switch (displayMode) {
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL:
            return OHOS::NWeb::FoldStatus::FULL;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN:
            return OHOS::NWeb::FoldStatus::MAIN;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB:
            return OHOS::NWeb::FoldStatus::SUB;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION:
            return OHOS::NWeb::FoldStatus::COORDINATION;
        default:
            return OHOS::NWeb::FoldStatus::UNKNOWN;
    }
}

void FoldStatusListenerAdapterImpl::OnFoldStatusChanged(NativeDisplayManager_FoldDisplayMode displayMode)
{
    if (listener_ != nullptr) {
        listener_->OnFoldStatusChanged(ConvertFoldStatus(displayMode));
    }
}

DisplayAdapterImpl::DisplayAdapterImpl(sptr<OHOS::Rosen::Display> display)
    : display_(display) {}

OHOS::NWeb::RotationType DisplayAdapterImpl::ConvertRotationType(OHOS::Rosen::Rotation type)
{
    switch (type) {
        case OHOS::Rosen::Rotation::ROTATION_0:
            return OHOS::NWeb::RotationType::ROTATION_0;
        case OHOS::Rosen::Rotation::ROTATION_90:
            return OHOS::NWeb::RotationType::ROTATION_90;
        case OHOS::Rosen::Rotation::ROTATION_180:
            return OHOS::NWeb::RotationType::ROTATION_180;
        case OHOS::Rosen::Rotation::ROTATION_270:
            return OHOS::NWeb::RotationType::ROTATION_270;
        default:
            return OHOS::NWeb::RotationType::ROTATION_BUTT;
    }
}

OHOS::NWeb::OrientationType DisplayAdapterImpl::ConvertOrientationType(OHOS::Rosen::Orientation type)
{
    switch (type) {
        case OHOS::Rosen::Orientation::UNSPECIFIED:
            return OHOS::NWeb::OrientationType::UNSPECIFIED;
        case OHOS::Rosen::Orientation::VERTICAL:
            return OHOS::NWeb::OrientationType::VERTICAL;
        case OHOS::Rosen::Orientation::HORIZONTAL:
            return OHOS::NWeb::OrientationType::HORIZONTAL;
        case OHOS::Rosen::Orientation::REVERSE_VERTICAL:
            return OHOS::NWeb::OrientationType::REVERSE_VERTICAL;
        case OHOS::Rosen::Orientation::REVERSE_HORIZONTAL:
            return OHOS::NWeb::OrientationType::REVERSE_HORIZONTAL;
        case OHOS::Rosen::Orientation::SENSOR:
            return OHOS::NWeb::OrientationType::SENSOR;
        case OHOS::Rosen::Orientation::SENSOR_VERTICAL:
            return OHOS::NWeb::OrientationType::SENSOR_VERTICAL;
        case OHOS::Rosen::Orientation::SENSOR_HORIZONTAL:
            return OHOS::NWeb::OrientationType::SENSOR_HORIZONTAL;
        default:
            return OHOS::NWeb::OrientationType::BUTT;
    }
}

OHOS::NWeb::DisplayOrientation DisplayAdapterImpl::ConvertDisplayOrientationType(OHOS::Rosen::DisplayOrientation type)
{
    switch (type) {
        case OHOS::Rosen::DisplayOrientation::PORTRAIT:
            return OHOS::NWeb::DisplayOrientation::PORTRAIT;
        case OHOS::Rosen::DisplayOrientation::LANDSCAPE:
            return OHOS::NWeb::DisplayOrientation::LANDSCAPE;
        case OHOS::Rosen::DisplayOrientation::PORTRAIT_INVERTED:
            return OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED;
        case OHOS::Rosen::DisplayOrientation::LANDSCAPE_INVERTED:
            return OHOS::NWeb::DisplayOrientation::LANDSCAPE_INVERTED;
        default:
            return OHOS::NWeb::DisplayOrientation::UNKNOWN;
    }
}


OHOS::NWeb::FoldStatus DisplayAdapterImpl::ConvertFoldStatus(NativeDisplayManager_FoldDisplayMode displayMode)
{
    switch (displayMode) {
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL:
            return OHOS::NWeb::FoldStatus::FULL;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN:
            return OHOS::NWeb::FoldStatus::MAIN;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB:
            return OHOS::NWeb::FoldStatus::SUB;
        case DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION:
            return OHOS::NWeb::FoldStatus::COORDINATION;
        default:
            return OHOS::NWeb::FoldStatus::UNKNOWN;
    }
}

DisplayId DisplayAdapterImpl::GetId()
{
    if (display_ != nullptr) {
        return display_->GetId();
    }
    return static_cast<DisplayId>(-1);
}

int32_t DisplayAdapterImpl::GetWidth()
{
    if (display_ != nullptr) {
        return display_->GetWidth();
    }
    return -1;
}

int32_t DisplayAdapterImpl::GetHeight()
{
    if (display_ != nullptr) {
        return display_->GetHeight();
    }
    return -1;
}

float DisplayAdapterImpl::GetVirtualPixelRatio()
{
    if (display_ != nullptr) {
        return display_->GetVirtualPixelRatio();
    }
    return -1;
}

RotationType DisplayAdapterImpl::GetRotation()
{
    if (display_ != nullptr) {
        return ConvertRotationType(display_->GetRotation());
    }
    return RotationType::ROTATION_BUTT;
}

OrientationType DisplayAdapterImpl::GetOrientation()
{
    if (display_ != nullptr) {
        return ConvertOrientationType(display_->GetOrientation());
    }
    return OrientationType::BUTT;
}

int32_t DisplayAdapterImpl::GetDpi()
{
    int32_t ppi = -1;
    if (!display_) {
        return ppi;
    }
    auto displayInfo = display_->GetDisplayInfo();
    if (!displayInfo) {
        return ppi;
    }
    float xDpi = displayInfo->GetXDpi();
    float yDpi = displayInfo->GetYDpi();
    if (xDpi < EPS || yDpi < EPS) {
        return ppi;
    }

    auto screenLength = sqrt(pow(displayInfo->GetWidth(), 2) + pow(displayInfo->GetHeight(), 2));
    auto phyScreenLength = sqrt(pow(displayInfo->GetWidth() / xDpi, 2) +
        pow(displayInfo->GetHeight() / yDpi, 2));
    if (phyScreenLength < EPS) {
        return ppi;
    }
    ppi = screenLength / phyScreenLength;
    WVLOG_D("dpi: %{public}d, xdpi: %{public}f,ydpi: %{public}f, width: %{public}d, height: %{public}d, "\
        "phyScreenLength: %{public}f", ppi, displayInfo->GetXDpi(), displayInfo->GetYDpi(),
        displayInfo->GetWidth(), displayInfo->GetHeight(), phyScreenLength);

    return ppi;
}

DisplayOrientation DisplayAdapterImpl::GetDisplayOrientation()
{
    if (display_ != nullptr) {
        auto displayInfo = display_->GetDisplayInfo();
        if (displayInfo != nullptr) {
            return ConvertDisplayOrientationType(displayInfo->GetDisplayOrientation());
        }
    }
    return DisplayOrientation::UNKNOWN;
}

FoldStatus DisplayAdapterImpl::GetFoldStatus()
{
    NativeDisplayManager_FoldDisplayMode displayMode =
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_UNKNOWN;
    OH_NativeDisplayManager_GetFoldDisplayMode(&displayMode);
    return ConvertFoldStatus(displayMode);
}

bool DisplayAdapterImpl::IsFoldable()
{
    return OH_NativeDisplayManager_IsFoldable();
}

DisplayId DisplayManagerAdapterImpl::GetDefaultDisplayId()
{
    return DisplayManager::GetInstance().GetDefaultDisplayId();
}

std::shared_ptr<DisplayAdapter> DisplayManagerAdapterImpl::GetDefaultDisplay()
{
    sptr<Display> display = DisplayManager::GetInstance().GetDefaultDisplay();
    return std::make_shared<DisplayAdapterImpl>(display);
}

uint32_t DisplayManagerAdapterImpl::RegisterDisplayListener(
    std::shared_ptr<DisplayListenerAdapter> listener)
{
    static uint32_t count = 1;
    sptr<DisplayListenerAdapterImpl> reg =
        new (std::nothrow) DisplayListenerAdapterImpl(listener);
    if (reg == nullptr) {
        return false;
    }

    uint32_t id = count++;
    if (count == 0) {
        count++;
    }

    reg_.emplace(std::make_pair(id, reg));
    if (DisplayManager::GetInstance().RegisterDisplayListener(reg) == DMError::DM_OK) {
        return id;
    } else {
        return 0;
    }
}

bool DisplayManagerAdapterImpl::UnregisterDisplayListener(uint32_t id)
{
    ListenerMap::iterator iter = reg_.find(id);
    if (iter == reg_.end()) {
        return false;
    }
    if (DisplayManager::GetInstance().UnregisterDisplayListener(iter->second) == DMError::DM_OK) {
        reg_.erase(iter);
        return true;
    }
    return false;
}

bool DisplayManagerAdapterImpl::IsDefaultPortrait()
{
    std::string deviceType = OHOS::system::GetDeviceType();
    return deviceType == "phone" || deviceType == "tablet" || deviceType == "default";
}

FoldStatusListenerMap DisplayManagerAdapterImpl::foldStatusReg_;

void FoldChangeCallBack(NativeDisplayManager_FoldDisplayMode displayMode)
{
    for (auto& iter : DisplayManagerAdapterImpl::foldStatusReg_) {
        iter.second->OnFoldStatusChanged(displayMode);
    }
}

uint32_t DisplayManagerAdapterImpl::RegisterFoldStatusListener(
    std::shared_ptr<FoldStatusListenerAdapter> listener)
{
    sptr<FoldStatusListenerAdapterImpl> reg =
        new (std::nothrow) FoldStatusListenerAdapterImpl(listener);
    if (reg == nullptr) {
        return false;
    }

    uint32_t id = 1;
    if (OH_NativeDisplayManager_RegisterFoldDisplayModeChangeListener(
        FoldChangeCallBack, &id) == NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
        foldStatusReg_.emplace(std::make_pair(id, reg));
        return id;
    } else {
        return 0;
    }
}

bool DisplayManagerAdapterImpl::UnregisterFoldStatusListener(uint32_t id)
{
    FoldStatusListenerMap::iterator iter = foldStatusReg_.find(id);
    if (iter == foldStatusReg_.end()) {
        return false;
    }
    if (OH_NativeDisplayManager_UnregisterFoldDisplayModeChangeListener(id) ==
        NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK) {
        foldStatusReg_.erase(iter);
        return true;
    }
    return false;
}
}

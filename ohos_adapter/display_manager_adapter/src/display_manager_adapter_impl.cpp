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

#include <deviceinfo.h>
#include <string>
#define WVLOG_E(...)

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
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
    if (listener_ != nullptr) {
        listener_->OnChange(id);
    }
}

DisplayAdapterImpl::DisplayAdapterImpl() {}

OHOS::NWeb::RotationType DisplayAdapterImpl::ConvertRotationType(NativeDisplayManager_Rotation type)
{
    switch (type) {
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_0:
            return OHOS::NWeb::RotationType::ROTATION_0;
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_90:
            return OHOS::NWeb::RotationType::ROTATION_90;
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_180:
            return OHOS::NWeb::RotationType::ROTATION_180;
        case NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_270:
            return OHOS::NWeb::RotationType::ROTATION_270;
        default:
            return OHOS::NWeb::RotationType::ROTATION_BUTT;
    }
}

OHOS::NWeb::OrientationType DisplayAdapterImpl::ConvertOrientationType(NativeDisplayManager_Orientation type)
{
    return OHOS::NWeb::DisplayOrientation::UNKNOWN;
}

OHOS::NWeb::DisplayOrientation DisplayAdapterImpl::ConvertDisplayOrientationType(NativeDisplayManager_Orientation type)
{
    switch (type) {
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_PORTRAIT:
            return OHOS::NWeb::DisplayOrientation::PORTRAIT;
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE:
            return OHOS::NWeb::DisplayOrientation::LANDSCAPE;
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_PORTRAIT_INVERTED:
            return OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED;
        case NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE_INVERTED:
            return OHOS::NWeb::DisplayOrientation::LANDSCAPE_INVERTED;
        default:
            return OHOS::NWeb::DisplayOrientation::UNKNOWN;
    }
    return OHOS::NWeb::DisplayOrientation::UNKNOWN;
}

DisplayId DisplayAdapterImpl::GetId()
{
    uint64_t displayId;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayId(&displayId);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return static_cast<DisplayId>(-1);
    }
    return displayId;
}

int32_t DisplayAdapterImpl::GetWidth()
{
    int32_t width;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayWidth(&width);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return static_cast<DisplayId>(-1);
    }
    return width;
}

int32_t DisplayAdapterImpl::GetHeight()
{
    int32_t height;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayHeight(&height);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return -1;
    }
    return height;
}

float DisplayAdapterImpl::GetVirtualPixelRatio()
{
    float virtualPixel;
    NativeDisplayManager_ErrorCode errorCode
        = OH_NativeDisplayManager_GetDefaultDisplayVirtualPixelRatio(&virtualPixel);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return -1;
    }
    return virtualPixel;
}

RotationType DisplayAdapterImpl::GetRotation()
{
    NativeDisplayManager_Rotation displayRotatio;
    NativeDisplayManager_ErrorCode errorCode
        = OH_NativeDisplayManager_GetDefaultDisplayOrientation(&displayRotatio);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return RotationType::ROTATION_BUTT;
    }
    return ConvertRotationType(displayRotatio);
}

OrientationType DisplayAdapterImpl::GetOrientation()
{
    NativeDisplayManager_Orientation displayOrientation;
    NativeDisplayManager_ErrorCode errorCode 
        = OH_NativeDisplayManager_GetDefaultDisplayOrientation(&displayOrientation);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return OrientationType::BUTT;
    }
    return ConvertOrientationType(displayOrientation);
}

int32_t DisplayAdapterImpl::GetDpi()
{
    int32_t densityDpi;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayDensityDpi(&densityDpi);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return -1;
    }
    return densityDpi;
}

DisplayOrientation DisplayAdapterImpl::GetDisplayOrientation()
{
    NativeDisplayManager_Orientation displayOrientation;
    NativeDisplayManager_ErrorCode errorCode
        = OH_NativeDisplayManager_GetDefaultDisplayOrientation(&displayOrientation);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return DisplayOrientation::UNKNOWN;
    }
    return ConvertDisplayOrientationType(displayOrientation);
}

ListenerMap DisplayAdapterImpl::reg_ = {};
DisplayId DisplayManagerAdapterImpl::GetDefaultDisplayId()
{
    uint64_t displayId;
    NativeDisplayManager_ErrorCode errorCode = OH_NativeDisplayManager_GetDefaultDisplayId(&displayId);
    if (NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK != errorCode) {
        return static_cast<DisplayId>(-1);
    }
    return displayId;
}

std::shared_ptr<DisplayAdapter> DisplayManagerAdapterImpl::GetDefaultDisplay()
{
    return std::make_shared<DisplayAdapterImpl>();
}

void DisplayManagerAdapterImp::DisplayChangeCallback(uint64_t displayId) {
    WLOG_D("DisplayManagerAdapterImpl::DisplayChangeCallback");
    for(auto iter = reg_.begin(); iter != reg_.end(); ++iter) {
        iter->second->OnChange(displayId);
    }
}

uint32_t DisplayManagerAdapterImpl::RegisterDisplayListener(
    std::shared_ptr<DisplayListenerAdapter> listener)
{
    WLOG_D("DisplayManagerAdapterImpl::RegisterDisplayListener");
    std::shared_ptr<DisplayListenerAdapterImpl> reg =
        std::make_shared<DisplayListenerAdapterImpl>(listener);
    if (reg == nullptr) {
        return 0;
    }
    
    uint32_t listenerIndex;
    if (OH_NativeDisplayManager_RegisterDisplayChangeListener(DisplayChangeCallback, reg)
        != NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK ) {
        return 0;
    }
    reg_.emplace(std::make_pair(listenerIndex, reg));
    return listenerIndex;
}

bool DisplayManagerAdapterImpl::UnregisterDisplayListener(uint32_t id)
{
    WLOG_D("DisplayManagerAdapterImpl::UnregisterDisplayListener");
    ListenerMap::iterator iter = reg_.find(id);
    if (iter == reg_.end()) {
        return false;
    }
    if (OH_NativeDisplayManager_UnregisterDisplayChangeListener(id)
        != NativeDisplayManager_ErrorCode::DISPLAY_MANAGER_OK ) {
        return false;
    }
    reg_.erase(iter);
    return true;
}

bool DisplayManagerAdapterImpl::IsDefaultPortrait()
{
    std::string deviceType = OH_GetDeviceType();
    return deviceType == "phone" || deviceType == "default";
}
}

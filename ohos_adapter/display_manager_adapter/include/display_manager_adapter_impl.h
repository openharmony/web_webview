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

#ifndef DISPLAY_MANAGER_ADAPTER_IMPL_H
#define DISPLAY_MANAGER_ADAPTER_IMPL_H

#include <map>
#include "display_manager_adapter.h"

#include "display.h"
#include "display_manager.h"
#include "dm_common.h"
#include "oh_display_manager.h"
#include "oh_display_info.h"

namespace OHOS::NWeb {
struct DisplayInfo {
    int32_t width_ {0};
    int32_t height_ {0};
    uint32_t refreshRate_ {0};
    float virtualPixelRatio_ {0.0f};
    float xDpi_ {0.0f};
    float yDpi_ {0.0f};
    OHOS::Rosen::Rotation rotationType_ {Rosen::Rotation::ROTATION_0};
    OHOS::Rosen::Orientation orientationType_ {Rosen::Orientation::UNSPECIFIED};
    OHOS::Rosen::DisplayOrientation displayOrientation_ {Rosen::DisplayOrientation::UNKNOWN};

    DisplayInfo() = default;
    bool operator == (const OHOS::NWeb::DisplayInfo& rhs)
    {
        return width_ == rhs.width_ && height_ == rhs.height_  &&
            std::abs(virtualPixelRatio_ - rhs.virtualPixelRatio_) < EPS &&
            std::abs(xDpi_ - rhs.xDpi_) < EPS && std::abs(yDpi_ - rhs.yDpi_) < EPS &&
            rotationType_ == rhs.rotationType_ && orientationType_ == rhs.orientationType_ &&
            displayOrientation_ == rhs.displayOrientation_;
    }
private:
    static constexpr double EPS = 0.0000001;
};

class DisplayListenerAdapterImpl
    : public OHOS::Rosen::DisplayManager::IDisplayListener {
public:
    explicit DisplayListenerAdapterImpl(std::shared_ptr<DisplayListenerAdapter> listener);
    ~DisplayListenerAdapterImpl() override = default;
    void OnCreate(DisplayId id) override;
    void OnDestroy(DisplayId id) override;
    void OnChange(DisplayId id) override;
private:
    bool CheckOnlyRefreshRateDecreased(DisplayId id);
    OHOS::NWeb::DisplayInfo ConvertDisplayInfo(const OHOS::Rosen::DisplayInfo& info);

    std::shared_ptr<DisplayListenerAdapter> listener_;
    DisplayInfo cachedDisplayedInfo_ {};
};

class FoldStatusListenerAdapterImpl
    : public virtual RefBase {
public:
    explicit FoldStatusListenerAdapterImpl(std::shared_ptr<FoldStatusListenerAdapter> listener);
    ~FoldStatusListenerAdapterImpl() = default;
    void OnFoldStatusChanged(NativeDisplayManager_FoldDisplayMode displayMode) ;
private:
    OHOS::NWeb::FoldStatus ConvertFoldStatus(NativeDisplayManager_FoldDisplayMode displayMode);
    std::shared_ptr<FoldStatusListenerAdapter> listener_;
};

class DisplayAdapterImpl : public DisplayAdapter {
public:
    DisplayAdapterImpl() = delete;
    explicit DisplayAdapterImpl(sptr<OHOS::Rosen::Display> display);
    ~DisplayAdapterImpl() override = default;
    DisplayId GetId() override;
    int32_t GetWidth() override;
    int32_t GetHeight() override;
    float GetVirtualPixelRatio() override;
    RotationType GetRotation() override;
    OrientationType GetOrientation() override;
    int32_t GetDpi() override;
    DisplayOrientation GetDisplayOrientation() override;
    FoldStatus GetFoldStatus() override;
    bool IsFoldable() override;
private:
    sptr<OHOS::Rosen::Display> display_;
    OHOS::NWeb::RotationType ConvertRotationType(OHOS::Rosen::Rotation type);
    OHOS::NWeb::OrientationType ConvertOrientationType(OHOS::Rosen::Orientation type);
    OHOS::NWeb::DisplayOrientation ConvertDisplayOrientationType(OHOS::Rosen::DisplayOrientation type);
    OHOS::NWeb::FoldStatus ConvertFoldStatus(NativeDisplayManager_FoldDisplayMode displayMode);
};

using ListenerMap =
    std::map<int32_t, sptr<DisplayListenerAdapterImpl>>;
using FoldStatusListenerMap =
    std::map<int32_t, sptr<FoldStatusListenerAdapterImpl>>;
class DisplayManagerAdapterImpl : public DisplayManagerAdapter {
public:
    DisplayManagerAdapterImpl() = default;
    ~DisplayManagerAdapterImpl() override = default;
    DisplayId GetDefaultDisplayId() override;
    std::shared_ptr<DisplayAdapter> GetDefaultDisplay() override;
    uint32_t RegisterDisplayListener(std::shared_ptr<DisplayListenerAdapter> listener) override;
    bool UnregisterDisplayListener(uint32_t id) override;
    bool IsDefaultPortrait() override;
    uint32_t RegisterFoldStatusListener(std::shared_ptr<FoldStatusListenerAdapter> listener) override;
    bool UnregisterFoldStatusListener(uint32_t id) override;
    static FoldStatusListenerMap foldStatusReg_;
private:
    ListenerMap reg_;
};
}

#endif
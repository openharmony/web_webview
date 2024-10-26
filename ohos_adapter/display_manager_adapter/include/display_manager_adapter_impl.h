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

namespace OHOS::NWeb {
class DisplayListenerAdapterImpl
    : public OHOS::Rosen::DisplayManager::IDisplayListener {
public:
    explicit DisplayListenerAdapterImpl(std::shared_ptr<DisplayListenerAdapter> listener);
    ~DisplayListenerAdapterImpl() override = default;
    void OnCreate(DisplayId id) override;
    void OnDestroy(DisplayId id) override;
    void OnChange(DisplayId id) override;
private:
    std::shared_ptr<DisplayListenerAdapter> listener_;
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
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

#include <window_manager/oh_display_info.h>
#include <window_manager/oh_display_manager.h>

namespace OHOS::NWeb {
class DisplayListenerAdapterImpl {
public:
    explicit DisplayListenerAdapterImpl(std::shared_ptr<DisplayListenerAdapter> listener);
    ~DisplayListenerAdapterImpl() override = default;
    void OnCreate(DisplayId id);
    void OnDestroy(DisplayId id);
    void OnChange(DisplayId id);
private:
    std::shared_ptr<DisplayListenerAdapter> listener_;
};

class DisplayAdapterImpl : public DisplayAdapter {
public:
    DisplayAdapterImpl();
    ~DisplayAdapterImpl() override = default;
    DisplayId GetId() override;
    int32_t GetWidth() override;
    int32_t GetHeight() override;
    float GetVirtualPixelRatio() override;
    RotationType GetRotation() override;
    OrientationType GetOrientation() override;
    int32_t GetDpi() override;
    DisplayOrientation GetDisplayOrientation() override;
private:
    OHOS::NWeb::RotationType ConvertRotationType(NativeDisplayManager_Rotation type);
    OHOS::NWeb::OrientationType ConvertOrientationType(NativeDisplayManager_Orientation type);
    OHOS::NWeb::DisplayOrientation ConvertDisplayOrientationType(NativeDisplayManager_Orientation type);
};

using ListenerMap =
    std::map<int32_t, std::shared_ptr<DisplayListenerAdapterImpl>>;
class DisplayManagerAdapterImpl : public DisplayManagerAdapter {
public:
    DisplayManagerAdapterImpl() = default;
    ~DisplayManagerAdapterImpl() override = default;
    DisplayId GetDefaultDisplayId() override;
    std::shared_ptr<DisplayAdapter> GetDefaultDisplay() override;
    uint32_t RegisterDisplayListener(std::shared_ptr<DisplayListenerAdapter> listener) override;
    bool UnregisterDisplayListener(uint32_t id) override;
    bool IsDefaultPortrait() override;
    static void DisplayChangeCallback(uint64_t displayId);
private:
    static ListenerMap reg_;
};
}

#endif

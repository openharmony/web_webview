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

#ifndef ARK_DISPLAY_ADAPTER_WRAPPER_H
#define ARK_DISPLAY_ADAPTER_WRAPPER_H
#pragma once

#include "display_manager_adapter.h"
#include "ohos_adapter/include/ark_display_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkDisplayAdapterWrapper : public OHOS::NWeb::DisplayAdapter {
public:
    explicit ArkDisplayAdapterWrapper(ArkWebRefPtr<ArkDisplayAdapter>);

    OHOS::NWeb::DisplayId GetId() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    float GetVirtualPixelRatio() override;

    OHOS::NWeb::RotationType GetRotation() override;

    OHOS::NWeb::OrientationType GetOrientation() override;

    int32_t GetDpi() override;

    OHOS::NWeb::DisplayOrientation GetDisplayOrientation() override;

    OHOS::NWeb::FoldStatus GetFoldStatus() override;

    bool IsFoldable() override;

    std::string GetName() override;

    int32_t GetAvailableWidth() override;

    int32_t GetAvailableHeight() override;

    bool GetAliveStatus() override;

    OHOS::NWeb::DisplayState GetDisplayState() override;

    int32_t GetDensityDpi() override;

    int32_t GetX() override;

    int32_t GetY() override;

    OHOS::NWeb::DisplaySourceMode GetDisplaySourceMode() override;

    int32_t GetPhysicalWidth() override;

    int32_t GetPhysicalHeight() override;

    float GetDefaultVirtualPixelRatio() override;
private:
    ArkWebRefPtr<ArkDisplayAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_ADAPTER_WRAPPER_H

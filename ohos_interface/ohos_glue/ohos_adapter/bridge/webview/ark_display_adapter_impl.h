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

#ifndef ARK_DISPLAY_ADAPTER_IMPL_H
#define ARK_DISPLAY_ADAPTER_IMPL_H
#pragma once

#include "display_manager_adapter.h"
#include "ohos_adapter/include/ark_display_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkDisplayAdapterImpl : public ArkDisplayAdapter {
public:
    explicit ArkDisplayAdapterImpl(std::shared_ptr<OHOS::NWeb::DisplayAdapter>);

    uint64_t GetId() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    float GetVirtualPixelRatio() override;

    uint32_t GetRotation() override;

    uint32_t GetOrientation() override;

    int32_t GetDpi() override;

    uint32_t GetDisplayOrientation() override;

    uint32_t GetFoldStatus() override;

    bool IsFoldable() override;

	ArkWebString GetName() override;

    int32_t GetAvailableWidth() override;

    int32_t GetAvailableHeight() override;

    bool GetAliveStatus() override;

    uint32_t GetDisplayState() override;

    int32_t GetDensityDpi() override;

    int32_t GetX() override;

    int32_t GetY() override;

    uint32_t GetDisplaySourceMode() override;

    int32_t GetPhysicalWidth() override;

    int32_t GetPhysicalHeight() override;

    float GetDefaultVirtualPixelRatio() override;

private:
    std::shared_ptr<OHOS::NWeb::DisplayAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkDisplayAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_ADAPTER_IMPL_H

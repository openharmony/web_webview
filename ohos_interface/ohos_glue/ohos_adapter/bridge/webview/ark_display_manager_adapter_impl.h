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

#ifndef ARK_DISPLAY_MANAGER_ADAPTER_IMPL_H
#define ARK_DISPLAY_MANAGER_ADAPTER_IMPL_H
#pragma once

#include "display_manager_adapter.h"
#include "ohos_adapter/include/ark_display_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkDisplayManagerAdapterImpl : public ArkDisplayManagerAdapter {
public:
    explicit ArkDisplayManagerAdapterImpl(std::shared_ptr<OHOS::NWeb::DisplayManagerAdapter>);

    uint64_t GetDefaultDisplayId() override;

    ArkWebRefPtr<ArkDisplayAdapter> GetDefaultDisplay() override;

    uint32_t RegisterDisplayListener(ArkWebRefPtr<ArkDisplayListenerAdapter> listener) override;

    bool UnregisterDisplayListener(uint32_t id) override;

    bool IsDefaultPortrait() override;

    uint32_t RegisterFoldStatusListener(ArkWebRefPtr<ArkFoldStatusListenerAdapter> listener) override;

    bool UnregisterFoldStatusListener(uint32_t id) override;

    ArkWebRefPtr<ArkDisplayAdapter> GetPrimaryDisplay() override;

    ArkDisplayAdapterVector GetAllDisplays() override;

private:
    std::shared_ptr<OHOS::NWeb::DisplayManagerAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkDisplayManagerAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_MANAGER_ADAPTER_IMPL_H

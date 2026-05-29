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

#ifndef ARK_DISPLAY_MANAGER_ADAPTER_WRAPPER_H
#define ARK_DISPLAY_MANAGER_ADAPTER_WRAPPER_H
#pragma once

#include <vector>

#include "display_manager_adapter.h"
#include "ohos_adapter/include/ark_display_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkDisplayManagerAdapterWrapper : public OHOS::NWeb::DisplayManagerAdapter {
public:
    explicit ArkDisplayManagerAdapterWrapper(ArkWebRefPtr<ArkDisplayManagerAdapter>);

    OHOS::NWeb::DisplayId GetDefaultDisplayId() override;

    std::shared_ptr<OHOS::NWeb::DisplayAdapter> GetDefaultDisplay() override;

    uint32_t RegisterDisplayListener(std::shared_ptr<OHOS::NWeb::DisplayListenerAdapter> listener) override;

    bool UnregisterDisplayListener(uint32_t id) override;

    bool IsDefaultPortrait() override;

    uint32_t RegisterFoldStatusListener(std::shared_ptr<OHOS::NWeb::FoldStatusListenerAdapter> listener) override;

    bool UnregisterFoldStatusListener(uint32_t id) override;

    std::shared_ptr<OHOS::NWeb::DisplayAdapter> GetPrimaryDisplay() override;

    std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>> GetAllDisplays() override;

private:
    ArkWebRefPtr<ArkDisplayManagerAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_MANAGER_ADAPTER_WRAPPER_H

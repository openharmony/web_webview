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

#ifndef ARK_POWER_MGR_CLIENT_ADAPTER_WRAPPER_H
#define ARK_POWER_MGR_CLIENT_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_power_mgr_client_adapter.h"
#include "power_mgr_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkPowerMgrClientAdapterWrapper : public OHOS::NWeb::PowerMgrClientAdapter {
public:
    ArkPowerMgrClientAdapterWrapper(ArkWebRefPtr<ArkPowerMgrClientAdapter>);

    std::shared_ptr<OHOS::NWeb::RunningLockAdapter> CreateRunningLock(
        const std::string& name, OHOS::NWeb::RunningLockAdapterType type) override;

private:
    ArkWebRefPtr<ArkPowerMgrClientAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_POWER_MGR_CLIENT_ADAPTER_WRAPPER_H
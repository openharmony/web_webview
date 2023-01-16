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

#ifndef POWER_MGR_CLIENT_ADAPTER_IMPL_H
#define POWER_MGR_CLIENT_ADAPTER_IMPL_H

#include "power_mgr_client.h"
#include "power_mgr_client_adapter.h"
#include "running_lock.h"

namespace OHOS::NWeb {
class RunningLockAdapterImpl : public RunningLockAdapter {
public:
    explicit RunningLockAdapterImpl(std::shared_ptr<OHOS::PowerMgr::RunningLock> lock);
    ~RunningLockAdapterImpl() override = default;
    bool IsUsed() override;
    int32_t Lock(uint32_t timeOutMs = 0) override;
    int32_t UnLock() override;
private:
    std::shared_ptr<OHOS::PowerMgr::RunningLock> lock_;
};

class PowerMgrClientAdapterImpl : public PowerMgrClientAdapter {
public:
    PowerMgrClientAdapterImpl() = default;
    ~PowerMgrClientAdapterImpl() override = default;
    std::shared_ptr<RunningLockAdapter> CreateRunningLock(
        const std::string& name, RunningLockAdapterType type) override;
};
}

#endif
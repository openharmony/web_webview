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

#include "nwebpowermgrrunlock_fuzzer.h"
#include <securec.h>

#define private public
#include "power_mgr_client_adapter_impl.h"

using namespace OHOS::PowerMgr;
using namespace OHOS::NWeb;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }

        uint32_t time = 0;
        if (memcpy_s(&time, sizeof(uint32_t), data, sizeof(uint32_t)) != 0) {
            return true;
        }

        std::unique_ptr<PowerMgrClientAdapterImpl> powerMgrImpl =
            std::make_unique<PowerMgrClientAdapterImpl>();
        std::shared_ptr<RunningLockAdapter> createdRunLock =
            powerMgrImpl->CreateRunningLock("screen", RunningLockAdapterType::SCREEN);
        createdRunLock->IsUsed();
        createdRunLock->Lock(time);
        createdRunLock->UnLock();
        std::shared_ptr<RunningLockAdapter> runLockAdapter =
            std::make_shared<RunningLockAdapterImpl>(nullptr);
        runLockAdapter->IsUsed();
        runLockAdapter->Lock(time);
        runLockAdapter->UnLock();
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

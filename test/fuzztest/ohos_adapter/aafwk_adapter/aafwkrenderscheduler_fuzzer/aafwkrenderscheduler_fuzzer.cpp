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

#include "aafwkrenderscheduler_fuzzer.h"

#include <cstring>
#include <securec.h>

#define private public
#include "aafwk_app_mgr_client_adapter_impl.h"
#include "aafwk_browser_host_impl.h"
#include "aafwk_render_scheduler_impl.h"
#undef private

#include "aafwk_render_scheduler_host_adapter.h"
#include "app_mgr_client.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
using namespace OHOS::AppExecFwk;

namespace OHOS {
bool AafwkRenderSchedulerFuzzTest(const uint8_t* data, size_t size)
{
    constexpr int fdNum = 3;
    if ((data == nullptr) || size < fdNum * sizeof(int32_t)) {
        return false;
    }
    std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter = nullptr;
    AafwkRenderSchedulerImpl render(adapter);
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    int32_t crashFd = 0;
    if (memcpy_s(&ipcFd, sizeof(int32_t), data, sizeof(int32_t)) != 0) {
        return false;
    }
    if (memcpy_s(&sharedFd, sizeof(int32_t), data + sizeof(int32_t), sizeof(int32_t)) != 0) {
        return false;
    }

    if (memcpy_s(&crashFd, sizeof(int32_t), data + sizeof(int32_t) + sizeof(int32_t), sizeof(int32_t)) != 0) {
        return false;
    }

    render.NotifyBrowserFd(ipcFd, sharedFd, crashFd, nullptr);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AafwkRenderSchedulerFuzzTest(data, size);
    return 0;
}

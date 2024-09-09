/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "vsyncadapter_fuzzer.h"

#include <cstring>

#define private public
#include "vsync_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;

namespace OHOS {
static void OnVsyncCallback() {}

bool CameraManagerAdapterFuzzTest(const uint8_t* data, size_t size)
{
    VSyncAdapterImpl& adapter = VSyncAdapterImpl::GetInstance();
    adapter.Init();
    adapter.Init();

    VSyncAdapterImpl vsyncAdapter;
    void* client = nullptr;
    adapter.OnVsync(1, client);
    client = &vsyncAdapter;
    adapter.OnVsync(1, client);
    adapter.VsyncCallbackInner(1);
    adapter.GetVSyncPeriod();
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFramePreferredRate(0);
    adapter.frameRateLinker_ = nullptr;
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFramePreferredRate(0);
    adapter.vsyncHandler_ = nullptr;
    adapter.SetOnVsyncCallback(OnVsyncCallback);
    adapter.SetOnVsyncEndCallback(OnVsyncCallback);
    adapter.OnVsync(1, client);
    adapter.SetIsGPUProcess(false);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::CameraManagerAdapterFuzzTest(data, size);
    return 0;
}

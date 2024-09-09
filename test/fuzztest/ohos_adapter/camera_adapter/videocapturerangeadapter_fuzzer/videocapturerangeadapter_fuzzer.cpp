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

#include "videocapturerangeadapter_fuzzer.h"

#define private public
#include "camera_manager_adapter_impl.h"
#include "video_capture_range_adapter_impl.cpp"
#include "video_capture_range_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {

bool VideoCaptureRangeAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    OHOS::NWeb::VideoCaptureRangeAdapterImpl adapter;
    adapter.SetMin(0.0);
    adapter.SetMax(1.0);
    adapter.SetStep(1.0);
    adapter.SetCurrent(0.0);

    adapter.GetMin();
    adapter.GetMax();
    adapter.GetStep();
    adapter.GetCurrent();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::VideoCaptureRangeAdapterFuzzTest(data, size);
    return 0;
}

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

#include "capabilitydataadapterimpl_fuzzer.h"

#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "capability_data_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

bool CapabilityDataAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    OHOS::NWeb::CapabilityDataAdapterImpl capabilityDataAdapter;

    FuzzedDataProvider dataProvider(data, size);
    int32_t width = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    int32_t height = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    int32_t frameRate = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);

    capabilityDataAdapter.SetMaxWidth(width);
    capabilityDataAdapter.SetMaxHeight(height);
    capabilityDataAdapter.SetMaxframeRate(frameRate);

    capabilityDataAdapter.GetMaxWidth();
    capabilityDataAdapter.GetMaxHeight();
    capabilityDataAdapter.GetMaxframeRate();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::CapabilityDataAdapterImplFuzzTest(data, size);
    return 0;
}
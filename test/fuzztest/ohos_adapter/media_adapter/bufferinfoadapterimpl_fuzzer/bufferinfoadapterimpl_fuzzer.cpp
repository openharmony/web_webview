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

#include "bufferinfoadapterimpl_fuzzer.h"

#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "buffer_info_adapter_impl.h"


using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

bool BufferInfoAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    OHOS::NWeb::BufferInfoAdapterImpl bufferInfoAdapterImpl;
    int64_t time = 1;

    FuzzedDataProvider dataProvider(data, size);

    int32_t usesize = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    int32_t offset = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);

    bufferInfoAdapterImpl.SetPresentationTimeUs(time);
    bufferInfoAdapterImpl.SetSize(usesize);
    bufferInfoAdapterImpl.SetOffset(offset);

    bufferInfoAdapterImpl.GetPresentationTimeUs();
    bufferInfoAdapterImpl.GetSize();
    bufferInfoAdapterImpl.GetOffset();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    
    OHOS::BufferInfoAdapterImplFuzzTest(data, size);
    return 0;
}
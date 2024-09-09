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

#include "flowbufferadapter_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>

#include "ohos_adapter_helper.h"
#define private public
#include "flowbuffer_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool FlowBufferAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    int fd = 0;
    size_t scriptLength = 10;
    flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    flowbufferAdapter->StartPerformanceBoost();
    close(fd);

    auto flowbufferAdapter1 = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    int fd1 = 1;
    size_t scriptLength1 = 10;
    flowbufferAdapter1->CreateAshmemWithFd(fd1, scriptLength1, PROT_READ);
    flowbufferAdapter1->StartPerformanceBoost();
    close(fd1);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FlowBufferAdapterFuzzTest(data, size);
    return 0;
}

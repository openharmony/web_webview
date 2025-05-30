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

#include "audiointerrupt_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "audio_renderer_adapter_impl.h"
#include "audio_system_manager_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

bool AudioInterruptFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::shared_ptr<AudioManagerCallbackAdapter> cb = nullptr;
    AudioManagerCallbackAdapterImpl adapter(cb);
    InterruptAction interruptAction;
    interruptAction.interruptHint = InterruptHint::INTERRUPT_HINT_PAUSE;
    adapter.OnInterrupt(interruptAction);
    interruptAction.interruptHint = InterruptHint::INTERRUPT_HINT_STOP;
    adapter.OnInterrupt(interruptAction);
    interruptAction.interruptHint = InterruptHint::INTERRUPT_HINT_RESUME;
    adapter.OnInterrupt(interruptAction);
    interruptAction.interruptHint = static_cast<InterruptHint>(-1);
    adapter.OnInterrupt(interruptAction);
    FuzzedDataProvider dataProvider(data, size);
    int32_t InterruptHintValue = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    interruptAction.interruptHint = static_cast<InterruptHint>(InterruptHintValue);
    adapter.OnInterrupt(interruptAction);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AudioInterruptFuzzTest(data, size);
    return 0;
}

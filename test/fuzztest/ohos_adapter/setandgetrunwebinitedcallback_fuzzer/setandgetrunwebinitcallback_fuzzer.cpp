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

#include "setandgetrunwebinitcallback_fuzzer.h"

#include "ohos_init_web_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
bool SetAndGetCallbackFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(WebRunInitedCallback*))) {
        return false;
    }

    // Assuming data is a pointer to a valid WebRunInitedCallback if not nullptr
    WebRunInitedCallback* callback = reinterpret_cast<WebRunInitedCallback*>(const_cast<uint8_t*>(data));

    // Create instance of OhosInitWebAdapterImpl
    std::shared_ptr<OhosInitWebAdapterImpl> webAdapterImpl = std::make_shared<OhosInitWebAdapterImpl>();

    // Set and get the callback
    webAdapterImpl->SetRunWebInitedCallback(callback);
    webAdapterImpl->GetRunWebInitedCallback();

    // Optionally if returnedCallback is callable, call its run method.

    return true;
}
} // namespace OHOS::NWeb

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data
    OHOS::NWeb::SetAndGetCallbackFuzzTest(data, size);
    return 0;
}
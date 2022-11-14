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

#include "ondeviceremoved_fuzzer.h"

#include <cstring>
#include <securec.h>
#include "mmi_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
class MMIListenerTest : public MMIListenerAdapter {
public:
    MMIListenerTest() = default;
    virtual ~MMIListenerTest() = default;
    void OnDeviceAdded(int32_t deviceId, const std::string &type) override {};
    void OnDeviceRemoved(int32_t deviceId, const std::string &type) override {};
};
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size < sizeof(int32_t))) {
            return false;
        }
        std::shared_ptr<MMIListenerAdapter> listener = std::make_shared<MMIListenerTest>();
        auto listenerAdapterImpl = std::make_shared<MMIListenerAdapterImpl>(listener);
        int32_t deviceId;
        if (memcpy_s(&deviceId, sizeof(int32_t), data, sizeof(int32_t)) != 0) {
            return false;
        }
        std::string type((const char*) data, size);
        listenerAdapterImpl->OnDeviceRemoved(deviceId, type);
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

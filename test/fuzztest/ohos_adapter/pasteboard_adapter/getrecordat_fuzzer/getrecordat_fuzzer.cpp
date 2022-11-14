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

#include "getrecordat_fuzzer.h"
#include "pasteboard_client_adapter_impl.h"
#include <securec.h>
using namespace OHOS::NWeb;
using namespace OHOS::MiscServices;
namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size < sizeof(size_t))) {
            return false;
        }
        std::size_t index = 0;
        if (memcpy_s(&index, sizeof(size_t), data, sizeof(size_t)) != 0) {
            return false;
        }
        std::shared_ptr<PasteData> dataName = std::make_shared<PasteData>();
        std::shared_ptr<PasteDataAdapterImpl> dataAdapterImpl = std::make_shared<PasteDataAdapterImpl>(dataName);
        dataAdapterImpl->GetRecordAt(index);
        dataAdapterImpl->GetRecordCount();
        dataAdapterImpl->AllRecords();
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

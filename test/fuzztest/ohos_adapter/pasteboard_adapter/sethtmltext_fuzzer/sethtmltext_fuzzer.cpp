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

#include "sethtmltext_fuzzer.h"
#define private public
#include "pasteboard_client_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;
using namespace OHOS::MiscServices;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }
        std::shared_ptr<PasteDataRecord> record = std::make_shared<PasteDataRecord>();
        std::shared_ptr<PasteDataRecordAdapterImpl> dataRecordAdapterImpl =
        std::make_shared<PasteDataRecordAdapterImpl>(record);
        dataRecordAdapterImpl->record_=nullptr;
        std::string text((const char*) data, size);
        std::shared_ptr<PasteDataRecordAdapter> dataRecordAdapter = PasteDataRecordAdapter::NewRecord(text);
        std::shared_ptr<std::string> htmlText = std::make_shared<std::string>(text);
        dataRecordAdapter->SetHtmlText(htmlText);
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

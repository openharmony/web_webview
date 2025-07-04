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

#include "addhtmlrecord_fuzzer.h"

#include "pasteboard_client_adapter_impl.h"

#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;

namespace OHOS {
bool AddHtmlRecordFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string html = dataProvider.ConsumeRandomLengthString(size);
    std::shared_ptr<PasteDataAdapterImpl> dataAdapterImpl = std::make_shared<PasteDataAdapterImpl>();
    dataAdapterImpl->AddHtmlRecord(html);
    dataAdapterImpl->AddTextRecord(html);
    dataAdapterImpl->GetMimeTypes();
    dataAdapterImpl->GetPrimaryHtml();
    dataAdapterImpl->GetPrimaryText();
    dataAdapterImpl->GetPrimaryMimeType();
    dataAdapterImpl->AllRecords();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AddHtmlRecordFuzzTest(data, size);
    return 0;
}

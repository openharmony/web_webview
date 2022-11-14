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

#include "getimgdata_fuzzer.h"
#define private public
#include "pasteboard_client_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }
        std::string text((const char*) data, size);
        ClipBoardImageData image;
        std::shared_ptr<PasteDataRecordAdapterImpl>  dataRecordAdapterImpl =
        std::make_shared<PasteDataRecordAdapterImpl>(text);
        dataRecordAdapterImpl->GetImgData(image);
        dataRecordAdapterImpl->GetUri();
        dataRecordAdapterImpl->GetCustomData();
        dataRecordAdapterImpl->ClearImgBuffer();
        dataRecordAdapterImpl->Clear();
        dataRecordAdapterImpl->GetRecord();
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

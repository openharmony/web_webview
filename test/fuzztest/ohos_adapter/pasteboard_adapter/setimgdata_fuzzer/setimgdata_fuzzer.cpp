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

#include "setimgdata_fuzzer.h"
#include "pasteboard_client_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        constexpr int ROE_BYTES = 5;
        if ((data == nullptr) || (size == 0)) {
            return false;
        }
        std::shared_ptr<PasteDataRecordAdapterImpl>  dataRecordAdapterImpl =
        std::make_shared<PasteDataRecordAdapterImpl>("pixelMap");
        uint32_t storage[][ROE_BYTES] = {
            {0xCA, 0xDA, 0xCA, 0xC9, 0xA3},
            {0xAC, 0xA8, 0x89, 0x47, 0x87},
            {0x4B, 0x25, 0x25, 0x25, 0x46},
            {0x90, 0x1, 0x25, 0x41, 0x33},
            {0x75, 0x55, 0x44, 0x20, 0x00},
        };
        ClipBoardImageData *image = new ClipBoardImageData;
        image->colorType = ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
        image->alphaType = ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
        image->data = storage[0];
        image->dataSize = sizeof(storage);
        image->rowBytes = ROE_BYTES;
        image->width = ROE_BYTES;
        image->height = ROE_BYTES;
        std::shared_ptr<ClipBoardImageData> imageData(image);
        dataRecordAdapterImpl->SetImgData(imageData);
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

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

#include "ohosimagedecoderadapterimpl_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>

#include "image_source.h"
#include "image_type.h"
#include "media_errors.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
const std::string DEFAULT_MOUSE_DRAG_IMAGE { "/system/etc/device_status/drag_icon/Copy_Drag.svg" };

namespace OHOS {

bool ValidateInput(const uint8_t* data, size_t size)
{
    return (data != nullptr) && (size >= sizeof(int32_t));
}

std::shared_ptr<OhosImageDecoderAdapter> CreateDecoderAdapter()
{
    return OhosAdapterHelper::GetInstance().CreateOhosImageDecoderAdapter();
}

void HandleAbnormalCase(const std::shared_ptr<OhosImageDecoderAdapter>& adapter)
{
    size_t len = 0;
    std::unique_ptr<uint8_t[]> rawData;

    adapter->ParseImageInfo(rawData.get(), len);
    adapter->DecodeToPixelMap(rawData.get(), len);
}

bool ProcessImageSource(std::shared_ptr<OhosImageDecoderAdapter>& adapter)
{
    uint32_t errorCode = 0;
    OHOS::Media::SourceOptions opts;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(DEFAULT_MOUSE_DRAG_IMAGE, opts, errorCode);
    if (!imageSource || errorCode != Media::SUCCESS) {
        return false;
    }

    OHOS::Media::DecodeOptions decodeOpts;
    auto pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    if (!pixelMap || errorCode != OHOS::Media::SUCCESS) {
        return false;
    }

    const uint8_t* imageData = pixelMap->GetPixels();
    if (!imageData) {
        return false;
    }

    size_t len = pixelMap->GetPixelBytes();
    adapter->ParseImageInfo(imageData, len);
    adapter->DecodeToPixelMap(imageData, len);
    return true;
}

bool ApplyOhosImageDecoderAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if (!ValidateInput(data, size)) {
        return false;
    }

    auto adapter = CreateDecoderAdapter();
    if (!adapter) {
        return false;
    }

    HandleAbnormalCase(adapter);
    return ProcessImageSource(adapter);
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ApplyOhosImageDecoderAdapterFuzzTest(data, size);
    return 0;
}

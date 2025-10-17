/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohosimagedecoderadapterimplgetmethods_fuzzer.h"

#include <securec.h>
#include <sys/mman.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "image_source.h"
#include "image_type.h"
#include "media_errors.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;
const std::string DEFAULT_MOUSE_DRAG_IMAGE { "/system/etc/device_status/drag_icon/Copy_Drag.svg" };

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;
std::shared_ptr<OhosImageDecoderAdapter> CreateDecoderAdapter()
{
    return OhosAdapterHelper::GetInstance().CreateOhosImageDecoderAdapter();
}

bool GetParameters(std::shared_ptr<OhosImageDecoderAdapter>& adapter, const uint8_t* data, size_t size)
{
    FuzzedDataProvider dataProvider(data, size);
    uint32_t errorCode = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_SET_NUMBER);
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
    adapter->GetEncodedFormat();
    adapter->GetImageWidth();
    adapter->GetImageHeight();
    adapter->GetFd();
    adapter->GetStride();
    adapter->GetOffset();
    adapter->GetSize();
    adapter->GetNativeWindowBuffer();
    adapter->GetPlanesCount();
    adapter->GetDecodeData();

    adapter->DecodeToPixelMap(imageData, len);
    adapter->GetEncodedFormat();
    adapter->GetImageWidth();
    adapter->GetImageHeight();
    adapter->GetFd();
    adapter->GetStride();
    adapter->GetOffset();
    adapter->GetSize();
    adapter->GetNativeWindowBuffer();
    adapter->GetPlanesCount();
    adapter->GetDecodeData();
    adapter->ReleasePixelMap();
    return true;
}

bool ApplyOhosImageDecoderAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    size_t callCount = data[0] % 10;
    for (size_t i = 0; i < callCount; ++i) {
        auto adapter = CreateDecoderAdapter();
        if (!adapter) {
            return false;
        }
        GetParameters(adapter, data, size);
    }
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::ApplyOhosImageDecoderAdapterFuzzTest(data, size);
    return 0;
}

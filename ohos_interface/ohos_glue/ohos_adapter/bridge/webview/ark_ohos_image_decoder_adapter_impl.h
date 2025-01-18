/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_OHOS_IMAGE_DECODER_ADAPTER_IMPL_H
#define ARK_OHOS_IMAGE_DECODER_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_ohos_image_decoder_adapter.h"
#include "ohos_image_decoder_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosImageDecoderAdapterImpl : public ArkOhosImageDecoderAdapter {
public:
    explicit ArkOhosImageDecoderAdapterImpl(std::shared_ptr<OHOS::NWeb::OhosImageDecoderAdapter>);

    bool ParseImageInfo(const uint8_t* data, uint32_t size) override;
    ArkWebString GetEncodedFormat() override;
    int32_t GetImageWidth() override;
    int32_t GetImageHeight() override;
    bool DecodeToPixelMap(const uint8_t* data, uint32_t size) override;
    bool Decode(const uint8_t* data, uint32_t size, uint32_t type, bool useYuv) override;
    int32_t GetFd() override;
    int32_t GetStride() override;
    int32_t GetOffset() override;
    uint64_t GetSize() override;
    void* GetNativeWindowBuffer() override;
    int32_t GetPlanesCount() override;
    void ReleasePixelMap() override;
    void* GetDecodeData() override;

private:
    std::shared_ptr<OHOS::NWeb::OhosImageDecoderAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkOhosImageDecoderAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif //  ARK_OHOS_IMAGE_DECODER_ADAPTER_IMPL_H

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

#ifndef OHOS_IMAGE_DECODER_ADAPTER_H
#define OHOS_IMAGE_DECODER_ADAPTER_H

#include <string>

namespace OHOS::NWeb {

enum class AllocatorType : int32_t {
    kDefault = 0,
    kHeapAlloc = 1,
    kShareMemAlloc = 2,
    kCustomAlloc = 3,
    kDmaAlloc = 4,
};

class OhosImageDecoderAdapter {
public:
    OhosImageDecoderAdapter() = default;

    virtual ~OhosImageDecoderAdapter() = default;

    virtual bool ParseImageInfo(const uint8_t* data, uint32_t size) = 0;

    virtual std::string GetEncodedFormat() = 0;

    virtual int32_t GetImageWidth() = 0;

    virtual int32_t GetImageHeight() = 0;

    virtual bool DecodeToPixelMap(const uint8_t* data, uint32_t size) = 0;

    virtual int32_t GetFd() = 0;

    virtual int32_t GetStride() = 0;

    virtual int32_t GetOffset() = 0;

    virtual uint64_t GetSize() = 0;

    virtual void* GetNativeWindowBuffer() = 0;

    virtual int32_t GetPlanesCount() = 0;

    virtual void ReleasePixelMap() = 0;

    virtual bool Decode(const uint8_t* data,
                              uint32_t size,
                              AllocatorType type,
                              bool useYuv) = 0;

    virtual void* GetDecodeData() = 0;

    // decode heic by path
    virtual bool DecodeByPath(const std::string& path, AllocatorType type) { return false; };
};

}  // namespace OHOS::NWeb

#endif  // OHOS_IMAGE_DECODER_ADAPTER_H
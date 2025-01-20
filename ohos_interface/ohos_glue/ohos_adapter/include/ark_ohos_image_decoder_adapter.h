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

#ifndef ARK_OHOS_IMAGE_DECODER_ADAPTER_H
#define ARK_OHOS_IMAGE_DECODER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkOhosImageDecoderAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool ParseImageInfo(const uint8_t* data, uint32_t size) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetEncodedFormat() = 0;

    /*--ark web()--*/
    virtual int32_t GetImageWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetImageHeight() = 0;

    /*--ark web()--*/
    virtual bool DecodeToPixelMap(const uint8_t* data, uint32_t size) = 0;

    /*--ark web()--*/
    virtual int32_t GetFd() = 0;

    /*--ark web()--*/
    virtual int32_t GetStride() = 0;

    /*--ark web()--*/
    virtual int32_t GetOffset() = 0;

    /*--ark web()--*/
    virtual uint64_t GetSize() = 0;

    /*--ark web()--*/
    virtual void* GetNativeWindowBuffer() = 0;

    /*--ark web()--*/
    virtual int32_t GetPlanesCount() = 0;

    /*--ark web()--*/
    virtual void ReleasePixelMap() = 0;

    /*--ark web()--*/
    virtual bool Decode(const uint8_t* data,
                        uint32_t size,
                        uint32_t type,
                        bool useYuv) = 0;

    /*--ark web()--*/
    virtual void* GetDecodeData() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_IMAGE_DECODER_ADAPTER_H

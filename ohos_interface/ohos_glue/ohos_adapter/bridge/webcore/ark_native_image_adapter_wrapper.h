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

#ifndef ARK_NATIVE_IMAGE_ADAPTER_WRAPPER_H
#define ARK_NATIVE_IMAGE_ADAPTER_WRAPPER_H
#pragma once

#include "graphic_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

class ArkNativeImageAdapterWrapper : public OHOS::NWeb::NativeImageAdapter {
public:
    explicit ArkNativeImageAdapterWrapper(ArkWebRefPtr<ArkNativeImageAdapter>);

    void CreateNativeImage(uint32_t textureId, uint32_t textureTarget) override;

    NWeb::NWebNativeWindow AquireNativeWindowFromNativeImage() override;

    int32_t AttachContext(uint32_t textureId) override;

    int32_t DetachContext() override;

    int32_t UpdateSurfaceImage() override;

    int64_t GetTimestamp() override;

    int32_t GetTransformMatrix(float matrix[16]) override;

    int32_t GetSurfaceId(uint64_t* surfaceId) override;

    int32_t SetOnFrameAvailableListener(std::shared_ptr<OHOS::NWeb::FrameAvailableListener> listener) override;

    int32_t UnsetOnFrameAvailableListener() override;

    void DestroyNativeImage() override;

    void NewNativeImage() override;

    int32_t AcquireNativeWindowBuffer(
        void** windowBuffer,
        int* acquireFenceFd) override;

    int32_t GetNativeBuffer(
        void* windowBuffer,
        void** nativeBuffer) override;

    int32_t ReleaseNativeWindowBuffer(void* windowBuffer, int fenceFd) override;

    void GetNativeWindowBufferSize(
        void* windowBuffer,
        uint32_t* width,
        uint32_t* height) override;

private:
    ArkWebRefPtr<ArkNativeImageAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_NATIVE_IMAGE_ADAPTER_WRAPPER_H

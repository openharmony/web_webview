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

#include "nativeimageadapter_fuzzer.h"

#include <cstring>

#define private public
#include "native_image_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;

namespace OHOS {

bool NativeImageAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    NativeImageAdapterImpl adapter;

    void** windowBuffer = nullptr;
    int* acquireFenceFd = nullptr;
    uint32_t textureId = 0;
    uint32_t textureTarget = 0;
    void* windowBuffer2 = nullptr;
    void** nativeBuffer = nullptr;
    uint32_t* width = nullptr;
    uint32_t* height = nullptr;
    uint64_t* surfaceId = nullptr;
    float matrix[16] = {0};
    std::shared_ptr<FrameAvailableListener> listener = nullptr;
    int fenceFd = 0;

    adapter.AcquireNativeWindowBuffer(windowBuffer, acquireFenceFd);
    adapter.AquireNativeWindowFromNativeImage();
    adapter.AttachContext(textureId);
    adapter.CreateNativeImage(textureId, textureTarget);
    adapter.DestroyNativeImage();
    adapter.DetachContext();
    adapter.GetNativeBuffer(windowBuffer2, nativeBuffer);
    adapter.GetNativeWindowBufferSize(windowBuffer2, width, height);
    adapter.GetSurfaceId(surfaceId);
    adapter.GetTimestamp();
    adapter.UpdateSurfaceImage();
    adapter.GetTransformMatrix(matrix);
    adapter.SetOnFrameAvailableListener(listener);
    adapter.UnsetOnFrameAvailableListener();
    adapter.NewNativeImage();
    adapter.ReleaseNativeWindowBuffer(windowBuffer2, fenceFd);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::NativeImageAdapterFuzzTest(data, size);
    return 0;
}

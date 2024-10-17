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

#include "nativeImageadapter_fuzzer.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <securec.h>

#include "native_image_adapter_impl.h"

namespace OHOS {

using namespace OHOS::NWeb;

class DummyFrameAvailableListener : public FrameAvailableListener {
public:
    void (*OnFrameAvailableCallback)() = nullptr;

    void SetOnFrameAvailableCallback(void (*callback)())
    {
        OnFrameAvailableCallback = callback;
    }

    void OnFrameAvailable()
    {
        if (OnFrameAvailableCallback) {
            OnFrameAvailableCallback();
        }
    }

    void* GetContext() override
    {
        return nullptr;
    }

    OnFrameAvailableCb GetOnFrameAvailableCb() override
    {
        return nullptr;
    }
};

bool NativeImageFuzzTest(const uint8_t* data, size_t size)
{
    NativeImageAdapterImpl adapter;

    uint32_t textureId = 1234;
    uint32_t textureTarget = 5678;
    adapter.CreateNativeImage(textureId, textureTarget);

    NWebNativeWindow window = adapter.AquireNativeWindowFromNativeImage();
    if (window == nullptr) {
        return false;
    }

    adapter.AttachContext(textureId);

    adapter.GetTimestamp();

    adapter.UpdateSurfaceImage();

    float matrix[16] = { 0 };
    int32_t result = adapter.GetTransformMatrix(matrix);
    if (result != 0) {
        return false;
    }

    uint64_t surfaceId;
    result = adapter.GetSurfaceId(&surfaceId);
    if (result != 0) {
        return false;
    }

    auto listener = std::make_shared<DummyFrameAvailableListener>();
    listener->SetOnFrameAvailableCallback([]() {});

    adapter.SetOnFrameAvailableListener(listener);
    adapter.UnsetOnFrameAvailableListener();
    adapter.DestroyNativeImage();

    return true;
}

} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    return OHOS::NativeImageFuzzTest(data, size);
}
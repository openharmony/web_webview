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

#ifndef SCREEN_CAPTURE_ADAPTER_IMPL_H
#define SCREEN_CAPTURE_ADAPTER_IMPL_H

#include <graphic_adapter.h>
#include <screen_capture_adapter.h>
#include <multimedia/player_framework/native_avscreen_capture.h>
#include <native_buffer/native_buffer.h>
#include <queue>

namespace OHOS::NWeb {
void ScreenCaptureCallbackOnError(OH_AVScreenCapture *capture, int32_t errorCode, void* userData);

void ScreenCaptureCallbackOnBufferAvailable(OH_AVScreenCapture *capture, OH_AVBuffer *buffer,
    OH_AVScreenCaptureBufferType bufferType, int64_t timestamp, void* userData);

void ScreenCaptureCallbackOnStateChange(struct OH_AVScreenCapture *capture,
    OH_AVScreenCaptureStateCode stateCode, void* userData);

class OH_SurfaceBufferAdapterImpl : public SurfaceBufferAdapter {
private:
    void* avBuffer_ = nullptr;
    OH_NativeBuffer_Config config_;
    uint32_t size_ = 0;

public:
    explicit OH_SurfaceBufferAdapterImpl(OH_AVBuffer* avBuffer, OH_NativeBuffer_Config config);

    ~OH_SurfaceBufferAdapterImpl() override;

    int32_t GetFileDescriptor() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetStride() override;

    int32_t GetFormat() override;

    uint32_t GetSize() override;

    void* GetVirAddr() override;
};

class ScreenCaptureAdapterImpl : public ScreenCaptureAdapter {
public:
    ScreenCaptureAdapterImpl() = default;
    ~ScreenCaptureAdapterImpl() override;

    int32_t Init(const std::shared_ptr<ScreenCaptureConfigAdapter> config) override;

    int32_t SetMicrophoneEnable(bool enable) override;

    int32_t StartCapture() override;

    int32_t StopCapture() override;

    int32_t SetCaptureCallback(const std::shared_ptr<ScreenCaptureCallbackAdapter> callback) override;

    std::shared_ptr<SurfaceBufferAdapter> AcquireVideoBuffer() override;

    int32_t ReleaseVideoBuffer() override;

private:
    void Release();

private:
    OH_AVScreenCapture *screenCapture_;
    std::shared_ptr<ScreenCaptureCallbackAdapter> callback_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // SCREEN_CAPTURE_ADAPTER_IMPL_H

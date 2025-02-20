/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "screen_capture.h"
#include "screen_capture_adapter.h"

namespace OHOS::NWeb {
class OHScreenCaptureCallback : public OHOS::Media::ScreenCaptureCallBack {
public:
    OHScreenCaptureCallback(const std::shared_ptr<ScreenCaptureCallbackAdapter>& callback) : callback_(callback) {}
    virtual ~OHScreenCaptureCallback() = default;

    void OnError(OHOS::Media::ScreenCaptureErrorType errorType, int32_t errorCode) override;

    void OnAudioBufferAvailable(bool isReady, OHOS::Media::AudioCaptureSourceType type) override;

    void OnVideoBufferAvailable(bool isReady) override;

    void OnStateChange(OHOS::Media::AVScreenCaptureStateCode stateCode) override;

private:
    std::shared_ptr<ScreenCaptureCallbackAdapter> callback_;
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

    int32_t AcquireAudioBuffer(
        std::shared_ptr<AudioBufferAdapter> audiobuffer, AudioCaptureSourceTypeAdapter type) override;

    int32_t ReleaseAudioBuffer(AudioCaptureSourceTypeAdapter type) override;

private:
    void Release();

private:
    std::shared_ptr<Media::ScreenCapture> screenCapture_ = nullptr;
    std::shared_ptr<Media::ScreenCaptureCallBack> screenCaptureCallback_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // SCREEN_CAPTURE_ADAPTER_IMPL_H

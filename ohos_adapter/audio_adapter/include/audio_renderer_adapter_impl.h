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

#ifndef AUDIO_RENDERER_ADAPTER_IMPL_H
#define AUDIO_RENDERER_ADAPTER_IMPL_H

#include "audio_renderer_adapter.h"
#include <mutex>

#include <ohaudio/native_audiorenderer.h>
#include <ohaudio/native_audiostreambuilder.h>

namespace OHOS::NWeb {
typedef struct {
    int32_t samplingRate;
    int32_t frameSize;
    int32_t channelCount;
    OH_AudioStream_EncodingType encodingType;
    OH_AudioStream_SampleFormat sampleFormat;
    OH_AudioStream_Usage streamUsage;
    OH_AudioStream_LatencyMode latencyMode;
} OHAudioRendererOptions;
class AudioRendererAdapterImpl : public AudioRendererAdapter {
public:
    AudioRendererAdapterImpl() = default;

    ~AudioRendererAdapterImpl() override;

    int32_t Create(const std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions,
        std::string cachePath = std::string()) override;

    bool Start() override;

    bool Pause() override;

    bool Stop() override;

    bool Release() override;

    int32_t Write(uint8_t* buffer, size_t bufferSize) override;

    int32_t GetLatency(uint64_t& latency) override;

    int32_t SetVolume(float volume) override;

    float GetVolume() override;

    int32_t SetAudioRendererCallback(const std::shared_ptr<AudioRendererCallbackAdapter>& callback) override;

    void SetInterruptMode(bool audioExclusive) override;

    bool IsRendererStateRunning() override;

    int32_t SetAudioOutputChangeCallback(const std::shared_ptr<AudioOutputChangeCallbackAdapter>& callback) override;

    int32_t OnError(OH_AudioStream_Result error);

    int32_t OnInterruptEvent(OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint);

    OH_AudioData_Callback_Result OnWriteDataCallback(void* buffer, int32_t length);

    void OnOutputDeviceChange(OH_AudioStream_DeviceChangeReason reason);

    static int32_t GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate);

    static OH_AudioStream_EncodingType GetAudioEncodingType(AudioAdapterEncodingType encodingType);

    static OH_AudioStream_SampleFormat GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat);

    static int32_t GetAudioChannel(AudioAdapterChannel channel);

    static OH_AudioStream_Usage GetAudioStreamUsage(AudioAdapterStreamUsage streamUsage);

    static OH_AudioStream_LatencyMode GetAudioLatency(int32_t renderflag);

    static AudioAdapterDeviceChangeReason GetChangeReason(
        OH_AudioStream_DeviceChangeReason reason);

    static void TransformToAudioRendererOptions(
        OHAudioRendererOptions& out, const std::shared_ptr<AudioRendererOptionsAdapter>& in);

    void SetStream(const std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions,
                   OH_AudioStreamBuilder* audioStreamBuilder);
private:
    OH_AudioStreamBuilder* audio_stream_builder_ = nullptr;
    OH_AudioRenderer* audio_renderer_ = nullptr;
    std::shared_ptr<AudioRendererCallbackAdapter> callback_ = nullptr;
    std::shared_ptr<AudioOutputChangeCallbackAdapter> ouputChangeCallback_ = nullptr;
    double nsPerFrame_ = 0;
};
} // namespace OHOS::NWeb

#endif // AUDIO_RENDERER_ADAPTER_IMPL_H

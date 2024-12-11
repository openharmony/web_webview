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

#ifndef AUDIO_RENDERER_ADAPTER_H
#define AUDIO_RENDERER_ADAPTER_H

#include <memory>
#include <string>

namespace OHOS::NWeb {

enum class AudioAdapterSampleFormat : int32_t {
    SAMPLE_U8 = 0,
    SAMPLE_S16LE = 1,
    SAMPLE_S24LE = 2,
    SAMPLE_S32LE = 3,
    SAMPLE_F32LE = 4,
    INVALID_WIDTH = -1
};

enum class AudioAdapterSamplingRate : int32_t {
    SAMPLE_RATE_8000 = 8000,
    SAMPLE_RATE_11025 = 11025,
    SAMPLE_RATE_12000 = 12000,
    SAMPLE_RATE_16000 = 16000,
    SAMPLE_RATE_22050 = 22050,
    SAMPLE_RATE_24000 = 24000,
    SAMPLE_RATE_32000 = 32000,
    SAMPLE_RATE_44100 = 44100,
    SAMPLE_RATE_48000 = 48000,
    SAMPLE_RATE_64000 = 64000,
    SAMPLE_RATE_96000 = 96000
};

enum class AudioAdapterChannel : int32_t {
    MONO = 1,
    STEREO = 2,
    CHANNEL_3 = 3,
    CHANNEL_4 = 4,
    CHANNEL_5 = 5,
    CHANNEL_6 = 6,
    CHANNEL_7 = 7,
    CHANNEL_8 = 8
};

enum class AudioAdapterEncodingType : int32_t { ENCODING_PCM = 0, ENCODING_INVALID = -1 };

enum class AudioAdapterContentType : int32_t {
    CONTENT_TYPE_UNKNOWN = 0,
    CONTENT_TYPE_SPEECH = 1,
    CONTENT_TYPE_MUSIC = 2,
    CONTENT_TYPE_MOVIE = 3,
    CONTENT_TYPE_SONIFICATION = 4,
    CONTENT_TYPE_RINGTONE = 5
};

enum class AudioAdapterStreamUsage : int32_t {
    STREAM_USAGE_UNKNOWN = 0,
    STREAM_USAGE_MEDIA = 1,
    STREAM_USAGE_VOICE_COMMUNICATION = 2,
    STREAM_USAGE_VOICE_ASSISTANT = 4,
    STREAM_USAGE_NOTIFICATION_RINGTONE = 6
};

enum class AudioAdapterConcurrencyMode : int32_t {
    INVALID = -1,
    DEFAULT = 0,
    MIX_WITH_OTHERS = 1,
    DUCK_OTHERS = 2,
    PAUSE_OTHERS = 3,
    SLIENT = 4,
};

class AudioRendererOptionsAdapter {
public:
    AudioRendererOptionsAdapter() = default;

    virtual ~AudioRendererOptionsAdapter() = default;

    virtual AudioAdapterSamplingRate GetSamplingRate() = 0;

    virtual AudioAdapterEncodingType GetEncodingType() = 0;

    virtual AudioAdapterSampleFormat GetSampleFormat() = 0;

    virtual AudioAdapterChannel GetChannel() = 0;

    virtual AudioAdapterContentType GetContentType() = 0;

    virtual AudioAdapterStreamUsage GetStreamUsage() = 0;

    virtual int32_t GetRenderFlags() = 0;

    virtual AudioAdapterConcurrencyMode GetConcurrencyMode() { return AudioAdapterConcurrencyMode::INVALID; }
};

enum AudioAdapterCode : int32_t {
    AUDIO_OK = 0,
    AUDIO_ERROR = -1,
    AUDIO_NULL_ERROR = -2,
};

class AudioRendererCallbackAdapter {
public:
    AudioRendererCallbackAdapter() = default;

    virtual ~AudioRendererCallbackAdapter() = default;

    virtual void OnSuspend() = 0;

    virtual void OnResume() = 0;
};

enum class AudioAdapterDeviceChangeReason : int32_t {
    UNKNOWN = 0,
    NEW_DEVICE_AVAILABLE = 1,
    OLD_DEVICE_UNAVALIABLE = 2,
    OVERRODE = 3
};

class AudioOutputChangeCallbackAdapter {
public:
    AudioOutputChangeCallbackAdapter() = default;

    virtual ~AudioOutputChangeCallbackAdapter() = default;

    virtual void OnOutputDeviceChange(int32_t reason) {}
};

class AudioRendererAdapter {
public:
    AudioRendererAdapter() = default;

    virtual ~AudioRendererAdapter() = default;

    virtual int32_t Create(
        const std::shared_ptr<AudioRendererOptionsAdapter> options, std::string cachePath = std::string()) = 0;

    virtual bool Start() = 0;

    virtual bool Pause() = 0;

    virtual bool Stop() = 0;

    virtual bool Release() = 0;

    virtual int32_t Write(uint8_t* buffer, size_t bufferSize) = 0;

    virtual int32_t GetLatency(uint64_t& latency) = 0;

    virtual int32_t SetVolume(float volume) = 0;

    virtual float GetVolume() = 0;

    virtual int32_t SetAudioRendererCallback(const std::shared_ptr<AudioRendererCallbackAdapter>& callback) = 0;

    virtual void SetInterruptMode(bool audioExclusive) = 0;

    virtual bool IsRendererStateRunning() = 0;

    virtual int32_t SetAudioOutputChangeCallback(const std::shared_ptr<AudioOutputChangeCallbackAdapter>& callback) = 0;

    virtual void SetAudioSilentMode(bool isSilentMode) = 0;
};

} // namespace OHOS::NWeb

#endif // AUDIO_RENDERER_ADAPTER_H

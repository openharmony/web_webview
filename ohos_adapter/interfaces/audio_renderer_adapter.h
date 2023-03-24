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
enum class AudioAdapterSampleFormat {
    SAMPLE_U8 = 0,
    SAMPLE_S16LE = 1,
    SAMPLE_S24LE = 2,
    SAMPLE_S32LE = 3,
    SAMPLE_F32LE = 4,
    INVALID_WIDTH = -1
};

enum class AudioAdapterSamplingRate {
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

enum class AudioAdapterChannel {
    MONO = 1,
    STEREO = 2,
    CHANNEL_3 = 3,
    CHANNEL_4 = 4,
    CHANNEL_5 = 5,
    CHANNEL_6 = 6,
    CHANNEL_7 = 7,
    CHANNEL_8 = 8
};

enum class AudioAdapterEncodingType {
    ENCODING_PCM = 0,
    ENCODING_INVALID = -1
};

enum class AudioAdapterContentType {
    CONTENT_TYPE_UNKNOWN = 0,
    CONTENT_TYPE_SPEECH = 1,
    CONTENT_TYPE_MUSIC = 2,
    CONTENT_TYPE_MOVIE = 3,
    CONTENT_TYPE_SONIFICATION = 4,
    CONTENT_TYPE_RINGTONE = 5
};

enum class AudioAdapterStreamUsage {
    STREAM_USAGE_UNKNOWN = 0,
    STREAM_USAGE_MEDIA = 1,
    STREAM_USAGE_VOICE_COMMUNICATION = 2,
    STREAM_USAGE_VOICE_ASSISTANT = 4,
    STREAM_USAGE_NOTIFICATION_RINGTONE = 6
};

struct AudioAdapterRendererOptions {
    AudioAdapterSamplingRate samplingRate;
    AudioAdapterEncodingType encoding;
    AudioAdapterSampleFormat format;
    AudioAdapterChannel channels;
    AudioAdapterContentType contentType;
    AudioAdapterStreamUsage streamUsage;
    int32_t rendererFlags;
};

enum AudioAdapterCode : int32_t {
    AUDIO_OK = 0,
    AUDIO_ERROR = -1,
    AUDIO_NULL_ERROR = -2,
};

struct BufferDescAdapter {
    uint8_t* buffer;
    size_t bufLength;
    size_t dataLength;
};

class AudioRendererWriteCallbackAdapter {
public:
    AudioRendererWriteCallbackAdapter() = default;
    virtual ~AudioRendererWriteCallbackAdapter() = default;
    virtual void OnWriteData(size_t length);
};

class AudioRendererAdapter {
public:
    AudioRendererAdapter() = default;

    virtual ~AudioRendererAdapter() = default;

    virtual int32_t Create(const AudioAdapterRendererOptions &rendererOptions,
        std::string cachePath = std::string()) = 0;

    virtual bool Start() = 0;

    virtual bool Stop() = 0;

    virtual bool Release() = 0;

    virtual int32_t Write(uint8_t *buffer, size_t bufferSize) = 0;

    virtual int32_t GetLatency(uint64_t &latency) const = 0;

    virtual int32_t SetVolume(float volume) const = 0;

    virtual float GetVolume() const = 0;

    virtual int32_t Enqueue(const BufferDescAdapter& bufDesc) = 0;

    virtual int32_t GetBufferDesc(BufferDescAdapter& bufDesc) = 0;

    virtual int32_t SetAudioRendererWriteCallbackAdapter(
        const std::shared_ptr<AudioRendererWriteCallbackAdapter>& callback) = 0;
};
} // namespace OHOS::NWeb

#endif // AUDIO_RENDERER_ADAPTER_H
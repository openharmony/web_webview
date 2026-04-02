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

#ifndef AUDIO_CAPTURE_ADAPTER_H
#define AUDIO_CAPTURE_ADAPTER_H

#include <memory>
#include <string>

#include "audio_renderer_adapter.h"

namespace OHOS::NWeb {

enum class AudioAdapterSourceType : int32_t {
    SOURCE_TYPE_INVALID = -1,
    SOURCE_TYPE_MIC,
    SOURCE_TYPE_VOICE_RECOGNITION = 1,
    SOURCE_TYPE_VOICE_COMMUNICATION = 7,
    SOURCE_TYPE_ULTRASONIC = 8
};

class AudioCapturerOptionsAdapter {
public:
    AudioCapturerOptionsAdapter() = default;

    virtual ~AudioCapturerOptionsAdapter() = default;

    virtual AudioAdapterSamplingRate GetSamplingRate() = 0;

    virtual AudioAdapterEncodingType GetEncoding() = 0;

    virtual AudioAdapterSampleFormat GetSampleFormat() = 0;

    virtual AudioAdapterChannel GetChannels() = 0;

    virtual AudioAdapterSourceType GetSourceType() = 0;

    virtual int32_t GetCapturerFlags() = 0;
};

class BufferDescAdapter {
public:
    BufferDescAdapter() = default;

    virtual ~BufferDescAdapter() = default;

    virtual uint8_t* GetBuffer() = 0;

    virtual size_t GetBufLength() = 0;

    virtual size_t GetDataLength() = 0;

    virtual void SetBuffer(uint8_t* buffer) = 0;

    virtual void SetBufLength(size_t bufLength) = 0;

    virtual void SetDataLength(size_t dataLength) = 0;
};

class AudioCapturerReadCallbackAdapter {
public:
    AudioCapturerReadCallbackAdapter() = default;

    virtual ~AudioCapturerReadCallbackAdapter() = default;

    virtual void OnReadData(size_t length) = 0;
};

class AudioCapturerAdapter {
public:
    AudioCapturerAdapter() = default;

    virtual ~AudioCapturerAdapter() = default;

    virtual int32_t Create(
        const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions, std::string cachePath = std::string()) = 0;

    virtual bool Start() = 0;

    virtual bool Stop() = 0;

    virtual bool Release() = 0;

    virtual bool Pause() {}

    virtual int32_t SetCapturerReadCallback(std::shared_ptr<AudioCapturerReadCallbackAdapter> callbck) = 0;

    virtual int32_t GetBufferDesc(std::shared_ptr<BufferDescAdapter> bufferDesc) = 0;

    virtual int32_t Enqueue(const std::shared_ptr<BufferDescAdapter> bufferDesc) = 0;

    virtual int32_t GetFrameCount(uint32_t& frameCount) = 0;

    virtual int64_t GetAudioTime() = 0;
};

} // namespace OHOS::NWeb

#endif // AUDIO_CAPTURE_ADAPTER_H

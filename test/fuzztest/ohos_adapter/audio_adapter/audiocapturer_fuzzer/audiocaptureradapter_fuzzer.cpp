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

#include "audiocaptureradapter_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "audio_capturer_adapter_impl.h"

using namespace OHOS::NWeb;

class MockAudioCapturerOptionsAdapter : public AudioCapturerOptionsAdapter {
public:
    MockAudioCapturerOptionsAdapter() = default;

    AudioAdapterSamplingRate GetSamplingRate() override
    {
        return samplingRate;
    }

    AudioAdapterEncodingType GetEncoding() override
    {
        return encoding;
    }

    AudioAdapterSampleFormat GetSampleFormat() override
    {
        return format;
    }

    AudioAdapterChannel GetChannels() override
    {
        return channels;
    }

    AudioAdapterSourceType GetSourceType() override
    {
        return sourceType;
    }

    int32_t GetCapturerFlags() override
    {
        return capturerFlags;
    }

    AudioAdapterSamplingRate samplingRate;
    AudioAdapterEncodingType encoding;
    AudioAdapterSampleFormat format;
    AudioAdapterChannel channels;
    AudioAdapterSourceType sourceType;
    int32_t capturerFlags;
};

namespace OHOS {
bool AudioCapturerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    std::shared_ptr<MockAudioCapturerOptionsAdapter> capturerOptions =
        std::make_shared<MockAudioCapturerOptionsAdapter>();

    capturerOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    capturerOptions->encoding = AudioAdapterEncodingType::ENCODING_PCM;
    capturerOptions->format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    capturerOptions->channels = AudioAdapterChannel::STEREO;
    capturerOptions->sourceType = AudioAdapterSourceType::SOURCE_TYPE_MIC;
    capturerOptions->capturerFlags = 0;

    std::string cachePath(reinterpret_cast<const char*>(data), size);
    AudioCapturerAdapterImpl adapter;

    adapter.Create(capturerOptions, cachePath);
    adapter.Start();
    adapter.Stop();
    adapter.Release();

    auto callback = std::make_shared<AudioCapturerReadCallbackAdapter>();
    adapter.SetCapturerReadCallback(callback);

    std::shared_ptr<BufferDescAdapter> bufferDesc = std::make_shared<BufferDescAdapter>();
    adapter.GetBufferDesc(bufferDesc);
    adapter.Enqueue(bufferDesc);

    uint32_t frameCount;
    adapter.GetFrameCount(frameCount);

    int64_t audioTime = adapter.GetAudioTime();

    adapter.GetAudioSamplingRate(AudioAdapterSamplingRate::SAMPLE_RATE_32000);
    adapter.GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_INVALID);
    adapter.GetAudioSampleFormat(AudioAdapterSampleFormat::SAMPLE_S16LE);
    adapter.GetAudioChannel(AudioAdapterChannel::CHANNEL_4);
    adapter.GetAudioSourceType(AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION);

    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::AudioCapturerFuzzTest(data, size);
    return 0;
}

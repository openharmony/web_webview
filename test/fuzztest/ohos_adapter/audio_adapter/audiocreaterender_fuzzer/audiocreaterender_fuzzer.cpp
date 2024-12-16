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

#include "audiocreaterender_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "audio_renderer_adapter_impl.h"

using namespace OHOS::NWeb;

class MockAudioRendererOptionsAdapter : public AudioRendererOptionsAdapter {
public:
    MockAudioRendererOptionsAdapter() = default;

    AudioAdapterSamplingRate GetSamplingRate() override
    {
        return samplingRate;
    }

    AudioAdapterEncodingType GetEncodingType() override
    {
        return encoding;
    }

    AudioAdapterSampleFormat GetSampleFormat() override
    {
        return format;
    }

    AudioAdapterChannel GetChannel() override
    {
        return channels;
    }

    AudioAdapterContentType GetContentType() override
    {
        return contentType;
    }

    AudioAdapterStreamUsage GetStreamUsage() override
    {
        return streamUsage;
    }

    int32_t GetRenderFlags() override
    {
        return rendererFlags;
    }

    AudioAdapterConcurrencyMode GetConcurrencyMode() override 
    {
        return concurrency_mode;
    }

    AudioAdapterSamplingRate samplingRate;
    AudioAdapterEncodingType encoding;
    AudioAdapterSampleFormat format;
    AudioAdapterChannel channels;
    AudioAdapterContentType contentType;
    AudioAdapterStreamUsage streamUsage;
    int32_t rendererFlags;
    AudioAdapterConcurrencyMode concurrency_mode;
};

namespace OHOS {
bool AudioCreateRenderFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    std::shared_ptr<MockAudioRendererOptionsAdapter> rendererOptions =
        std::make_shared<MockAudioRendererOptionsAdapter>();
    rendererOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions->encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions->format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions->channels = AudioAdapterChannel::STEREO;
    rendererOptions->contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions->streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions->rendererFlags = 0;
    rendererOptions->concurrency_mode = AudioAdapterConcurrencyMode::INVALID;
    std::string cachePath(reinterpret_cast<const char*>(data), size);
    AudioRendererAdapterImpl adapter;
    adapter.Create(rendererOptions, cachePath);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AudioCreateRenderFuzzTest(data, size);
    return 0;
}

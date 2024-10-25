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

#include "audiorenderadapter_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "audio_renderer_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool AudioRendererAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    AudioRendererAdapterImpl audioRendererAdapterImpl;
    uint8_t buffer[100];
    size_t bufferSize = sizeof(buffer);
    float volume = 0.5;
    std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions;

    AudioAdapterSamplingRate samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    AudioAdapterEncodingType encodingType = AudioAdapterEncodingType::ENCODING_PCM;
    AudioAdapterSampleFormat sampleFormat = AudioAdapterSampleFormat::SAMPLE_S16LE;
    AudioAdapterChannel channel = AudioAdapterChannel::STEREO;
    AudioAdapterContentType contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    AudioAdapterStreamUsage streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    audioRendererAdapterImpl.Create(rendererOptions, "");
    audioRendererAdapterImpl.Start();
    audioRendererAdapterImpl.Pause();
    audioRendererAdapterImpl.Stop();
    audioRendererAdapterImpl.Release();
    audioRendererAdapterImpl.Write(buffer, bufferSize);
    uint64_t latency;
    audioRendererAdapterImpl.GetLatency(latency);
    audioRendererAdapterImpl.SetVolume(volume);
    audioRendererAdapterImpl.GetVolume();
    audioRendererAdapterImpl.SetAudioRendererCallback(nullptr);
    audioRendererAdapterImpl.SetAudioOutputChangeCallback(nullptr);
    audioRendererAdapterImpl.SetInterruptMode(true);
    audioRendererAdapterImpl.SetAudioSilentMode(false);
    audioRendererAdapterImpl.IsRendererStateRunning();
    audioRendererAdapterImpl.GetAudioSamplingRate(samplingRate);
    audioRendererAdapterImpl.GetAudioEncodingType(encodingType);
    audioRendererAdapterImpl.GetAudioSampleFormat(sampleFormat);
    audioRendererAdapterImpl.GetAudioChannel(channel);
    audioRendererAdapterImpl.GetAudioContentType(contentType);
    audioRendererAdapterImpl.GetAudioStreamUsage(streamUsage);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AudioRendererAdapterFuzzTest(data, size);
    return 0;
}
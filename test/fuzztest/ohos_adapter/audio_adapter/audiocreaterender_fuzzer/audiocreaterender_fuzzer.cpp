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

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }
        AudioAdapterRendererOptions rendererOptions;
        rendererOptions.samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
        rendererOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
        rendererOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
        rendererOptions.channels = AudioAdapterChannel::STEREO;
        rendererOptions.contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
        rendererOptions.streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
        rendererOptions.rendererFlags = 0;
        std::string cachePath(reinterpret_cast<const char*>(data), size);
        AudioRendererAdapterImpl adapter;
        adapter.Create(rendererOptions, cachePath);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

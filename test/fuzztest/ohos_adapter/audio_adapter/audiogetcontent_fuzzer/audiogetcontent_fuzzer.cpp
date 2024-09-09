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

#include "audiogetcontent_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <array>

#include "audio_renderer_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool AudioGetContentFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    constexpr int typenums = 6;
    std::array<AudioAdapterContentType, typenums> contentArray = {
        AudioAdapterContentType::CONTENT_TYPE_UNKNOWN,
        AudioAdapterContentType::CONTENT_TYPE_SPEECH,
        AudioAdapterContentType::CONTENT_TYPE_MUSIC,
        AudioAdapterContentType::CONTENT_TYPE_MOVIE,
        AudioAdapterContentType::CONTENT_TYPE_SONIFICATION,
        AudioAdapterContentType::CONTENT_TYPE_RINGTONE,
    };
    for (auto& content : contentArray)
        AudioRendererAdapterImpl::GetAudioContentType(content);

    AudioRendererAdapterImpl::GetAudioContentType(static_cast<AudioAdapterContentType>(-1));
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AudioGetContentFuzzTest(data, size);
    return 0;
}

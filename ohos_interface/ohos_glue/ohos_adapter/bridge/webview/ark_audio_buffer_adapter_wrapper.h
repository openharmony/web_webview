/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_AUDIO_BUFFER_ADAPTER_WRAPPER_H
#define ARK_AUDIO_BUFFER_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_screen_capture_adapter.h"
#include "screen_capture_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioBufferAdapterWrapper : public NWeb::AudioBufferAdapter {
public:
    explicit ArkAudioBufferAdapterWrapper(ArkWebRefPtr<ArkAudioBufferAdapter>);

    uint8_t* GetBuffer() override;

    int32_t GetLength() override;

    int64_t GetTimestamp() override;

    NWeb::AudioCaptureSourceTypeAdapter GetSourcetype() override;

    void SetBuffer(uint8_t* buffer) override;

    void SetLength(int32_t length) override;

    void SetTimestamp(int64_t timestamp) override;

    void SetSourcetype(NWeb::AudioCaptureSourceTypeAdapter sourcetype) override;

private:
    ArkWebRefPtr<ArkAudioBufferAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_BUFFER_ADAPTER_WRAPPER_H
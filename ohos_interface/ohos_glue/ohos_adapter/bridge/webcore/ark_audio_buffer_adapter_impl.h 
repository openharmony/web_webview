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
#ifndef ARK_AUDIO_BUFFER_ADAPTER_IMPL_H
#define ARK_AUDIO_BUFFER_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_screen_capture_adapter.h"
#include "screen_capture_adapter.h"

namespace OHOS::ArkWeb {
class ArkAudioBufferAdapterImpl : public ArkAudioBufferAdapter {
public:
    explicit ArkAudioBufferAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioBufferAdapter>);

    uint8_t* GetBuffer() override;

    int32_t GetLength() override;

    int64_t GetTimestamp() override;

    int32_t GetSourcetype() override;

    void SetBuffer(uint8_t* buffer) override;

    void SetLength(int32_t length) override;

    void SetTimestamp(int64_t timestamp) override;

    void SetSourcetype(int32_t sourcetype) override;

private:
    std::shared_ptr<OHOS::NWeb::AudioBufferAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAudioBufferAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_BUFFER_ADAPTER_IMPL_H
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

#ifndef ARK_AUDIO_RENDERER_ADAPTER_WRAPPER_H
#define ARK_AUDIO_RENDERER_ADAPTER_WRAPPER_H
#pragma once

#include "audio_renderer_adapter.h"
#include "ohos_adapter/include/ark_audio_renderer_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioRendererAdapterWrapper : public OHOS::NWeb::AudioRendererAdapter {
public:
    explicit ArkAudioRendererAdapterWrapper(ArkWebRefPtr<ArkAudioRendererAdapter>);

    int32_t Create(const std::shared_ptr<NWeb::AudioRendererOptionsAdapter> options,
        std::string cachePath = std::string()) override;

    bool Start() override;

    bool Pause() override;

    bool Stop() override;

    bool Release() override;

    int32_t Write(uint8_t* buffer, size_t bufferSize) override;

    int32_t GetLatency(uint64_t& latency) override;

    int32_t SetVolume(float volume) override;

    float GetVolume() override;

    int32_t SetAudioRendererCallback(const std::shared_ptr<NWeb::AudioRendererCallbackAdapter>& callback) override;

    void SetInterruptMode(bool audioExclusive) override;

    bool IsRendererStateRunning() override;

    int32_t SetAudioOutputChangeCallback(
        const std::shared_ptr<NWeb::AudioOutputChangeCallbackAdapter>& callback) override;

    void SetAudioSilentMode(bool isSilentMode) override;
private:
    ArkWebRefPtr<ArkAudioRendererAdapter> ctocpp_;
};
} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_RENDERER_ADAPTER_H

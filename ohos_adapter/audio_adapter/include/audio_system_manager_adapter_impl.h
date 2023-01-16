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

#ifndef AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H
#define AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H

#include "audio_system_manager_adapter.h"

#include "audio_system_manager.h"

namespace OHOS::NWeb {
using namespace OHOS::AudioStandard;

class AudioManagerCallbackAdapterImpl : public AudioManagerCallback {
public:
    explicit AudioManagerCallbackAdapterImpl(std::shared_ptr<AudioManagerCallbackAdapter> cb);

    ~AudioManagerCallbackAdapterImpl() override = default;

    void OnInterrupt(const InterruptAction &interruptAction) override;

private:
    std::shared_ptr<AudioManagerCallbackAdapter> cb_ = nullptr;
};

class AudioSystemManagerAdapterImpl : public AudioSystemManagerAdapter {
public:
    AudioSystemManagerAdapterImpl() = default;

    static AudioSystemManagerAdapterImpl& GetInstance();

    ~AudioSystemManagerAdapterImpl() override = default;

    bool HasAudioOutputDevices() const override;

    bool HasAudioInputDevices() const override;

    int32_t RequestAudioFocus(const AudioAdapterInterrupt &audioInterrupt) override;

    int32_t AbandonAudioFocus(const AudioAdapterInterrupt &audioInterrupt) override;

    int32_t SetAudioManagerInterruptCallback(const std::shared_ptr<AudioManagerCallbackAdapter> &callback) override;

    int32_t UnsetAudioManagerInterruptCallback() override;

    static AudioStreamType GetStreamType(AudioAdapterStreamType streamType);

private:
    std::shared_ptr<AudioManagerCallbackAdapterImpl> callback_;
};
}  // namespace OHOS::NWeb

#endif // AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H
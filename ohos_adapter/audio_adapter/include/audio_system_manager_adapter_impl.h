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

#include <ohaudio/native_audio_routing_manager.h>

#include "audio_system_manager_adapter.h"

namespace OHOS::NWeb {
using namespace OHOS::AudioStandard;
class AudioSystemManagerAdapterImpl : public AudioSystemManagerAdapter {
public:
    AudioSystemManagerAdapterImpl() = default;

    static AudioSystemManagerAdapterImpl& GetInstance();

    ~AudioSystemManagerAdapterImpl() override = default;

    bool HasAudioOutputDevices() override;

    bool HasAudioInputDevices() override;

    int32_t RequestAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt) override;

    int32_t AbandonAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt) override;

    int32_t SetAudioManagerInterruptCallback(std::shared_ptr<AudioManagerCallbackAdapter> callback) override;

    int32_t UnsetAudioManagerInterruptCallback() override;

    virtual std::vector<std::shared_ptr<AudioDeviceDescAdapter>> GetDevices(AdapterDeviceFlag flag) override;

    int32_t SelectAudioDeviceById(int32_t deviceId, bool isInput) override;

    std::shared_ptr<AudioDeviceDescAdapter> GetDefaultOutputDevice() override;

    int32_t SetDeviceChangeCallback(std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> callback) override;

    int32_t UnsetDeviceChangeCallback() override;

    std::shared_ptr<AudioDeviceDescAdapter> GetDefaultInputDevice() override;

    bool SetLanguage(const std::string& language) override;

    std::string GetDeviceName(OH_AudioDevice_Type deviceType);

    std::string language_ = "zh";

private:
    int32_t SelectAudioOutputDevice(bool isCallDevice, OH_AudioDeviceDescriptorArray& audioDeviceArray) const;
};
} // namespace OHOS::NWeb

#endif // AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H

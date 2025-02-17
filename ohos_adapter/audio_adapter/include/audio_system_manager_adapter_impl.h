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

#if defined(NWEB_AUDIO_ENABLE)
#include "audio_system_manager.h"
#endif

namespace OHOS::NWeb {
#if defined(NWEB_AUDIO_ENABLE)
using namespace OHOS::AudioStandard;

class AudioManagerCallbackAdapterImpl : public AudioManagerCallback {
public:
    explicit AudioManagerCallbackAdapterImpl(std::shared_ptr<AudioManagerCallbackAdapter> cb);

    ~AudioManagerCallbackAdapterImpl() override = default;

    void OnInterrupt(const InterruptAction& interruptAction) override;

private:
    std::shared_ptr<AudioManagerCallbackAdapter> cb_ = nullptr;
};

class AudioManagerDeviceChangeCallbackAdapterImpl : public AudioManagerDeviceChangeCallback {
public:
    explicit AudioManagerDeviceChangeCallbackAdapterImpl(std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> cb);

    ~AudioManagerDeviceChangeCallbackAdapterImpl() override = default;

    void OnDeviceChange(const DeviceChangeAction& deviceChangeAction) override;

private:
    std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> cb_ = nullptr;
};
#endif

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

    std::string GetDeviceName(DeviceType deviceType);

    std::string language_ = "zh";

#if defined(NWEB_AUDIO_ENABLE)
    static AudioStreamType GetStreamType(AudioAdapterStreamType streamType);

private:
    int32_t SelectAudioOutputDevice(bool isCallDevice, const std::vector<sptr<AudioDeviceDescriptor>>& device) const;
    bool GetDeviceIdByDescriptor(AudioDeviceDescriptor *audioDeviceDescriptor, int32_t &deviceId);
    int32_t SelectDefaultAudioDevice(bool isCallDevice);

private:
    std::shared_ptr<AudioManagerCallbackAdapterImpl> callback_;
    std::shared_ptr<AudioManagerDeviceChangeCallbackAdapterImpl> deviceChangeCallback_ = nullptr;
    std::map<int32_t, AudioDeviceDescriptor> audioOutputDeviceInfo_;
#endif
};
} // namespace OHOS::NWeb

#endif // AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H

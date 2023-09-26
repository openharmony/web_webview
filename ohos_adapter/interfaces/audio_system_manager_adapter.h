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

#ifndef AUDIO_SYSTEM_MANAGER_ADAPTER_H
#define AUDIO_SYSTEM_MANAGER_ADAPTER_H

#include <vector>

#include "audio_renderer_adapter.h"

namespace OHOS::NWeb {
enum class AudioAdapterStreamType {
    STREAM_DEFAULT = -1,
    STREAM_VOICE_CALL = 0,
    STREAM_MUSIC = 1,
    STREAM_RING = 2,
    STREAM_MEDIA = 3,
    STREAM_VOICE_ASSISTANT = 4,
    STREAM_SYSTEM = 5,
    STREAM_ALARM = 6,
    STREAM_NOTIFICATION = 7,
    STREAM_BLUETOOTH_SCO = 8,
    STREAM_ENFORCED_AUDIBLE = 9,
    STREAM_DTMF = 10,
    STREAM_TTS = 11,
    STREAM_ACCESSIBILITY = 12,
    STREAM_RECORDING = 13,
    STREAM_ALL = 100
};

enum AdapterDeviceFlag {
    NONE_DEVICES_FLAG = 0,
    OUTPUT_DEVICES_FLAG = 1,
    INPUT_DEVICES_FLAG = 2,
    ALL_DEVICES_FLAG = 3,
    DISTRIBUTED_OUTPUT_DEVICES_FLAG = 4,
    DISTRIBUTED_INPUT_DEVICES_FLAG = 8,
    ALL_DISTRIBUTED_DEVICES_FLAG = 12,
    ALL_L_D_DEVICES_FLAG = 15,
    DEVICE_FLAG_MAX
};

struct AudioAdapterInterrupt {
    AudioAdapterStreamUsage streamUsage;
    AudioAdapterContentType contentType;
    AudioAdapterStreamType streamType;
    uint32_t sessionID;
    bool pauseWhenDucked;
};

struct AudioAdapterDeviceDesc {
    int32_t deviceId;
    std::string deviceName;
};

class AudioManagerCallbackAdapter {
public:
    AudioManagerCallbackAdapter() = default;

    virtual ~AudioManagerCallbackAdapter() = default;

    virtual void OnSuspend() = 0;

    virtual void OnResume() = 0;
};

class AudioManagerDeviceChangeCallbackAdapter {
public:
    AudioManagerDeviceChangeCallbackAdapter() = default;

    virtual ~AudioManagerDeviceChangeCallbackAdapter() = default;

    virtual void OnDeviceChange() = 0;
};

class AudioSystemManagerAdapter {
public:
    AudioSystemManagerAdapter() = default;

    virtual ~AudioSystemManagerAdapter() = default;

    virtual bool HasAudioOutputDevices() const = 0;

    virtual bool HasAudioInputDevices() const = 0;

    virtual int32_t RequestAudioFocus(const AudioAdapterInterrupt& audioInterrupt) = 0;

    virtual int32_t AbandonAudioFocus(const AudioAdapterInterrupt& audioInterrupt) = 0;

    virtual int32_t SetAudioManagerInterruptCallback(const std::shared_ptr<AudioManagerCallbackAdapter>& callback) = 0;

    virtual int32_t UnsetAudioManagerInterruptCallback() = 0;

    virtual std::vector<AudioAdapterDeviceDesc> GetDevices(AdapterDeviceFlag flag) const = 0;

    virtual int32_t SelectAudioDevice(AudioAdapterDeviceDesc desc, bool isInput) const = 0;

    virtual int32_t SetDeviceChangeCallback(
        const std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter>& callback) = 0;

    virtual int32_t UnsetDeviceChangeCallback() = 0;

    virtual AudioAdapterDeviceDesc GetDefaultOutputDevice() = 0;
};
} // namespace OHOS::NWeb

#endif // AUDIO_RENDERER_ADAPTER_H
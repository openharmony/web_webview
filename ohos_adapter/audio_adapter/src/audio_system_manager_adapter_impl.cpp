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

#include "audio_system_manager_adapter_impl.h"

#include <unordered_map>

#include "audio_errors.h"
#include "audio_renderer_adapter_impl.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
const std::unordered_map<AudioAdapterStreamType, AudioStreamType> STREAM_TYPE_MAP = {
    { AudioAdapterStreamType::STREAM_DEFAULT, AudioStreamType::STREAM_DEFAULT },
    { AudioAdapterStreamType::STREAM_VOICE_CALL, AudioStreamType::STREAM_VOICE_CALL },
    { AudioAdapterStreamType::STREAM_MUSIC, AudioStreamType::STREAM_MUSIC },
    { AudioAdapterStreamType::STREAM_RING, AudioStreamType::STREAM_RING },
    { AudioAdapterStreamType::STREAM_MEDIA, AudioStreamType::STREAM_MEDIA },
    { AudioAdapterStreamType::STREAM_VOICE_ASSISTANT, AudioStreamType::STREAM_VOICE_ASSISTANT },
    { AudioAdapterStreamType::STREAM_SYSTEM, AudioStreamType::STREAM_SYSTEM },
    { AudioAdapterStreamType::STREAM_ALARM, AudioStreamType::STREAM_ALARM },
    { AudioAdapterStreamType::STREAM_NOTIFICATION, AudioStreamType::STREAM_NOTIFICATION },
    { AudioAdapterStreamType::STREAM_BLUETOOTH_SCO, AudioStreamType::STREAM_BLUETOOTH_SCO },
    { AudioAdapterStreamType::STREAM_ENFORCED_AUDIBLE, AudioStreamType::STREAM_ENFORCED_AUDIBLE },
    { AudioAdapterStreamType::STREAM_DTMF, AudioStreamType::STREAM_DTMF },
    { AudioAdapterStreamType::STREAM_TTS, AudioStreamType::STREAM_TTS },
    { AudioAdapterStreamType::STREAM_ACCESSIBILITY, AudioStreamType::STREAM_ACCESSIBILITY },
    { AudioAdapterStreamType::STREAM_RECORDING, AudioStreamType::STREAM_RECORDING },
    { AudioAdapterStreamType::STREAM_ALL, AudioStreamType::STREAM_ALL },
};

const std::unordered_map<AdapterDeviceFlag, DeviceFlag> DEVICE_FLAG_MAP = {
    { AdapterDeviceFlag::NONE_DEVICES_FLAG, DeviceFlag::NONE_DEVICES_FLAG },
    { AdapterDeviceFlag::OUTPUT_DEVICES_FLAG, DeviceFlag::OUTPUT_DEVICES_FLAG },
    { AdapterDeviceFlag::INPUT_DEVICES_FLAG, DeviceFlag::INPUT_DEVICES_FLAG },
    { AdapterDeviceFlag::ALL_DEVICES_FLAG, DeviceFlag::ALL_DEVICES_FLAG },
    { AdapterDeviceFlag::DISTRIBUTED_OUTPUT_DEVICES_FLAG, DeviceFlag::DISTRIBUTED_OUTPUT_DEVICES_FLAG },
    { AdapterDeviceFlag::DISTRIBUTED_INPUT_DEVICES_FLAG, DeviceFlag::DISTRIBUTED_INPUT_DEVICES_FLAG },
    { AdapterDeviceFlag::ALL_DISTRIBUTED_DEVICES_FLAG, DeviceFlag::ALL_DISTRIBUTED_DEVICES_FLAG },
    { AdapterDeviceFlag::ALL_L_D_DEVICES_FLAG, DeviceFlag::ALL_L_D_DEVICES_FLAG },
    { AdapterDeviceFlag::DEVICE_FLAG_MAX, DeviceFlag::DEVICE_FLAG_MAX },
};

const std::string DEVICE_TYPE_NONE = "device/none";
const std::string DEVICE_TYPE_INVALID = "device/invalid";
const std::string DEVICE_TYPE_EARPIECE = "device/earpiece";
const std::string DEVICE_TYPE_SPEAKER = "device/speaker";
const std::string DEVICE_TYPE_WIRED_HEADSET = "device/wired_headset";
const std::string DEVICE_TYPE_WIRED_HEADPHONES = "device/wired_headphones";
const std::string DEVICE_TYPE_BLUETOOTH_SCO = "device/bluetooth_sco";
const std::string DEVICE_TYPE_BLUETOOTH_A2DP = "device/bluetooth_a2dp";
const std::string DEVICE_TYPE_MIC = "device/mic";
const std::string DEVICE_TYPE_USB_HEADSET = "device/usb_headset";
const std::string DEVICE_TYPE_FILE_SINK = "device/file_sink";
const std::string DEVICE_TYPE_FILE_SOURCE = "device/file_source";
const std::string DEVICE_TYPE_MAX = "device/max";

const std::unordered_map<DeviceType, std::string> DEVICE_TYPE_MAP = {
    { DeviceType::DEVICE_TYPE_NONE, DEVICE_TYPE_NONE },
    { DeviceType::DEVICE_TYPE_INVALID, DEVICE_TYPE_INVALID },
    { DeviceType::DEVICE_TYPE_EARPIECE, DEVICE_TYPE_EARPIECE },
    { DeviceType::DEVICE_TYPE_SPEAKER, DEVICE_TYPE_SPEAKER },
    { DeviceType::DEVICE_TYPE_WIRED_HEADSET, DEVICE_TYPE_WIRED_HEADSET },
    { DeviceType::DEVICE_TYPE_WIRED_HEADPHONES, DEVICE_TYPE_WIRED_HEADPHONES },
    { DeviceType::DEVICE_TYPE_BLUETOOTH_SCO, DEVICE_TYPE_BLUETOOTH_SCO },
    { DeviceType::DEVICE_TYPE_BLUETOOTH_A2DP, DEVICE_TYPE_BLUETOOTH_A2DP },
    { DeviceType::DEVICE_TYPE_MIC, DEVICE_TYPE_MIC },
    { DeviceType::DEVICE_TYPE_USB_HEADSET, DEVICE_TYPE_USB_HEADSET },
    { DeviceType::DEVICE_TYPE_FILE_SINK, DEVICE_TYPE_FILE_SINK },
    { DeviceType::DEVICE_TYPE_FILE_SOURCE, DEVICE_TYPE_FILE_SOURCE },
    { DeviceType::DEVICE_TYPE_MAX, DEVICE_TYPE_MAX },
};

const int32_t ADAPTER_AUDIO_DEFAULT_DEVICE_ID = 1000000;
const char* ADAPTER_AUDIO_DEFAULT_DEVICE_NAME = "(default)";

AudioManagerCallbackAdapterImpl::AudioManagerCallbackAdapterImpl(std::shared_ptr<AudioManagerCallbackAdapter> cb)
    : cb_(cb) {};

void AudioManagerCallbackAdapterImpl::OnInterrupt(const InterruptAction& interruptAction)
{
    if (!cb_) {
        return;
    }
    switch (interruptAction.interruptHint) {
        case InterruptHint::INTERRUPT_HINT_PAUSE:
        case InterruptHint::INTERRUPT_HINT_STOP:
            cb_->OnSuspend();
            break;
        case InterruptHint::INTERRUPT_HINT_RESUME:
            cb_->OnResume();
            break;
        default:
            WVLOG_E("audio manager interrupt hint not foud, code: %{public}d", interruptAction.interruptHint);
            break;
    }
}

AudioManagerDeviceChangeCallbackAdapterImpl::AudioManagerDeviceChangeCallbackAdapterImpl(
    std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> cb)
    : cb_(cb) {};

void AudioManagerDeviceChangeCallbackAdapterImpl::OnDeviceChange(const DeviceChangeAction& deviceChangeAction)
{
    if (!cb_) {
        return;
    }
    cb_->OnDeviceChange();
}

AudioSystemManagerAdapterImpl& AudioSystemManagerAdapterImpl::GetInstance()
{
    static AudioSystemManagerAdapterImpl instance;
    return instance;
}

bool AudioSystemManagerAdapterImpl::HasAudioOutputDevices() const
{
    DeviceType outputDeviceType = AudioSystemManager::GetInstance()->GetActiveOutputDevice();
    if (outputDeviceType == DeviceType::DEVICE_TYPE_NONE || outputDeviceType == DeviceType::DEVICE_TYPE_INVALID) {
        return false;
    }
    return true;
}

bool AudioSystemManagerAdapterImpl::HasAudioInputDevices() const
{
    DeviceType inputDeviceType = AudioSystemManager::GetInstance()->GetActiveInputDevice();
    if (inputDeviceType == DeviceType::DEVICE_TYPE_NONE || inputDeviceType == DeviceType::DEVICE_TYPE_INVALID) {
        return false;
    }
    return true;
}

int32_t AudioSystemManagerAdapterImpl::RequestAudioFocus(const AudioAdapterInterrupt& audioInterrupt)
{
    AudioInterrupt interruptParams;
    interruptParams.streamUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(audioInterrupt.streamUsage);
    interruptParams.contentType = AudioRendererAdapterImpl::GetAudioContentType(audioInterrupt.contentType);
    interruptParams.audioFocusType.streamType = GetStreamType(audioInterrupt.streamType);

    int32_t ret = AudioSystemManager::GetInstance()->RequestAudioFocus(interruptParams);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio request audio focus failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::AbandonAudioFocus(const AudioAdapterInterrupt& audioInterrupt)
{
    AudioInterrupt interruptParams;
    interruptParams.streamUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(audioInterrupt.streamUsage);
    interruptParams.contentType = AudioRendererAdapterImpl::GetAudioContentType(audioInterrupt.contentType);
    interruptParams.audioFocusType.streamType = GetStreamType(audioInterrupt.streamType);

    int32_t ret = AudioSystemManager::GetInstance()->AbandonAudioFocus(interruptParams);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio abandon audio focus failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::SetAudioManagerInterruptCallback(
    const std::shared_ptr<AudioManagerCallbackAdapter>& callback)
{
    if (callback == nullptr) {
        WVLOG_E("set audio manager interrupt callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    callback_ = std::make_shared<AudioManagerCallbackAdapterImpl>(callback);

    int32_t ret = AudioSystemManager::GetInstance()->SetAudioManagerInterruptCallback(callback_);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio manager set interrupt callback failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::UnsetAudioManagerInterruptCallback()
{
    int32_t ret = AudioSystemManager::GetInstance()->UnsetAudioManagerInterruptCallback();
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio manager unset interrupt callback failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    callback_ = nullptr;
    return AUDIO_OK;
}

std::vector<AudioAdapterDeviceDesc> AudioSystemManagerAdapterImpl::GetDevices(AdapterDeviceFlag flag) const
{
    auto item = DEVICE_FLAG_MAP.find(flag);
    if (item == DEVICE_FLAG_MAP.end()) {
        WVLOG_E("audio device type not found");
        return std::vector<AudioAdapterDeviceDesc>();
    }
    auto deviceFlag = item->second;
    auto audioDeviceList = AudioSystemManager::GetInstance()->GetDevices(deviceFlag);
    std::vector<AudioAdapterDeviceDesc> audioAdapterDeviceList;
    for (auto audioDevice : audioDeviceList) {
        AudioAdapterDeviceDesc desc;
        desc.deviceId = audioDevice->deviceId_;
        auto deviceTypeKey = DEVICE_TYPE_MAP.find(audioDevice->deviceType_);
        if (deviceTypeKey != DEVICE_TYPE_MAP.end()) {
            desc.deviceName = deviceTypeKey->second;
        }
        audioAdapterDeviceList.emplace_back(desc);
    }
    return audioAdapterDeviceList;
}

int32_t AudioSystemManagerAdapterImpl::SelectAudioDevice(AudioAdapterDeviceDesc desc, bool isInput) const
{
    WVLOG_I("AudioSystemManagerAdapterImpl::SelectAudioDevice isInput: %{public}s", isInput ? "true" : "false");
    auto item = isInput ? DEVICE_FLAG_MAP.find(AdapterDeviceFlag::INPUT_DEVICES_FLAG)
                        : DEVICE_FLAG_MAP.find(AdapterDeviceFlag::OUTPUT_DEVICES_FLAG);
    if (item == DEVICE_FLAG_MAP.end()) {
        WVLOG_E("audio device type not found");
        return AUDIO_ERROR;
    }
    auto deviceFlag = item->second;
    if (!isInput && desc.deviceId == ADAPTER_AUDIO_DEFAULT_DEVICE_ID) {
        WVLOG_I("Select default audio output Device.");
        AudioRendererInfo rendererInfo;
        rendererInfo.contentType = ContentType::CONTENT_TYPE_SPEECH;
        rendererInfo.streamUsage = StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION;
        rendererInfo.rendererFlags = 0;
        std::vector<sptr<AudioDeviceDescriptor>> defaultOutputDevice;
        AudioRoutingManager::GetInstance()->GetPreferredOutputDeviceForRendererInfo(rendererInfo, defaultOutputDevice);
        AudioSystemManager::GetInstance()->SelectInputDevice(defaultOutputDevice);
        return AUDIO_OK;
    }
    auto audioDeviceList = AudioSystemManager::GetInstance()->GetDevices(deviceFlag);
    for (auto device : audioDeviceList) {
        if (device->deviceId_ == desc.deviceId) {
            std::vector<sptr<AudioDeviceDescriptor>> selectedAudioDevice { device };
            return isInput ? AudioSystemManager::GetInstance()->SelectInputDevice(selectedAudioDevice)
                           : AudioSystemManager::GetInstance()->SelectOutputDevice(selectedAudioDevice);
        }
    }
    WVLOG_E("can't find any device by audio device id");
    return AUDIO_ERROR;
}

AudioAdapterDeviceDesc AudioSystemManagerAdapterImpl::GetDefaultOutputDevice()
{
    AudioRendererInfo rendererInfo;
    rendererInfo.contentType = ContentType::CONTENT_TYPE_SPEECH;
    rendererInfo.streamUsage = StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION;
    rendererInfo.rendererFlags = 0;
    std::vector<sptr<AudioDeviceDescriptor>> defaultOutputDevice;
    AudioRoutingManager::GetInstance()->GetPreferredOutputDeviceForRendererInfo(rendererInfo, defaultOutputDevice);

    AudioAdapterDeviceDesc desc;
    auto deviceTypeKey = DEVICE_TYPE_MAP.find(defaultOutputDevice[0]->deviceType_);
    desc.deviceId = defaultOutputDevice[0]->deviceId_;
    if (deviceTypeKey != DEVICE_TYPE_MAP.end()) {
        desc.deviceName = deviceTypeKey->second;
    }
    return desc;
}

int32_t AudioSystemManagerAdapterImpl::SetDeviceChangeCallback(
    const std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter>& callback)
{
    if (callback == nullptr) {
        WVLOG_E("audio device change callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    DeviceFlag deviceFlag = DeviceFlag::OUTPUT_DEVICES_FLAG;
    deviceChangeCallback_ = std::make_shared<AudioManagerDeviceChangeCallbackAdapterImpl>(callback);
    if (deviceChangeCallback_ == nullptr) {
        WVLOG_E("audio device change callback impl is nullptr");
        return AUDIO_NULL_ERROR;
    }
    int32_t ret = AudioSystemManager::GetInstance()->SetDeviceChangeCallback(deviceFlag, deviceChangeCallback_);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio manager set audio device change callback failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::UnsetDeviceChangeCallback()
{
    int32_t ret = AudioSystemManager::GetInstance()->UnsetDeviceChangeCallback();
    return ret;
}

AudioStreamType AudioSystemManagerAdapterImpl::GetStreamType(AudioAdapterStreamType streamType)
{
    auto item = STREAM_TYPE_MAP.find(streamType);
    if (item == STREAM_TYPE_MAP.end()) {
        WVLOG_E("audio stream type not found");
        return AudioStreamType::STREAM_DEFAULT;
    }
    return item->second;
}
} // namespace OHOS::NWeb
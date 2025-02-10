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
#include "audio_device_desc_adapter_impl.h"

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

const std::string DEVICE_TYPE_NONE_ZH_CN = "无";
const std::string DEVICE_TYPE_INVALID_ZH_CN = "未知设备";
const std::string DEVICE_TYPE_EARPIECE_ZH_CN = "耳机";
const std::string DEVICE_TYPE_SPEAKER_ZH_CN = "扬声器";
const std::string DEVICE_TYPE_WIRED_HEADSET_ZH_CN = "有线耳麦";
const std::string DEVICE_TYPE_WIRED_HEADPHONES_ZH_CN = "头戴式耳机";
const std::string DEVICE_TYPE_BLUETOOTH_SCO_ZH_CN = "未知耳机";
const std::string DEVICE_TYPE_BLUETOOTH_A2DP_ZH_CN = "未知耳机";
const std::string DEVICE_TYPE_MIC_ZH_CN = "麦克风";
const std::string DEVICE_TYPE_USB_HEADSET_ZH_CN = "USB 耳机";
const std::string DEVICE_TYPE_FILE_SINK_ZH_CN = "device/file_sink";
const std::string DEVICE_TYPE_FILE_SOURCE_ZH_CN = "device/file_source";
const std::string DEVICE_TYPE_MAX_ZH_CN = "device/max";

const std::unordered_map<DeviceType, std::string> DEVICE_TYPE_NAME_ZH_CN_MAP = {
    { DeviceType::DEVICE_TYPE_NONE, DEVICE_TYPE_NONE_ZH_CN},
    { DeviceType::DEVICE_TYPE_INVALID, DEVICE_TYPE_INVALID_ZH_CN},
    { DeviceType::DEVICE_TYPE_EARPIECE, DEVICE_TYPE_EARPIECE_ZH_CN},
    { DeviceType::DEVICE_TYPE_SPEAKER, DEVICE_TYPE_SPEAKER_ZH_CN},
    { DeviceType::DEVICE_TYPE_WIRED_HEADSET, DEVICE_TYPE_WIRED_HEADSET_ZH_CN},
    { DeviceType::DEVICE_TYPE_WIRED_HEADPHONES, DEVICE_TYPE_WIRED_HEADPHONES_ZH_CN},
    { DeviceType::DEVICE_TYPE_BLUETOOTH_SCO, DEVICE_TYPE_BLUETOOTH_SCO_ZH_CN},
    { DeviceType::DEVICE_TYPE_BLUETOOTH_A2DP, DEVICE_TYPE_BLUETOOTH_A2DP_ZH_CN},
    { DeviceType::DEVICE_TYPE_MIC, DEVICE_TYPE_MIC_ZH_CN},
    { DeviceType::DEVICE_TYPE_USB_HEADSET, DEVICE_TYPE_USB_HEADSET_ZH_CN},
    { DeviceType::DEVICE_TYPE_FILE_SINK, DEVICE_TYPE_FILE_SINK_ZH_CN},
    { DeviceType::DEVICE_TYPE_FILE_SOURCE, DEVICE_TYPE_FILE_SOURCE_ZH_CN},
    { DeviceType::DEVICE_TYPE_MAX, DEVICE_TYPE_MAX_ZH_CN},
};

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
const int32_t ADAPTER_AUDIO_UNDEFINED_DEVICE_ID = 1000001;
const char* ADAPTER_AUDIO_DEFAULT_DEVICE_NAME = "(default)";
const char* ADAPTER_AUDIO_UNDEFINED_DEVICE_NAME = "(undefined)";

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

bool AudioSystemManagerAdapterImpl::GetDeviceIdByDescriptor(
    AudioDeviceDescriptor* audioDeviceDescriptor, int32_t& deviceId)
{
    if (audioDeviceDescriptor == nullptr) {
        return false;
    }
    WVLOG_I("GetDeviceIdByDescriptor: deviceId: %{public}d", deviceId);
    WVLOG_I("GetDeviceIdByDescriptor: deviceType: %{public}d", audioDeviceDescriptor->deviceType_);
    WVLOG_I("GetDeviceIdByDescriptor: macAddress_: %{public}s", audioDeviceDescriptor->macAddress_.c_str());
    if (audioOutputDeviceInfo_.empty()) {
        deviceId = 1;
        AudioDeviceDescriptor newDevice(*audioDeviceDescriptor);
        audioOutputDeviceInfo_.emplace(std::make_pair(deviceId, newDevice));
        return true;
    }

    auto it = audioOutputDeviceInfo_.begin();
    int32_t newDeviceId = 0;
    while (it != audioOutputDeviceInfo_.end()) {
        if (audioDeviceDescriptor->IsSameDeviceDesc(it->second)) {
            deviceId = it->first;
            return true;
        }
        if ((audioDeviceDescriptor->deviceType_ == DeviceType::DEVICE_TYPE_BLUETOOTH_SCO ||
            audioDeviceDescriptor->deviceType_ == DeviceType::DEVICE_TYPE_BLUETOOTH_A2DP) &&
            (audioDeviceDescriptor->macAddress_ == it->second.macAddress_)) {
            deviceId = it->first;
            it = audioOutputDeviceInfo_.erase(it);
            AudioDeviceDescriptor newDevice(*audioDeviceDescriptor);
            audioOutputDeviceInfo_.emplace(std::make_pair(deviceId, newDevice));
            return true;
        }
        newDeviceId = it->first;
        it++;
    }
    deviceId = newDeviceId + 1;
    AudioDeviceDescriptor newDevice(*audioDeviceDescriptor);
    audioOutputDeviceInfo_.emplace(std::make_pair(deviceId, newDevice));
    return true;
}

bool AudioSystemManagerAdapterImpl::HasAudioOutputDevices()
{
    DeviceType outputDeviceType = AudioSystemManager::GetInstance()->GetActiveOutputDevice();
    if (outputDeviceType == DeviceType::DEVICE_TYPE_NONE || outputDeviceType == DeviceType::DEVICE_TYPE_INVALID) {
        return false;
    }
    return true;
}

bool AudioSystemManagerAdapterImpl::HasAudioInputDevices()
{
    DeviceType inputDeviceType = AudioSystemManager::GetInstance()->GetActiveInputDevice();
    if (inputDeviceType == DeviceType::DEVICE_TYPE_NONE || inputDeviceType == DeviceType::DEVICE_TYPE_INVALID) {
        return false;
    }
    return true;
}

int32_t AudioSystemManagerAdapterImpl::RequestAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt)
{
    if (!audioInterrupt) {
        WVLOG_E("audio request audio focus failed, audioInterrupt is null");
        return AUDIO_ERROR;
    }

    AudioInterrupt interruptParams;
    interruptParams.streamUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(audioInterrupt->GetStreamUsage());
    interruptParams.contentType = AudioRendererAdapterImpl::GetAudioContentType(audioInterrupt->GetContentType());
    interruptParams.audioFocusType.streamType = GetStreamType(audioInterrupt->GetStreamType());

    int32_t ret = AudioSystemManager::GetInstance()->RequestAudioFocus(interruptParams);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio request audio focus failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::AbandonAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt)
{
    if (!audioInterrupt) {
        WVLOG_E("audio abandon audio focus failed, audioInterrupt is null");
        return AUDIO_ERROR;
    }

    AudioInterrupt interruptParams;
    interruptParams.streamUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(audioInterrupt->GetStreamUsage());
    interruptParams.contentType = AudioRendererAdapterImpl::GetAudioContentType(audioInterrupt->GetContentType());
    interruptParams.audioFocusType.streamType = GetStreamType(audioInterrupt->GetStreamType());

    int32_t ret = AudioSystemManager::GetInstance()->AbandonAudioFocus(interruptParams);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio abandon audio focus failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::SetAudioManagerInterruptCallback(
    std::shared_ptr<AudioManagerCallbackAdapter> callback)
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

std::string AudioSystemManagerAdapterImpl::GetDeviceName(DeviceType deviceType)
{
    WVLOG_I("GetDeviceName language: %{public}s", language_.c_str());
    if (language_ == "zh") {
        auto deviceTypeKey = DEVICE_TYPE_NAME_ZH_CN_MAP.find(deviceType);
        if (deviceTypeKey != DEVICE_TYPE_NAME_ZH_CN_MAP.end()) {
            return deviceTypeKey->second;
        }
        return DEVICE_TYPE_NONE_ZH_CN;
    } else {
        auto deviceTypeKey = DEVICE_TYPE_MAP.find(deviceType);
        if (deviceTypeKey != DEVICE_TYPE_MAP.end()) {
            return deviceTypeKey->second;
        }
        return DEVICE_TYPE_NONE;
    }
}

std::vector<std::shared_ptr<AudioDeviceDescAdapter>> AudioSystemManagerAdapterImpl::GetDevices(AdapterDeviceFlag flag)
{
    bool isCallDevice = false;
    auto audioScene = AudioSystemManager::GetInstance()->GetAudioScene();
    if (audioScene == AudioStandard::AudioScene::AUDIO_SCENE_PHONE_CALL ||
        audioScene == AudioStandard::AudioScene::AUDIO_SCENE_PHONE_CHAT) {
        isCallDevice = true;
    }
    std::vector<std::unique_ptr<AudioDeviceDescriptor>> audioDeviceList;
    if (flag == AdapterDeviceFlag::OUTPUT_DEVICES_FLAG) {
        audioDeviceList = AudioRoutingManager::GetInstance()->GetAvailableDevices(
            isCallDevice ? AudioDeviceUsage::CALL_OUTPUT_DEVICES : AudioDeviceUsage::MEDIA_OUTPUT_DEVICES);
    } else if (flag == AdapterDeviceFlag::INPUT_DEVICES_FLAG) {
        audioDeviceList = AudioRoutingManager::GetInstance()->GetAvailableDevices(AudioDeviceUsage::CALL_INPUT_DEVICES);
    } else if (flag == AdapterDeviceFlag::ALL_DEVICES_FLAG) {
        audioDeviceList = AudioRoutingManager::GetInstance()->GetAvailableDevices(
            isCallDevice ? AudioDeviceUsage::ALL_CALL_DEVICES : AudioDeviceUsage::ALL_MEDIA_DEVICES);
    }

    std::vector<std::shared_ptr<AudioDeviceDescAdapter>> audioDeviceAdapterList;
    for (auto& audioDevice : audioDeviceList) {
        std::shared_ptr<AudioDeviceDescAdapterImpl> desc = std::make_shared<AudioDeviceDescAdapterImpl>();
        if (!desc) {
            WVLOG_E("AudioSystemManagerAdapterImpl::GetDevices create desc failed");
            return audioDeviceAdapterList;
        }

        int32_t currentDeviceId = audioDevice->deviceId_;
        if (flag == AdapterDeviceFlag::OUTPUT_DEVICES_FLAG) {
           if (!GetDeviceIdByDescriptor(audioDevice.get(), currentDeviceId)) {
                WVLOG_E("AudioSystemManagerAdapterImpl::invalid audio device descriptor");
                continue;
            }
        }
        desc->SetDeviceId(currentDeviceId);
        if (audioDevice->deviceName_.empty()) {
            desc->SetDeviceName(GetDeviceName(audioDevice->deviceType_));
        } else {
            desc->SetDeviceName(audioDevice->deviceName_);
        }
        audioDeviceAdapterList.emplace_back(desc);
    }
    return audioDeviceAdapterList;
}

int32_t AudioSystemManagerAdapterImpl::SelectAudioOutputDevice(
    bool isCallDevice, const std::vector<sptr<AudioDeviceDescriptor>>& device) const
{
    if (isCallDevice) {
        sptr<AudioRendererFilter> filter = new (std::nothrow) AudioRendererFilter;
        if (!filter) {
            WVLOG_E("AudioSystemManagerAdapterImpl::SelectAudioOutputDevice new filter failed");
            return AUDIO_ERROR;
        }
        filter->rendererInfo.streamUsage = StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION;
        filter->rendererInfo.rendererFlags = 0;
        return AudioSystemManager::GetInstance()->SelectOutputDevice(filter, device);
    }
    return AudioSystemManager::GetInstance()->SelectOutputDevice(device);
}

int32_t AudioSystemManagerAdapterImpl::SelectDefaultAudioDevice(bool isCallDevice)
{
    WVLOG_I("Select default audio output Device.");
    AudioRendererInfo rendererInfo;
    rendererInfo.contentType = ContentType::CONTENT_TYPE_SPEECH;
    rendererInfo.streamUsage =
        isCallDevice ? StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION : StreamUsage::STREAM_USAGE_MUSIC;
    rendererInfo.rendererFlags = 0;
    std::vector<sptr<AudioDeviceDescriptor>> defaultOutputDevice;
    AudioRoutingManager::GetInstance()->GetPreferredOutputDeviceForRendererInfo(rendererInfo, defaultOutputDevice);

    return SelectAudioOutputDevice(isCallDevice, defaultOutputDevice);
}

int32_t AudioSystemManagerAdapterImpl::SelectAudioDeviceById(int32_t deviceId, bool isInput)
{
    WVLOG_I("AudioSystemManagerAdapterImpl::SelectAudioDevice deviceId: %{public}d, isInput: %{public}s", deviceId,
        isInput ? "true" : "false");
    if (deviceId == ADAPTER_AUDIO_UNDEFINED_DEVICE_ID) {
        WVLOG_E("Cannot select undefined audio device.");
        return AUDIO_ERROR;
    }
    bool isCallDevice = false;
    auto audioScene = AudioSystemManager::GetInstance()->GetAudioScene();
    if (audioScene == AudioStandard::AudioScene::AUDIO_SCENE_PHONE_CALL ||
        audioScene == AudioStandard::AudioScene::AUDIO_SCENE_PHONE_CHAT) {
        isCallDevice = true;
    }
    if (!isInput && deviceId == ADAPTER_AUDIO_DEFAULT_DEVICE_ID) {
        return SelectDefaultAudioDevice(isCallDevice);
    }
    if (isInput && deviceId == ADAPTER_AUDIO_DEFAULT_DEVICE_ID) {
        WVLOG_I("Select default audio input Device.");
        AudioCapturerInfo capturerInfo;
        capturerInfo.sourceType = AudioStandard::SourceType::SOURCE_TYPE_VOICE_COMMUNICATION;
        capturerInfo.capturerFlags = 0;
        std::vector<sptr<AudioDeviceDescriptor>> defaultInputDevice;
        AudioRoutingManager::GetInstance()->GetPreferredInputDeviceForCapturerInfo(capturerInfo, defaultInputDevice);
        return AudioSystemManager::GetInstance()->SelectInputDevice(defaultInputDevice);
    }

    std::vector<std::unique_ptr<AudioDeviceDescriptor>> audioDeviceList;
    if (isCallDevice) {
        audioDeviceList = AudioRoutingManager::GetInstance()->GetAvailableDevices(
            isInput ? AudioDeviceUsage::CALL_INPUT_DEVICES : AudioDeviceUsage::CALL_OUTPUT_DEVICES);
    } else {
        audioDeviceList = AudioRoutingManager::GetInstance()->GetAvailableDevices(
            isInput ? AudioDeviceUsage::CALL_INPUT_DEVICES : AudioDeviceUsage::MEDIA_OUTPUT_DEVICES);
    }
    for (auto& device : audioDeviceList) {
        int32_t currentDeviceId = device->deviceId_;
        if (!isInput) {
            if (!GetDeviceIdByDescriptor(device.get(), currentDeviceId)) {
                WVLOG_E("AudioSystemManagerAdapterImpl::invalid audio device descriptor");
                continue;
            }
        }
        if (currentDeviceId == deviceId) {
            std::vector<sptr<AudioDeviceDescriptor>> selectedAudioDevice { device.release() };
            return isInput ? AudioSystemManager::GetInstance()->SelectInputDevice(selectedAudioDevice)
                           : SelectAudioOutputDevice(isCallDevice, selectedAudioDevice);
        }
    }
    WVLOG_E("can't find any device by audio device id");
    return AUDIO_ERROR;
}

std::shared_ptr<AudioDeviceDescAdapter> AudioSystemManagerAdapterImpl::GetDefaultOutputDevice()
{
    bool isCallDevice = false;
    auto audioScene = AudioSystemManager::GetInstance()->GetAudioScene();
    if (audioScene == AudioStandard::AudioScene::AUDIO_SCENE_PHONE_CALL ||
        audioScene == AudioStandard::AudioScene::AUDIO_SCENE_PHONE_CHAT) {
        isCallDevice = true;
    }
    AudioRendererInfo rendererInfo;
    rendererInfo.contentType = ContentType::CONTENT_TYPE_SPEECH;
    rendererInfo.streamUsage =
        isCallDevice ? StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION : StreamUsage::STREAM_USAGE_MUSIC;
    rendererInfo.rendererFlags = 0;
    std::vector<sptr<AudioDeviceDescriptor>> defaultOutputDevice;
    AudioRoutingManager::GetInstance()->GetPreferredOutputDeviceForRendererInfo(rendererInfo, defaultOutputDevice);

    if (defaultOutputDevice.empty() || !defaultOutputDevice[0]) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultOutputDevice failed.");
        return nullptr;
    }
    auto defaultDevice = defaultOutputDevice[0];

    std::shared_ptr<AudioDeviceDescAdapterImpl> desc = std::make_shared<AudioDeviceDescAdapterImpl>();
    if (!desc) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultOutputDevice create desc failed");
        return nullptr;
    }

    int32_t currentDeviceId = 0;
    if (!GetDeviceIdByDescriptor(defaultDevice, currentDeviceId)) {
        WVLOG_E("AudioSystemManagerAdapterImpl::invalid audio device descriptor");
        return nullptr;
    }
    desc->SetDeviceId(currentDeviceId);
    if (defaultDevice->deviceName_.empty()) {
        desc->SetDeviceName(GetDeviceName(defaultDevice->deviceType_));
    } else {
        desc->SetDeviceName(defaultDevice->deviceName_);
    }
    return desc;
}

std::shared_ptr<AudioDeviceDescAdapter> AudioSystemManagerAdapterImpl::GetDefaultInputDevice()
{
    AudioCapturerInfo capturerInfo;
    capturerInfo.sourceType = AudioStandard::SourceType::SOURCE_TYPE_VOICE_COMMUNICATION;
    capturerInfo.capturerFlags = 0;
    std::vector<sptr<AudioDeviceDescriptor>> defaultInputDevice;
    AudioRoutingManager::GetInstance()->GetPreferredInputDeviceForCapturerInfo(capturerInfo, defaultInputDevice);
    if (defaultInputDevice.empty() || !defaultInputDevice[0]) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultInputDevice failed.");
        return nullptr;
    }
    auto defaultDevice = defaultInputDevice[0];

    std::shared_ptr<AudioDeviceDescAdapterImpl> desc = std::make_shared<AudioDeviceDescAdapterImpl>();
    if (!desc) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultOutputDevice create desc failed");
        return nullptr;
    }

    int32_t currentDeviceId = 0;
    if (!GetDeviceIdByDescriptor(defaultDevice, currentDeviceId)) {
        WVLOG_E("AudioSystemManagerAdapterImpl::invalid audio device descriptor");
        return nullptr;
    }
    desc->SetDeviceId(currentDeviceId);
    if (defaultDevice->deviceName_.empty()) {
        desc->SetDeviceName(GetDeviceName(defaultDevice->deviceType_));
    } else {
        desc->SetDeviceName(defaultDevice->deviceName_);
    }
    return desc;
}

bool AudioSystemManagerAdapterImpl::SetLanguage(const std::string& language)
{
    if (language.empty()) {
        WVLOG_E("SetLanguage language is nullptr");
        return false;
    }
    language_ = language;
    return true;
}

int32_t AudioSystemManagerAdapterImpl::SetDeviceChangeCallback(
    std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> callback)
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

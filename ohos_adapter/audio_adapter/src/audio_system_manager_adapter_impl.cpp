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

#include <ohaudio/native_audio_device_base.h>
#include <ohaudio/native_audio_manager.h>
#include <ohaudio/native_audio_routing_manager.h>
#include <ohaudio/native_audiostreambuilder.h>
#include <unordered_map>

#include "audio_device_desc_adapter_impl.h"
#define WVLOG_E(...)
#define WVLOG_I(...)

namespace OHOS::NWeb {
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
const std::string DEVICE_TYPE_DISPLAY_PORT = "device/display_port";
const std::string DEVICE_TYPE_REMOTE_CAST = "device/remote_cast";
const std::string DEVICE_TYPE_DEFAULT = "device/default";
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
const std::string DEVICE_TYPE_DISPLAY_PORT_ZH_CN = "DP设备";
const std::string DEVICE_TYPE_REMOTE_CAST_ZH_CN = "投屏设备";
const std::string DEVICE_TYPE_DEFAULT_ZH_CN = "device/default";
const std::string DEVICE_TYPE_MAX_ZH_CN = "device/max";

const std::unordered_map<OH_AudioDevice_Type, std::string> OH_DEVICE_TYPE_NAME_ZH_CN_MAP = {
    { AUDIO_DEVICE_TYPE_INVALID, DEVICE_TYPE_INVALID_ZH_CN },
    { AUDIO_DEVICE_TYPE_EARPIECE, DEVICE_TYPE_EARPIECE_ZH_CN },
    { AUDIO_DEVICE_TYPE_SPEAKER, DEVICE_TYPE_SPEAKER_ZH_CN },
    { AUDIO_DEVICE_TYPE_WIRED_HEADSET, DEVICE_TYPE_WIRED_HEADSET_ZH_CN },
    { AUDIO_DEVICE_TYPE_WIRED_HEADPHONES, DEVICE_TYPE_WIRED_HEADPHONES_ZH_CN },
    { AUDIO_DEVICE_TYPE_BLUETOOTH_SCO, DEVICE_TYPE_BLUETOOTH_SCO_ZH_CN },
    { AUDIO_DEVICE_TYPE_BLUETOOTH_A2DP, DEVICE_TYPE_BLUETOOTH_A2DP_ZH_CN },
    { AUDIO_DEVICE_TYPE_MIC, DEVICE_TYPE_MIC_ZH_CN },
    { AUDIO_DEVICE_TYPE_USB_HEADSET, DEVICE_TYPE_USB_HEADSET_ZH_CN },
    { AUDIO_DEVICE_TYPE_DISPLAY_PORT, DEVICE_TYPE_DISPLAY_PORT_ZH_CN },
    { AUDIO_DEVICE_TYPE_REMOTE_CAST, DEVICE_TYPE_REMOTE_CAST_ZH_CN },
    { AUDIO_DEVICE_TYPE_DEFAULT, DEVICE_TYPE_DEFAULT_ZH_CN },
};

const std::unordered_map<OH_AudioDevice_Type, std::string> OH_DEVICE_TYPE_MAP = {
    { AUDIO_DEVICE_TYPE_INVALID, DEVICE_TYPE_INVALID },
    { AUDIO_DEVICE_TYPE_EARPIECE, DEVICE_TYPE_EARPIECE },
    { AUDIO_DEVICE_TYPE_SPEAKER, DEVICE_TYPE_SPEAKER },
    { AUDIO_DEVICE_TYPE_WIRED_HEADSET, DEVICE_TYPE_WIRED_HEADSET },
    { AUDIO_DEVICE_TYPE_WIRED_HEADPHONES, DEVICE_TYPE_WIRED_HEADPHONES },
    { AUDIO_DEVICE_TYPE_BLUETOOTH_SCO, DEVICE_TYPE_BLUETOOTH_SCO },
    { AUDIO_DEVICE_TYPE_BLUETOOTH_A2DP, DEVICE_TYPE_BLUETOOTH_A2DP },
    { AUDIO_DEVICE_TYPE_MIC, DEVICE_TYPE_MIC },
    { AUDIO_DEVICE_TYPE_USB_HEADSET, DEVICE_TYPE_USB_HEADSET },
    { AUDIO_DEVICE_TYPE_DISPLAY_PORT, DEVICE_TYPE_DISPLAY_PORT },
    { AUDIO_DEVICE_TYPE_REMOTE_CAST, DEVICE_TYPE_REMOTE_CAST },
    { AUDIO_DEVICE_TYPE_DEFAULT, DEVICE_TYPE_DEFAULT },
};

const int32_t ADAPTER_AUDIO_DEFAULT_DEVICE_ID = 1000000;
const int32_t ADAPTER_AUDIO_UNDEFINED_DEVICE_ID = 1000001;
const char* ADAPTER_AUDIO_DEFAULT_DEVICE_NAME = "(default)";
const char* ADAPTER_AUDIO_UNDEFINED_DEVICE_NAME = "(undefined)";

AudioSystemManagerAdapterImpl& AudioSystemManagerAdapterImpl::GetInstance()
{
    static AudioSystemManagerAdapterImpl instance;
    return instance;
}

bool AudioSystemManagerAdapterImpl::HasAudioOutputDevices()
{
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    OH_AudioDeviceDescriptorArray* audioDeviceArray;
    OH_AudioCommon_Result ohRet =
        OH_AudioRoutingManager_GetDevices(audioRoutingManager, AUDIO_DEVICE_FLAG_OUTPUT, &audioDeviceArray);
    if (ohRet != AUDIOCOMMON_RESULT_SUCCESS) {
        return false;
    }
    OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
    return true;
}

bool AudioSystemManagerAdapterImpl::HasAudioInputDevices()
{
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    OH_AudioDeviceDescriptorArray* audioDeviceArray;
    OH_AudioCommon_Result ohRet =
        OH_AudioRoutingManager_GetDevices(audioRoutingManager, AUDIO_DEVICE_FLAG_INPUT, &audioDeviceArray);
    if (ohRet != AUDIOCOMMON_RESULT_SUCCESS) {
        return false;
    }
    OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
    return true;
}

int32_t AudioSystemManagerAdapterImpl::RequestAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt)
{
    if (!audioInterrupt) {
        WVLOG_E("audio request audio focus failed, audioInterrupt is null");
        return AUDIO_ERROR;
    }

    // in actual, this interface is invalid.
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::AbandonAudioFocus(const std::shared_ptr<AudioInterruptAdapter> audioInterrupt)
{
    if (!audioInterrupt) {
        WVLOG_E("audio abandon audio focus failed, audioInterrupt is null");
        return AUDIO_ERROR;
    }

    // in actual, this interface is invalid.
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::SetAudioManagerInterruptCallback(
    std::shared_ptr<AudioManagerCallbackAdapter> callback)
{
    if (callback == nullptr) {
        WVLOG_E("set audio manager interrupt callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    // in actual, this interface is invalid.
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::UnsetAudioManagerInterruptCallback()
{
    // in actual, this interface is invalid.
    return AUDIO_OK;
}

std::string AudioSystemManagerAdapterImpl::GetDeviceName(OH_AudioDevice_Type deviceType)
{
    WVLOG_I("GetDeviceName language: %{public}s", language_.c_str());
    if (language_ == "zh") {
        auto deviceTypeKey = OH_DEVICE_TYPE_NAME_ZH_CN_MAP.find(deviceType);
        if (deviceTypeKey != OH_DEVICE_TYPE_NAME_ZH_CN_MAP.end()) {
            return deviceTypeKey->second;
        }
        return DEVICE_TYPE_NONE_ZH_CN;
    } else {
        auto deviceTypeKey = OH_DEVICE_TYPE_MAP.find(deviceType);
        if (deviceTypeKey != OH_DEVICE_TYPE_MAP.end()) {
            return deviceTypeKey->second;
        }
        return DEVICE_TYPE_NONE;
    }
}

std::vector<std::shared_ptr<AudioDeviceDescAdapter>> AudioSystemManagerAdapterImpl::GetDevices(AdapterDeviceFlag flag)
{
    bool isCallDevice = false;
    OH_AudioManager* audioManager;
    OH_AudioScene audioScene;
    OH_GetAudioManager(&audioManager);
    OH_GetAudioScene(audioManager, &audioScene);
    if (audioScene == AUDIO_SCENE_PHONE_CALL || audioScene == AUDIO_SCENE_VOICE_CHAT) {
        isCallDevice = true;
    }
    std::vector<std::shared_ptr<AudioDeviceDescAdapter>> audioDeviceAdapterList;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioDeviceDescriptorArray* audioDeviceArray = nullptr;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    if (flag == AdapterDeviceFlag::OUTPUT_DEVICES_FLAG) {
        OH_AudioRoutingManager_GetAvailableDevices(audioRoutingManager,
            isCallDevice ? AUDIO_DEVICE_USAGE_CALL_OUTPUT : AUDIO_DEVICE_USAGE_MEDIA_OUTPUT, &audioDeviceArray);
    } else if (flag == AdapterDeviceFlag::INPUT_DEVICES_FLAG) {
        OH_AudioRoutingManager_GetAvailableDevices(
            audioRoutingManager, AUDIO_DEVICE_USAGE_CALL_INPUT, &audioDeviceArray);
    } else if (flag == AdapterDeviceFlag::ALL_DEVICES_FLAG) {
        OH_AudioRoutingManager_GetAvailableDevices(audioRoutingManager,
            isCallDevice ? AUDIO_DEVICE_USAGE_CALL_ALL : AUDIO_DEVICE_USAGE_MEDIA_ALL, &audioDeviceArray);
    }

    if (audioDeviceArray == nullptr) {
        return audioDeviceAdapterList;
    }
    for (uint32_t i = 0; i < audioDeviceArray->size; i++) {
        OH_AudioDeviceDescriptor* ohAudioDeviceDescriptor = (OH_AudioDeviceDescriptor*)audioDeviceArray->descriptors[i];
        std::shared_ptr<AudioDeviceDescAdapterImpl> desc = std::make_shared<AudioDeviceDescAdapterImpl>();
        if (!desc) {
            return audioDeviceAdapterList;
        }
        uint32_t deviceId = 0;
        OH_AudioDeviceDescriptor_GetDeviceId(ohAudioDeviceDescriptor, &deviceId);
        desc->SetDeviceId(deviceId);
        char* deviceNameNDK;
        OH_AudioDeviceDescriptor_GetDeviceName(ohAudioDeviceDescriptor, &deviceNameNDK);
        std::string deviceName(deviceNameNDK);
        if (deviceName.empty()) {
            OH_AudioDevice_Type deviceType;
            OH_AudioDeviceDescriptor_GetDeviceType(ohAudioDeviceDescriptor, &deviceType);
            desc->SetDeviceName(GetDeviceName(deviceType));
        } else {
            desc->SetDeviceName(deviceName);
        }
        audioDeviceAdapterList.emplace_back(desc);
    }
    OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
    return audioDeviceAdapterList;
}

int32_t AudioSystemManagerAdapterImpl::SelectAudioOutputDevice(
    bool isCallDevice, OH_AudioDeviceDescriptorArray& audioDeviceArray) const
{
    // 1. create builder
    OH_AudioStreamBuilder* builder;
    OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_RENDERER);
    // 2. create audioRenderer
    OH_AudioRenderer* audioRenderer;
    OH_AudioDevice_Type deviceType;
    OH_AudioDeviceDescriptor* defaultDeviceDescriptor = (OH_AudioDeviceDescriptor*)audioDeviceArray.descriptors[0];
    OH_AudioDeviceDescriptor_GetDeviceType(defaultDeviceDescriptor, &deviceType);
    OH_AudioStreamBuilder_GenerateRenderer(builder, &audioRenderer);
    return (int32_t)OH_AudioRenderer_SetDefaultOutputDevice(audioRenderer, deviceType);
}

int32_t AudioSystemManagerAdapterImpl::SelectAudioDeviceByIdImpl(int32_t deviceId, bool isInput, bool isCallDevice)
{
    OH_AudioDeviceDescriptorArray* audioDeviceArray = nullptr;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    if (isCallDevice) {
        OH_AudioRoutingManager_GetAvailableDevices(audioRoutingManager,
            isInput ? AUDIO_DEVICE_USAGE_CALL_INPUT : AUDIO_DEVICE_USAGE_CALL_OUTPUT, &audioDeviceArray);
    } else {
        OH_AudioRoutingManager_GetAvailableDevices(audioRoutingManager,
            isInput ? AUDIO_DEVICE_USAGE_CALL_INPUT : AUDIO_DEVICE_USAGE_MEDIA_OUTPUT, &audioDeviceArray);
    }
    if (audioDeviceArray == nullptr) {
        WVLOG_E("AudioSystemManagerAdapterImpl::SelectAudioDeviceByIdImpl device list empty");
        return AUDIO_ERROR;
    }
    for (uint32_t i = 0; i < audioDeviceArray->size; i++) {
        uint32_t ohAudioDeviceId = 0;
        OH_AudioDeviceDescriptor* ohAudioDeviceDescriptor = (OH_AudioDeviceDescriptor*)audioDeviceArray->descriptors[i];
        OH_AudioDeviceDescriptor_GetDeviceId(ohAudioDeviceDescriptor, &ohAudioDeviceId);
        if (ohAudioDeviceId == deviceId) {
            OH_AudioDeviceDescriptorArray* tmpAudioDeviceArray =
                (OH_AudioDeviceDescriptorArray*)malloc(sizeof(OH_AudioDeviceDescriptorArray));
            tmpAudioDeviceArray->size = 1;
            tmpAudioDeviceArray->descriptors[0] = ohAudioDeviceDescriptor;
            int32_t ret =
                isInput ? AUDIOSTREAM_ERROR_ILLEGAL_STATE : SelectAudioOutputDevice(isCallDevice, *tmpAudioDeviceArray);
            free(tmpAudioDeviceArray);
            OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
            return ret;
        }
    }
    OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
    return AUDIO_ERROR;
}

int32_t AudioSystemManagerAdapterImpl::SelectAudioDeviceById(int32_t deviceId, bool isInput)
{
    if (deviceId == ADAPTER_AUDIO_UNDEFINED_DEVICE_ID) {
        WVLOG_E("Cannot select undefined audio device.");
        return AUDIO_ERROR;
    }
    bool isCallDevice = false;
    OH_AudioManager* audioManager;
    OH_AudioScene audioScene;
    OH_GetAudioManager(&audioManager);
    OH_GetAudioScene(audioManager, &audioScene);
    if (audioScene == AUDIO_SCENE_PHONE_CALL || audioScene == AUDIO_SCENE_VOICE_CHAT) {
        isCallDevice = true;
    }
    OH_AudioDeviceDescriptorArray* audioDeviceArray = nullptr;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    if (!isInput && deviceId == ADAPTER_AUDIO_DEFAULT_DEVICE_ID) {
        WVLOG_I("Select default audio output Device.");
        OH_AudioRoutingManager_GetPreferredOutputDevice(audioRoutingManager,
            isCallDevice ? AUDIOSTREAM_USAGE_VOICE_COMMUNICATION : AUDIOSTREAM_USAGE_MUSIC, &audioDeviceArray);
        int32_t ret =
            audioDeviceArray == nullptr ? AUDIO_ERROR : SelectAudioOutputDevice(isCallDevice, *audioDeviceArray);
        OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
        return ret;
    }
    if (isInput && deviceId == ADAPTER_AUDIO_DEFAULT_DEVICE_ID) {
        return AUDIOSTREAM_ERROR_ILLEGAL_STATE;
    }
    return SelectAudioDeviceByIdImpl(deviceId, isInput, isCallDevice);
}

std::shared_ptr<AudioDeviceDescAdapter> AudioSystemManagerAdapterImpl::GetDefaultOutputDevice()
{
    bool isCallDevice = false;
    OH_AudioManager* audioManager;
    OH_AudioScene audioScene;
    OH_GetAudioManager(&audioManager);
    OH_GetAudioScene(audioManager, &audioScene);
    if (audioScene == AUDIO_SCENE_PHONE_CALL || audioScene == AUDIO_SCENE_VOICE_CHAT) {
        isCallDevice = true;
    }
    OH_AudioDeviceDescriptorArray* audioDeviceArray = nullptr;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    OH_AudioRoutingManager_GetPreferredOutputDevice(audioRoutingManager,
        isCallDevice ? AUDIOSTREAM_USAGE_VOICE_COMMUNICATION : AUDIOSTREAM_USAGE_MUSIC, &audioDeviceArray);

    if (audioDeviceArray == nullptr) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultOutputDevice failed.");
        return nullptr;
    }

    uint32_t ohAudioDeviceId = 0;
    OH_AudioDeviceDescriptor* defaultDeviceDescriptor = (OH_AudioDeviceDescriptor*)audioDeviceArray->descriptors[0];
    OH_AudioDeviceDescriptor_GetDeviceId(defaultDeviceDescriptor, &ohAudioDeviceId);

    std::shared_ptr<AudioDeviceDescAdapterImpl> desc = std::make_shared<AudioDeviceDescAdapterImpl>();
    if (!desc) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultOutputDevice create desc failed");
        return nullptr;
    }

    desc->SetDeviceId(ohAudioDeviceId);
    char* deviceNameNDK;
    OH_AudioDeviceDescriptor_GetDeviceName(defaultDeviceDescriptor, &deviceNameNDK);
    std::string deviceName(deviceNameNDK);
    if (deviceName.empty()) {
        OH_AudioDevice_Type deviceType;
        OH_AudioDeviceDescriptor_GetDeviceType(defaultDeviceDescriptor, &deviceType);
        desc->SetDeviceName(GetDeviceName(deviceType));
    } else {
        desc->SetDeviceName(deviceName);
    }
    OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
    return desc;
}

std::shared_ptr<AudioDeviceDescAdapter> AudioSystemManagerAdapterImpl::GetDefaultInputDevice()
{
    OH_AudioDeviceDescriptorArray* audioDeviceArray = nullptr;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    OH_AudioRoutingManager_GetPreferredInputDevice(
        audioRoutingManager, AUDIOSTREAM_SOURCE_TYPE_VOICE_COMMUNICATION, &audioDeviceArray);

    if (audioDeviceArray == nullptr) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultInputDevice failed.");
        return nullptr;
    }

    uint32_t ohAudioDeviceId = 0;
    OH_AudioDeviceDescriptor* defaultDeviceDescriptor = (OH_AudioDeviceDescriptor*)audioDeviceArray->descriptors[0];
    OH_AudioDeviceDescriptor_GetDeviceId(defaultDeviceDescriptor, &ohAudioDeviceId);

    std::shared_ptr<AudioDeviceDescAdapterImpl> desc = std::make_shared<AudioDeviceDescAdapterImpl>();
    if (!desc) {
        WVLOG_E("AudioSystemManagerAdapterImpl::GetDefaultOutputDevice create desc failed");
        return nullptr;
    }

    desc->SetDeviceId(ohAudioDeviceId);
    char* deviceNameNDK;
    OH_AudioDeviceDescriptor_GetDeviceName(defaultDeviceDescriptor, &deviceNameNDK);
    std::string deviceName(deviceNameNDK);
    if (deviceName.empty()) {
        OH_AudioDevice_Type deviceType;
        OH_AudioDeviceDescriptor_GetDeviceType(defaultDeviceDescriptor, &deviceType);
        desc->SetDeviceName(GetDeviceName(deviceType));
    } else {
        desc->SetDeviceName(deviceName);
    }
    OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
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
static std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> ohCallbackImpl = nullptr;
static int32_t OHDeviceChangedCallback(
    OH_AudioDevice_ChangeType type, OH_AudioDeviceDescriptorArray* audioDeviceDescriptorArray)
{
    ohCallbackImpl->OnDeviceChange();
    return 0;
}

int32_t AudioSystemManagerAdapterImpl::SetDeviceChangeCallback(
    std::shared_ptr<AudioManagerDeviceChangeCallbackAdapter> callback)
{
    if (callback == nullptr) {
        WVLOG_E("audio device change callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    ohCallbackImpl = callback;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    OH_AudioCommon_Result ohRet = OH_AudioRoutingManager_RegisterDeviceChangeCallback(
        audioRoutingManager, AUDIO_DEVICE_FLAG_OUTPUT, OHDeviceChangedCallback);
    if (ohRet != AUDIOCOMMON_RESULT_SUCCESS) {
        WVLOG_E("audio manager set audio device change callback failed, code: %{public}d", ohRet);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioSystemManagerAdapterImpl::UnsetDeviceChangeCallback()
{
    ohCallbackImpl = nullptr;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    int32_t ret = (int32_t)OH_AudioRoutingManager_UnregisterDeviceChangeCallback(audioRoutingManager, nullptr);
    return ret;
}
} // namespace OHOS::NWeb

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

#include "ohos_adapter/bridge/ark_audio_system_manager_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_audio_device_desc_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_interrupt_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_manager_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_manager_device_change_callback_adapter_impl.h"
#include "ohos_adapter/ctocpp/ark_audio_device_desc_adapter_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioSystemManagerAdapterWrapper::ArkAudioSystemManagerAdapterWrapper(ArkWebRefPtr<ArkAudioSystemManagerAdapter> ref)
    : ctocpp_(ref)
{}

bool ArkAudioSystemManagerAdapterWrapper::HasAudioOutputDevices()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->HasAudioOutputDevices();
}

bool ArkAudioSystemManagerAdapterWrapper::HasAudioInputDevices()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->HasAudioInputDevices();
}

int32_t ArkAudioSystemManagerAdapterWrapper::RequestAudioFocus(
    const std::shared_ptr<NWeb::AudioInterruptAdapter> audioInterrupt)
{
    if (!ctocpp_) {
        return -1;
    }

    if (!audioInterrupt) {
        return ctocpp_->RequestAudioFocus(nullptr);
    }

    return ctocpp_->RequestAudioFocus(new ArkAudioInterruptAdapterImpl(audioInterrupt));
}

int32_t ArkAudioSystemManagerAdapterWrapper::AbandonAudioFocus(
    const std::shared_ptr<NWeb::AudioInterruptAdapter> audioInterrupt)
{
    if (!ctocpp_) {
        return -1;
    }

    if (!audioInterrupt) {
        return ctocpp_->AbandonAudioFocus(nullptr);
    }

    return ctocpp_->AbandonAudioFocus(new ArkAudioInterruptAdapterImpl(audioInterrupt));
}

int32_t ArkAudioSystemManagerAdapterWrapper::SetAudioManagerInterruptCallback(
    std::shared_ptr<OHOS::NWeb::AudioManagerCallbackAdapter> callback)
{
    if (!ctocpp_) {
        return -1;
    }
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->SetAudioManagerInterruptCallback(nullptr);
    }

    return ctocpp_->SetAudioManagerInterruptCallback(new ArkAudioManagerCallbackAdapterImpl(callback));
}

int32_t ArkAudioSystemManagerAdapterWrapper::UnsetAudioManagerInterruptCallback()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->UnsetAudioManagerInterruptCallback();
}

std::vector<std::shared_ptr<NWeb::AudioDeviceDescAdapter>> ArkAudioSystemManagerAdapterWrapper::GetDevices(
    NWeb::AdapterDeviceFlag flag)
{
    if (!ctocpp_) {
        return std::vector<std::shared_ptr<NWeb::AudioDeviceDescAdapter>>();
    }

    ArkAudioDeviceDescAdapterVector ark_vector = ctocpp_->GetDevices((int32_t)flag);

    std::vector<std::shared_ptr<NWeb::AudioDeviceDescAdapter>> result =
        ArkAudioDeviceDescAdapterVectorStructToClass(ark_vector);

    ArkAudioDeviceDescAdapterVectorStructRelease(ark_vector);

    return result;
}

int32_t ArkAudioSystemManagerAdapterWrapper::SelectAudioDeviceById(int32_t deviceId, bool isInput)
{
    if (!ctocpp_) {
        return -1;
    }

    return ctocpp_->SelectAudioDeviceById(deviceId, isInput);
}

int32_t ArkAudioSystemManagerAdapterWrapper::SetDeviceChangeCallback(
    const std::shared_ptr<OHOS::NWeb::AudioManagerDeviceChangeCallbackAdapter> callback)
{
    if (!ctocpp_) {
        return -1;
    }

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->SetDeviceChangeCallback(nullptr);
    }

    return ctocpp_->SetDeviceChangeCallback(new ArkAudioManagerDeviceChangeCallbackAdapterImpl(callback));
}

int32_t ArkAudioSystemManagerAdapterWrapper::UnsetDeviceChangeCallback()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->UnsetDeviceChangeCallback();
}

std::shared_ptr<NWeb::AudioDeviceDescAdapter> ArkAudioSystemManagerAdapterWrapper::GetDefaultOutputDevice()
{
    if (!ctocpp_) {
        return nullptr;
    }

    ArkWebRefPtr<ArkAudioDeviceDescAdapter> adapter = ctocpp_->GetDefaultOutputDevice();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkAudioDeviceDescAdapterWrapper>(adapter);
}

std::shared_ptr<NWeb::AudioDeviceDescAdapter> ArkAudioSystemManagerAdapterWrapper::GetDefaultInputDevice()
{
    if (!ctocpp_) {
        return nullptr;
    }

    ArkWebRefPtr<ArkAudioDeviceDescAdapter> adapter = ctocpp_->GetDefaultInputDevice();
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkAudioDeviceDescAdapterWrapper>(adapter);
}

bool ArkAudioSystemManagerAdapterWrapper::SetLanguage(const std::string& language)
{
    if (!ctocpp_) {
        return false;
    }

    return ctocpp_->SetLanguage(ArkWebStringClassToStruct(language));
}

} // namespace OHOS::ArkWeb

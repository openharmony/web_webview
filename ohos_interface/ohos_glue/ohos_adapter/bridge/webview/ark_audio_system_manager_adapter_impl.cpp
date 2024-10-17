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

#include "ohos_adapter/bridge/ark_audio_system_manager_adapter_impl.h"

#include "ohos_adapter/bridge/ark_audio_device_desc_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_interrupt_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_manager_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_manager_device_change_callback_adapter_wrapper.h"
#include "ohos_adapter/cpptoc/ark_audio_device_desc_adapter_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioSystemManagerAdapterImpl::ArkAudioSystemManagerAdapterImpl(NWeb::AudioSystemManagerAdapter& ref) : real_(ref) {}

bool ArkAudioSystemManagerAdapterImpl::HasAudioOutputDevices()
{
    return real_.HasAudioOutputDevices();
}

bool ArkAudioSystemManagerAdapterImpl::HasAudioInputDevices()
{
    return real_.HasAudioInputDevices();
}

int32_t ArkAudioSystemManagerAdapterImpl::RequestAudioFocus(const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt)
{
    if (CHECK_REF_PTR_IS_NULL(audioInterrupt)) {
        return real_.RequestAudioFocus(nullptr);
    }
    return real_.RequestAudioFocus(std::make_shared<ArkAudioInterruptAdapterWrapper>(audioInterrupt));
}

int32_t ArkAudioSystemManagerAdapterImpl::AbandonAudioFocus(const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt)
{
    if (CHECK_REF_PTR_IS_NULL(audioInterrupt)) {
        return real_.AbandonAudioFocus(nullptr);
    }
    return real_.AbandonAudioFocus(std::make_shared<ArkAudioInterruptAdapterWrapper>(audioInterrupt));
}

int32_t ArkAudioSystemManagerAdapterImpl::SetAudioManagerInterruptCallback(
    ArkWebRefPtr<ArkAudioManagerCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_.SetAudioManagerInterruptCallback(nullptr);
    }

    return real_.SetAudioManagerInterruptCallback(std::make_shared<ArkAudioManagerCallbackAdapterWrapper>(callback));
}

int32_t ArkAudioSystemManagerAdapterImpl::UnsetAudioManagerInterruptCallback()
{
    return real_.UnsetAudioManagerInterruptCallback();
}

ArkAudioDeviceDescAdapterVector ArkAudioSystemManagerAdapterImpl::GetDevices(int32_t flag)
{
    std::vector<std::shared_ptr<NWeb::AudioDeviceDescAdapter>> descs = real_.GetDevices((NWeb::AdapterDeviceFlag)flag);
    ArkAudioDeviceDescAdapterVector result = ArkAudioDeviceDescAdapterVectorClassToStruct(descs);
    return result;
}

int32_t ArkAudioSystemManagerAdapterImpl::SelectAudioDeviceById(int32_t id, bool isInput)
{
    return real_.SelectAudioDeviceById(id, isInput);
}

int32_t ArkAudioSystemManagerAdapterImpl::SetDeviceChangeCallback(
    ArkWebRefPtr<ArkAudioManagerDeviceChangeCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_.SetDeviceChangeCallback(nullptr);
    }

    return real_.SetDeviceChangeCallback(std::make_shared<ArkAudioManagerDeviceChangeCallbackAdapterWrapper>(callback));
}

int32_t ArkAudioSystemManagerAdapterImpl::UnsetDeviceChangeCallback()
{
    return real_.UnsetDeviceChangeCallback();
}

ArkWebRefPtr<ArkAudioDeviceDescAdapter> ArkAudioSystemManagerAdapterImpl::GetDefaultOutputDevice()
{
    std::shared_ptr<NWeb::AudioDeviceDescAdapter> desc = real_.GetDefaultOutputDevice();
    if (CHECK_SHARED_PTR_IS_NULL(desc)) {
        return nullptr;
    }

    return new ArkAudioDeviceDescAdapterImpl(desc);
}

ArkWebRefPtr<ArkAudioDeviceDescAdapter> ArkAudioSystemManagerAdapterImpl::GetDefaultInputDevice()
{
    std::shared_ptr<NWeb::AudioDeviceDescAdapter> desc = real_.GetDefaultInputDevice();
    if (CHECK_SHARED_PTR_IS_NULL(desc)) {
        return nullptr;
    }

    return new ArkAudioDeviceDescAdapterImpl(desc);
}

bool ArkAudioSystemManagerAdapterImpl::SetLanguage(ArkWebString language)
{
    return real_.SetLanguage(ArkWebStringStructToClass(language));
}

} // namespace OHOS::ArkWeb

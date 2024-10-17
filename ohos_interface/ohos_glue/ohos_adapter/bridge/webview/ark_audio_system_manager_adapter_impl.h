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

#ifndef ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H
#define ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H
#pragma once

#include "audio_system_manager_adapter.h"
#include "ohos_adapter/include/ark_audio_system_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkAudioSystemManagerAdapterImpl : public ArkAudioSystemManagerAdapter {
public:
    explicit ArkAudioSystemManagerAdapterImpl(NWeb::AudioSystemManagerAdapter&);

    bool HasAudioOutputDevices() override;

    bool HasAudioInputDevices() override;

    int32_t RequestAudioFocus(const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt) override;

    int32_t AbandonAudioFocus(const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt) override;

    int32_t SetAudioManagerInterruptCallback(ArkWebRefPtr<ArkAudioManagerCallbackAdapter> callback) override;

    int32_t UnsetAudioManagerInterruptCallback() override;

    ArkAudioDeviceDescAdapterVector GetDevices(int32_t flag) override;

    int32_t SelectAudioDeviceById(int32_t deviceId, bool isInput) override;

    int32_t SetDeviceChangeCallback(ArkWebRefPtr<ArkAudioManagerDeviceChangeCallbackAdapter> callback) override;

    int32_t UnsetDeviceChangeCallback() override;

    ArkWebRefPtr<ArkAudioDeviceDescAdapter> GetDefaultOutputDevice() override;

    ArkWebRefPtr<ArkAudioDeviceDescAdapter> GetDefaultInputDevice() override;

    bool SetLanguage(ArkWebString language) override;

private:
    NWeb::AudioSystemManagerAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkAudioSystemManagerAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_IMPL_H

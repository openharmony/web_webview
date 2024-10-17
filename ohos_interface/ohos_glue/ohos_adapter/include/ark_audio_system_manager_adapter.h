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

#ifndef ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_H
#define ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_audio_device_desc_adapter_vector.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkAudioInterruptAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetStreamUsage() = 0;

    /*--ark web()--*/
    virtual int32_t GetContentType() = 0;

    /*--ark web()--*/
    virtual int32_t GetStreamType() = 0;

    /*--ark web()--*/
    virtual uint32_t GetSessionID() = 0;

    /*--ark web()--*/
    virtual bool GetPauseWhenDucked() = 0;
};

/*--ark web(source=webview)--*/
class ArkAudioDeviceDescAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetDeviceId() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetDeviceName() = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioManagerCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnSuspend() = 0;

    /*--ark web()--*/
    virtual void OnResume() = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioManagerDeviceChangeCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnDeviceChange() = 0;
};

/*--ark web(source=webview)--*/
class ArkAudioSystemManagerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool HasAudioOutputDevices() = 0;

    /*--ark web()--*/
    virtual bool HasAudioInputDevices() = 0;

    /*--ark web()--*/
    virtual int32_t RequestAudioFocus(const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt) = 0;

    /*--ark web()--*/
    virtual int32_t AbandonAudioFocus(const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt) = 0;

    /*--ark web()--*/
    virtual int32_t SetAudioManagerInterruptCallback(ArkWebRefPtr<ArkAudioManagerCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual int32_t UnsetAudioManagerInterruptCallback() = 0;

    /*--ark web()--*/
    virtual ArkAudioDeviceDescAdapterVector GetDevices(int32_t flag) = 0;

    /*--ark web()--*/
    virtual int32_t SelectAudioDeviceById(int32_t deviceId, bool isInput) = 0;

    /*--ark web()--*/
    virtual int32_t SetDeviceChangeCallback(ArkWebRefPtr<ArkAudioManagerDeviceChangeCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual int32_t UnsetDeviceChangeCallback() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioDeviceDescAdapter> GetDefaultOutputDevice() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioDeviceDescAdapter> GetDefaultInputDevice() = 0;

    /*--ark web()--*/
    virtual bool SetLanguage(ArkWebString language) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_SYSTEM_MANAGER_ADAPTER_H

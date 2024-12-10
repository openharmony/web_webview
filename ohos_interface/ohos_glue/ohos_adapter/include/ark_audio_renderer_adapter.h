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

#ifndef ARK_AUDIO_RENDERER_ADAPTER_H
#define ARK_AUDIO_RENDERER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkAudioRendererOptionsAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetSamplingRate() = 0;

    /*--ark web()--*/
    virtual int32_t GetEncodingType() = 0;

    /*--ark web()--*/
    virtual int32_t GetSampleFormat() = 0;

    /*--ark web()--*/
    virtual int32_t GetChannel() = 0;

    /*--ark web()--*/
    virtual int32_t GetContentType() = 0;

    /*--ark web()--*/
    virtual int32_t GetStreamUsage() = 0;

    /*--ark web()--*/
    virtual int32_t GetRenderFlags() = 0;

    /*--ark web()--*/
    virtual int32_t GetConcurrencyMode() = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioRendererCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnSuspend() = 0;

    /*--ark web()--*/
    virtual void OnResume() = 0;
};

/*--ark web(source=webcore)--*/
class ArkAudioOutputChangeCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnOutputDeviceChange(int32_t reason) = 0;
};

/*--ark web(source=webview)--*/
class ArkAudioRendererAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t Create(const ArkWebRefPtr<ArkAudioRendererOptionsAdapter> options, ArkWebString& str) = 0;

    /*--ark web()--*/
    virtual bool Start() = 0;

    /*--ark web()--*/
    virtual bool Pause() = 0;

    /*--ark web()--*/
    virtual bool Stop() = 0;

    /*--ark web()--*/
    virtual bool Release2() = 0;

    /*--ark web()--*/
    virtual int32_t Write(uint8_t* buffer, size_t bufferSize) = 0;

    /*--ark web()--*/
    virtual int32_t GetLatency(uint64_t& latency) = 0;

    /*--ark web()--*/
    virtual int32_t SetVolume(float volume) = 0;

    /*--ark web()--*/
    virtual float GetVolume() = 0;

    /*--ark web()--*/
    virtual int32_t SetAudioRendererCallback(const ArkWebRefPtr<ArkAudioRendererCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual void SetInterruptMode(bool audioExclusive) = 0;

    /*--ark web()--*/
    virtual bool IsRendererStateRunning() = 0;

    /*--ark web()--*/
    virtual int32_t SetAudioOutputChangeCallback(const ArkWebRefPtr<ArkAudioOutputChangeCallbackAdapter> callback) = 0;

    /**
    * @brief SetAudioSilentMode, when isSilentMode is true, audio_render will not interrupt other andio output
    */
    /*--ark web()--*/
    virtual void SetAudioSilentMode(bool isSilentMode) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AUDIO_RENDERER_ADAPTER_H

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

#include "ohos_adapter/bridge/ark_audio_renderer_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_audio_output_change_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_renderer_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_renderer_options_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioRendererAdapterWrapper::ArkAudioRendererAdapterWrapper(ArkWebRefPtr<ArkAudioRendererAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkAudioRendererAdapterWrapper::Create(
    const std::shared_ptr<NWeb::AudioRendererOptionsAdapter> options, std::string cachePath)
{
    ArkWebString str = ArkWebStringClassToStruct(cachePath);
    int result;
    if (CHECK_SHARED_PTR_IS_NULL(options)) {
        result = ctocpp_->Create(nullptr, str);
    } else {
        result = ctocpp_->Create(new ArkAudioRendererOptionsAdapterImpl(options), str);
    }

    ArkWebStringStructRelease(str);
    return result;
}

bool ArkAudioRendererAdapterWrapper::Start()
{
    return ctocpp_->Start();
}

bool ArkAudioRendererAdapterWrapper::Pause()
{
    return ctocpp_->Pause();
}

bool ArkAudioRendererAdapterWrapper::Stop()
{
    return ctocpp_->Stop();
}

bool ArkAudioRendererAdapterWrapper::Release()
{
    return ctocpp_->Release2();
}

int32_t ArkAudioRendererAdapterWrapper::Write(uint8_t* buffer, size_t bufferSize)
{
    return ctocpp_->Write(buffer, bufferSize);
}

int32_t ArkAudioRendererAdapterWrapper::GetLatency(uint64_t& latency)
{
    return ctocpp_->GetLatency(latency);
}

int32_t ArkAudioRendererAdapterWrapper::SetVolume(float volume)
{
    return ctocpp_->SetVolume(volume);
}

float ArkAudioRendererAdapterWrapper::GetVolume()
{
    return ctocpp_->GetVolume();
}

int32_t ArkAudioRendererAdapterWrapper::SetAudioRendererCallback(
    const std::shared_ptr<NWeb::AudioRendererCallbackAdapter>& callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->SetAudioRendererCallback(nullptr);
    }

    return ctocpp_->SetAudioRendererCallback(new ArkAudioRendererCallbackAdapterImpl(callback));
}

void ArkAudioRendererAdapterWrapper::SetInterruptMode(bool audioExclusive)
{
    ctocpp_->SetInterruptMode(audioExclusive);
}

void ArkAudioRendererAdapterWrapper::SetAudioSilentMode(bool isSilentMode)
{
    ctocpp_->SetAudioSilentMode(isSilentMode);
}

bool ArkAudioRendererAdapterWrapper::IsRendererStateRunning()
{
    return ctocpp_->IsRendererStateRunning();
}

int32_t ArkAudioRendererAdapterWrapper::SetAudioOutputChangeCallback(
    const std::shared_ptr<NWeb::AudioOutputChangeCallbackAdapter>& callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->SetAudioOutputChangeCallback(nullptr);
    }

    return ctocpp_->SetAudioOutputChangeCallback(new ArkAudioOutputChangeCallbackAdapterImpl(callback));
}

} // namespace OHOS::ArkWeb

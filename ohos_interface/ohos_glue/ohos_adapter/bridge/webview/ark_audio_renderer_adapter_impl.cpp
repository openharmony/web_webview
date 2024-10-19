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

#include "ohos_adapter/bridge/ark_audio_renderer_adapter_impl.h"

#include "ohos_adapter/bridge/ark_audio_output_change_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_renderer_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_renderer_options_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioRendererAdapterImpl::ArkAudioRendererAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioRendererAdapter> ref)
    : real_(ref)
{}

int32_t ArkAudioRendererAdapterImpl::Create(
    const ArkWebRefPtr<ArkAudioRendererOptionsAdapter> options, ArkWebString& cachePath)
{
    std::string str = ArkWebStringStructToClass(cachePath);
    if (CHECK_REF_PTR_IS_NULL(options)) {
        return real_->Create(nullptr, str);
    }
    return real_->Create(std::make_shared<ArkAudioRendererOptionsAdapterWrapper>(options), str);
}

bool ArkAudioRendererAdapterImpl::Start()
{
    return real_->Start();
}

bool ArkAudioRendererAdapterImpl::Pause()
{
    return real_->Pause();
}

bool ArkAudioRendererAdapterImpl::Stop()
{
    return real_->Stop();
}

bool ArkAudioRendererAdapterImpl::Release2()
{
    return real_->Release();
}

int32_t ArkAudioRendererAdapterImpl::Write(uint8_t* buffer, size_t bufferSize)
{
    return real_->Write(buffer, bufferSize);
}

int32_t ArkAudioRendererAdapterImpl::GetLatency(uint64_t& latency)
{
    return real_->GetLatency(latency);
}

int32_t ArkAudioRendererAdapterImpl::SetVolume(float volume)
{
    return real_->SetVolume(volume);
}

float ArkAudioRendererAdapterImpl::GetVolume()
{
    return real_->GetVolume();
}

int32_t ArkAudioRendererAdapterImpl::SetAudioRendererCallback(
    const ArkWebRefPtr<ArkAudioRendererCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_->SetAudioRendererCallback(nullptr);
    }

    return real_->SetAudioRendererCallback(std::make_shared<ArkAudioRendererCallbackAdapterWrapper>(callback));
}

void ArkAudioRendererAdapterImpl::SetInterruptMode(bool audioExclusive)
{
    real_->SetInterruptMode(audioExclusive);
}

bool ArkAudioRendererAdapterImpl::IsRendererStateRunning()
{
    return real_->IsRendererStateRunning();
}

int32_t ArkAudioRendererAdapterImpl::SetAudioOutputChangeCallback(
    const ArkWebRefPtr<ArkAudioOutputChangeCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_->SetAudioOutputChangeCallback(nullptr);
    }

    return real_->SetAudioOutputChangeCallback(std::make_shared<ArkAudioOutputChangeCallbackAdapterWrapper>(callback));
}

} // namespace OHOS::ArkWeb

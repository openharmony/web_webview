/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_audio_buffer_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {
ArkAudioBufferAdapterImpl::ArkAudioBufferAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AudioBufferAdapter> ref)
    : real_(ref)
{}

uint8_t* ArkAudioBufferAdapterImpl::GetBuffer()
{
    return (uint8_t*)real_->GetBuffer();
}

int32_t ArkAudioBufferAdapterImpl::GetLength()
{
    return (int32_t)real_->GetLength();
}

int64_t ArkAudioBufferAdapterImpl::GetTimestamp()
{
    return (int64_t)real_->GetTimestamp();
}

int32_t ArkAudioBufferAdapterImpl::GetSourcetype()
{
    return (int32_t)real_->GetSourcetype();
}

void ArkAudioBufferAdapterImpl::SetBuffer(uint8_t* buffer)
{
    real_->SetBuffer(buffer);
}

void ArkAudioBufferAdapterImpl::SetLength(int32_t length)
{
    real_->SetLength(length);
}

void ArkAudioBufferAdapterImpl::SetTimestamp(int64_t timestamp)
{
    real_->SetTimestamp(timestamp);
}

void ArkAudioBufferAdapterImpl::SetSourcetype(int32_t sourcetype)
{
    real_->SetSourcetype((NWeb::AudioCaptureSourceTypeAdapter)sourcetype);
}

} // namespace OHOS::ArkWeb
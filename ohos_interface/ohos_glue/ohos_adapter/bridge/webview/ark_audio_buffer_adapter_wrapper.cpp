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

#include "ohos_adapter/bridge/ark_audio_buffer_adapter_wrapper.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioBufferAdapterWrapper::ArkAudioBufferAdapterWrapper(
    ArkWebRefPtr<ArkAudioBufferAdapter> ref)
    : ctocpp_(ref)
{}

uint8_t* ArkAudioBufferAdapterWrapper::GetBuffer()
{
    return (uint8_t*)ctocpp_->GetBuffer();
}

int32_t ArkAudioBufferAdapterWrapper::GetLength()
{
    return (int32_t)ctocpp_->GetLength();
}

int64_t ArkAudioBufferAdapterWrapper::GetTimestamp()
{
    return (int64_t)ctocpp_->GetTimestamp();
}

NWeb::AudioCaptureSourceTypeAdapter ArkAudioBufferAdapterWrapper::GetSourcetype()
{
    return (NWeb::AudioCaptureSourceTypeAdapter)ctocpp_->GetSourcetype();
}

void ArkAudioBufferAdapterWrapper::SetBuffer(uint8_t* buffer)
{
    ctocpp_->SetBuffer(buffer);
}

void ArkAudioBufferAdapterWrapper::SetLength(int32_t length)
{
    ctocpp_->SetLength(length);
}

void ArkAudioBufferAdapterWrapper::SetTimestamp(int64_t timestamp)
{
    ctocpp_->SetTimestamp(timestamp);
}

void ArkAudioBufferAdapterWrapper::SetSourcetype(NWeb::AudioCaptureSourceTypeAdapter sourcetype)
{
    ctocpp_->SetSourcetype((int32_t)sourcetype);
}

} // namespace OHOS::ArkWeb
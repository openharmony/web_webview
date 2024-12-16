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

#include "ohos_adapter/bridge/ark_audio_renderer_options_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAudioRendererOptionsAdapterWrapper::ArkAudioRendererOptionsAdapterWrapper(
    ArkWebRefPtr<ArkAudioRendererOptionsAdapter> ref)
    : ctocpp_(ref)
{}

NWeb::AudioAdapterSamplingRate ArkAudioRendererOptionsAdapterWrapper::GetSamplingRate()
{
    return (NWeb::AudioAdapterSamplingRate)ctocpp_->GetSamplingRate();
}

NWeb::AudioAdapterEncodingType ArkAudioRendererOptionsAdapterWrapper::GetEncodingType()
{
    return (NWeb::AudioAdapterEncodingType)ctocpp_->GetEncodingType();
}

NWeb::AudioAdapterSampleFormat ArkAudioRendererOptionsAdapterWrapper::GetSampleFormat()
{
    return (NWeb::AudioAdapterSampleFormat)ctocpp_->GetSampleFormat();
}

NWeb::AudioAdapterChannel ArkAudioRendererOptionsAdapterWrapper::GetChannel()
{
    return (NWeb::AudioAdapterChannel)ctocpp_->GetChannel();
}

NWeb::AudioAdapterContentType ArkAudioRendererOptionsAdapterWrapper::GetContentType()
{
    return (NWeb::AudioAdapterContentType)ctocpp_->GetContentType();
}

NWeb::AudioAdapterStreamUsage ArkAudioRendererOptionsAdapterWrapper::GetStreamUsage()
{
    return (NWeb::AudioAdapterStreamUsage)ctocpp_->GetStreamUsage();
}

int32_t ArkAudioRendererOptionsAdapterWrapper::GetRenderFlags()
{
    return ctocpp_->GetRenderFlags();
}

NWeb::AudioAdapterConcurrencyMode ArkAudioRendererOptionsAdapterWrapper::GetConcurrencyMode()
{
    return (NWeb::AudioAdapterConcurrencyMode)ctocpp_->GetConcurrencyMode();
}

} // namespace OHOS::ArkWeb

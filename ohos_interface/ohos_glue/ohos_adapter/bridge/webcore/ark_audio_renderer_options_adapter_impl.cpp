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

#include "ohos_adapter/bridge/ark_audio_renderer_options_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkAudioRendererOptionsAdapterImpl::ArkAudioRendererOptionsAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AudioRendererOptionsAdapter> ref)
    : real_(ref)
{}

int32_t ArkAudioRendererOptionsAdapterImpl::GetSamplingRate()
{
    return (int32_t)(real_->GetSamplingRate());
}

int32_t ArkAudioRendererOptionsAdapterImpl::GetEncodingType()
{
    return (int32_t)(real_->GetEncodingType());
}

int32_t ArkAudioRendererOptionsAdapterImpl::GetSampleFormat()
{
    return (int32_t)(real_->GetSampleFormat());
}

int32_t ArkAudioRendererOptionsAdapterImpl::GetChannel()
{
    return (int32_t)(real_->GetChannel());
}

int32_t ArkAudioRendererOptionsAdapterImpl::GetContentType()
{
    return (int32_t)(real_->GetContentType());
}

int32_t ArkAudioRendererOptionsAdapterImpl::GetStreamUsage()
{
    return (int32_t)(real_->GetStreamUsage());
}

int32_t ArkAudioRendererOptionsAdapterImpl::GetRenderFlags()
{
    return (int32_t)(real_->GetRenderFlags());
}

int32_t ArkAudioRendererOptionsAdapterImpl::GetConcurrencyMode()
{
    return (int32_t)(real_->GetConcurrencyMode());
}

} // namespace OHOS::ArkWeb

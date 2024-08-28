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

#include "ohos_adapter/bridge/ark_audio_renderer_callback_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkAudioRendererCallbackAdapterImpl::ArkAudioRendererCallbackAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AudioRendererCallbackAdapter> ref)
    : real_(ref)
{}

void ArkAudioRendererCallbackAdapterImpl::OnSuspend()
{
    real_->OnSuspend();
}

void ArkAudioRendererCallbackAdapterImpl::OnResume()
{
    real_->OnResume();
}

int32_t ArkAudioRendererCallbackAdapterImpl::OnWriteDataCallback(void* buffer, int32_t length)
{
    return real_->OnWriteDataCallback(buffer, length);
}
} // namespace OHOS::ArkWeb

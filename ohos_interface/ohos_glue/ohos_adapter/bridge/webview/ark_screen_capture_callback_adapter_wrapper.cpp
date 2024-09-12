/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_screen_capture_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkScreenCaptureCallbackAdapterWrapper::ArkScreenCaptureCallbackAdapterWrapper(
    ArkWebRefPtr<ArkScreenCaptureCallbackAdapter> ref)
    : ctocpp_(ref)
{}

void ArkScreenCaptureCallbackAdapterWrapper::OnError(int32_t errorCode)
{
    ctocpp_->OnError(errorCode);
}

void ArkScreenCaptureCallbackAdapterWrapper::OnAudioBufferAvailable(
    bool isReady, OHOS::NWeb::AudioCaptureSourceTypeAdapter type)
{
    ctocpp_->OnAudioBufferAvailable(isReady, (int32_t)type);
}

void ArkScreenCaptureCallbackAdapterWrapper::OnVideoBufferAvailable(bool isReady)
{
    return ctocpp_->OnVideoBufferAvailable(isReady);
}

void ArkScreenCaptureCallbackAdapterWrapper::OnStateChange(OHOS::NWeb::ScreenCaptureStateCodeAdapter stateCode)
{
    ctocpp_->OnStateChange((int32_t)stateCode);
}

} // namespace OHOS::ArkWeb

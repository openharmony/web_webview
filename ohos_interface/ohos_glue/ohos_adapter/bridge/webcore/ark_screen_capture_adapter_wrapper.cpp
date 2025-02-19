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

#include "ohos_adapter/bridge/ark_screen_capture_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_screen_capture_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_screen_capture_config_adapter_impl.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkScreenCaptureAdapterWrapper::ArkScreenCaptureAdapterWrapper(ArkWebRefPtr<ArkScreenCaptureAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkScreenCaptureAdapterWrapper::Init(const std::shared_ptr<NWeb::ScreenCaptureConfigAdapter> config)
{
    if (CHECK_SHARED_PTR_IS_NULL(config)) {
        return ctocpp_->Init(nullptr);
    }
    return ctocpp_->Init(new ArkScreenCaptureConfigAdapterImpl(config));
}

int32_t ArkScreenCaptureAdapterWrapper::SetMicrophoneEnable(bool enable)
{
    return ctocpp_->SetMicrophoneEnable(enable);
}

int32_t ArkScreenCaptureAdapterWrapper::StartCapture()
{
    return ctocpp_->StartCapture();
}

int32_t ArkScreenCaptureAdapterWrapper::StopCapture()
{
    return ctocpp_->StopCapture();
}

int32_t ArkScreenCaptureAdapterWrapper::SetCaptureCallback(
    const std::shared_ptr<OHOS::NWeb::ScreenCaptureCallbackAdapter> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->SetCaptureCallback(nullptr);
    }

    return ctocpp_->SetCaptureCallback(new ArkScreenCaptureCallbackAdapterImpl(callback));
}

std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> ArkScreenCaptureAdapterWrapper::AcquireVideoBuffer()
{
    ArkWebRefPtr<ArkSurfaceBufferAdapter> ref = ctocpp_->AcquireVideoBuffer();
    if (CHECK_REF_PTR_IS_NULL(ref)) {
        return nullptr;
    }

    return std::make_shared<ArkSurfaceBufferAdapterWrapper>(ref);
}

int32_t ArkScreenCaptureAdapterWrapper::ReleaseVideoBuffer()
{
    return ctocpp_->ReleaseVideoBuffer();
}

} // namespace OHOS::ArkWeb

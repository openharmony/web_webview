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

#include "ohos_adapter/bridge/ark_screen_capture_adapter_impl.h"

#include "ohos_adapter/bridge/ark_screen_capture_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_screen_capture_config_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkScreenCaptureAdapterImpl::ArkScreenCaptureAdapterImpl(std::shared_ptr<OHOS::NWeb::ScreenCaptureAdapter> ref)
    : real_(ref)
{}

int32_t ArkScreenCaptureAdapterImpl::Init(const ArkWebRefPtr<ArkScreenCaptureConfigAdapter> config)
{
    if (CHECK_REF_PTR_IS_NULL(config)) {
        return real_->Init(nullptr);
    }

    return real_->Init(std::make_shared<ArkScreenCaptureConfigAdapterWrapper>(config));
}

int32_t ArkScreenCaptureAdapterImpl::SetMicrophoneEnable(bool enable)
{
    return real_->SetMicrophoneEnable(enable);
}

int32_t ArkScreenCaptureAdapterImpl::StartCapture()
{
    return real_->StartCapture();
}

int32_t ArkScreenCaptureAdapterImpl::StopCapture()
{
    return real_->StopCapture();
}

int32_t ArkScreenCaptureAdapterImpl::SetCaptureCallback(const ArkWebRefPtr<ArkScreenCaptureCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return real_->SetCaptureCallback(nullptr);
    }

    return real_->SetCaptureCallback(std::make_shared<ArkScreenCaptureCallbackAdapterWrapper>(callback));
}

ArkWebRefPtr<ArkSurfaceBufferAdapter> ArkScreenCaptureAdapterImpl::AcquireVideoBuffer()
{
    std::shared_ptr<NWeb::SurfaceBufferAdapter> buffer = real_->AcquireVideoBuffer();
    if (CHECK_SHARED_PTR_IS_NULL(buffer)) {
        return nullptr;
    }

    return new ArkSurfaceBufferAdapterImpl(buffer);
}

int32_t ArkScreenCaptureAdapterImpl::ReleaseVideoBuffer()
{
    return real_->ReleaseVideoBuffer();
}

} // namespace OHOS::ArkWeb

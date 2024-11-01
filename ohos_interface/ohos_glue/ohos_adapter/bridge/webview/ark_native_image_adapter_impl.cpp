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

#include "ohos_adapter/bridge/ark_native_image_adapter_impl.h"

#include "ohos_adapter/bridge/ark_frame_available_listener_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNativeImageAdapterImpl::ArkNativeImageAdapterImpl(std::shared_ptr<OHOS::NWeb::NativeImageAdapter> ref) : real_(ref)
{}

void ArkNativeImageAdapterImpl::CreateNativeImage(uint32_t textureId, uint32_t textureTarget)
{
    return real_->CreateNativeImage(textureId, textureTarget);
}
void* ArkNativeImageAdapterImpl::AquireNativeWindowFromNativeImage()
{
    return real_->AquireNativeWindowFromNativeImage();
}
int32_t ArkNativeImageAdapterImpl::AttachContext(uint32_t textureId)
{
    return real_->AttachContext(textureId);
}
int32_t ArkNativeImageAdapterImpl::DetachContext()
{
    return real_->DetachContext();
}

int32_t ArkNativeImageAdapterImpl::UpdateSurfaceImage()
{
    return real_->UpdateSurfaceImage();
}

int64_t ArkNativeImageAdapterImpl::GetTimestamp()
{
    return real_->GetTimestamp();
}

int32_t ArkNativeImageAdapterImpl::GetTransformMatrix(float matrix[16])
{
    return real_->GetTransformMatrix(matrix);
}

int32_t ArkNativeImageAdapterImpl::GetSurfaceId(uint64_t* surfaceId)
{
    return real_->GetSurfaceId(surfaceId);
}

int32_t ArkNativeImageAdapterImpl::SetOnFrameAvailableListener(ArkWebRefPtr<ArkFrameAvailableListener> listener)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->SetOnFrameAvailableListener(nullptr);
    }

    return real_->SetOnFrameAvailableListener(std::make_shared<ArkFrameAvailableListenerWrapper>(listener));
}

int32_t ArkNativeImageAdapterImpl::UnsetOnFrameAvailableListener()
{
    return real_->UnsetOnFrameAvailableListener();
}

void ArkNativeImageAdapterImpl::DestroyNativeImage()
{
    return real_->DestroyNativeImage();
}

void ArkNativeImageAdapterImpl::NewNativeImage()
{
    return real_->NewNativeImage();
}

int32_t ArkNativeImageAdapterImpl::AcquireNativeWindowBuffer(
    void** windowBuffer,
    int* acquireFenceFd)
{
    return real_->AcquireNativeWindowBuffer(windowBuffer, acquireFenceFd);
}

int32_t ArkNativeImageAdapterImpl::GetNativeBuffer(
    void* windowBuffer,
    void** nativeBuffer)
{
    return real_->GetNativeBuffer(windowBuffer, nativeBuffer);
}

int32_t ArkNativeImageAdapterImpl::ReleaseNativeWindowBuffer(void* windowBuffer, int fenceFd)
{
    return real_->ReleaseNativeWindowBuffer(windowBuffer, fenceFd);
}

void ArkNativeImageAdapterImpl::GetNativeWindowBufferSize(
    void* windowBuffer,
    uint32_t* width,
    uint32_t* height)
{
    return real_->GetNativeWindowBufferSize(windowBuffer, width, height);
}
} // namespace OHOS::ArkWeb

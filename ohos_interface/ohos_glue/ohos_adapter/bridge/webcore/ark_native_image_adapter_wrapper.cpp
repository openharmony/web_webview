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

#include "ohos_adapter/bridge/ark_native_image_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_frame_available_listener_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNativeImageAdapterWrapper::ArkNativeImageAdapterWrapper(ArkWebRefPtr<ArkNativeImageAdapter> ref) : ctocpp_(ref) {}

void ArkNativeImageAdapterWrapper::CreateNativeImage(uint32_t textureId, uint32_t textureTarget)
{
    return ctocpp_->CreateNativeImage(textureId, textureTarget);
}

NWeb::NWebNativeWindow ArkNativeImageAdapterWrapper::AquireNativeWindowFromNativeImage()
{
    return ctocpp_->AquireNativeWindowFromNativeImage();
}

int32_t ArkNativeImageAdapterWrapper::AttachContext(uint32_t textureId)
{
    return ctocpp_->AttachContext(textureId);
}

int32_t ArkNativeImageAdapterWrapper::DetachContext()
{
    return ctocpp_->DetachContext();
}

int32_t ArkNativeImageAdapterWrapper::UpdateSurfaceImage()
{
    return ctocpp_->UpdateSurfaceImage();
}

int64_t ArkNativeImageAdapterWrapper::GetTimestamp()
{
    return ctocpp_->GetTimestamp();
}

int32_t ArkNativeImageAdapterWrapper::GetTransformMatrix(float matrix[16])
{
    return ctocpp_->GetTransformMatrix(matrix);
}

int32_t ArkNativeImageAdapterWrapper::GetSurfaceId(uint64_t* surfaceId)
{
    return ctocpp_->GetSurfaceId(surfaceId);
}

int32_t ArkNativeImageAdapterWrapper::SetOnFrameAvailableListener(
    std::shared_ptr<OHOS::NWeb::FrameAvailableListener> listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->SetOnFrameAvailableListener(nullptr);
    }
    return ctocpp_->SetOnFrameAvailableListener(new ArkFrameAvailableListenerImpl(listener));
}

int32_t ArkNativeImageAdapterWrapper::UnsetOnFrameAvailableListener()
{
    return ctocpp_->UnsetOnFrameAvailableListener();
}

void ArkNativeImageAdapterWrapper::DestroyNativeImage()
{
    return ctocpp_->DestroyNativeImage();
}

void ArkNativeImageAdapterWrapper::NewNativeImage()
{
    return ctocpp_->NewNativeImage();
}

int32_t ArkNativeImageAdapterWrapper::AcquireNativeWindowBuffer(
    void** windowBuffer,
    int* acquireFenceFd)
{
    return ctocpp_->AcquireNativeWindowBuffer(windowBuffer, acquireFenceFd);
}

int32_t ArkNativeImageAdapterWrapper::GetNativeBuffer(
    void* windowBuffer,
    void** nativeBuffer)
{
    return ctocpp_->GetNativeBuffer(windowBuffer, nativeBuffer);
}

int32_t ArkNativeImageAdapterWrapper::ReleaseNativeWindowBuffer(void* windowBuffer, int fenceFd)
{
    return ctocpp_->ReleaseNativeWindowBuffer(windowBuffer, fenceFd);
}

void ArkNativeImageAdapterWrapper::GetNativeWindowBufferSize(
    void* windowBuffer,
    uint32_t* width,
    uint32_t* height)
{
    return ctocpp_->GetNativeWindowBufferSize(windowBuffer, width, height);
}

} // namespace OHOS::ArkWeb

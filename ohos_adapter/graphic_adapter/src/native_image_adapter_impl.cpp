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

#include "native_image_adapter_impl.h"

#include <native_image/graphic_error_code.h>

namespace OHOS::NWeb {

NativeImageAdapterImpl::~NativeImageAdapterImpl()
{
    DestroyNativeImage();
}

void NativeImageAdapterImpl::CreateNativeImage(uint32_t textureId, uint32_t textureTarget)
{
    ohNativeImage_ = OH_NativeImage_Create(textureId, textureTarget);
}

NWebNativeWindow NativeImageAdapterImpl::AquireNativeWindowFromNativeImage()
{
    if (ohNativeImage_ == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<NWebNativeWindow>(OH_NativeImage_AcquireNativeWindow(ohNativeImage_));
}

int32_t NativeImageAdapterImpl::AttachContext(uint32_t textureId)
{
    if (ohNativeImage_ == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    return OH_NativeImage_AttachContext(ohNativeImage_, textureId);
}

int32_t NativeImageAdapterImpl::DetachContext()
{
    if (ohNativeImage_ == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    return OH_NativeImage_DetachContext(ohNativeImage_);
}

int32_t NativeImageAdapterImpl::UpdateSurfaceImage()
{
    if (ohNativeImage_ == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    return OH_NativeImage_UpdateSurfaceImage(ohNativeImage_);
}

int64_t NativeImageAdapterImpl::GetTimestamp()
{
    if (ohNativeImage_ == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    return OH_NativeImage_GetTimestamp(ohNativeImage_);
}

int32_t NativeImageAdapterImpl::GetTransformMatrix(float matrix[16])
{
    if (ohNativeImage_ == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    return OH_NativeImage_GetTransformMatrix(ohNativeImage_, matrix);
}

int32_t NativeImageAdapterImpl::GetSurfaceId(uint64_t* surfaceId)
{
    if (ohNativeImage_ == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    return OH_NativeImage_GetSurfaceId(ohNativeImage_, surfaceId);
}

int32_t NativeImageAdapterImpl::SetOnFrameAvailableListener(std::shared_ptr<FrameAvailableListener> listener)
{
    if (ohNativeImage_ == nullptr || listener == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    OH_OnFrameAvailableListener callback;
    callback.onFrameAvailable = listener->GetOnFrameAvailableCb();
    callback.context = listener->GetContext();
    return OH_NativeImage_SetOnFrameAvailableListener(ohNativeImage_, callback);
}

int32_t NativeImageAdapterImpl::UnsetOnFrameAvailableListener()
{
    if (ohNativeImage_ == nullptr) {
        return NATIVE_ERROR_UNKNOWN;
    }
    return OH_NativeImage_UnsetOnFrameAvailableListener(ohNativeImage_);
}

void NativeImageAdapterImpl::DestroyNativeImage()
{
    if (ohNativeImage_ == nullptr) {
        return;
    }
    OH_NativeImage_Destroy(&ohNativeImage_);
    ohNativeImage_ = nullptr;
}
}

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

#include "graphic_common_c.h"
#include "iconsumer_surface.h"
#include "nweb_log.h"

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
        return SURFACE_ERROR_ERROR;
    }
    return OH_NativeImage_AttachContext(ohNativeImage_, textureId);
}

int32_t NativeImageAdapterImpl::DetachContext()
{
    if (ohNativeImage_ == nullptr) {
        return SURFACE_ERROR_ERROR;
    }
    return OH_NativeImage_DetachContext(ohNativeImage_);
}

int32_t NativeImageAdapterImpl::UpdateSurfaceImage()
{
    if (ohNativeImage_ == nullptr) {
        return SURFACE_ERROR_ERROR;
    }
    return OH_NativeImage_UpdateSurfaceImage(ohNativeImage_);
}

int64_t NativeImageAdapterImpl::GetTimestamp()
{
    if (ohNativeImage_ == nullptr) {
        return SURFACE_ERROR_ERROR;
    }
    return OH_NativeImage_GetTimestamp(ohNativeImage_);
}

int32_t NativeImageAdapterImpl::GetTransformMatrix(float matrix[16])
{
    if (ohNativeImage_ == nullptr) {
        return SURFACE_ERROR_ERROR;
    }
    return OH_NativeImage_GetTransformMatrix(ohNativeImage_, matrix);
}

int32_t NativeImageAdapterImpl::GetSurfaceId(uint64_t* surfaceId)
{
    if (ohNativeImage_ == nullptr) {
        return SURFACE_ERROR_ERROR;
    }
    return OH_NativeImage_GetSurfaceId(ohNativeImage_, surfaceId);
}

int32_t NativeImageAdapterImpl::SetOnFrameAvailableListener(std::shared_ptr<FrameAvailableListener> listener)
{
    if (ohNativeImage_ == nullptr || listener == nullptr) {
        return SURFACE_ERROR_ERROR;
    }
    OH_OnFrameAvailableListener callback;
    callback.onFrameAvailable = listener->GetOnFrameAvailableCb();
    callback.context = listener->GetContext();
    return OH_NativeImage_SetOnFrameAvailableListener(ohNativeImage_, callback);
}

int32_t NativeImageAdapterImpl::UnsetOnFrameAvailableListener()
{
    if (ohNativeImage_ == nullptr) {
        return SURFACE_ERROR_ERROR;
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


void NativeImageAdapterImpl::NewNativeImage()
{
    ohNativeImage_ = OH_ConsumerSurface_Create();
}

int32_t NativeImageAdapterImpl::AcquireNativeWindowBuffer(
    void** windowBuffer,
    int* acquireFenceFd)
{
    if (ohNativeImage_ == nullptr) {
        WVLOG_E("native image is null.");
        return SURFACE_ERROR_ERROR;
    }

    OHNativeWindowBuffer* buffer = nullptr;
    int32_t ret = OH_NativeImage_AcquireNativeWindowBuffer(ohNativeImage_, &buffer, acquireFenceFd);
    if (ret != SURFACE_ERROR_OK || !buffer) {
        WVLOG_E("acquireNativeWindowBuffer fail. ret = %{public}d or buffer is nullptr", ret);
        return ret;
    }
    *windowBuffer = buffer;
    return SURFACE_ERROR_OK;
}

int32_t NativeImageAdapterImpl::GetNativeBuffer(
    void* windowBuffer,
    void** nativeBuffer)
{
    OH_NativeBuffer* buffer = nullptr;
    int32_t ret = OH_NativeBuffer_FromNativeWindowBuffer(static_cast<OHNativeWindowBuffer*>(windowBuffer), &buffer);
    if (ret != SURFACE_ERROR_OK || !buffer) {
        WVLOG_E("getNativeBuffer fail. ret = %{public}d or buffer is nullptr", ret);
        return ret;
    }

    *nativeBuffer = buffer;
    return SURFACE_ERROR_OK;
}

int32_t NativeImageAdapterImpl::ReleaseNativeWindowBuffer(void* windowBuffer, int fenceFd)
{
    if (ohNativeImage_ == nullptr) {
        return SURFACE_ERROR_ERROR;
    }
    return OH_NativeImage_ReleaseNativeWindowBuffer(ohNativeImage_,
        static_cast<OHNativeWindowBuffer*>(windowBuffer), fenceFd);
}

void NativeImageAdapterImpl::GetNativeWindowBufferSize(void* windowBuffer, uint32_t* width, uint32_t* height)
{
    if (windowBuffer == nullptr || width == nullptr || height == nullptr) {
        return;
    }
    BufferHandle *handle = OH_NativeWindow_GetBufferHandleFromNative(static_cast<OHNativeWindowBuffer*>(windowBuffer));
    if (handle) {
        *height = handle->height;
        *width = handle->width;
    }
}
}

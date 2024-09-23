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

#include "ohos_image_decoder_adapter_impl.h"

#include "fstream"
#include "istream"
#include "nweb_log.h"
#include "sstream"
#include "string"

namespace OHOS {
namespace NWeb {

const std::string IMAGE_HEIF = "image/heif";

void ReleaseDecodeOptions(OH_DecodingOptions* decodeOptions)
{
    if (decodeOptions) {
        Image_ErrorCode errorCode = OH_DecodingOptions_Release(decodeOptions);
        decodeOptions = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] ReleaseDecodeOptions failed, errorCode %{public}d", errorCode);
        }
        WVLOG_D("[HeifSupport] ReleaseDecodeOptions success");
    }
    WVLOG_D("[HeifSupport] ReleaseDecodeOptions options is null, do not need release");
}

void OhosImageDecoderAdapterImpl::NativeBufferFromPixelMap()
{
    if (pixelMap_) {
        Image_ErrorCode errorCode = OH_PixelmapNative_GetNativeBuffer(pixelMap_, &nativeBuffer_);
        if (errorCode == Image_ErrorCode::IMAGE_SUCCESS) {
            return;
        }
        WVLOG_E("[HeifSupport] NativeBufferFromPixelMap GetNativeBuffer failed, errorCode %{public}d", errorCode);
        return;
    }
    WVLOG_E("[HeifSupport] NativeBufferFromPixelMap GetNativeBuffer pixelMap_ not exists");
}

bool OhosImageDecoderAdapterImpl::ParseRawData(const uint8_t* data, uint32_t size)
{
    Image_ErrorCode errorCode = OH_ImageSourceNative_CreateFromData(const_cast<uint8_t*>(data), size, &imageSource_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] ParseRawData create imageSource failed, errorCode %{public}d", errorCode);
        return false;
    }
    if (!imageInfo_) {
        errorCode = OH_ImageSourceInfo_Create(&imageInfo_);
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] ParseRawData create imageInfo failed, errorCode %{public}d", errorCode);
            return false;
        }
    }
    errorCode = OH_ImageSourceNative_GetImageInfo(imageSource_, 0, imageInfo_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] ParseRawData GetImageInfo failed, errorCode %{public}d", errorCode);
        return false;
    }
    return true;
}

OhosImageDecoderAdapterImpl::OhosImageDecoderAdapterImpl()
{
    Image_ErrorCode errorCode = OH_PixelmapInitializationOptions_Create(&opt_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] init create options failed, errorCode %{public}d", errorCode);
    }
    errorCode = OH_ImageSourceInfo_Create(&imageInfo_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] init create imageInfo failed, errorCode %{public}d", errorCode);
    }
}

OhosImageDecoderAdapterImpl::~OhosImageDecoderAdapterImpl()
{
    ReleasePixelMap();
}

bool OhosImageDecoderAdapterImpl::ParseImageInfo(const uint8_t* data, uint32_t size)
{
    WVLOG_D("[HeifSupport] ParseImageInfo size = %{public}d", size);
    return ParseRawData(data, size);
}

std::string OhosImageDecoderAdapterImpl::GetEncodedFormat()
{
    WVLOG_W("[HeifSupport] GetEncodedFormat do not implement, return default = %{public}s", IMAGE_HEIF.c_str());
    return IMAGE_HEIF;
}

int32_t OhosImageDecoderAdapterImpl::GetImageWidth()
{
    uint32_t width = 0;
    Image_ErrorCode errorCode = OH_ImageSourceInfo_GetWidth(imageInfo_, &width);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] GetImageWidth failed, errorCode %{public}d", errorCode);
    }
    WVLOG_D("[HeifSupport] GetImageWidth = %{public}d", width);
    return width;
}

int32_t OhosImageDecoderAdapterImpl::GetImageHeight()
{
    uint32_t height = 0;
    Image_ErrorCode errorCode = OH_ImageSourceInfo_GetHeight(imageInfo_, &height);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] GetImageHeight failed, errorCode %{public}d", errorCode);
    }
    WVLOG_D("[HeifSupport] GetImageHeight = %{public}d", height);
    return height;
}

bool OhosImageDecoderAdapterImpl::DecodeToPixelMap(const uint8_t* data, uint32_t size)
{
    WVLOG_D("[HeifSupport] DecodeToPixelMap size = %{public}d", size);
    return Decode(data, size, AllocatorType::kDmaAlloc, false);
}

bool OhosImageDecoderAdapterImpl::Decode(const uint8_t* data,
                                         uint32_t size,
                                         AllocatorType type,
                                         bool useYuv)
{
    WVLOG_D("[HeifSupport] Decode size = %{public}d, type = %{public}d, useYuv = %{public}d",
        size, static_cast<int>(type), useYuv);
    if (!ParseRawData(data, size)) {
        WVLOG_E("[HeifSupport] Decode, fail to get image source.");
        return false;
    }
    OH_DecodingOptions* decodeOptions;
    Image_ErrorCode errorCode = OH_DecodingOptions_Create(&decodeOptions);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] Decode create decode options failed, errorCode %{public}d", errorCode);
        return false;
    }
    errorCode = OH_DecodingOptions_SetPixelFormat(decodeOptions,
        useYuv ? PIXEL_FORMAT::PIXEL_FORMAT_NV12 : PIXEL_FORMAT::PIXEL_FORMAT_RGBA_8888);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] Decode set pixel format failed, errorCode %{public}d", errorCode);
        ReleaseDecodeOptions(decodeOptions);
        return false;
    }
    errorCode = OH_ImageSourceNative_CreatePixelmap(imageSource_, decodeOptions, &pixelMap_);
    if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
        WVLOG_E("[HeifSupport] Decode create pixel map failed, errorCode %{public}d", errorCode);
        ReleaseDecodeOptions(decodeOptions);
        return false;
    }
    ReleaseDecodeOptions(decodeOptions);
    return true;
}

int32_t OhosImageDecoderAdapterImpl::GetFd()
{
    if (!GetBufferHandle()) {
        WVLOG_E("[HeifSupport] GetFd bufferHandle is null.");
        return -1;
    }
    WVLOG_D("[HeifSupport] GetFd %{public}d", bufferHandle_->fd);
    return bufferHandle_->fd;
}

int32_t OhosImageDecoderAdapterImpl::GetStride()
{
    if (!GetBufferHandle()) {
        WVLOG_E("[HeifSupport] GetStride bufferHandle is null.");
        return 0;
    }
    WVLOG_D("[HeifSupport] GetStride %{public}d", bufferHandle_->stride);
    return bufferHandle_->stride;
}

int32_t OhosImageDecoderAdapterImpl::GetOffset()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] GetOffset. PixelMap is null.");
        return 0;
    }
    NativeBufferFromPixelMap();
    if (nativeBuffer_) {
        OH_NativeBuffer_Planes* nativeBufferPlanes = new OH_NativeBuffer_Planes;
        void* virAddr;
        int32_t errorCode = OH_NativeBuffer_MapPlanes(nativeBuffer_, &virAddr, nativeBufferPlanes);
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] GetOffset get planes failed, errorCode %{public}d", errorCode);
            delete nativeBufferPlanes;
            return 0;
        }
        int32_t offset = nativeBufferPlanes->planes[0].offset;
        delete nativeBufferPlanes;
        WVLOG_D("[HeifSupport] GetOffset %{public}d", offset);
        return offset;
    }
    WVLOG_E("[HeifSupport] GetOffset native buffer is null");
    return 0;
}

uint64_t OhosImageDecoderAdapterImpl::GetSize()
{
    if (!GetBufferHandle()) {
        WVLOG_E("[HeifSupport] GetSize bufferHandle is null.");
        return 0;
    }
    WVLOG_D("[HeifSupport] GetSize %{public}d", bufferHandle_->size);
    return bufferHandle_->size;
}

void* OhosImageDecoderAdapterImpl::GetNativeWindowBuffer()
{
    if (!CreateNativeWindowBuffer()) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer failed, return nullptr");
        return nullptr;
    }
    return static_cast<void*>(nativeWindowBuffer_);
}

// Used for NV12
int32_t OhosImageDecoderAdapterImpl::GetPlanesCount()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] GetPlanesCount. PixelMap is null.");
        return 0;
    }
    NativeBufferFromPixelMap();
    if (nativeBuffer_) {
        OH_NativeBuffer_Planes* nativeBufferPlanes = new OH_NativeBuffer_Planes;
        void* virAddr;
        int32_t errorCode = OH_NativeBuffer_MapPlanes(nativeBuffer_, &virAddr, nativeBufferPlanes);
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] GetPlanesCount get planes failed, errorCode %{public}d", errorCode);
            delete nativeBufferPlanes;
            return 0;
        }
        int32_t planeCount = static_cast<int32_t>(nativeBufferPlanes->planeCount);
        delete nativeBufferPlanes;
        WVLOG_D("[HeifSupport] GetPlanesCount %{public}d", planeCount);
        return planeCount;
    }
    WVLOG_E("[HeifSupport] GetPlanesCount native buffer is null");
    return 0;
}

void OhosImageDecoderAdapterImpl::ReleasePixelMap()
{
    WVLOG_I("[HeifSupport] OhosImageDecoderAdapterImpl release pixelmap and native window buffer.");
    if (imageSource_) {
        Image_ErrorCode errorCode = OH_ImageSourceNative_Release(imageSource_);
        imageSource_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_ImageSourceNative_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (imageInfo_) {
        Image_ErrorCode errorCode = OH_ImageSourceInfo_Release(imageInfo_);
        imageInfo_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_ImageSourceInfo_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (pixelMap_) {
        Image_ErrorCode errorCode = OH_PixelmapNative_Release(pixelMap_);
        pixelMap_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_PixelmapNative_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (nativeWindowBuffer_) {
        OH_NativeWindow_DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
    if (nativeBuffer_) {
        int errorCode = OH_NativeBuffer_Unreference(nativeBuffer_);
        nativeBuffer_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_NativeBuffer_Unreference failed, errorCode = %{public}d", errorCode);
        }
    }
    if (opt_) {
        Image_ErrorCode errorCode = OH_PixelmapInitializationOptions_Release(opt_);
        opt_ = nullptr;
        if (errorCode != Image_ErrorCode::IMAGE_SUCCESS) {
            WVLOG_E("[HeifSupport] OH_PixelmapInitializationOptions_Release failed, errorCode = %{public}d", errorCode);
        }
    }
    if (bufferHandle_) {
        bufferHandle_ = nullptr;
    }
}

bool OhosImageDecoderAdapterImpl::CreateNativeWindowBuffer()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer PixelMap is nullptr");
        return false;
    }
    NativeBufferFromPixelMap();
    if (!nativeBuffer_) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer nativeBuffer is nullptr");
        return false;
    }
    nativeWindowBuffer_ = OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(nativeBuffer_);
    if (!nativeWindowBuffer_) {
        WVLOG_E("[HeifSupport] CreateNativeWindowBuffer create nativeWindowBuffer failed.");
        return false;
    }
    return true;
}

bool OhosImageDecoderAdapterImpl::GetBufferHandle()
{
    if (bufferHandle_) {
        return true;
    }
    if (!nativeWindowBuffer_ && !CreateNativeWindowBuffer()) {
        WVLOG_E("[HeifSupport] GetBufferHandle, nativeWindowBuffer get and create both failed.");
        return false;
    }
    bufferHandle_ = OH_NativeWindow_GetBufferHandleFromNative(nativeWindowBuffer_);
    if (!bufferHandle_) {
        WVLOG_E("[HeifSupport] GetBufferHandle, get bufferHandle failed.");
        return false;
    }
    return true;
}

}  // namespace NWeb
}  // namespace OHOS
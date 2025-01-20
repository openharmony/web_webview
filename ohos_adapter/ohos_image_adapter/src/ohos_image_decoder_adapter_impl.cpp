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

#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/window.h"
#include "fstream"
#include "image_source.h"
#include "istream"
#include "media_errors.h"
#include "nweb_log.h"
#include "pixel_map.h"
#include "sstream"
#include "string"

namespace OHOS {
namespace NWeb {

namespace {

std::unique_ptr<Media::ImageSource> ParseRawData(const uint8_t* data,
                                                 uint32_t size,
                                                 Media::ImageInfo& imageInfo)
{
    uint32_t errorCode = 0;
    Media::SourceOptions sourceOptions;
    auto imageSource = Media::ImageSource::CreateImageSource(
        data, size, sourceOptions, errorCode);
    if (errorCode != Media::SUCCESS) {
        WVLOG_E("[HeifSupport] ParseRawData failed, errorCode %{public}d", errorCode);
        return nullptr;
    }

    auto ret = imageSource->GetImageInfo(imageInfo);
    if (ret != Media::SUCCESS) {
        WVLOG_E(
            "[HeifSupport] ParseRawData GetImageInfo failed, errorCode %{public}d", ret);
        return nullptr;
    }
    return imageSource;
}

SurfaceBuffer* SurfaceBufferFromPixelMap(Media::PixelMap* pixelMap)
{
    if (pixelMap && pixelMap->GetFd()) {
        return reinterpret_cast<SurfaceBuffer*>(pixelMap->GetFd());
    }
    return nullptr;
}

};  // namespace

OhosImageDecoderAdapterImpl::OhosImageDecoderAdapterImpl() = default;

OhosImageDecoderAdapterImpl::~OhosImageDecoderAdapterImpl()
{
    ReleasePixelMap();
}

bool OhosImageDecoderAdapterImpl::ParseImageInfo(const uint8_t* data, uint32_t size)
{
    return ParseRawData(data, size, imageInfo_) != nullptr;
}

std::string OhosImageDecoderAdapterImpl::GetEncodedFormat()
{
    return imageInfo_.encodedFormat;
}

int32_t OhosImageDecoderAdapterImpl::GetImageWidth()
{
    return imageInfo_.size.width;
}

int32_t OhosImageDecoderAdapterImpl::GetImageHeight()
{
    return imageInfo_.size.height;
}

bool OhosImageDecoderAdapterImpl::DecodeToPixelMap(const uint8_t* data, uint32_t size)
{
    return Decode(data, size, AllocatorType::kDmaAlloc, false);
}

bool OhosImageDecoderAdapterImpl::Decode(const uint8_t* data,
                                         uint32_t size,
                                         AllocatorType type,
                                         bool useYuv)
{
    // Manage lifecycle of pixelmap and native window buffer with map next.
    WVLOG_I("[HeifSupport] OhosImageDecoderAdapterImpl DecodeToPixelMap.");
    auto imageSource = ParseRawData(data, size, imageInfo_);
    if (imageSource == nullptr) {
        WVLOG_E(
            "[HeifSupport] OhosImageDecoderAdapterImpl::DecodeToPixelMap, fail to get image source.");
        return false;
    }

    uint32_t errorCode = 0;
    Media::DecodeOptions decodeOptions;
    decodeOptions.desiredPixelFormat =
        useYuv ? Media::PixelFormat::NV12 : Media::PixelFormat::RGBA_8888;
    decodeOptions.allocatorType = static_cast<Media::AllocatorType>(type);
    pixelMap_ = imageSource->CreatePixelMap(decodeOptions, errorCode);
    if (errorCode != Media::SUCCESS) {
        WVLOG_E("[HeifSupport] CreatePixelMap failed, errorCode %{public}d", errorCode);
        return false;
    }

    return true;
}

int32_t OhosImageDecoderAdapterImpl::GetFd()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] OhosImageDecoderAdapterImpl::GetFd. PixelMap is null.");
        return -1;
    }
    if (auto* surfaceBuffer = SurfaceBufferFromPixelMap(pixelMap_.get())) {
        return surfaceBuffer->GetFileDescriptor();
    }
    WVLOG_E(
        "[HeifSupport] OhosImageDecoderAdapterImpl::GetFd. Fail to get surface buffer.");

    return -1;
}

int32_t OhosImageDecoderAdapterImpl::GetStride()
{
    if (!pixelMap_) {
        WVLOG_E(
            "[HeifSupport] OhosImageDecoderAdapterImpl::GetStride. PixelMap is null.");
        return 0;
    }
    if (pixelMap_->GetAllocatorType() == Media::AllocatorType::SHARE_MEM_ALLOC) {
        WVLOG_D("[HeifSupport] OhosImageDecoderAdapterImpl::GetStride. share mem get row stride.");
        return pixelMap_->GetRowStride();
    }
    if (auto* surfaceBuffer = SurfaceBufferFromPixelMap(pixelMap_.get())) {
        // Pixmap row stride is suface buffer stride as We only support DMA_ALLOC now.
        return surfaceBuffer->GetStride();
    }
    WVLOG_E(
        "[HeifSupport] OhosImageDecoderAdapterImpl::GetStride. Fail to get surface buffer.");

    return 0;
}

int32_t OhosImageDecoderAdapterImpl::GetOffset()
{
    if (!pixelMap_) {
        WVLOG_E(
            "[HeifSupport] OhosImageDecoderAdapterImpl::GetOffset. PixelMap is null.");
        return 0;
    }
    if (auto* surfaceBuffer = SurfaceBufferFromPixelMap(pixelMap_.get())) {
        OH_NativeBuffer_Planes* native_buffer_planes_;
        surfaceBuffer->GetPlanesInfo((void**)&native_buffer_planes_);
        if (!native_buffer_planes_) {
            WVLOG_E(
                "[HeifSupport] OhosImageDecoderAdapterImpl::GetOffset. Fail to get native buffer Planes.");
            return 0;
        }
        return native_buffer_planes_->planes[0].offset;
    }
    WVLOG_E(
        "[HeifSupport] OhosImageDecoderAdapterImpl::GetStride. Fail to get surface buffer.");

    return 0;
}

uint64_t OhosImageDecoderAdapterImpl::GetSize()
{
    if (!pixelMap_) {
        WVLOG_E(
            "[HeifSupport] OhosImageDecoderAdapterImpl::GetSize. PixelMap is null.");
        return 0;
    }
    if (auto* surfaceBuffer = SurfaceBufferFromPixelMap(pixelMap_.get())) {
        return surfaceBuffer->GetSize();
    }
    WVLOG_E(
        "[HeifSupport] OhosImageDecoderAdapterImpl::GetSize. Fail to get surface buffer.");

    return 0;
}

void* OhosImageDecoderAdapterImpl::GetNativeWindowBuffer()
{
    if (!pixelMap_) {
        WVLOG_E(
            "[HeifSupport] OhosImageDecoderAdapterImpl::GetNativeWindowBuffer. PixelMap is null.");
        return nullptr;
    }
    if (!nativeWindowBuffer_) {
        if (auto* surfaceBuffer = SurfaceBufferFromPixelMap(pixelMap_.get())) {
            nativeWindowBuffer_ =
                CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
        }
    }
    return static_cast<void*>(nativeWindowBuffer_);
}

// Used for NV12
int32_t OhosImageDecoderAdapterImpl::GetPlanesCount()
{
    if (!pixelMap_) {
        WVLOG_E(
            "[HeifSupport] OhosImageDecoderAdapterImpl::GetPlanesCount. PixelMap is null.");
        return 0;
    }

    if (auto* surfaceBuffer = SurfaceBufferFromPixelMap(pixelMap_.get())) {
        OH_NativeBuffer_Planes* nativeBufferPlanes;
        surfaceBuffer->GetPlanesInfo((void**)&nativeBufferPlanes);
        if (!nativeBufferPlanes) {
            WVLOG_E(
                "[HeifSupport] OhosImageDecoderAdapterImpl::GetPlanesCount. Fail to get native buffer Planes.");
            return 0;
        }
        return nativeBufferPlanes->planeCount;
    }
    WVLOG_E(
        "[HeifSupport] OhosImageDecoderAdapterImpl::GetPlanesCount. Fail to get surface buffer.");

    return 0;
}

void OhosImageDecoderAdapterImpl::ReleasePixelMap()
{
    WVLOG_I("[HeifSupport] OhosImageDecoderAdapterImpl release pixelmap and native window buffer.");
    if (pixelMap_) {
        pixelMap_.reset();
        pixelMap_ = nullptr;
    }
    if (nativeWindowBuffer_) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
}

void* OhosImageDecoderAdapterImpl::GetDecodeData()
{
    if (!pixelMap_) {
        WVLOG_E("[HeifSupport] OhosImageDecoderAdapterImpl::GetDecodeData. PixelMap is null.");
        return nullptr;
    }
    return pixelMap_->GetWritablePixels();
}

}  // namespace NWeb
}  // namespace OHOS
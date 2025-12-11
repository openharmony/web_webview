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

#include "surface_adapter_impl.h"

#include <unordered_map>

#include "v1_0/display_composer_type.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
namespace {
bool IsSupportFormat(int32_t format)
{
    switch (format) {
        case PixelFormat::PIXEL_FMT_RGBA_8888:
        case PixelFormat::PIXEL_FMT_YCBCR_420_SP:
            return true;
        default:
            return false;
    }
}
} // namespace

SurfaceBufferAdapterImpl::SurfaceBufferAdapterImpl(sptr<SurfaceBuffer> buffer) : buffer_(buffer) {}

int32_t SurfaceBufferAdapterImpl::GetFileDescriptor()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetFileDescriptor();
}

int32_t SurfaceBufferAdapterImpl::GetWidth()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetWidth();
}

int32_t SurfaceBufferAdapterImpl::GetHeight()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetHeight();
}

int32_t SurfaceBufferAdapterImpl::GetStride()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetStride();
}

int32_t SurfaceBufferAdapterImpl::GetFormat()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetFormat();
}

uint32_t SurfaceBufferAdapterImpl::GetSize()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return 0;
    }
    return buffer_->GetSize();
}

void* SurfaceBufferAdapterImpl::GetVirAddr()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return nullptr;
    }
    return buffer_->GetVirAddr();
}

sptr<SurfaceBuffer>& SurfaceBufferAdapterImpl::GetBuffer()
{
    return buffer_;
}

BufferConsumerListenerImpl::BufferConsumerListenerImpl(
    wptr<IConsumerSurface> surface, std::shared_ptr<IBufferConsumerListenerAdapter> listener)
    : cSurface_(surface), listener_(std::move(listener))
{}

void BufferConsumerListenerImpl::OnBufferAvailable()
{
    if (cSurface_ == nullptr || listener_ == nullptr) {
        WVLOG_E("cSurface_ or listener_ is nullptr");
        return;
    }
    auto surfaceTemp = cSurface_.promote();
    if (surfaceTemp == nullptr) {
        WVLOG_E("surface is nullptr");
        return;
    }
    sptr<SurfaceBuffer> buffer;
    int32_t fence;
    int64_t timestamp;
    Rect damage;
    GSError ret = surfaceTemp->AcquireBuffer(buffer, fence, timestamp, damage);
    if (ret != (int32_t)GSERROR_OK) {
        WVLOG_E("acquire buffer failed, ret=%{public}d", ret);
        return;
    }
    int32_t format = buffer->GetFormat();
    if (!IsSupportFormat(format)) {
        WVLOG_E("unsupport format for:%{public}d", format);
        surfaceTemp->ReleaseBuffer(buffer, -1);
        return;
    }
    auto bufferAdapter = std::make_shared<SurfaceBufferAdapterImpl>(buffer);
    listener_->OnBufferAvailable(std::move(bufferAdapter));
}

ConsumerSurfaceAdapterImpl::ConsumerSurfaceAdapterImpl() : cSurface_(IConsumerSurface::Create()) {}

int32_t ConsumerSurfaceAdapterImpl::RegisterConsumerListener(
    std::shared_ptr<IBufferConsumerListenerAdapter> listenerAdapter)
{
    if (!cSurface_ || !listenerAdapter) {
        WVLOG_E("cSurface_ or listener_ is nullptr");
        return -1;
    }
    sptr<IBufferConsumerListener> listener =
        new (std::nothrow) BufferConsumerListenerImpl(cSurface_, std::move(listenerAdapter));
    if (!listener) {
        WVLOG_E("listener create failed");
        return -1;
    }
    return cSurface_->RegisterConsumerListener(listener);
}

int32_t ConsumerSurfaceAdapterImpl::ReleaseBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence)
{
    if (!cSurface_ || !bufferAdapter) {
        WVLOG_E("cSurface_ or bufferAdapter is nullptr");
        return -1;
    }
    auto bufferImpl = static_cast<SurfaceBufferAdapterImpl*>(bufferAdapter.get());
    return cSurface_->ReleaseBuffer(bufferImpl->GetBuffer(), fence);
}

int32_t ConsumerSurfaceAdapterImpl::SetUserData(const std::string& key, const std::string& val)
{
    if (!cSurface_) {
        WVLOG_E("cSurface_ is nullptr");
        return -1;
    }
    return cSurface_->SetUserData(key, val);
}

int32_t ConsumerSurfaceAdapterImpl::SetQueueSize(uint32_t queueSize)
{
    if (!cSurface_) {
        WVLOG_E("cSurface_ is nullptr");
        return -1;
    }
    return cSurface_->SetQueueSize(queueSize);
}

sptr<IConsumerSurface>& ConsumerSurfaceAdapterImpl::GetConsumerSurface()
{
    return cSurface_;
}

const std::unordered_map<TransformTypeAdapter, GraphicTransformType> TO_TRANSFORM_TYPE_MAP = {
    { TransformTypeAdapter::ROTATE_NONE, GraphicTransformType::GRAPHIC_ROTATE_NONE },
    { TransformTypeAdapter::ROTATE_90, GraphicTransformType::GRAPHIC_ROTATE_90 },
    { TransformTypeAdapter::ROTATE_180, GraphicTransformType::GRAPHIC_ROTATE_180 },
    { TransformTypeAdapter::ROTATE_270, GraphicTransformType::GRAPHIC_ROTATE_270 },
    { TransformTypeAdapter::FLIP_H, GraphicTransformType::GRAPHIC_FLIP_H },
    { TransformTypeAdapter::FLIP_V, GraphicTransformType::GRAPHIC_FLIP_V },
    { TransformTypeAdapter::FLIP_H_ROT90, GraphicTransformType::GRAPHIC_FLIP_H_ROT90 },
    { TransformTypeAdapter::FLIP_V_ROT90, GraphicTransformType::GRAPHIC_FLIP_V_ROT90 },
    { TransformTypeAdapter::FLIP_H_ROT180, GraphicTransformType::GRAPHIC_FLIP_H_ROT180 },
    { TransformTypeAdapter::FLIP_V_ROT180, GraphicTransformType::GRAPHIC_FLIP_V_ROT180 },
    { TransformTypeAdapter::FLIP_H_ROT270, GraphicTransformType::GRAPHIC_FLIP_H_ROT270 },
    { TransformTypeAdapter::FLIP_V_ROT270, GraphicTransformType::GRAPHIC_FLIP_V_ROT270 },
    { TransformTypeAdapter::ROTATE_BUTT, GraphicTransformType::GRAPHIC_ROTATE_BUTT },
};

const std::unordered_map<ColorGamutAdapter, GraphicColorGamut> TO_GRAPHIC_COLOR_GAMUT_MAP = {
    { ColorGamutAdapter::INVALID, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_INVALID },
    { ColorGamutAdapter::NATIVE, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE },
    { ColorGamutAdapter::STANDARD_BT601, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_STANDARD_BT601 },
    { ColorGamutAdapter::STANDARD_BT709, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_STANDARD_BT709 },
    { ColorGamutAdapter::DCI_P3, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3 },
    { ColorGamutAdapter::SRGB, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB },
    { ColorGamutAdapter::ADOBE_RGB, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB },
    { ColorGamutAdapter::DISPLAY_P3, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3 },
    { ColorGamutAdapter::BT2020, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020 },
    { ColorGamutAdapter::BT2100_PQ, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2100_PQ },
    { ColorGamutAdapter::BT2100_HLG, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2100_HLG },
    { ColorGamutAdapter::DISPLAY_BT2020, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020 },
};

ProducerSurfaceAdapterImpl::ProducerSurfaceAdapterImpl(sptr<Surface> surface) : surface_(surface) {}

GraphicTransformType ProducerSurfaceAdapterImpl::TransToTransformType(const TransformTypeAdapter& type)
{
    auto item = TO_TRANSFORM_TYPE_MAP.find(type);
    if (item == TO_TRANSFORM_TYPE_MAP.end()) {
        WVLOG_E("to graphic transform type %{public}d not found", type);
        return GraphicTransformType::GRAPHIC_ROTATE_NONE;
    }
    return item->second;
}

GraphicColorGamut ProducerSurfaceAdapterImpl::TransToGraphicColorGamut(const ColorGamutAdapter& colorGamut)
{
    auto item = TO_GRAPHIC_COLOR_GAMUT_MAP.find(colorGamut);
    if (item == TO_GRAPHIC_COLOR_GAMUT_MAP.end()) {
        WVLOG_E("to graphic color gamut %{public}d not found", colorGamut);
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    }
    return item->second;
}

void ProducerSurfaceAdapterImpl::TransToBufferConfig(
    const std::shared_ptr<BufferRequestConfigAdapter> configAdapter, BufferRequestConfig& config)
{
    if (!configAdapter) {
        WVLOG_E("TransToBufferConfig configAdapter is null");
        return;
    }

    config.width = configAdapter->GetWidth();
    config.height = configAdapter->GetHeight();
    config.strideAlignment = configAdapter->GetStrideAlignment();
    config.format = GRAPHIC_PIXEL_FMT_YCBCR_420_P;
    config.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    config.timeout = configAdapter->GetTimeout();
    config.colorGamut = TransToGraphicColorGamut(configAdapter->GetColorGamut());
    config.transform = TransToTransformType(configAdapter->GetTransformType());
}

std::shared_ptr<SurfaceBufferAdapter> ProducerSurfaceAdapterImpl::RequestBuffer(
    int32_t& fence, std::shared_ptr<BufferRequestConfigAdapter> configAdapter)
{
    if (!surface_) {
        WVLOG_E("Surface_ is nullptr when request");
        return nullptr;
    }

    if (!configAdapter) {
        WVLOG_E("configAdapter is nullptr when request");
        return nullptr;
    }

    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    BufferRequestConfig config;
    TransToBufferConfig(configAdapter, config);
    surface_->RequestBuffer(buffer, fence, config);
    return std::make_shared<SurfaceBufferAdapterImpl>(buffer);
}

int32_t ProducerSurfaceAdapterImpl::FlushBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence,
    std::shared_ptr<BufferFlushConfigAdapter> flushConfigAdapter)
{
    if (!surface_ || !bufferAdapter || !flushConfigAdapter) {
        WVLOG_E("Surface_ or params is nullptr when flush");
        return -1;
    }
    auto bufferImpl = static_cast<SurfaceBufferAdapterImpl*>(bufferAdapter.get());
    BufferFlushConfig config;
    config.damage.x = flushConfigAdapter->GetX();
    config.damage.y = flushConfigAdapter->GetY();
    config.damage.w = flushConfigAdapter->GetW();
    config.damage.h = flushConfigAdapter->GetH();
    config.timestamp = flushConfigAdapter->GetTimestamp();
    return surface_->FlushBuffer(bufferImpl->GetBuffer(), fence, config);
}
} // namespace OHOS::NWeb

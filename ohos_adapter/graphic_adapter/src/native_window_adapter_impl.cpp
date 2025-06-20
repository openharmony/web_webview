/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "native_window_adapter_impl.h"

#include <native_window/external_window.h>
#include <sys/mman.h>
#include <unordered_map>

#include "nweb_log.h"


namespace OHOS::NWeb {
constexpr int32_t INVALID = -1;
namespace {
bool IsSupportFormat(int32_t format)
{
    switch (format) {
        case OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888:
        case OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_YCBCR_420_SP:
            return true;
        default:
            return false;
    }
}
void OnBufferAvailableWapper(void *context)
{
    NativeBufferConsumerListenerImpl *callback = (NativeBufferConsumerListenerImpl*)(context);
    callback->OnBufferAvailable();
}
} // namespace

NativeBufferAdapterImpl::NativeBufferAdapterImpl(OHNativeWindowBuffer *buffer) {
    if (buffer == nullptr) {
        WVLOG_E("buffer is nullptr");
        return;
    }
    buffer_ = buffer;
    windowHandle_ = OH_NativeWindow_GetBufferHandleFromNative(buffer_);
}

NativeBufferAdapterImpl::~NativeBufferAdapterImpl() {
    if (mappedAddr_ != nullptr && windowHandle_ != nullptr) {
        int32_t ret = munmap(mappedAddr_, windowHandle_->size);
        if (ret != 0) {
            WVLOG_E("Unmap ashmem failed, ret = %{public}d", ret);
        }
        mappedAddr_ = nullptr;
    }
}

int32_t NativeBufferAdapterImpl::GetFileDescriptor()
{
    if (windowHandle_ == nullptr) {
        WVLOG_E("windowHandle_ is nullptr");
        return -1;
    }
    return windowHandle_->fd;
}

int32_t NativeBufferAdapterImpl::GetWidth()
{
    if (windowHandle_ == nullptr) {
        WVLOG_E("windowHandle_ is nullptr");
        return -1;
    }
    return windowHandle_->width;
}

int32_t NativeBufferAdapterImpl::GetHeight()
{
    if (windowHandle_ == nullptr) {
        WVLOG_E("windowHandle_ is nullptr");
        return -1;
    }
    return windowHandle_->height;
}

int32_t NativeBufferAdapterImpl::GetStride()
{
    if (windowHandle_ == nullptr) {
        WVLOG_E("windowHandle_ is nullptr");
        return -1;
    }
    return windowHandle_->stride;
}

int32_t NativeBufferAdapterImpl::GetFormat()
{
    if (windowHandle_ == nullptr) {
        WVLOG_E("windowHandle_ is nullptr");
        return -1;
    }
    return windowHandle_->format;
}

uint32_t NativeBufferAdapterImpl::GetSize()
{
    if (windowHandle_ == nullptr) {
        WVLOG_E("windowHandle_ is nullptr");
        return -1;
    }
    return windowHandle_->size;
}

void* NativeBufferAdapterImpl::GetVirAddr()
{
    if (mappedAddr_ != nullptr) {
        return mappedAddr_;
    }

    if (windowHandle_ == nullptr) {
        WVLOG_E("windowHandle_ is nullptr");
        return nullptr;
    }

    if (windowHandle_->size <= 0) {
        WVLOG_E("Invalid size %{public}d", windowHandle_->size);
        return nullptr;
    }

    mappedAddr_ = mmap(nullptr, windowHandle_->size, PROT_READ | PROT_WRITE, MAP_SHARED, windowHandle_->fd, 0);
    if ((mappedAddr_ == MAP_FAILED) || (mappedAddr_ == nullptr)) {
        WVLOG_E("Map ashmem failed, ret = 0x{public}lx", mappedAddr_);
        return nullptr;
    }

    return mappedAddr_;
}

OHNativeWindowBuffer* NativeBufferAdapterImpl::GetBuffer()
{
    return buffer_;
}

NativeBufferConsumerListenerImpl::NativeBufferConsumerListenerImpl(
    OH_NativeImage *nativeImage, std::shared_ptr<IBufferConsumerListenerAdapter> listener)
    : nativeImage_(nativeImage), listener_(std::move(listener))
{}

void NativeBufferConsumerListenerImpl::OnBufferAvailable()
{
    if (nativeImage_ == nullptr || listener_ == nullptr) {
        WVLOG_E("cSurface_ or nativeImage_ or listener_ is nullptr");
        return;
    }
    OHNativeWindowBuffer *buffer;
    int32_t fenceFd;
    int32_t ret = OH_NativeImage_AcquireNativeWindowBuffer(nativeImage_, &buffer, &fenceFd);
    if (ret != (int32_t)GSERROR_OK) {
        WVLOG_E("acquire buffer failed, ret=%{public}d", ret);
        return;
    }
    auto bufferAdapter = std::make_shared<NativeBufferAdapterImpl>(buffer);
    int32_t format = bufferAdapter->GetFormat();
    if (!IsSupportFormat(format)) {
        WVLOG_E("unsupport format for:%{public}d", format);
        OH_NativeImage_ReleaseNativeWindowBuffer(nativeImage_, buffer, fenceFd);
        return;
    }
    listener_->OnBufferAvailable(std::move(bufferAdapter));
}

ConsumerNativeAdapterImpl::ConsumerNativeAdapterImpl()
{
    cImage_ = OH_NativeImage_Create(0, 0);
}

ConsumerNativeAdapterImpl::~ConsumerNativeAdapterImpl()
{
    OH_NativeImage_Destroy(&cImage_);
}

int32_t ConsumerNativeAdapterImpl::RegisterConsumerListener(
    std::shared_ptr<IBufferConsumerListenerAdapter> listenerAdapter)
{
    if (!cImage_ || !listenerAdapter) {
        WVLOG_E("cImage_ or listener_ is nullptr");
        return -1;
    }
    listenerAdapter_ = std::make_shared<NativeBufferConsumerListenerImpl>(cImage_, listenerAdapter);
    OH_OnFrameAvailableListener listenerWapper;
    listenerWapper.context = listenerAdapter_.get();
    listenerWapper.onFrameAvailable = OnBufferAvailableWapper;
    return OH_NativeImage_SetOnFrameAvailableListener(cImage_, listenerWapper);
}

int32_t ConsumerNativeAdapterImpl::ReleaseBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence)
{
    if (!cImage_ || !bufferAdapter) {
        WVLOG_E("cImage_ or bufferAdapter is nullptr");
        return -1;
    }
    auto bufferImpl = static_cast<NativeBufferAdapterImpl*>(bufferAdapter.get());
    return OH_NativeImage_ReleaseNativeWindowBuffer(cImage_, bufferImpl->GetBuffer(), fence);
}

int32_t ConsumerNativeAdapterImpl::SetUserData(const std::string& key, const std::string& val)
{
    WVLOG_I("SetUserData don't need call");
    return 0;
}

int32_t ConsumerNativeAdapterImpl::SetQueueSize(uint32_t queueSize)
{
    WVLOG_I("SetQueueSize don't need call");
    return 0;
}

OH_NativeImage* ConsumerNativeAdapterImpl::GetConsumerSurface()
{
    return cImage_;
}

const std::unordered_map<TransformTypeAdapter, OH_NativeBuffer_TransformType> TO_TRANSFORM_TYPE_MAP = {
    { TransformTypeAdapter::ROTATE_NONE, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_NONE },
    { TransformTypeAdapter::ROTATE_90, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_90 },
    { TransformTypeAdapter::ROTATE_180, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_180 },
    { TransformTypeAdapter::ROTATE_270, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_270 },
    { TransformTypeAdapter::FLIP_H, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_H },
    { TransformTypeAdapter::FLIP_V, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_V },
    { TransformTypeAdapter::FLIP_H_ROT90, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_H_ROT90 },
    { TransformTypeAdapter::FLIP_V_ROT90, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_V_ROT90 },
    { TransformTypeAdapter::FLIP_H_ROT180, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_H_ROT180 },
    { TransformTypeAdapter::FLIP_V_ROT180, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_V_ROT180 },
    { TransformTypeAdapter::FLIP_H_ROT270, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_H_ROT270 },
    { TransformTypeAdapter::FLIP_V_ROT270, OH_NativeBuffer_TransformType::NATIVEBUFFER_FLIP_V_ROT270 },
    { TransformTypeAdapter::ROTATE_BUTT, (OH_NativeBuffer_TransformType)INVALID }, // if is ROTATE_BUTT, set -1
};

const std::unordered_map<ColorGamutAdapter, OH_NativeBuffer_ColorGamut> TO_GRAPHIC_COLOR_GAMUT_MAP = {
    { ColorGamutAdapter::INVALID, (OH_NativeBuffer_ColorGamut)INVALID }, // if is INVALID, set -1
    { ColorGamutAdapter::NATIVE, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_NATIVE },
    { ColorGamutAdapter::STANDARD_BT601, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_STANDARD_BT601 },
    { ColorGamutAdapter::STANDARD_BT709, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_STANDARD_BT709 },
    { ColorGamutAdapter::DCI_P3, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_DCI_P3 },
    { ColorGamutAdapter::SRGB, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_SRGB },
    { ColorGamutAdapter::ADOBE_RGB, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_ADOBE_RGB },
    { ColorGamutAdapter::DISPLAY_P3, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_DISPLAY_P3 },
    { ColorGamutAdapter::BT2020, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_BT2020 },
    { ColorGamutAdapter::BT2100_PQ, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_BT2100_PQ },
    { ColorGamutAdapter::BT2100_HLG, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_BT2100_HLG },
    { ColorGamutAdapter::DISPLAY_BT2020, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_DISPLAY_BT2020 },
};

ProducerNativeAdapterImpl::ProducerNativeAdapterImpl(OHNativeWindow* window) : window_(window) {}
ProducerNativeAdapterImpl::~ProducerNativeAdapterImpl() {
    if (window_ != nullptr) {
        OH_NativeWindow_DestroyNativeWindow(window_);
        window_ = nullptr;
    }
}

OH_NativeBuffer_TransformType ProducerNativeAdapterImpl::TransToTransformType(const TransformTypeAdapter& type)
{
    auto item = TO_TRANSFORM_TYPE_MAP.find(type);
    if (item == TO_TRANSFORM_TYPE_MAP.end()) {
        WVLOG_E("to graphic transform type %{public}d not found", type);
        return OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_NONE;
    }
    return item->second;
}

OH_NativeBuffer_ColorGamut ProducerNativeAdapterImpl::TransToGraphicColorGamut(const ColorGamutAdapter& colorGamut)
{
    auto item = TO_GRAPHIC_COLOR_GAMUT_MAP.find(colorGamut);
    if (item == TO_GRAPHIC_COLOR_GAMUT_MAP.end()) {
        WVLOG_E("to graphic color gamut %{public}d not found", colorGamut);
        return OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_SRGB;
    }
    return item->second;
}

void ProducerNativeAdapterImpl::TransToBufferConfig(
    const std::shared_ptr<BufferRequestConfigAdapter> configAdapter)
{
    if (!configAdapter) {
        WVLOG_E("TransToBufferConfig configAdapter is null");
        return;
    }

    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_BUFFER_GEOMETRY,
        configAdapter->GetWidth(), configAdapter->GetHeight());
    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_STRIDE, configAdapter->GetStrideAlignment());
    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_FORMAT, NATIVEBUFFER_PIXEL_FMT_YCBCR_420_P);
    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_USAGE,
        NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE | NATIVEBUFFER_USAGE_MEM_DMA);
    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_TIMEOUT, configAdapter->GetTimeout());
    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_COLOR_GAMUT,
        TransToGraphicColorGamut(configAdapter->GetColorGamut()));
    int code = static_cast<int32_t>(TransToTransformType(configAdapter->GetTransformType()));
    if (code != INVALID) { // if is ROTATE_BUTT, do not set transform
        OH_NativeWindow_NativeWindowHandleOpt(window_, SET_TRANSFORM, code);
    }
}

std::shared_ptr<SurfaceBufferAdapter> ProducerNativeAdapterImpl::RequestBuffer(
    int32_t& fence, std::shared_ptr<BufferRequestConfigAdapter> configAdapter)
{
    if (!window_) {
        WVLOG_E("Window_ is nullptr when request");
        return nullptr;
    }
    if (!configAdapter) {
        WVLOG_E("configAdapter is nullptr when request");
        return nullptr;
    }

    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_UI_TIMESTAMP, configAdapter->GetTimestamp());

    TransToBufferConfig(configAdapter);
    OHNativeWindowBuffer* buffer = nullptr;
    if (OH_NativeWindow_NativeWindowRequestBuffer(window_, &buffer, &fence) != 0) {
        WVLOG_E("native window request buffer failed");
        return nullptr;
    }
    return std::make_shared<NativeBufferAdapterImpl>(buffer);
}

int32_t ProducerNativeAdapterImpl::FlushBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence,
    std::shared_ptr<BufferFlushConfigAdapter> flushConfigAdapter)
{
    if (!window_ || !bufferAdapter || !flushConfigAdapter) {
        WVLOG_E("Surface_ or params is nullptr when flush");
        return -1;
    }
    auto bufferImpl = static_cast<NativeBufferAdapterImpl*>(bufferAdapter.get());
    Region::Rect rect = { flushConfigAdapter->GetX(), flushConfigAdapter->GetY(),
        flushConfigAdapter->GetW(), flushConfigAdapter->GetH() };
    Region region = {.rects = &rect, .rectNumber = 0};
    
    return OH_NativeWindow_NativeWindowFlushBuffer(window_, bufferImpl->GetBuffer(), fence, region);
}
} // namespace OHOS::NWeb

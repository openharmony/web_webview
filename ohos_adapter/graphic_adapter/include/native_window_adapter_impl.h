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

#ifndef NATIVE_WINDOW_ADAPTER_IMPL_H
#define NATIVE_WINDOW_ADAPTER_IMPL_H

#include "graphic_adapter.h"
#include <native_image/native_image.h>
#include <native_window/buffer_handle.h>
#include <native_buffer/native_buffer.h>

namespace OHOS::NWeb {
class NativeBufferAdapterImpl : public SurfaceBufferAdapter {
public:
    NativeBufferAdapterImpl();

    explicit NativeBufferAdapterImpl(OHNativeWindowBuffer *buffer);

    ~NativeBufferAdapterImpl() override;

    int32_t GetFileDescriptor() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetStride() override;

    int32_t GetFormat() override;

    uint32_t GetSize() override;

    void* GetVirAddr() override;

    OHNativeWindowBuffer *GetBuffer();

private:
    OHNativeWindowBuffer *buffer_ = nullptr;
    BufferHandle *windowHandle_ = nullptr;
};

class NativeBufferConsumerListenerImpl {
public:
    NativeBufferConsumerListenerImpl(
        OH_NativeImage *nativeImage, std::shared_ptr<IBufferConsumerListenerAdapter> listener);

    ~NativeBufferConsumerListenerImpl() = default;

    void OnBufferAvailable();

private:
    OH_NativeImage *nativeImage_ = nullptr;
    std::shared_ptr<IBufferConsumerListenerAdapter> listener_ = nullptr;
};

class ConsumerNativeAdapterImpl : public IConsumerSurfaceAdapter {
public:
    ConsumerNativeAdapterImpl();

    ~ConsumerNativeAdapterImpl();

    int32_t RegisterConsumerListener(std::shared_ptr<IBufferConsumerListenerAdapter> listenerAdapter) override;

    int32_t ReleaseBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence) override;

    int32_t SetUserData(const std::string& key, const std::string& val) override;

    int32_t SetQueueSize(uint32_t queueSize) override;

    OH_NativeImage* GetConsumerSurface();
private:
    OH_NativeImage* cImage_ = nullptr;
    std::shared_ptr<NativeBufferConsumerListenerImpl> listenerAdapter_;
};

class ProducerNativeAdapterImpl : public ProducerSurfaceAdapter {
public:
    explicit ProducerNativeAdapterImpl(OHNativeWindow *window);

    ~ProducerNativeAdapterImpl() override;

    std::shared_ptr<SurfaceBufferAdapter> RequestBuffer(
        int32_t& fence, std::shared_ptr<BufferRequestConfigAdapter> config) override;

    int32_t FlushBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence,
        std::shared_ptr<BufferFlushConfigAdapter> flushConfigAdapter) override;

private:
    void TransToBufferConfig(
        const std::shared_ptr<BufferRequestConfigAdapter> configAdapter);
    OH_NativeBuffer_ColorGamut TransToGraphicColorGamut(const ColorGamutAdapter& colorGamut);
    OH_NativeBuffer_TransformType TransToTransformType(const TransformTypeAdapter& type);
    OHNativeWindow *window_ = nullptr;
};
} // namespace OHOS::NWeb

#endif // NATIVE_WINDOW_ADAPTER_IMPL_H

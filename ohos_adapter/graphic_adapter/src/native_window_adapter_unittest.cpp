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

#include "testing/gtest/include/gtest/gtest.h"
#include "nweb_log.h"
#define private public
#include "native_window_adapter_impl.h"
#undef private
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
namespace {
const int32_t DEFAULT_WIDTH = 1396;
const int32_t DEFAULT_HEIGHT = 1396;
}
class BufferRequestConfigAdapterMock : public BufferRequestConfigAdapter {
public:
    BufferRequestConfigAdapterMock() = default;
    ~BufferRequestConfigAdapterMock() override = default;

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    int32_t GetStrideAlignment() override
    {
        return strideAlignment;
    }

    int32_t GetFormat() override
    {
        return format;
    }

    uint64_t GetUsage() override
    {
        return usage;
    }

    int32_t GetTimeout() override
    {
        return timeout;
    }

    ColorGamutAdapter GetColorGamut() override
    {
        return colorGamut;
    }

    TransformTypeAdapter GetTransformType() override
    {
        return transform;
    }

    int32_t width = DEFAULT_WIDTH;
    int32_t height = DEFAULT_HEIGHT;
    int32_t strideAlignment = 1;
    int32_t format = 1;
    uint64_t usage = 1;
    int32_t timeout = 1;
    ColorGamutAdapter colorGamut = ColorGamutAdapter::STANDARD_BT601;
    TransformTypeAdapter transform = TransformTypeAdapter::ROTATE_90;
};

class BufferFlushConfigAdapterMock : public BufferFlushConfigAdapter {
public:
    BufferFlushConfigAdapterMock() = default;

    ~BufferFlushConfigAdapterMock() override = default;

    int32_t GetX() override
    {
        return 1;
    }

    int32_t GetY() override
    {
        return 1;
    }

    int32_t GetW() override
    {
        return DEFAULT_WIDTH;
    }

    int32_t GetH() override
    {
        return DEFAULT_HEIGHT;
    }

    int64_t GetTimestamp() override
    {
        return 1;
    }
};

class BufferConsumerListenerTest : public IBufferConsumerListenerAdapter {
public:
    BufferConsumerListenerTest() = default;
    ~BufferConsumerListenerTest() override = default;
    void OnBufferAvailable(std::shared_ptr<SurfaceBufferAdapter> buffer) override
    {
        WVLOG_I("test buffer is available, the format=%{public}d", buffer->GetFormat());
    }
};

class NativeWindowAdapterImplTest : public testing::Test {
protected:
    void SetUp() override;
    BufferRequestConfigAdapterMock GetBufferRequestConfig(int32_t myFormat);
    std::unique_ptr<ConsumerNativeAdapterImpl> nativeAdapter_;
    std::unique_ptr<IBufferConsumerListenerAdapter> listenerTest_;
    static inline std::shared_ptr<BufferFlushConfigAdapter> flushConfig_ 
        = std::make_shared<BufferFlushConfigAdapterMock>();
};

void NativeWindowAdapterImplTest::SetUp()
{
    nativeAdapter_ = std::make_unique<ConsumerNativeAdapterImpl>();
    ASSERT_NE(nativeAdapter_, nullptr);
    ASSERT_NE(nativeAdapter_->cImage_, nullptr);
    listenerTest_ = std::make_unique<BufferConsumerListenerTest>();
    EXPECT_NE(listenerTest_, nullptr);
}

BufferRequestConfigAdapterMock NativeWindowAdapterImplTest::GetBufferRequestConfig(int32_t myFormat)
{
    BufferRequestConfigAdapterMock requestConfig;
    requestConfig.width = 0x100;
    requestConfig.height = 0x100;
    requestConfig.strideAlignment = 0x8;
    requestConfig.format = myFormat;
    requestConfig.usage = NATIVEBUFFER_USAGE_CPU_READ | NATIVEBUFFER_USAGE_CPU_WRITE | NATIVEBUFFER_USAGE_MEM_DMA;
    requestConfig.timeout = 0;
    return requestConfig;
}

/**
 * @tc.name: InvalidSceneOfSurfaceAdapterImpl.
 * @tc.desc: test invalid scene of SurfaceAdapterImpl.
 * @tc.type: FUNC.
 */
TEST_F(NativeWindowAdapterImplTest, InvalidSceneOfSurfaceAdapterImpl)
{
    EXPECT_EQ(nativeAdapter_->RegisterConsumerListener(nullptr), -1);
    EXPECT_EQ(nativeAdapter_->ReleaseBuffer(nullptr, 0), -1);

    nativeAdapter_->cImage_ = nullptr;
    EXPECT_EQ(nativeAdapter_->RegisterConsumerListener(std::move(listenerTest_)), -1);
    EXPECT_EQ(nativeAdapter_->ReleaseBuffer(nullptr, 0), -1);
    EXPECT_EQ(nativeAdapter_->SetUserData("testkey", "testval"), 0);
    EXPECT_EQ(nativeAdapter_->SetQueueSize(0), 0);
}

/**
 * @tc.name: InvalidSceneOfNativeBufferConsumerListenerImpl.
 * @tc.desc: test invalid scene of NativeBufferConsumerListenerImpl.
 * @tc.type: FUNC.
 */
TEST_F(NativeWindowAdapterImplTest, InvalidSceneOfNativeBufferConsumerListenerImpl)
{
    auto consumerListener =
        std::make_unique<NativeBufferConsumerListenerImpl>(nativeAdapter_->cImage_, std::move(listenerTest_));
    ASSERT_NE(consumerListener, nullptr);
    consumerListener->OnBufferAvailable();
    consumerListener->listener_ = nullptr;
    consumerListener->OnBufferAvailable();
    consumerListener->nativeImage_ = nullptr;
    consumerListener->OnBufferAvailable();
}

/**
 * @tc.name: InvalidSceneOfNativeBufferAdapterImpl.
 * @tc.desc: test invalid scene of NativeBufferAdapterImpl.
 * @tc.type: FUNC.
 */
TEST_F(NativeWindowAdapterImplTest, InvalidSceneOfNativeBufferAdapterImpl)
{
    auto bufferAdapter = std::make_unique<NativeBufferAdapterImpl>(nullptr);
    ASSERT_NE(bufferAdapter, nullptr);
    EXPECT_EQ(bufferAdapter->GetFileDescriptor(), -1);
    EXPECT_EQ(bufferAdapter->GetWidth(), -1);
    EXPECT_EQ(bufferAdapter->GetHeight(), -1);
    EXPECT_EQ(bufferAdapter->GetStride(), -1);
    EXPECT_EQ(bufferAdapter->GetFormat(), -1);
    EXPECT_EQ(bufferAdapter->GetSize(), -1);
    EXPECT_EQ(bufferAdapter->GetVirAddr(), nullptr);
}

/**
 * @tc.name: HandlesNormalScene.
 * @tc.desc: test normal scene of SurfaceAdapterImpl.
 * @tc.type: FUNC.
 */
TEST_F(NativeWindowAdapterImplTest, HandlesNormalScene)
{
    EXPECT_EQ(nativeAdapter_->RegisterConsumerListener(std::move(listenerTest_)), 0);
    EXPECT_EQ(nativeAdapter_->SetUserData("testkey", "testval"), 0);
    EXPECT_EQ(nativeAdapter_->SetQueueSize(1), 0);
    OH_NativeImage* image = nativeAdapter_->GetConsumerSurface();
    OHNativeWindow* window = OH_NativeImage_AcquireNativeWindow(image);
    ProducerNativeAdapterImpl producerAdapter = ProducerNativeAdapterImpl(window);
    int32_t fence = -1;
    std::array formatArray = { OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_YCBCR_420_SP };
    for (auto format : formatArray) {
        BufferRequestConfigAdapterMock config = GetBufferRequestConfig(format);
        std::shared_ptr<BufferRequestConfigAdapterMock> requestConfig
            = std::make_shared<BufferRequestConfigAdapterMock>(config);
        std::shared_ptr<SurfaceBufferAdapter> bufferAdapter = producerAdapter.RequestBuffer(fence, requestConfig); 
        ASSERT_NE(bufferAdapter, nullptr);
        EXPECT_EQ(producerAdapter.FlushBuffer(bufferAdapter, fence, flushConfig_), 0);
        EXPECT_GE(bufferAdapter->GetFileDescriptor(), 0);
        EXPECT_GE(bufferAdapter->GetWidth(), 0);
        EXPECT_GE(bufferAdapter->GetHeight(), 0);
        EXPECT_GE(bufferAdapter->GetStride(), 0);
        EXPECT_GE(bufferAdapter->GetFormat(), 0);
        EXPECT_GT(bufferAdapter->GetSize(), 0);
        EXPECT_NE(bufferAdapter->GetVirAddr(), nullptr);
        nativeAdapter_->ReleaseBuffer(std::move(bufferAdapter), fence);
    }
}

/**
 * @tc.name: HandlesNormalScene.
 * @tc.desc: test normal scene of PixelFormatAdapter.
 * @tc.type: FUNC.
 */
TEST(PixelFormatAdapterTest, HandlesNormalScene)
{
    EXPECT_NE(PixelFormatAdapter::PIXEL_FMT_RGBA_8888, 0);
    EXPECT_NE(PixelFormatAdapter::PIXEL_FMT_YCBCR_420_SP, 0);
    EXPECT_EQ((int)PixelFormatAdapter::PIXEL_FMT_RGBA_8888,
        (int)OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888);
    EXPECT_EQ((int)PixelFormatAdapter::PIXEL_FMT_YCBCR_420_SP,
        (int)OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_YCBCR_420_SP);
}

/**
 * @tc.name: ProducerNativeAdapterImpl.
 * @tc.desc: test normal scene of NativeWindowAdapterImplTest.
 * @tc.type: FUNC.
 */
TEST_F(NativeWindowAdapterImplTest, ProducerNativeAdapterImpl)
{
    int32_t fence = -1;
    auto nativeAdapter = std::make_unique<ConsumerNativeAdapterImpl>();
    OH_NativeImage* image = nativeAdapter->GetConsumerSurface();
    ASSERT_NE(image, nullptr);
    OHNativeWindow* window = OH_NativeImage_AcquireNativeWindow(image);
    auto adapter = std::make_unique<ProducerNativeAdapterImpl>(window);
    ASSERT_NE(adapter, nullptr);
    OH_NativeBuffer_TransformType type = adapter->TransToTransformType(TransformTypeAdapter::ROTATE_NONE);
    EXPECT_EQ(type, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_NONE);
    type = adapter->TransToTransformType(static_cast<TransformTypeAdapter>(-1));
    EXPECT_EQ(type, OH_NativeBuffer_TransformType::NATIVEBUFFER_ROTATE_NONE);
    OH_NativeBuffer_ColorGamut gamut = adapter->TransToGraphicColorGamut(ColorGamutAdapter::INVALID);
    EXPECT_EQ(gamut, (OH_NativeBuffer_ColorGamut)(-1));
    int32_t invalid = -2;
    gamut = adapter->TransToGraphicColorGamut(static_cast<ColorGamutAdapter>(invalid));
    EXPECT_EQ(gamut, OH_NativeBuffer_ColorGamut::NATIVEBUFFER_COLOR_GAMUT_SRGB);
    auto configAdapter = std::make_shared<BufferRequestConfigAdapterMock>();
    ASSERT_NE(configAdapter, nullptr);
    auto flushConfigAdapter = std::make_shared<BufferFlushConfigAdapterMock>();
    ASSERT_NE(flushConfigAdapter, nullptr);
    
    BufferRequestConfigAdapterMock config
        = GetBufferRequestConfig(OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_YCBCR_420_SP);
    configAdapter = std::make_shared<BufferRequestConfigAdapterMock>(config);
    std::shared_ptr<SurfaceBufferAdapter> buffer = adapter->RequestBuffer(fence, configAdapter);
    ASSERT_NE(buffer, nullptr);
    buffer = adapter->RequestBuffer(fence, nullptr);
    ASSERT_EQ(buffer, nullptr);
    int32_t result = adapter->FlushBuffer(nullptr, 1, flushConfigAdapter);
    EXPECT_EQ(result, -1);
    result = adapter->FlushBuffer(buffer, 1, flushConfigAdapter);
    EXPECT_NE(result, 0);
    result = adapter->FlushBuffer(buffer, 1, nullptr);
    EXPECT_EQ(result, -1);
    adapter->window_ = nullptr;
    buffer = adapter->RequestBuffer(fence, configAdapter);
    EXPECT_EQ(buffer, nullptr);
    result = adapter->FlushBuffer(buffer, 1, flushConfigAdapter);
    EXPECT_EQ(result, -1);
    result = adapter->FlushBuffer(nullptr, 1, flushConfigAdapter);
    EXPECT_EQ(result, -1);
    OH_NativeWindow_DestroyNativeWindow(window);
}
} // namespace OHOS::NWeb

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

#include "gtest/gtest.h"
#define private public
#include "avcodec_list.h"
#include "avcodec_video_encoder_impl.h"
#include "media_codec_encoder_adapter_impl.h"
#include "media_codec_list_adapter_impl.h"
#undef private
#include "avsharedmemory.h"
#include "avsharedmemorybase.h"

using testing::ext::TestSize;
using namespace OHOS::MediaAVCodec;

namespace OHOS::NWeb {
namespace {
const int32_t DEFAULT_WIDTH = 1396;
const int32_t DEFAULT_HEIGHT = 1396;
}
class EncoderCallbackImplTest : public testing::Test {};

class EncoderCallbackAdapterMock : public CodecCallbackAdapter {
public:
    EncoderCallbackAdapterMock() = default;

    ~EncoderCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(const std::shared_ptr<CodecFormatAdapter> formatApadter_) override {}

    void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) override {}

    void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag,
        std::shared_ptr<OhosBufferAdapter> buffer) override
    {}
};

class ProducerSurfaceAdapterMock : public ProducerSurfaceAdapter {
public:
    ProducerSurfaceAdapterMock() = default;

    virtual ~ProducerSurfaceAdapterMock() = default;

    std::shared_ptr<SurfaceBufferAdapter> RequestBuffer(
        int32_t& fence, std::shared_ptr<BufferRequestConfigAdapter> configAdapter) override
    {
        return nullptr;
    };

    int32_t FlushBuffer(std::shared_ptr<SurfaceBufferAdapter> bufferAdapter, int32_t fence,
        std::shared_ptr<BufferFlushConfigAdapter> flushConfigAdapter) override
    {
        return 0;
    };
};

class CodecConfigParaAdapterMock : public CodecConfigParaAdapter {
public:
    CodecConfigParaAdapterMock() = default;

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    int64_t GetBitRate() override
    {
        return bitRate;
    }

    double GetFrameRate() override
    {
        return frameRate;
    }

    int32_t width;
    int32_t height;
    int64_t bitRate;
    double frameRate;
};

class BufferRequestConfigAdapterMock : public BufferRequestConfigAdapter {
public:
    BufferRequestConfigAdapterMock() = default;

    ~BufferRequestConfigAdapterMock() override = default;

    int32_t GetWidth() override
    {
        return DEFAULT_WIDTH;
    }

    int32_t GetHeight() override
    {
        return DEFAULT_HEIGHT;
    }

    int32_t GetStrideAlignment() override
    {
        return 1;
    }

    int32_t GetFormat() override
    {
        return 1;
    }

    uint64_t GetUsage() override
    {
        return 1;
    }

    int32_t GetTimeout() override
    {
        return 1;
    }

    ColorGamutAdapter GetColorGamut() override
    {
        return ColorGamutAdapter::STANDARD_BT601;
    }

    TransformTypeAdapter GetTransformType() override
    {
        return TransformTypeAdapter::ROTATE_90;
    }
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

class MediaCodecEncoderAdapterImplTest : public testing::Test {
protected:
    void SetUp() override;
    std::shared_ptr<CodecConfigParaAdapterMock> config_ = nullptr;
    std::shared_ptr<MediaCodecEncoderAdapterImpl> mediaCodecEncoderAdapterImpl = nullptr;
};

void MediaCodecEncoderAdapterImplTest::SetUp()
{
    constexpr int32_t testWidth = 640;
    constexpr int32_t testHeight = 360;
    constexpr int32_t testBitRate = 300000;
    constexpr int32_t testFrameRate = 15;

    EXPECT_EQ(mediaCodecEncoderAdapterImpl, nullptr);
    mediaCodecEncoderAdapterImpl = std::make_unique<MediaCodecEncoderAdapterImpl>();
    EXPECT_NE(mediaCodecEncoderAdapterImpl, nullptr);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->encoder_, nullptr);
    EXPECT_EQ(config_, nullptr);
    config_ = std::make_shared<CodecConfigParaAdapterMock>();
    EXPECT_NE(config_, nullptr);
    config_->width = testWidth;
    config_->height = testHeight;
    config_->bitRate = testBitRate;
    config_->frameRate = testFrameRate;
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_CreateVideoEncoderByName_001.
 * @tc.desc: test of MediaCodecEncoderAdapterImpl::CreateVideoEncoderByName() CreateVideoEncoderByName()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_CreateVideoEncoderByName_001, TestSize.Level1)
{
    const std::string mimetype_ = "testmimeType";
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->CreateVideoCodecByMime(mimetype_), CodecCodeAdapter::ERROR);
    const std::string name_ = "testname";
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->CreateVideoCodecByName(name_), CodecCodeAdapter::ERROR);
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_InvalidValueTest_002.
 * @tc.desc: test of InvalidValueScene in MediaCodecEncoderAdapterImpl
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_InvalidValueTest_002, TestSize.Level1)
{
    uint32_t index = 0;
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Configure(config_), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Prepare(), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Start(), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Stop(), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Reset(), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Release(), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->ReleaseOutputBuffer(index, true), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->RequestKeyFrameSoon(), CodecCodeAdapter::ERROR);
    std::shared_ptr<CodecCallbackAdapter> callback = std::make_shared<EncoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->SetCodecCallback(callback), CodecCodeAdapter::ERROR);
    std::shared_ptr<ProducerSurfaceAdapter> surface = mediaCodecEncoderAdapterImpl->CreateInputSurface();
    EXPECT_EQ(surface, nullptr);
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_NormalTest_003.
 * @tc.desc: test of NormalScene in MediaCodecEncoderAdapterImpl
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_NormalTest_003, TestSize.Level1)
{
    mediaCodecEncoderAdapterImpl->encoder_ = std::make_shared<MediaAVCodec::AVCodecVideoEncoderImpl>();
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Configure(config_), CodecCodeAdapter::ERROR);
    std::shared_ptr<CodecCallbackAdapter> callback = std::make_shared<EncoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->SetCodecCallback(callback), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Prepare(), CodecCodeAdapter::ERROR);
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_SetCallbackDec_004.
 * @tc.desc: test of MediaCodecEncoderAdapterImpl::SetCodecCallback
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_SetCodecCallback_004, TestSize.Level1)
{
    std::shared_ptr<CodecCallbackAdapter> callback = nullptr;
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->SetCodecCallback(callback), CodecCodeAdapter::ERROR);
    callback = std::make_shared<EncoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->SetCodecCallback(callback), CodecCodeAdapter::ERROR);
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_GetTypeOrFlag_005.
 * @tc.desc: test of MediaCodecEncoderAdapterImpl::GetErrorType() GetBufferFlag()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_GetTypeOrFlag_005, TestSize.Level1)
{
    EXPECT_EQ(
        mediaCodecEncoderAdapterImpl->GetErrorType(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_EXTEND_START),
        ErrorType::CODEC_ERROR_EXTEND_START);
    EXPECT_EQ(
        mediaCodecEncoderAdapterImpl->GetBufferFlag(OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS),
        BufferFlag::CODEC_BUFFER_FLAG_EOS);
    AVCodecErrorType testAVCodecErrorType = static_cast<AVCodecErrorType>(100);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->GetErrorType(testAVCodecErrorType), ErrorType::CODEC_ERROR_INTERNAL);
    AVCodecBufferFlag testAVCodecBufferFlag = static_cast<AVCodecBufferFlag>(100);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->GetBufferFlag(testAVCodecBufferFlag), BufferFlag::CODEC_BUFFER_FLAG_NONE);
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_OnError_006.
 * @tc.desc: test of MediaCodecEncoderAdapterImpl::GetErrorType() GetBufferFlag()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_OnError_006, TestSize.Level1)
{
    std::shared_ptr<CodecCallbackAdapter> cb = nullptr;
    std::shared_ptr<EncoderCallbackImpl> callbackImpl = std::make_shared<EncoderCallbackImpl>(cb);
    EXPECT_NE(callbackImpl, nullptr);
    std::shared_ptr<CodecCallbackAdapter> callback = std::make_shared<EncoderCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    callbackImpl->cb_ = callback;
    callbackImpl->OnError(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_EXTEND_START, 1);
    MediaAVCodec::Format fomat;
    callbackImpl->OnOutputFormatChanged(fomat);
    callbackImpl->OnInputBufferAvailable(1, nullptr);
    std::shared_ptr<Media::AVSharedMemory> memory = std::make_shared<Media::AVSharedMemoryBase>(1, 1.0, "test");
    callbackImpl->OnInputBufferAvailable(1, memory);
    AVCodecBufferInfo info;
    callbackImpl->OnOutputBufferAvailable(1, info, AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS, memory);
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_GetList_001.
 * @tc.desc: test of MediaCodecListAdapterImpl::GetCodecCapability()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_GetList_001, TestSize.Level1)
{
    MediaCodecListAdapterImpl& codecListImpl = MediaCodecListAdapterImpl::GetInstance();

    std::shared_ptr<CapabilityDataAdapter> capaData = codecListImpl.GetCodecCapability("test", true);
    EXPECT_NE(capaData, nullptr);
    EXPECT_EQ(capaData->GetMaxWidth(), 0);

    capaData = codecListImpl.GetCodecCapability("video/avc", true);
    EXPECT_NE(capaData, nullptr);
    EXPECT_NE(capaData->GetMaxWidth(), 0);
}

/**
 * @tc.name: MediaCodecEncoderAdapterImpl_Surface_001.
 * @tc.desc: test of ProducerSurfaceAdapterImpl::RequestBuffer()、FlushBuffer()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecEncoderAdapterImplTest, MediaCodecEncoderAdapterImpl_Surface_001, TestSize.Level1)
{
    const std::string mimetype = "video/avc";
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->CreateVideoCodecByMime(mimetype), CodecCodeAdapter::OK);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Configure(config_), CodecCodeAdapter::OK);
    std::shared_ptr<ProducerSurfaceAdapter> surfaceAdapter =
        mediaCodecEncoderAdapterImpl->CreateInputSurface();
    ASSERT_NE(surfaceAdapter, nullptr);
    int32_t fence = -1;
    auto configAdapter = std::make_shared<BufferRequestConfigAdapterMock>();
    EXPECT_NE(configAdapter, nullptr);
    std::shared_ptr<SurfaceBufferAdapter> SurfaceBufferAdapter =
        surfaceAdapter->RequestBuffer(fence, configAdapter);
    EXPECT_NE(SurfaceBufferAdapter, nullptr);

    auto fulshConfigAdapter = std::make_shared<BufferFlushConfigAdapterMock>();
    EXPECT_NE(fulshConfigAdapter, nullptr);
    int32_t ret = surfaceAdapter->FlushBuffer(SurfaceBufferAdapter, fence, fulshConfigAdapter);
    EXPECT_NE(ret, 0);

    uint32_t index = 0;
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Prepare(), CodecCodeAdapter::OK);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Start(), CodecCodeAdapter::OK);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Stop(), CodecCodeAdapter::OK);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Reset(), CodecCodeAdapter::OK);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->Release(), CodecCodeAdapter::OK);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->ReleaseOutputBuffer(index, true), CodecCodeAdapter::ERROR);
    EXPECT_EQ(mediaCodecEncoderAdapterImpl->RequestKeyFrameSoon(), CodecCodeAdapter::ERROR);
}
} // namespace OHOS::NWeb

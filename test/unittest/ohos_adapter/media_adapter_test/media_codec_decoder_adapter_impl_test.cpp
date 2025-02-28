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

#include "gtest/gtest.h"
#include <gmock/gmock.h>
#define private public
#include "avcodec_video_decoder_impl.h"
#include "media_codec_decoder_adapter_impl.h"
#undef private
#include "avsharedmemory.h"
#include "avsharedmemorybase.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/window.h"
#include "native_window.h"

using namespace testing;
using namespace testing::ext;
using namespace std;
using testing::ext::TestSize;
using namespace OHOS::MediaAVCodec;

namespace OHOS {
namespace MediaAVCodec {
class AVCodecVideoDecoderImplMock : public AVCodecVideoDecoderImpl {
public:
    MOCK_METHOD0(Prepare, int32_t());
};
}
namespace NWeb {
class DecoderCallbackImplTest : public testing::Test {};

class DecoderCallbackAdapterMock : public DecoderCallbackAdapter {
public:
    DecoderCallbackAdapterMock() = default;

    ~DecoderCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(int32_t width, int32_t height, double frameRate) override {}

    void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) override {}

    void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag) override {}
};

class DecoderFormatAdapterMock : public DecoderFormatAdapter {
public:
    DecoderFormatAdapterMock() = default;

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    double GetFrameRate() override
    {
        return frameRate;
    }

    void SetWidth(int32_t w) override
    {
        width = w;
    }

    void SetHeight(int32_t h) override
    {
        height = h;
    }

    void SetFrameRate(double fr) override
    {
        frameRate = fr;
    }

    int32_t width;
    int32_t height;
    double frameRate;
};

class MediaCodecDecoderAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<DecoderFormatAdapterMock> format_ = nullptr;
    std::shared_ptr<MediaCodecDecoderAdapterImpl> mediaCodecDecoderAdapterImpl_ = nullptr;
};

void MediaCodecDecoderAdapterImplTest::SetUpTestCase(void) {}

void MediaCodecDecoderAdapterImplTest::TearDownTestCase(void) {}

void MediaCodecDecoderAdapterImplTest::SetUp()
{
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_, nullptr);
    mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    EXPECT_NE(mediaCodecDecoderAdapterImpl_, nullptr);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->decoder_, nullptr);
    EXPECT_EQ(format_, nullptr);
    format_ = std::make_unique<DecoderFormatAdapterMock>();
    EXPECT_NE(format_, nullptr);
    format_->width = 200;
    format_->height = 300;
    format_->frameRate = 20;
}

void MediaCodecDecoderAdapterImplTest::TearDown(void) {}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName() CreateVideoDecoderByName()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001, TestSize.Level1)
{
    std::string mimetype_ = "testmimeType";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_), DecoderAdapterCode::DECODER_ERROR);
    std::string name_ = "testname";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(name_), DecoderAdapterCode::DECODER_ERROR);
    mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_InvalidValueTest_002.
 * @tc.desc: test of InvalidValueScene in MediaCodecDecoderAdapterImpl
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_InvalidValueTest_002, TestSize.Level1)
{
    uint32_t index_ = 0;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(nullptr), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StartDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StopDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->FlushDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ResetDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_ERROR);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_NormalTest_003.
 * @tc.desc: test of NormalScene in MediaCodecDecoderAdapterImpl
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_NormalTest_003, TestSize.Level1)
{
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_ERROR);
    AVCodecVideoDecoderImplMock *mock = new AVCodecVideoDecoderImplMock();
    mediaCodecDecoderAdapterImpl_->decoder_.reset(mock);
    uint32_t index_ = 1;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(nullptr), DecoderAdapterCode::DECODER_ERROR);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_CALL(*mock, Prepare())
        .Times(1)
        .WillRepeatedly(::testing::Return(0));
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StartDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StopDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->FlushDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ResetDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_SetCallbackDec_004.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::SetCallbackDec
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetCallbackDec_004, TestSize.Level1)
{
    std::shared_ptr<DecoderCallbackAdapter> callback = nullptr;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
    callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_GetTypeOrFlag_005.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::GetErrorType() GetBufferFlag() GetAVBufferFlag()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_GetTypeOrFlag_005, TestSize.Level1)
{
    EXPECT_EQ(
        mediaCodecDecoderAdapterImpl_->GetErrorType(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_EXTEND_START),
        ErrorType::CODEC_ERROR_EXTEND_START);
    EXPECT_EQ(
        mediaCodecDecoderAdapterImpl_->GetBufferFlag(OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS),
        BufferFlag::CODEC_BUFFER_FLAG_EOS);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetAVBufferFlag(BufferFlag::CODEC_BUFFER_FLAG_EOS),
        OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS);
    AVCodecErrorType testAVCodecErrorType = static_cast<AVCodecErrorType>(100);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetErrorType(testAVCodecErrorType), ErrorType::CODEC_ERROR_INTERNAL);
    AVCodecBufferFlag testAVCodecBufferFlag = static_cast<AVCodecBufferFlag>(100);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetBufferFlag(testAVCodecBufferFlag), BufferFlag::CODEC_BUFFER_FLAG_NONE);
    BufferFlag testBufferFlag = static_cast<BufferFlag>(100);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetAVBufferFlag(testBufferFlag),
        AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE);
    std::string codecName = "video/avc";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(codecName), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_OnError_006.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::GetErrorType() GetBufferFlag() GetAVBufferFlag()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_OnError_006, TestSize.Level1)
{
    std::shared_ptr<DecoderCallbackAdapter> cb = nullptr;
    std::shared_ptr<DecoderCallbackImpl> decoderCallbackImpl = std::make_shared<DecoderCallbackImpl>(cb);
    EXPECT_NE(decoderCallbackImpl, nullptr);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    decoderCallbackImpl->cb_ = callback;
    decoderCallbackImpl->OnError(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_EXTEND_START, 1);
    MediaAVCodec::Format fomat;
    decoderCallbackImpl->OnOutputFormatChanged(fomat);
    decoderCallbackImpl->OnInputBufferAvailable(1, nullptr);
    std::shared_ptr<Media::AVSharedMemory> memory = std::make_shared<Media::AVSharedMemoryBase>(1, 1.0, "test");
    decoderCallbackImpl->OnInputBufferAvailable(1, memory);
    AVCodecBufferInfo info;
    decoderCallbackImpl->OnOutputBufferAvailable(1, info, AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS, nullptr);
}
}
} // namespace OHOS::NWeb

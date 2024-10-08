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
#define private public
#include "media_codec_decoder_adapter_impl.h"
#undef private


namespace OHOS::NWeb {
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

/**
 * @tc.name: DecoderCallbackImpl_NormalTest_001.
 * @tc.desc: test of DecoderCallbackImpl::OnError() OnOutputFormatChanged() OnInputBufferAvailable()
 * @tc.type: FUNC.
 */
TEST_F(DecoderCallbackImplTest, DecoderCallbackImpl_NormalTest_001)
{
    std::shared_ptr<DecoderCallbackAdapter> cb_ = std::make_shared<DecoderCallbackAdapterMock>();
    auto mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    std::string mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    mediaCodecDecoderAdapterImpl_->SetCallbackDec(cb_);
    const int32_t errorcode_ = 0;
    mediaCodecDecoderAdapterImpl_->OnError(errorcode_);
    OH_AVFormat* fomat = OH_AVFormat_Create();
    mediaCodecDecoderAdapterImpl_->OnOutputFormatChanged(fomat);
    uint32_t index_ = 1;
    OH_AVBuffer* buffer = OH_AVBuffer_Create(1);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(index_, buffer);
    mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(1, buffer);
    OH_AVFormat_Destroy(fomat);
    fomat = nullptr;
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;
}

class MediaCodecDecoderAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void TearDown();

protected:
    void SetUp() override;
    std::shared_ptr<DecoderFormatAdapterMock> format_ = nullptr;
    std::shared_ptr<MediaCodecDecoderAdapterImpl> mediaCodecDecoderAdapterImpl_ = nullptr;
};

void MediaCodecDecoderAdapterImplTest::SetUpTestCase(void) {}

void MediaCodecDecoderAdapterImplTest::TearDownTestCase(void) {}

void MediaCodecDecoderAdapterImplTest::SetUp()
{
    constexpr int32_t testWidth = 200;
    constexpr int32_t testHeight = 300;
    constexpr int32_t testFrameRate = 20;

    EXPECT_EQ(mediaCodecDecoderAdapterImpl_, nullptr);
    mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    EXPECT_NE(mediaCodecDecoderAdapterImpl_, nullptr);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->decoder_, nullptr);
    EXPECT_EQ(format_, nullptr);
    format_ = std::make_unique<DecoderFormatAdapterMock>();
    EXPECT_NE(format_, nullptr);
    format_->width = testWidth;
    format_->height = testHeight;
    format_->frameRate = testFrameRate;
}

void MediaCodecDecoderAdapterImplTest::TearDown(void) {}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName() CreateVideoDecoderByName()
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001)
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
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_InvalidValueTest_002)
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
 * @tc.name: MediaCodecDecoderAdapterImpl_SetCallbackDec_004.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::SetCallbackDec
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetCallbackDec_004)
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
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_GetTypeOrFlag_005)
{
    auto mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    std::string mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(
        mediaCodecDecoderAdapterImpl_->GetBufferFlag(OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS),
        BufferFlag::CODEC_BUFFER_FLAG_EOS);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetAVBufferFlag(BufferFlag::CODEC_BUFFER_FLAG_EOS),
        OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS);
    OH_AVCodecBufferFlags testAVCodecBufferFlag = static_cast<OH_AVCodecBufferFlags>(100);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetBufferFlag(testAVCodecBufferFlag), BufferFlag::CODEC_BUFFER_FLAG_NONE);
    BufferFlag testBufferFlag = static_cast<BufferFlag>(100);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetAVBufferFlag(testBufferFlag),
        OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE);
    std::string codecName = "OH.Media.Codec.Decoder.Video.AVC";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(codecName), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_OnError_006.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::GetErrorType() GetBufferFlag() GetAVBufferFlag()
 * @tc.type: FUNC.
 */
TEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_OnError_006)
{
    auto mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    std::string mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback);
    mediaCodecDecoderAdapterImpl_->OnError(1);
    OH_AVFormat* fomat = OH_AVFormat_Create();
    mediaCodecDecoderAdapterImpl_->OnOutputFormatChanged(fomat);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(1, nullptr);
    OH_AVBuffer* memory = OH_AVBuffer_Create(1);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(1, memory);
    mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(1, memory);
    OH_AVFormat_Destroy(fomat);
    fomat = nullptr;
    OH_AVBuffer_Destroy(memory);
    memory = nullptr;
}
} // namespace OHOS::NWeb

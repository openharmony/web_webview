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
#define private public
#include "media_codec_decoder_adapter_impl.h"
#include "avcodec_video_decoder_impl.h"
#undef private
#include "foundation/graphic/graphic_2d/interfaces/inner_api/surface/window.h"
#include "native_window.h"

using testing::ext::TestSize;
using namespace OHOS::MediaAVCodec;

namespace OHOS::NWeb {
class DecoderCallbackImplTest : public testing::Test {
};

class DecoderCallbackAdapterMock : public DecoderCallbackAdapter {
public:
    DecoderCallbackAdapterMock() = default;

    ~DecoderCallbackAdapterMock() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override {}

    void OnStreamChanged(const DecoderFormat &format) override {}

    void OnNeedInputData(uint32_t index, OhosBuffer buffer) override {}

    void OnNeedOutputData(uint32_t index, BufferInfo info, BufferFlag flag) override {}
};

/**
 * @tc.name: DecoderCallbackImpl_NormalTest_001.
 * @tc.desc: test of DecoderCallbackImpl::OnError() OnOutputFormatChanged() OnInputBufferAvailable()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DecoderCallbackImplTest, DecoderCallbackImpl_NormalTest_001, TestSize.Level1)
{
    std::shared_ptr<DecoderCallbackAdapter> cb_ = nullptr;
    std::shared_ptr<DecoderCallbackImpl> decoderCallbackImpl_ = std::make_shared<DecoderCallbackImpl>(cb_);
    const int32_t errorcode_ = 0;
    const AVCodecErrorType errorType_ = AVCODEC_ERROR_EXTEND_START;
    decoderCallbackImpl_->OnError(errorType_, errorcode_);
    MediaAVCodec::Format fomat_;
    decoderCallbackImpl_->OnOutputFormatChanged(fomat_);
    uint32_t index_ = 1;
    std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer_ = nullptr;
    decoderCallbackImpl_->OnInputBufferAvailable(index_, buffer_);
    AVCodecBufferInfo info;
    decoderCallbackImpl_->OnOutputBufferAvailable(1, info, AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS, nullptr);
}

class MediaCodecDecoderAdapterImplTest : public testing::Test {
protected:
    void SetUp() override;
    DecoderFormat format_ = {200, 300, 20};
    std::shared_ptr<MediaCodecDecoderAdapterImpl> mediaCodecDecoderAdapterImpl_ = nullptr;
};

void MediaCodecDecoderAdapterImplTest::SetUp()
{
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_, nullptr);
    mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    EXPECT_NE(mediaCodecDecoderAdapterImpl_, nullptr);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->decoder_, nullptr);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName() CreateVideoDecoderByName()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001, TestSize.Level1)
{
    const std::string mimetype_ = "testmimeType";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_), DecoderAdapterCode::DECODER_ERROR);
    const std::string name_ = "testname";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(name_), DecoderAdapterCode::DECODER_ERROR);
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
    BufferInfo buffer_;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, buffer_, BufferFlag::CODEC_BUFFER_FLAG_NONE),
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
    std::shared_ptr<DecoderCallbackAdapter> callback =  std::make_shared<DecoderCallbackAdapterMock>();
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
    mediaCodecDecoderAdapterImpl_->decoder_ = std::make_shared<MediaAVCodec::AVCodecVideoDecoderImpl>();
    uint32_t index_ = 1;
    BufferInfo buffer_;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, buffer_, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(nullptr), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StartDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StopDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->FlushDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ResetDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
    OHNativeWindow* nativeWindow = new OHNativeWindow();
    void* window = nativeWindow;
    mediaCodecDecoderAdapterImpl_->SetOutputSurface(window);
    delete nativeWindow;
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
    callback =  std::make_shared<DecoderCallbackAdapterMock>();
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
    std::shared_ptr<DecoderCallbackAdapter> callback =  std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    decoderCallbackImpl->cb_ = callback;
    decoderCallbackImpl->OnError(OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_EXTEND_START, 1);
    MediaAVCodec::Format fomat;
    decoderCallbackImpl->OnOutputFormatChanged(fomat);
    decoderCallbackImpl->OnInputBufferAvailable(1, nullptr);
    AVCodecBufferInfo info;
    decoderCallbackImpl->OnOutputBufferAvailable(1, info, AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS, nullptr);
}
} // namespace OHOS::NWeb

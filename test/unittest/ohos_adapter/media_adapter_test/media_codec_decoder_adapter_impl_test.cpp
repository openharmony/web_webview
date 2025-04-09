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
#include "audio_cenc_info_adapter_impl.h"
#undef private
#include "avsharedmemory.h"
#include "avsharedmemorybase.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/window.h"
#include "native_window.h"
#include "native_avformat.h"
#include "native_avbuffer.h"
#include "native_avbuffer_info.h"
#include "native_drm_base.h"
#include "native_mediakeysystem.h"

using namespace testing;
using namespace testing::ext;
using namespace std;
using testing::ext::TestSize;
using namespace OHOS::MediaAVCodec;

static const std::string GetKeySystemName()
{
    if (OH_MediaKeySystem_IsSupported("com.clearplay.drm")) {
        return "com.clearplay.drm";
    } else if (OH_MediaKeySystem_IsSupported("com.wiseplay.drm")) {
        return "com.wiseplay.drm";
    } else {
        return "ERROR";
    }
}

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
    void SetCencInfoAboutKeyIdIvAlgo(std::shared_ptr<AudioCencInfoAdapter> cencInfo);
    void SetCencInfoAboutClearHeaderAndPayLoadLens(std::shared_ptr<AudioCencInfoAdapter> cencInfo);

protected:
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
    EXPECT_EQ(format_, nullptr);
    format_ = std::make_unique<DecoderFormatAdapterMock>();
    EXPECT_NE(format_, nullptr);
    format_->width = testWidth;
    format_->height = testHeight;
    format_->frameRate = testFrameRate;
}

void MediaCodecDecoderAdapterImplTest::TearDown(void) {}

void MediaCodecDecoderAdapterImplTest::SetCencInfoAboutKeyIdIvAlgo(
    std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    uint8_t keyId[] = {
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
    uint8_t iv[] = {
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
    cencInfo->SetAlgo(0);
    cencInfo->SetKeyId(keyId);
    cencInfo->SetKeyIdLen(DRM_KEY_ID_SIZE);
    cencInfo->SetIv(iv);
    cencInfo->SetIvLen(DRM_KEY_ID_SIZE);
}

void MediaCodecDecoderAdapterImplTest::SetCencInfoAboutClearHeaderAndPayLoadLens(
    std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    const std::vector<uint32_t> clearHeaderLens = {1, 2, 3};
    const std::vector<uint32_t> payLoadLens = {4, 5, 6};
    const uint32_t ERR_BLOCK_COUNT = 10000;
    cencInfo->SetClearHeaderLens(clearHeaderLens);
    cencInfo->SetPayLoadLens(payLoadLens);
    cencInfo->SetEncryptedBlockCount(ERR_BLOCK_COUNT);
    cencInfo->SetSkippedBlockCount(0);
    cencInfo->SetFirstEncryptedOffset(0);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName() CreateVideoDecoderByName()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_CreateVideoDecoderByName_001, TestSize.Level1)
{
    // create error decoder
    const std::string errorMimetype = "testmimeType";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(
	    errorMimetype), DecoderAdapterCode::DECODER_ERROR);
    const std::string errorName = "testname";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(errorName), DecoderAdapterCode::DECODER_ERROR);

    // create normal decoder
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_OK);

    // drm dncrypt decoder
    std::string systemName = GetKeySystemName();
    bool isSupported = OH_MediaKeySystem_IsSupported2(systemName.c_str(), validMimetype.c_str());
    if (isSupported) {
        MediaKeySession session;
        mediaCodecDecoderAdapterImpl_->SetDecryptionConfig(static_cast<void*>(&session), true);
        ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
        EXPECT_EQ(ret, DecoderAdapterCode::DECODER_ERROR);
        EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_ERROR);
        mediaCodecDecoderAdapterImpl_->SetDecryptionConfig(static_cast<void*>(&session), false);
    }
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_InvalidValueTest_002.
 * @tc.desc: test of InvalidValueScene in MediaCodecDecoderAdapterImpl
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_InvalidValueTest_002, TestSize.Level1)
{
    // not create decoder and test function
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
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetDecryptionConfig(nullptr, false), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, nullptr), DecoderAdapterCode::DECODER_ERROR);

    // create decoder and test function in error case
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(nullptr), DecoderAdapterCode::DECODER_ERROR);
    constexpr int32_t errWidth = 10000;
    format_->width = errWidth;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(nullptr), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(nullptr), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetOutputSurface(nullptr), DecoderAdapterCode::DECODER_ERROR);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_NormalTest_003.
 * @tc.desc: test of NormalScene in MediaCodecDecoderAdapterImpl
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_NormalTest_003, TestSize.Level1)
{
    uint32_t index_ = 0;
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->PrepareDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StartDecoder(), DecoderAdapterCode::DECODER_OK);

    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(index_, buffer);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->QueueInputBufferDec(index_, 0, 0, 0, BufferFlag::CODEC_BUFFER_FLAG_NONE),
        DecoderAdapterCode::DECODER_ERROR);
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;

    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->GetOutputFormatDec(format_), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->FlushDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetParameterDecoder(format_), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->StopDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ResetDecoder(), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, true), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseOutputBufferDec(index_, false), DecoderAdapterCode::DECODER_ERROR);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_SetCallbackDec_004.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::SetCallbackDec
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetCallbackDec_004, TestSize.Level1)
{
    const std::string validMimetype = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(validMimetype);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<DecoderCallbackAdapter> callback = nullptr;
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_ERROR);
    callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ReleaseDecoder(), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_GetTypeOrFlag_005.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::GetBufferFlag() GetAVBufferFlag()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_GetTypeOrFlag_005, TestSize.Level1)
{
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
	const std::string codecName = "video/avc";
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByName(codecName), DecoderAdapterCode::DECODER_OK);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->ConfigureDecoder(format_), DecoderAdapterCode::DECODER_OK);
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_OnError_006.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::OnError() OnOutputFormatChanged() OnInputBufferAvailable()
 * OnOutputBufferAvailable()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_OnError_006, TestSize.Level1)
{
    auto mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    std::string mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);
    std::shared_ptr<DecoderCallbackAdapter> callback = std::make_shared<DecoderCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    mediaCodecDecoderAdapterImpl_->SetCallbackDec(callback);

    mediaCodecDecoderAdapterImpl_->OnError(100);
    mediaCodecDecoderAdapterImpl_->OnOutputFormatChanged(nullptr);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(1, nullptr);
    mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(1, nullptr);

    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVFormat* codecFormat = OH_AVFormat_Create();
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnError(1);
    mediaCodecDecoderAdapterImpl_->OnOutputFormatChanged(codecFormat);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(1, buffer);
    mediaCodecDecoderAdapterImpl_->OnOutputBufferAvailable(1, buffer);
    OH_AVFormat_Destroy(codecFormat);
    OH_AVBuffer_Destroy(buffer);
    codecFormat = nullptr;
    buffer = nullptr;
}

/**
 * @tc.name: MediaCodecDecoderAdapterImpl_SetAVCencInfo_007.
 * @tc.desc: test of MediaCodecDecoderAdapterImpl::SetAVCencInfo()
 * OnOutputBufferAvailable()
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(MediaCodecDecoderAdapterImplTest, MediaCodecDecoderAdapterImpl_SetAVCencInfo_007, TestSize.Level1)
{
    auto mediaCodecDecoderAdapterImpl_ = std::make_unique<MediaCodecDecoderAdapterImpl>();
    std::string mimetype_ = "video/avc";
    DecoderAdapterCode ret = mediaCodecDecoderAdapterImpl_->CreateVideoDecoderByMime(mimetype_);
    EXPECT_EQ(ret, DecoderAdapterCode::DECODER_OK);

    std::shared_ptr<AudioCencInfoAdapter> cencInfo = std::make_shared<AudioCencInfoAdapterImpl>();
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);

    const uint32_t ERR_ALGO = 10000;
    cencInfo->SetAlgo(ERR_ALGO);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    cencInfo->SetAlgo(0);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    SetCencInfoAboutKeyIdIvAlgo(cencInfo);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    SetCencInfoAboutClearHeaderAndPayLoadLens(cencInfo);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    cencInfo->SetEncryptedBlockCount(0);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    const uint32_t ERR_MODE = 10000;
    cencInfo->SetMode(ERR_MODE);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    cencInfo->SetMode(0);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    constexpr int32_t MEMSIZE = 1024 * 1024;
    OH_AVBuffer* buffer = OH_AVBuffer_Create(MEMSIZE);
    mediaCodecDecoderAdapterImpl_->OnInputBufferAvailable(0, buffer);
    EXPECT_EQ(mediaCodecDecoderAdapterImpl_->SetAVCencInfo(0, cencInfo), DecoderAdapterCode::DECODER_ERROR);
    OH_AVBuffer_Destroy(buffer);
    buffer = nullptr;
}
}
} // namespace OHOS::NWeb

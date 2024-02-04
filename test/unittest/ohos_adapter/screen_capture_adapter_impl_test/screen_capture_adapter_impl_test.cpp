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

#include <cstring>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <securec.h>
#include <thread>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "nweb_adapter_helper.h"
#include "token_setproc.h"
#include "screen_capture.h"
#include "foundation/graphic/graphic_surface/surface/include/surface_buffer_impl.h"

#define private public
#include "screen_capture_adapter_impl.h"
#undef private
#include "foundation/multimedia/player_framework/interfaces/inner_api/native/media_errors.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Media;

namespace OHOS {
namespace Media {
class ScreenCaptureImplMock : public ScreenCapture {
public:
    MOCK_METHOD0(Init, int32_t());
    MOCK_METHOD1(Init, int32_t(AVScreenCaptureConfig));
    MOCK_METHOD1(SetMicrophoneEnabled, int32_t(bool));
    MOCK_METHOD0(StartScreenCapture, int32_t());
    MOCK_METHOD0(StopScreenCapture, int32_t());
    MOCK_METHOD2(AcquireAudioBuffer, int32_t(std::shared_ptr<AudioBuffer> &, AudioCaptureSourceType));
    MOCK_METHOD3(AcquireVideoBuffer, sptr<OHOS::SurfaceBuffer>(int32_t &, int64_t &, OHOS::Rect &));
    MOCK_METHOD1(ReleaseAudioBuffer, int32_t(AudioCaptureSourceType));
    MOCK_METHOD0(ReleaseVideoBuffer, int32_t());
    MOCK_METHOD0(Release, int32_t());
    MOCK_METHOD1(SetScreenCaptureCallback, int32_t(const std::shared_ptr<ScreenCaptureCallBack> &));
};
} // namespace Media
namespace NWeb {

namespace {
constexpr int32_t AUDIO_SAMPLE_RATE = 16000;
constexpr int32_t AUDIO_CHANNELS = 2;
constexpr int32_t SCREEN_WIDTH = 1080;
constexpr int32_t SCREEN_HEIGHT = 720;
std::shared_ptr<ScreenCaptureAdapterImpl> g_screenCapture = nullptr;

class ScreenCaptureCallbackAdapterTest : public ScreenCaptureCallbackAdapter {
public:
    ScreenCaptureCallbackAdapterTest() = default;
    ~ScreenCaptureCallbackAdapterTest() override = default;

    void OnError(int32_t errorCode) override
    {
        (void)errorCode;
    }

    void OnAudioBufferAvailable(bool isReady, AudioCaptureSourceTypeAdapter type) override
    {
        (void)isReady;
        (void)type;
    }

    void OnVideoBufferAvailable(bool isReady) override
    {
        if (!isReady || !g_screenCapture) {
            return;
        }
        std::unique_ptr<SurfaceBufferAdapter> buffer = g_screenCapture->AcquireVideoBuffer();
        if (buffer) {
            g_screenCapture->ReleaseVideoBuffer();
        }
    }
};
}

class ScreenCaptureAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ScreenCaptureAdapterImplTest::SetUpTestCase(void)
{
    // set native token
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    perms[1] = "ohos.permission.MICROPHONE";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "web_screen_capture_tdd",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    
    g_screenCapture = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(g_screenCapture, nullptr);
    ScreenCaptureConfigAdapter config;
    config.captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    config.dataType = DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.micCapInfo.audioChannels = AUDIO_CHANNELS;
    config.audioInfo.innerCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.innerCapInfo.audioChannels = AUDIO_CHANNELS;
    config.videoInfo.videoCapInfo.videoFrameWidth = SCREEN_WIDTH;
    config.videoInfo.videoCapInfo.videoFrameHeight = SCREEN_HEIGHT;
    int32_t result = g_screenCapture->Init(config);
    EXPECT_EQ(result, 0);
}

void ScreenCaptureAdapterImplTest::TearDownTestCase(void)
{}

void ScreenCaptureAdapterImplTest::SetUp(void)
{}

void ScreenCaptureAdapterImplTest::TearDown(void)
{}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_Init_001
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_Init_001, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    ScreenCaptureConfigAdapter config;
    config.captureMode = CaptureModeAdapter::CAPTURE_INVAILD;
    config.dataType = DataTypeAdapter::INVAILD_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.micCapInfo.audioChannels = AUDIO_CHANNELS;
    config.audioInfo.innerCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.innerCapInfo.audioChannels = AUDIO_CHANNELS;
    config.videoInfo.videoCapInfo.videoFrameWidth = SCREEN_WIDTH;
    config.videoInfo.videoCapInfo.videoFrameHeight = SCREEN_HEIGHT;
    config.audioInfo.micCapInfo.audioSource = AudioCaptureSourceTypeAdapter::SOURCE_INVALID;
    config.audioInfo.audioEncInfo.audioCodecformat = AudioCodecFormatAdapter::AUDIO_DEFAULT;
    config.videoInfo.videoCapInfo.videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_YUV;
    config.videoInfo.videoEncInfo.videoCodec = VideoCodecFormatAdapter::VIDEO_DEFAULT;
    int32_t result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    config.dataType = DataTypeAdapter::ENCODED_STREAM_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSource = AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
    config.audioInfo.audioEncInfo.audioCodecformat = AudioCodecFormatAdapter::AAC_LC;
    config.videoInfo.videoCapInfo.videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_ES;
    config.videoInfo.videoEncInfo.videoCodec = VideoCodecFormatAdapter::H264;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.captureMode = CaptureModeAdapter::CAPTURE_SPECIFIED_SCREEN;
    config.dataType = DataTypeAdapter::CAPTURE_FILE_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSource =  AudioCaptureSourceTypeAdapter::MIC;
    config.audioInfo.audioEncInfo.audioCodecformat = AudioCodecFormatAdapter::AUDIO_CODEC_FORMAT_BUTT;
    config.videoInfo.videoCapInfo.videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA;
    config.videoInfo.videoEncInfo.videoCodec = VideoCodecFormatAdapter::H265;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.captureMode = CaptureModeAdapter::CAPTURE_SPECIFIED_WINDOW;
    config.dataType = DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSource = AudioCaptureSourceTypeAdapter::ALL_PLAYBACK;
    config.videoInfo.videoCapInfo.videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_BUTT;
    config.videoInfo.videoEncInfo.videoCodec = VideoCodecFormatAdapter::MPEG4;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.audioInfo.micCapInfo.audioSource = AudioCaptureSourceTypeAdapter::APP_PLAYBACK;
    config.videoInfo.videoEncInfo.videoCodec = VideoCodecFormatAdapter::VP8;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.videoInfo.videoEncInfo.videoCodec = VideoCodecFormatAdapter::VP9;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_SetMicrophoneEnable_002
 * @tc.desc: SetMicrophoneEnable.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_SetMicrophoneEnable_002, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    int32_t result = adapterImpl->SetMicrophoneEnable(false);
    EXPECT_EQ(result, -1);
    result = g_screenCapture->SetMicrophoneEnable(false);
    EXPECT_EQ(result, 0);
    result = g_screenCapture->SetMicrophoneEnable(true);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_AcquireVideoBuffer_003
 * @tc.desc: AcquireVideoBuffer.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_AcquireVideoBuffer_003, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    std::unique_ptr<SurfaceBufferAdapter> buffer = adapterImpl->AcquireVideoBuffer();
    EXPECT_EQ(buffer, nullptr);
    int32_t result = adapterImpl->ReleaseVideoBuffer();
    EXPECT_EQ(result, -1);
    g_screenCapture->AcquireVideoBuffer();
    g_screenCapture->ReleaseVideoBuffer();
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_Capture_004
 * @tc.desc: Capture.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_Capture_004, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    int32_t result = adapterImpl->StartCapture();
    EXPECT_EQ(result, -1);
    result = adapterImpl->StopCapture();
    EXPECT_EQ(result, -1);
    ScreenCaptureImplMock *mock = new ScreenCaptureImplMock();
    EXPECT_NE(mock, nullptr);
    adapterImpl->screenCapture_.reset(mock);
    EXPECT_CALL(*mock, StartScreenCapture())
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_OK));
    EXPECT_CALL(*mock, StopScreenCapture())
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_OK));
    EXPECT_CALL(*mock, SetMicrophoneEnabled(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_NO_MEMORY));
    result = adapterImpl->StartCapture();
    EXPECT_EQ(result, 0);
    adapterImpl->SetMicrophoneEnable(true);
    result = adapterImpl->StopCapture();
    EXPECT_EQ(result, 0);
    EXPECT_CALL(*mock, StartScreenCapture())
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_NO_MEMORY));
    EXPECT_CALL(*mock, StopScreenCapture())
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_NO_MEMORY));
    result = adapterImpl->StartCapture();
    EXPECT_EQ(result, -1);
    result = adapterImpl->StopCapture();
    EXPECT_EQ(result, -1);
    auto callbackAdapter = std::make_shared<ScreenCaptureCallbackAdapterTest>();
    EXPECT_NE(callbackAdapter, nullptr);
    result = g_screenCapture->SetCaptureCallback(callbackAdapter);
    EXPECT_EQ(result, 0);
    EXPECT_CALL(*mock, SetScreenCaptureCallback(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_NO_MEMORY));
    adapterImpl->SetCaptureCallback(callbackAdapter);
    result = g_screenCapture->SetCaptureCallback(nullptr);
    EXPECT_EQ(result, -1);
    g_screenCapture->screenCapture_ = nullptr;
    result = g_screenCapture->SetCaptureCallback(callbackAdapter);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_OHScreenCaptureCallback_005
 * @tc.desc: OHScreenCaptureCallback.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_OHScreenCaptureCallback_005, TestSize.Level1)
{
    auto callbackMock = std::make_shared<ScreenCaptureCallbackAdapterTest>();
    EXPECT_NE(callbackMock, nullptr);
    auto callback = std::make_shared<OHScreenCaptureCallback>(callbackMock);
    EXPECT_NE(callback, nullptr);
    EXPECT_NE(callback->callback_, nullptr);
    callback->OnError(ScreenCaptureErrorType::SCREEN_CAPTURE_ERROR_EXTEND_START, 0);
    callback->OnAudioBufferAvailable(true, AudioCaptureSourceType::SOURCE_DEFAULT);
    callback->OnAudioBufferAvailable(true, AudioCaptureSourceType::MIC);
    callback->OnAudioBufferAvailable(true, AudioCaptureSourceType::ALL_PLAYBACK);
    callback->OnAudioBufferAvailable(true, AudioCaptureSourceType::APP_PLAYBACK);
    callback->OnAudioBufferAvailable(true, AudioCaptureSourceType::SOURCE_INVALID);
    callback->OnVideoBufferAvailable(true);
    callback->callback_ = nullptr;
    callback->OnError(ScreenCaptureErrorType::SCREEN_CAPTURE_ERROR_EXTEND_START, 0);
    callback->OnAudioBufferAvailable(true, AudioCaptureSourceType::SOURCE_DEFAULT);
    callback->OnVideoBufferAvailable(true);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_Init_006
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_Init_006, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    ScreenCaptureConfigAdapter config;
    config.captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    config.dataType = DataTypeAdapter::ENCODED_STREAM_DATA_TYPE;
    int32_t result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.dataType = DataTypeAdapter::CAPTURE_FILE_DATA_TYPE;
    config.audioInfo.micCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.micCapInfo.audioChannels = AUDIO_CHANNELS;
    config.audioInfo.innerCapInfo.audioSampleRate = AUDIO_SAMPLE_RATE;
    config.audioInfo.innerCapInfo.audioChannels = AUDIO_CHANNELS;
    config.videoInfo.videoCapInfo.videoFrameWidth = SCREEN_WIDTH;
    config.videoInfo.videoCapInfo.videoFrameHeight = SCREEN_HEIGHT;
    config.videoInfo.videoEncInfo.videoCodec = VideoCodecFormatAdapter::VIDEO_CODEC_FORMAT_BUTT;
    config.recorderInfo.fileFormat = ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    config.recorderInfo.fileFormat = ContainerFormatTypeAdapter::CFT_MPEG_4_TYPE;
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, -1);
    adapterImpl->screenCapture_ = OHOS::Media::ScreenCaptureFactory::CreateScreenCapture();
    result = adapterImpl->Init(config);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_AcquireVideoBuffer_007
 * @tc.desc: AcquireVideoBuffer.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_AcquireVideoBuffer_007, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    ScreenCaptureImplMock *mock = new ScreenCaptureImplMock();
    EXPECT_NE(mock, nullptr);
    sptr<OHOS::SurfaceBuffer> surfacebuffer = new SurfaceBufferImpl(0);
    EXPECT_NE(surfacebuffer, nullptr);
    adapterImpl->screenCapture_.reset(mock);
    EXPECT_CALL(*mock, AcquireVideoBuffer(::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(surfacebuffer));

    EXPECT_CALL(*mock, ReleaseVideoBuffer())
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_OK));
    std::unique_ptr<SurfaceBufferAdapter> buffer = adapterImpl->AcquireVideoBuffer();
    EXPECT_NE(buffer, nullptr);
    int32_t result = adapterImpl->ReleaseVideoBuffer();
    EXPECT_EQ(result, 0);
    adapterImpl->screenCapture_.reset();
}
}
} // namespace NWeb
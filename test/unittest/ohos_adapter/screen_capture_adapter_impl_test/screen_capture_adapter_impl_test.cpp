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

#include <cstdint>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <securec.h>
#include <thread>

#include "accesstoken_kit.h"
#include "foundation/graphic/graphic_surface/surface/include/surface_buffer_impl.h"
#include "nativetoken_kit.h"
#include "nweb_adapter_helper.h"
#include "screen_capture.h"
#include "token_setproc.h"

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
    MOCK_METHOD1(Init, int32_t(AVScreenCaptureConfig));
    MOCK_METHOD1(Init, int32_t(OHOS::AudioStandard::AppInfo&));
    MOCK_METHOD1(SetMicrophoneEnabled, int32_t(bool));
    MOCK_METHOD1(SetCanvasRotation, int32_t(bool));
	MOCK_METHOD1(ShowCursor, int32_t(bool));
    MOCK_METHOD2(ResizeCanvas, int32_t(int32_t, int32_t));
    MOCK_METHOD1(SkipPrivacyMode, int32_t(std::vector<uint64_t>&));
    MOCK_METHOD1(SetMaxVideoFrameRate, int32_t(int32_t));
    MOCK_METHOD0(StartScreenCapture, int32_t());
    MOCK_METHOD1(StartScreenCaptureWithSurface, int32_t(sptr<Surface>));
    MOCK_METHOD0(StopScreenCapture, int32_t());
    MOCK_METHOD0(StartScreenRecording, int32_t());
    MOCK_METHOD0(StopScreenRecording, int32_t());
    MOCK_METHOD2(AcquireAudioBuffer, int32_t(std::shared_ptr<AudioBuffer>&, AudioCaptureSourceType));
    MOCK_METHOD3(AcquireVideoBuffer, sptr<OHOS::SurfaceBuffer>(int32_t&, int64_t&, OHOS::Rect&));
    MOCK_METHOD1(ReleaseAudioBuffer, int32_t(AudioCaptureSourceType));
    MOCK_METHOD0(ReleaseVideoBuffer, int32_t());
    MOCK_METHOD0(Release, int32_t());
    MOCK_METHOD1(SetScreenCaptureCallback, int32_t(const std::shared_ptr<ScreenCaptureCallBack>&));
    MOCK_METHOD1(ExcludeContent, int32_t(ScreenCaptureContentFilter&));
    MOCK_METHOD0(SetPrivacyAuthorityEnabled, int32_t());
    MOCK_METHOD1(SetScreenCaptureStrategy, int32_t(ScreenCaptureStrategy));
};
} // namespace Media
namespace NWeb {

class MockAudioCaptureInfoAdapter : public AudioCaptureInfoAdapter {
public:
    MockAudioCaptureInfoAdapter() = default;

    int32_t GetAudioSampleRate() override
    {
        return audioSampleRate;
    }

    int32_t GetAudioChannels() override
    {
        return audioChannels;
    }

    AudioCaptureSourceTypeAdapter GetAudioSource() override
    {
        return audioSource;
    }

    int32_t audioSampleRate;
    int32_t audioChannels;
    AudioCaptureSourceTypeAdapter audioSource = AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
};

class MockAudioEncInfoAdapter : public AudioEncInfoAdapter {
public:
    MockAudioEncInfoAdapter() = default;

    int32_t GetAudioBitrate() override
    {
        return audioBitrate;
    }

    AudioCodecFormatAdapter GetAudioCodecformat() override
    {
        return audioCodecformat;
    }

    int32_t audioBitrate = 0;
    AudioCodecFormatAdapter audioCodecformat = AudioCodecFormatAdapter::AUDIO_DEFAULT;
};

class MockAudioInfoAdapter : public AudioInfoAdapter {
public:
    MockAudioInfoAdapter() = default;

    std::shared_ptr<AudioCaptureInfoAdapter> GetMicCapInfo() override
    {
        return micCapInfo;
    }

    std::shared_ptr<AudioCaptureInfoAdapter> GetInnerCapInfo() override
    {
        return innerCapInfo;
    }

    std::shared_ptr<AudioEncInfoAdapter> GetAudioEncInfo() override
    {
        return audioEncInfo;
    }

    std::shared_ptr<MockAudioCaptureInfoAdapter> micCapInfo = nullptr;
    std::shared_ptr<MockAudioCaptureInfoAdapter> innerCapInfo = nullptr;
    std::shared_ptr<MockAudioEncInfoAdapter> audioEncInfo = nullptr;
};

class MockVideoCaptureInfoAdapter : public VideoCaptureInfoAdapter {
public:
    MockVideoCaptureInfoAdapter() = default;

    uint64_t GetDisplayId() override
    {
        return displayId;
    }

    std::list<int32_t> GetTaskIDs() override
    {
        return taskIDs;
    }

    int32_t GetVideoFrameWidth() override
    {
        return videoFrameWidth;
    }

    int32_t GetVideoFrameHeight() override
    {
        return videoFrameHeight;
    }

    VideoSourceTypeAdapter GetVideoSourceType() override
    {
        return videoSource;
    }

    uint64_t displayId = 0;
    std::list<int32_t> taskIDs;
    int32_t videoFrameWidth = 0;
    int32_t videoFrameHeight = 0;
    VideoSourceTypeAdapter videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA;
};

class MockVideoEncInfoAdapter : public VideoEncInfoAdapter {
public:
    MockVideoEncInfoAdapter() = default;

    VideoCodecFormatAdapter GetVideoCodecFormat() override
    {
        return videoCodec;
    }

    int32_t GetVideoBitrate() override
    {
        return videoBitrate;
    }

    int32_t GetVideoFrameRate() override
    {
        return videoFrameRate;
    }

    VideoCodecFormatAdapter videoCodec = VideoCodecFormatAdapter::VIDEO_DEFAULT;
    int32_t videoBitrate = 0;
    int32_t videoFrameRate = 0;
};

class MockVideoInfoAdapter : public VideoInfoAdapter {
public:
    MockVideoInfoAdapter() = default;

    std::shared_ptr<VideoCaptureInfoAdapter> GetVideoCapInfo()
    {
        return videoCapInfo;
    }

    std::shared_ptr<VideoEncInfoAdapter> GetVideoEncInfo()
    {
        return videoEncInfo;
    }

    std::shared_ptr<MockVideoCaptureInfoAdapter> videoCapInfo = nullptr;
    std::shared_ptr<MockVideoEncInfoAdapter> videoEncInfo = nullptr;
};

class MockRecorderInfoAdapter : public RecorderInfoAdapter {
public:
    MockRecorderInfoAdapter() = default;

    std::string GetUrl() override
    {
        return url;
    }

    ContainerFormatTypeAdapter GetFileFormat() override
    {
        return fileFormat;
    }

    std::string url = "";
    ContainerFormatTypeAdapter fileFormat = ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE;
};

class MockScreenCaptureConfigAdapter : public ScreenCaptureConfigAdapter {
public:
    MockScreenCaptureConfigAdapter() = default;

    CaptureModeAdapter GetCaptureMode() override
    {
        return captureMode;
    }

    DataTypeAdapter GetDataType() override
    {
        return dataType;
    }

    std::shared_ptr<AudioInfoAdapter> GetAudioInfo()
    {
        return audioInfo;
    }

    std::shared_ptr<VideoInfoAdapter> GetVideoInfo()
    {
        return videoInfo;
    }

    std::shared_ptr<RecorderInfoAdapter> GetRecorderInfo()
    {
        return recorderInfo;
    }

    CaptureModeAdapter captureMode = CaptureModeAdapter::CAPTURE_INVAILD;
    DataTypeAdapter dataType = DataTypeAdapter::INVAILD_DATA_TYPE;
    std::shared_ptr<MockAudioInfoAdapter> audioInfo = nullptr;
    std::shared_ptr<MockVideoInfoAdapter> videoInfo = nullptr;
    std::shared_ptr<MockRecorderInfoAdapter> recorderInfo = nullptr;
};

class MockAudioBufferAdapter  : public AudioBufferAdapter  {
public:
    MockAudioBufferAdapter() = default;

    uint8_t* GetBuffer() override
    {
        return m_buffer;
    }

    int32_t GetLength() override
    {
        return m_length;
    }

    int64_t GetTimestamp() override
    {
        return m_timestamp;
    }

    AudioCaptureSourceTypeAdapter GetSourcetype() override
    {
        return m_sourcetype;
    }

    void SetBuffer(uint8_t* buffer) override
    {
        m_buffer = buffer;
    }

    void SetLength(int32_t length) override
    {
        m_length = length;
    }

    void SetTimestamp(int64_t timestamp) override
    {
        m_timestamp = timestamp;
    }

    void SetSourcetype(AudioCaptureSourceTypeAdapter sourcetype) override
    {
        m_sourcetype = sourcetype;
    }

    uint8_t* m_buffer = nullptr;
    int32_t m_length = 0;
    int64_t m_timestamp = 0;
    AudioCaptureSourceTypeAdapter m_sourcetype = AudioCaptureSourceTypeAdapter::SOURCE_INVALID;
};

ScreenCaptureStateCodeAdapter GetScreenCaptureStateCodeAdapter(const OHOS::Media::AVScreenCaptureStateCode& stateCode)
{
    switch (stateCode) {
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STARTED:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STARTED;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_CANCELED:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STOPPED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_USER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE;
        case OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE;
        default:
            return ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;
    }
}

OHOS::Media::AudioCaptureSourceType ConvertAudioCaptureSourceType(const AudioCaptureSourceTypeAdapter& type)
{
    switch (type) {
        case AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT:
            return OHOS::Media::AudioCaptureSourceType::SOURCE_DEFAULT;
        case AudioCaptureSourceTypeAdapter::MIC:
            return OHOS::Media::AudioCaptureSourceType::MIC;
        case AudioCaptureSourceTypeAdapter::ALL_PLAYBACK:
            return OHOS::Media::AudioCaptureSourceType::ALL_PLAYBACK;
        case AudioCaptureSourceTypeAdapter::APP_PLAYBACK:
            return OHOS::Media::AudioCaptureSourceType::APP_PLAYBACK;
        default:
            return OHOS::Media::AudioCaptureSourceType::SOURCE_INVALID;
    }
}

namespace {
constexpr int32_t AUDIO_SAMPLE_RATE = 16000;
constexpr int32_t AUDIO_CHANNELS = 2;
constexpr int32_t SCREEN_WIDTH = 1080;
constexpr int32_t SCREEN_HEIGHT = 720;
std::shared_ptr<ScreenCaptureAdapterImpl> g_screenCapture = nullptr;
std::shared_ptr<MockAudioCaptureInfoAdapter> g_micCapInfo = nullptr;
std::shared_ptr<MockAudioCaptureInfoAdapter> g_innerCapInfo = nullptr;
std::shared_ptr<MockAudioEncInfoAdapter> g_audioEncInfo = nullptr;
std::shared_ptr<MockAudioInfoAdapter> g_audioInfo = nullptr;
std::shared_ptr<MockVideoCaptureInfoAdapter> g_videoCaptureInfo = nullptr;
std::shared_ptr<MockVideoEncInfoAdapter> g_videoEncInfo = nullptr;
std::shared_ptr<MockVideoInfoAdapter> g_videoInfo = nullptr;
std::shared_ptr<MockRecorderInfoAdapter> g_recorderInfo = nullptr;
std::shared_ptr<MockScreenCaptureConfigAdapter> g_screenCaptureConfig = nullptr;
std::shared_ptr<MockAudioBufferAdapter> g_audioBufferAdapter = nullptr;

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
        std::shared_ptr<SurfaceBufferAdapter> buffer = g_screenCapture->AcquireVideoBuffer();
        if (buffer) {
            g_screenCapture->ReleaseVideoBuffer();
        }
    }
};
} // namespace

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
    const char* perms[2];
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
    g_micCapInfo = std::make_shared<MockAudioCaptureInfoAdapter>();
    g_innerCapInfo = std::make_shared<MockAudioCaptureInfoAdapter>();
    g_audioEncInfo = std::make_shared<MockAudioEncInfoAdapter>();
    g_audioInfo = std::make_shared<MockAudioInfoAdapter>();
    g_videoCaptureInfo = std::make_shared<MockVideoCaptureInfoAdapter>();
    g_videoEncInfo = std::make_shared<MockVideoEncInfoAdapter>();
    g_videoInfo = std::make_shared<MockVideoInfoAdapter>();
    g_recorderInfo = std::make_shared<MockRecorderInfoAdapter>();
    g_screenCaptureConfig = std::make_shared<MockScreenCaptureConfigAdapter>();
    g_audioBufferAdapter = std::make_shared<MockAudioBufferAdapter>();

    EXPECT_NE(g_screenCapture, nullptr);
    EXPECT_NE(g_micCapInfo, nullptr);
    EXPECT_NE(g_innerCapInfo, nullptr);
    EXPECT_NE(g_audioEncInfo, nullptr);
    EXPECT_NE(g_audioInfo, nullptr);
    EXPECT_NE(g_videoCaptureInfo, nullptr);
    EXPECT_NE(g_videoEncInfo, nullptr);
    EXPECT_NE(g_videoInfo, nullptr);
    EXPECT_NE(g_recorderInfo, nullptr);
    EXPECT_NE(g_screenCaptureConfig, nullptr);
    EXPECT_NE(g_audioBufferAdapter, nullptr);
    int32_t result = g_screenCapture->Init(nullptr);
    EXPECT_EQ(result, -1);

    g_screenCaptureConfig->captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    g_screenCaptureConfig->dataType = DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE;
    g_micCapInfo->audioSampleRate = AUDIO_SAMPLE_RATE;
    g_micCapInfo->audioChannels = AUDIO_CHANNELS;
    g_innerCapInfo->audioSampleRate = AUDIO_SAMPLE_RATE;
    g_innerCapInfo->audioChannels = AUDIO_CHANNELS;
    g_audioInfo->micCapInfo = g_micCapInfo;
    g_audioInfo->innerCapInfo = g_innerCapInfo;
    g_audioInfo->audioEncInfo = g_audioEncInfo;
    g_videoCaptureInfo->videoFrameWidth = SCREEN_WIDTH;
    g_videoCaptureInfo->videoFrameHeight = SCREEN_HEIGHT;
    g_videoInfo->videoCapInfo = g_videoCaptureInfo;
    g_videoInfo->videoEncInfo = g_videoEncInfo;
    result = g_screenCapture->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, 0);
    g_screenCaptureConfig->audioInfo = g_audioInfo;
    result = g_screenCapture->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, 0);
    g_screenCaptureConfig->videoInfo = g_videoInfo;
    result = g_screenCapture->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, 0);
    g_screenCaptureConfig->recorderInfo = g_recorderInfo;
    result = g_screenCapture->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, 0);
}

void ScreenCaptureAdapterImplTest::TearDownTestCase(void) {}

void ScreenCaptureAdapterImplTest::SetUp(void) {}

void ScreenCaptureAdapterImplTest::TearDown(void) {}

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
    g_screenCaptureConfig->captureMode = CaptureModeAdapter::CAPTURE_INVAILD;
    g_screenCaptureConfig->dataType = DataTypeAdapter::INVAILD_DATA_TYPE;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioSampleRate = AUDIO_SAMPLE_RATE;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioChannels = AUDIO_CHANNELS;
    g_screenCaptureConfig->audioInfo->innerCapInfo->audioSampleRate = AUDIO_SAMPLE_RATE;
    g_screenCaptureConfig->audioInfo->innerCapInfo->audioChannels = AUDIO_CHANNELS;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoFrameWidth = SCREEN_WIDTH;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoFrameHeight = SCREEN_HEIGHT;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioSource = AudioCaptureSourceTypeAdapter::SOURCE_INVALID;
    g_screenCaptureConfig->audioInfo->audioEncInfo->audioCodecformat = AudioCodecFormatAdapter::AUDIO_DEFAULT;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_YUV;
    g_screenCaptureConfig->videoInfo->videoEncInfo->videoCodec = VideoCodecFormatAdapter::VIDEO_DEFAULT;
    int32_t result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    g_screenCaptureConfig->captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    g_screenCaptureConfig->dataType = DataTypeAdapter::ENCODED_STREAM_DATA_TYPE;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioSource = AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
    g_screenCaptureConfig->audioInfo->audioEncInfo->audioCodecformat = AudioCodecFormatAdapter::AAC_LC;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_ES;
    g_screenCaptureConfig->videoInfo->videoEncInfo->videoCodec = VideoCodecFormatAdapter::H264;
    result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    g_screenCaptureConfig->captureMode = CaptureModeAdapter::CAPTURE_SPECIFIED_SCREEN;
    g_screenCaptureConfig->dataType = DataTypeAdapter::CAPTURE_FILE_DATA_TYPE;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioSource = AudioCaptureSourceTypeAdapter::MIC;
    g_screenCaptureConfig->audioInfo->audioEncInfo->audioCodecformat = AudioCodecFormatAdapter::AUDIO_CODEC_FORMAT_BUTT;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA;
    g_screenCaptureConfig->videoInfo->videoEncInfo->videoCodec = VideoCodecFormatAdapter::H265;
    result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    g_screenCaptureConfig->captureMode = CaptureModeAdapter::CAPTURE_SPECIFIED_WINDOW;
    g_screenCaptureConfig->dataType = DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioSource = AudioCaptureSourceTypeAdapter::ALL_PLAYBACK;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoSource = VideoSourceTypeAdapter::VIDEO_SOURCE_BUTT;
    g_screenCaptureConfig->videoInfo->videoEncInfo->videoCodec = VideoCodecFormatAdapter::MPEG4;
    result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    g_screenCaptureConfig->audioInfo->micCapInfo->audioSource = AudioCaptureSourceTypeAdapter::APP_PLAYBACK;
    g_screenCaptureConfig->videoInfo->videoEncInfo->videoCodec = VideoCodecFormatAdapter::VP8;
    result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    g_screenCaptureConfig->videoInfo->videoEncInfo->videoCodec = VideoCodecFormatAdapter::VP9;
    result = adapterImpl->Init(g_screenCaptureConfig);
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
    std::shared_ptr<SurfaceBufferAdapter> buffer = adapterImpl->AcquireVideoBuffer();
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
    ScreenCaptureImplMock* mock = new ScreenCaptureImplMock();
    EXPECT_NE(mock, nullptr);
    adapterImpl->screenCapture_.reset(mock);
    EXPECT_CALL(*mock, StartScreenCapture()).Times(1).WillRepeatedly(::testing::Return(Media::MSERR_OK));
    EXPECT_CALL(*mock, StopScreenCapture()).Times(1).WillRepeatedly(::testing::Return(Media::MSERR_OK));
    EXPECT_CALL(*mock, SetMicrophoneEnabled(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(Media::MSERR_NO_MEMORY));
    result = adapterImpl->StartCapture();
    EXPECT_EQ(result, 0);
    adapterImpl->SetMicrophoneEnable(true);
    result = adapterImpl->StopCapture();
    EXPECT_EQ(result, 0);
    EXPECT_CALL(*mock, StartScreenCapture()).Times(1).WillRepeatedly(::testing::Return(Media::MSERR_NO_MEMORY));
    EXPECT_CALL(*mock, StopScreenCapture()).Times(1).WillRepeatedly(::testing::Return(Media::MSERR_NO_MEMORY));
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
    g_screenCaptureConfig->captureMode = CaptureModeAdapter::CAPTURE_HOME_SCREEN;
    g_screenCaptureConfig->dataType = DataTypeAdapter::ENCODED_STREAM_DATA_TYPE;
    int32_t result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    g_screenCaptureConfig->dataType = DataTypeAdapter::CAPTURE_FILE_DATA_TYPE;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioSampleRate = AUDIO_SAMPLE_RATE;
    g_screenCaptureConfig->audioInfo->micCapInfo->audioChannels = AUDIO_CHANNELS;
    g_screenCaptureConfig->audioInfo->innerCapInfo->audioSampleRate = AUDIO_SAMPLE_RATE;
    g_screenCaptureConfig->audioInfo->innerCapInfo->audioChannels = AUDIO_CHANNELS;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoFrameWidth = SCREEN_WIDTH;
    g_screenCaptureConfig->videoInfo->videoCapInfo->videoFrameHeight = SCREEN_HEIGHT;
    g_screenCaptureConfig->videoInfo->videoEncInfo->videoCodec = VideoCodecFormatAdapter::VIDEO_CODEC_FORMAT_BUTT;
    g_screenCaptureConfig->recorderInfo->fileFormat = ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE;
    result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    g_screenCaptureConfig->recorderInfo->fileFormat = ContainerFormatTypeAdapter::CFT_MPEG_4_TYPE;
    result = adapterImpl->Init(g_screenCaptureConfig);
    EXPECT_EQ(result, -1);
    adapterImpl->screenCapture_ = OHOS::Media::ScreenCaptureFactory::CreateScreenCapture();
    result = adapterImpl->Init(g_screenCaptureConfig);
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
    ScreenCaptureImplMock* mock = new ScreenCaptureImplMock();
    EXPECT_NE(mock, nullptr);
    sptr<OHOS::SurfaceBuffer> surfacebuffer = new SurfaceBufferImpl(0);
    EXPECT_NE(surfacebuffer, nullptr);
    adapterImpl->screenCapture_.reset(mock);
    EXPECT_CALL(*mock, AcquireVideoBuffer(::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(surfacebuffer));

    EXPECT_CALL(*mock, ReleaseVideoBuffer()).Times(1).WillRepeatedly(::testing::Return(Media::MSERR_OK));
    std::shared_ptr<SurfaceBufferAdapter> buffer = adapterImpl->AcquireVideoBuffer();
    EXPECT_NE(buffer, nullptr);
    int32_t result = adapterImpl->ReleaseVideoBuffer();
    EXPECT_EQ(result, 0);
    adapterImpl->screenCapture_.reset();
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_AcquireAudioBuffer_008
 * @tc.desc: AcquireAudioBuffer.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_AcquireAudioBuffer_008, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    ScreenCaptureImplMock* mock = new ScreenCaptureImplMock();
    EXPECT_NE(mock, nullptr);
    std::shared_ptr<MockAudioBufferAdapter> audiobuffer = nullptr;
    EXPECT_EQ(audiobuffer, nullptr);
    adapterImpl->screenCapture_.reset(mock);

    EXPECT_CALL(*mock, ReleaseAudioBuffer(::testing::_)).Times(1).WillRepeatedly(::testing::Return(0));
    AudioCaptureSourceTypeAdapter type = AudioCaptureSourceTypeAdapter::MIC;
    int32_t ret = adapterImpl->AcquireAudioBuffer(audiobuffer, type);
    EXPECT_EQ(ret, -1);
    int32_t result = adapterImpl->ReleaseAudioBuffer(type);
    EXPECT_EQ(result, 0);
    adapterImpl->screenCapture_.reset();
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_AcquireAudioBuffer_009
 * @tc.desc: AcquireAudioBuffer.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_AcquireAudioBuffer_009, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    ScreenCaptureImplMock* mock = new ScreenCaptureImplMock();
    EXPECT_NE(mock, nullptr);
    adapterImpl->screenCapture_.reset(mock);
    EXPECT_CALL(*mock, AcquireAudioBuffer(::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(-1));
    
    EXPECT_CALL(*mock, ReleaseAudioBuffer(::testing::_)).Times(1).WillRepeatedly(::testing::Return(-1));
    AudioCaptureSourceTypeAdapter type = AudioCaptureSourceTypeAdapter::ALL_PLAYBACK;
    int32_t ret = adapterImpl->AcquireAudioBuffer(g_audioBufferAdapter, type);
    EXPECT_EQ(ret, -1);
    int32_t result = adapterImpl->ReleaseAudioBuffer(type);
    EXPECT_EQ(result, -1);
    adapterImpl->screenCapture_.reset();
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_ConvertAudioCaptureSourceType_010
 * @tc.desc: ConvertAudioCaptureSourceType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ScreenCaptureAdapterImplTest, ScreenCaptureAdapterImplTest_ConvertAudioCaptureSourceType_010, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    AudioCaptureSourceTypeAdapter type;
    OHOS::Media::AudioCaptureSourceType result;

    type = AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT;
    result = ConvertAudioCaptureSourceType(type);
    EXPECT_EQ(result, OHOS::Media::AudioCaptureSourceType::SOURCE_DEFAULT);

    type = AudioCaptureSourceTypeAdapter::MIC;
    result = ConvertAudioCaptureSourceType(type);
    EXPECT_EQ(result, OHOS::Media::AudioCaptureSourceType::MIC);

    type = AudioCaptureSourceTypeAdapter::ALL_PLAYBACK;
    result = ConvertAudioCaptureSourceType(type);
    EXPECT_EQ(result, OHOS::Media::AudioCaptureSourceType::ALL_PLAYBACK);

    type = AudioCaptureSourceTypeAdapter::APP_PLAYBACK;
    result = ConvertAudioCaptureSourceType(type);
    EXPECT_EQ(result, OHOS::Media::AudioCaptureSourceType::APP_PLAYBACK);

    type = static_cast<AudioCaptureSourceTypeAdapter>(999); // Invalid source
    result = ConvertAudioCaptureSourceType(type);
    EXPECT_EQ(result, OHOS::Media::AudioCaptureSourceType::SOURCE_INVALID);
}

/**
 * @tc.name: ScreenCaptureAdapterImplTest_GetScreenCaptureStateCodeAdapter_011
 * @tc.desc: GetScreenCaptureStateCodeAdapter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ScreenCaptureAdapterImplTest,
    ScreenCaptureAdapterImplTest_GetScreenCaptureStateCodeAdapter_011, TestSize.Level1)
{
    auto adapterImpl = std::make_shared<ScreenCaptureAdapterImpl>();
    EXPECT_NE(adapterImpl, nullptr);
    OHOS::Media::AVScreenCaptureStateCode stateCode;
    ScreenCaptureStateCodeAdapter result;

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STARTED;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STARTED);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_CANCELED;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_CANCELED);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STOPPED_BY_USER;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_USER);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STOPPED_BY_CALL);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNAVAILABLE);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_MUTED_BY_USER);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_MIC_UNMUTED_BY_USER);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE);

    stateCode = OHOS::Media::AVScreenCaptureStateCode::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_EXIT_PRIVATE_SCENE);

    stateCode = static_cast<OHOS::Media::AVScreenCaptureStateCode>(999);;
    result = GetScreenCaptureStateCodeAdapter(stateCode);
    EXPECT_EQ(result, ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID);
}
} // namespace NWeb
} // namespace OHOS

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#define private public
#include "accesstoken_kit.h"
#include "application_context.h"
#include "audio_capturer_adapter.h"
#include "audio_capturer_adapter_impl.h"
#include "audio_renderer_adapter.h"
#include "audio_renderer_adapter_impl.h"
#include "audio_system_manager_adapter_impl.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace {
const int RESULT_OK = 0;
const bool TRUE_OK = true;
const std::string LANGUAGE_EN = "en";
const std::string DEVICE_TYPE_NONE_TEST = "device/none";
const std::string DEVICE_TYPE_NONE_ZH_CN_TEST = "无";
const std::string CACHE_PATH = "/data/local/tmp";
std::shared_ptr<NWeb::AudioRendererAdapterImpl> g_audioRender = nullptr;
std::shared_ptr<NWeb::AudioCapturerAdapterImpl> g_audioCapturer = nullptr;
std::shared_ptr<AbilityRuntime::ApplicationContext> g_applicationContext = nullptr;
const int32_t ADAPTER_AUDIO_UNDEFINED_DEVICEID = 1000001;
} // namespace

namespace AbilityRuntime {
std::shared_ptr<ApplicationContext> Context::GetApplicationContext()
{
    return g_applicationContext;
}
} // namespace AbilityRuntime

namespace NWeb {
class NWebAudioAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class AudioRendererOptionsAdapterMock : public AudioRendererOptionsAdapter {
public:
    AudioRendererOptionsAdapterMock() = default;

    AudioAdapterSamplingRate GetSamplingRate() override
    {
        return samplingRate;
    }

    AudioAdapterEncodingType GetEncodingType() override
    {
        return encoding;
    }

    AudioAdapterSampleFormat GetSampleFormat() override
    {
        return format;
    }

    AudioAdapterChannel GetChannel() override
    {
        return channels;
    }

    AudioAdapterContentType GetContentType() override
    {
        return contentType;
    }

    AudioAdapterStreamUsage GetStreamUsage() override
    {
        return streamUsage;
    }

    int32_t GetRenderFlags() override
    {
        return rendererFlags;
    }

    AudioAdapterConcurrencyMode GetConcurrencyMode() override 
    {
        return concurrency_mode;
    }

    AudioAdapterSamplingRate samplingRate;
    AudioAdapterEncodingType encoding;
    AudioAdapterSampleFormat format;
    AudioAdapterChannel channels;
    AudioAdapterContentType contentType;
    AudioAdapterStreamUsage streamUsage;
    int32_t rendererFlags;
    AudioAdapterConcurrencyMode concurrency_mode;
};

class AudioInterruptAdapterMock : public AudioInterruptAdapter {
public:
    AudioInterruptAdapterMock() = default;

    AudioAdapterStreamUsage GetStreamUsage() override
    {
        return streamUsage;
    }

    AudioAdapterContentType GetContentType() override
    {
        return contentType;
    }

    AudioAdapterStreamType GetStreamType() override
    {
        return streamType;
    }

    uint32_t GetSessionID() override
    {
        return sessionID;
    }

    bool GetPauseWhenDucked() override
    {
        return pauseWhenDucked;
    }

    AudioAdapterStreamUsage streamUsage;
    AudioAdapterContentType contentType;
    AudioAdapterStreamType streamType;
    uint32_t sessionID;
    bool pauseWhenDucked;
};

class AudioCapturerOptionsAdapterMock : public AudioCapturerOptionsAdapter {
public:
    AudioCapturerOptionsAdapterMock() = default;

    AudioAdapterSamplingRate GetSamplingRate() override
    {
        return samplingRate;
    }

    AudioAdapterEncodingType GetEncoding() override
    {
        return encoding;
    }

    AudioAdapterSampleFormat GetSampleFormat() override
    {
        return format;
    }

    AudioAdapterChannel GetChannels() override
    {
        return channels;
    }

    AudioAdapterSourceType GetSourceType() override
    {
        return sourceType;
    }

    int32_t GetCapturerFlags() override
    {
        return capturerFlags;
    }

    AudioAdapterSamplingRate samplingRate;
    AudioAdapterEncodingType encoding;
    AudioAdapterSampleFormat format;
    AudioAdapterChannel channels;
    AudioAdapterSourceType sourceType;
    int32_t capturerFlags;
};

class BufferDescAdapterMock : public BufferDescAdapter {
public:
    BufferDescAdapterMock() = default;

    uint8_t* GetBuffer() override
    {
        return buffer;
    }

    size_t GetBufLength() override
    {
        return bufLength;
    }

    size_t GetDataLength() override
    {
        return dataLength;
    }

    void SetBuffer(uint8_t* buf) override
    {
        buffer = buf;
    }

    void SetBufLength(size_t bufLen) override
    {
        bufLength = bufLen;
    }

    void SetDataLength(size_t dataLen) override
    {
        dataLength = dataLen;
    }

    uint8_t* buffer;
    size_t bufLength;
    size_t dataLength;
};

class ApplicationContextMock : public ApplicationContext {
public:
    MOCK_METHOD0(GetCacheDir, std::string());
};

class AudioCallbackTest : public AudioManagerCallbackAdapter {
public:
    AudioCallbackTest() = default;

    virtual ~AudioCallbackTest() = default;

    void OnSuspend() override {};

    void OnResume() override {};
};

class AudioRendererCallbackMock : public AudioRendererCallbackAdapter {
public:
    AudioRendererCallbackMock() = default;

    virtual ~AudioRendererCallbackMock() = default;

    void OnSuspend() override {};

    void OnResume() override {};
};

class AudioOutputChangeCallbackMock : public AudioOutputChangeCallbackAdapter {
public:
    AudioOutputChangeCallbackMock() = default;

    virtual ~AudioOutputChangeCallbackMock() = default;

    void OnOutputDeviceChange(int32_t reason) override {};
};

class AudioCapturerCallbackMock : public AudioCapturerReadCallbackAdapter {
public:
    AudioCapturerCallbackMock() = default;

    virtual ~AudioCapturerCallbackMock() = default;

    void OnReadData(size_t length) override {};
};

class AudioManagerDeviceChangeCallbackAdapterMock : public AudioManagerDeviceChangeCallbackAdapter {
public:
    AudioManagerDeviceChangeCallbackAdapterMock() = default;

    virtual ~AudioManagerDeviceChangeCallbackAdapterMock() = default;

    void OnDeviceChange() override {};
};

void NWebAudioAdapterTest::SetUpTestCase(void)
{
    uint64_t tokenId;
    const char* perms[1];
    perms[0] = "ohos.permission.MICROPHONE";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "web_audio_tdd",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void NWebAudioAdapterTest::TearDownTestCase(void) {}

void NWebAudioAdapterTest::SetUp(void) {}

void NWebAudioAdapterTest::TearDown(void) {}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_001.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_001, TestSize.Level1)
{
    ApplicationContextMock* contextMock = new ApplicationContextMock();
    EXPECT_NE(contextMock, nullptr);
    EXPECT_EQ(g_applicationContext, nullptr);
    g_applicationContext.reset(contextMock);
    EXPECT_NE(g_applicationContext, nullptr);

    g_audioRender = std::make_shared<AudioRendererAdapterImpl>();
    ASSERT_NE(g_audioRender, nullptr);

    AudioRendererOptions rendererOptions;
    AudioRendererInfo renderInfo;
    rendererOptions.streamInfo.samplingRate = AudioSamplingRate::SAMPLE_RATE_44100;
    rendererOptions.streamInfo.encoding = AudioEncodingType::ENCODING_PCM;
    rendererOptions.streamInfo.format = AudioSampleFormat::SAMPLE_S16LE;
    rendererOptions.streamInfo.channels = AudioChannel::STEREO;
    rendererOptions.rendererInfo = renderInfo;
    g_audioRender->audio_renderer_ = AudioRenderer::Create(rendererOptions);
    ASSERT_NE(g_audioRender->audio_renderer_, nullptr);

    bool ret = g_audioRender->Start();
    EXPECT_EQ(ret, TRUE_OK);

    std::array<uint8_t, 4> bufferArray = { 0, 0, 0, 0 };
    g_audioRender->Write(bufferArray.data(), bufferArray.size());

    uint64_t latency;
    int32_t retNum = g_audioRender->GetLatency(latency);
    EXPECT_EQ(retNum, RESULT_OK);

    float volume = 0.8;
    retNum = g_audioRender->SetVolume(volume);
    EXPECT_EQ(retNum, RESULT_OK);

    float nowVolume = g_audioRender->GetVolume();
    EXPECT_EQ(nowVolume, volume);
    retNum = g_audioRender->SetAudioRendererCallback(nullptr);
    EXPECT_NE(retNum, 0);
    std::shared_ptr<AudioRendererCallbackAdapter> callback = std::make_shared<AudioRendererCallbackMock>();
    EXPECT_NE(callback, nullptr);
    retNum = g_audioRender->SetAudioRendererCallback(callback);
    EXPECT_EQ(retNum, 0);
    g_audioRender->SetInterruptMode(true);
    g_audioRender->SetInterruptMode(false);
    g_audioRender->SetAudioSilentMode(true);
    g_audioRender->SetAudioSilentMode(false);
    ret = g_audioRender->IsRendererStateRunning();
    EXPECT_EQ(ret, TRUE_OK);
    ret = g_audioRender->Pause();
    EXPECT_EQ(ret, TRUE_OK);
    ret = g_audioRender->Stop();
    EXPECT_EQ(ret, TRUE_OK);
    ret = g_audioRender->Release();
    EXPECT_EQ(ret, TRUE_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_002.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_002, TestSize.Level1)
{
    std::array<AudioAdapterSamplingRate, 11> samplingArray = {
        AudioAdapterSamplingRate::SAMPLE_RATE_8000,
        AudioAdapterSamplingRate::SAMPLE_RATE_11025,
        AudioAdapterSamplingRate::SAMPLE_RATE_12000,
        AudioAdapterSamplingRate::SAMPLE_RATE_16000,
        AudioAdapterSamplingRate::SAMPLE_RATE_22050,
        AudioAdapterSamplingRate::SAMPLE_RATE_24000,
        AudioAdapterSamplingRate::SAMPLE_RATE_32000,
        AudioAdapterSamplingRate::SAMPLE_RATE_44100,
        AudioAdapterSamplingRate::SAMPLE_RATE_48000,
        AudioAdapterSamplingRate::SAMPLE_RATE_64000,
        AudioAdapterSamplingRate::SAMPLE_RATE_96000,
    };
    for (auto& sampling : samplingArray)
        AudioRendererAdapterImpl::GetAudioSamplingRate(sampling);

    AudioSamplingRate testSampling =
        AudioRendererAdapterImpl::GetAudioSamplingRate(static_cast<AudioAdapterSamplingRate>(0));
    EXPECT_EQ(testSampling, AudioSamplingRate::SAMPLE_RATE_44100);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_003.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_003, TestSize.Level1)
{
    AudioRendererAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_PCM);
    AudioRendererAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_INVALID);

    AudioEncodingType testEncodingType =
        AudioRendererAdapterImpl::GetAudioEncodingType(static_cast<AudioAdapterEncodingType>(1));
    EXPECT_EQ(testEncodingType, AudioEncodingType::ENCODING_INVALID);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_004.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_004, TestSize.Level1)
{
    std::array<AudioAdapterSampleFormat, 6> formatArray = {
        AudioAdapterSampleFormat::SAMPLE_U8,
        AudioAdapterSampleFormat::SAMPLE_S16LE,
        AudioAdapterSampleFormat::SAMPLE_S24LE,
        AudioAdapterSampleFormat::SAMPLE_S32LE,
        AudioAdapterSampleFormat::SAMPLE_F32LE,
        AudioAdapterSampleFormat::INVALID_WIDTH,
    };
    for (auto& format : formatArray)
        AudioRendererAdapterImpl::GetAudioSampleFormat(format);

    AudioSampleFormat testFormat =
        AudioRendererAdapterImpl::GetAudioSampleFormat(static_cast<AudioAdapterSampleFormat>(-2));
    EXPECT_EQ(testFormat, AudioSampleFormat::INVALID_WIDTH);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_005.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_005, TestSize.Level1)
{
    std::array<AudioAdapterChannel, 8> channelArray = {
        AudioAdapterChannel::MONO,
        AudioAdapterChannel::STEREO,
        AudioAdapterChannel::CHANNEL_3,
        AudioAdapterChannel::CHANNEL_4,
        AudioAdapterChannel::CHANNEL_5,
        AudioAdapterChannel::CHANNEL_6,
        AudioAdapterChannel::CHANNEL_7,
        AudioAdapterChannel::CHANNEL_8,
    };
    for (auto& channel : channelArray)
        AudioRendererAdapterImpl::GetAudioChannel(channel);

    AudioChannel testChannel = AudioRendererAdapterImpl::GetAudioChannel(static_cast<AudioAdapterChannel>(0));
    EXPECT_EQ(testChannel, AudioChannel::STEREO);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_006.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_006, TestSize.Level1)
{
    std::array<AudioAdapterContentType, 6> contentArray = {
        AudioAdapterContentType::CONTENT_TYPE_UNKNOWN,
        AudioAdapterContentType::CONTENT_TYPE_SPEECH,
        AudioAdapterContentType::CONTENT_TYPE_MUSIC,
        AudioAdapterContentType::CONTENT_TYPE_MOVIE,
        AudioAdapterContentType::CONTENT_TYPE_SONIFICATION,
        AudioAdapterContentType::CONTENT_TYPE_RINGTONE,
    };
    for (auto& content : contentArray)
        AudioRendererAdapterImpl::GetAudioContentType(content);

    ContentType testContent = AudioRendererAdapterImpl::GetAudioContentType(static_cast<AudioAdapterContentType>(-1));
    EXPECT_EQ(testContent, ContentType::CONTENT_TYPE_MUSIC);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_007.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_007, TestSize.Level1)
{
    std::array<AudioAdapterStreamUsage, 5> usageArray = {
        AudioAdapterStreamUsage::STREAM_USAGE_UNKNOWN,
        AudioAdapterStreamUsage::STREAM_USAGE_MEDIA,
        AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION,
        AudioAdapterStreamUsage::STREAM_USAGE_VOICE_ASSISTANT,
        AudioAdapterStreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE,
    };
    for (auto& usage : usageArray)
        AudioRendererAdapterImpl::GetAudioStreamUsage(usage);

    StreamUsage testUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(static_cast<AudioAdapterStreamUsage>(-1));
    EXPECT_EQ(testUsage, StreamUsage::STREAM_USAGE_MEDIA);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_008.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_008, TestSize.Level1)
{
    bool ret = AudioSystemManagerAdapterImpl::GetInstance().HasAudioOutputDevices();
    EXPECT_EQ(ret, TRUE_OK);

    AudioSystemManagerAdapterImpl::GetInstance().HasAudioInputDevices();

    std::shared_ptr<AudioDeviceDescAdapter> result =
        AudioSystemManagerAdapterImpl::GetInstance().GetDefaultOutputDevice();
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->GetDeviceId(), -1);
    int32_t status = AudioSystemManagerAdapterImpl::GetInstance().SetDeviceChangeCallback(nullptr);
    EXPECT_NE(status, 0);
    auto mock = std::make_shared<AudioManagerDeviceChangeCallbackAdapterMock>();
    status = AudioSystemManagerAdapterImpl::GetInstance().SetDeviceChangeCallback(mock);
    EXPECT_EQ(status, 0);

    AudioStreamType type =
        AudioSystemManagerAdapterImpl::GetInstance().GetStreamType(AudioAdapterStreamType::STREAM_VOICE_CALL);
    EXPECT_EQ(type, AudioStreamType::STREAM_VOICE_CALL);
    type = AudioSystemManagerAdapterImpl::GetInstance().GetStreamType(static_cast<AudioAdapterStreamType>(-2));
    EXPECT_EQ(type, AudioStreamType::STREAM_DEFAULT);

    std::vector<std::shared_ptr<AudioDeviceDescriptor>> device;
    int32_t select = AudioSystemManagerAdapterImpl::GetInstance().SelectAudioOutputDevice(false, device);
    EXPECT_NE(select, 0);
    select = AudioSystemManagerAdapterImpl::GetInstance().SelectAudioOutputDevice(true, device);
    EXPECT_NE(select, 0);
    AudioSystemManagerAdapterImpl::GetInstance().GetDefaultInputDevice();
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_009.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_009, TestSize.Level1)
{
    std::shared_ptr<AudioInterruptAdapterMock> interrupt = std::make_shared<AudioInterruptAdapterMock>();
    EXPECT_NE(interrupt, nullptr);
    interrupt->streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    interrupt->contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    interrupt->streamType = AudioAdapterStreamType::STREAM_DEFAULT;

    int32_t ret = AudioSystemManagerAdapterImpl::GetInstance().RequestAudioFocus(interrupt);
    EXPECT_NE(ret, RESULT_OK);
    ret = AudioSystemManagerAdapterImpl::GetInstance().AbandonAudioFocus(interrupt);
    EXPECT_NE(ret, RESULT_OK);

    interrupt->streamType = AudioAdapterStreamType::STREAM_MUSIC;
    ret = AudioSystemManagerAdapterImpl::GetInstance().RequestAudioFocus(interrupt);
    EXPECT_EQ(ret, RESULT_OK);

    ret = AudioSystemManagerAdapterImpl::GetInstance().AbandonAudioFocus(interrupt);
    EXPECT_EQ(ret, RESULT_OK);
    ret = AudioSystemManagerAdapterImpl::GetInstance().RequestAudioFocus(nullptr);
    EXPECT_NE(ret, RESULT_OK);
    ret = AudioSystemManagerAdapterImpl::GetInstance().AbandonAudioFocus(nullptr);
    EXPECT_NE(ret, RESULT_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_010.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_010, TestSize.Level1)
{
    auto callback = std::make_shared<AudioCallbackTest>();
    int32_t ret = AudioSystemManagerAdapterImpl::GetInstance().SetAudioManagerInterruptCallback(nullptr);
    EXPECT_NE(ret, RESULT_OK);

    ret = AudioSystemManagerAdapterImpl::GetInstance().SetAudioManagerInterruptCallback(callback);
    EXPECT_EQ(ret, RESULT_OK);

    ret = AudioSystemManagerAdapterImpl::GetInstance().UnsetAudioManagerInterruptCallback();
    EXPECT_EQ(ret, RESULT_OK);

    AudioSystemManagerAdapterImpl::GetInstance().GetDevices(AdapterDeviceFlag::OUTPUT_DEVICES_FLAG);
    AudioSystemManagerAdapterImpl::GetInstance().GetDevices(static_cast<AdapterDeviceFlag>(-1));

    ret = AudioSystemManagerAdapterImpl::GetInstance().SelectAudioDeviceById(-1, true);
    EXPECT_NE(ret, RESULT_OK);
    ret = AudioSystemManagerAdapterImpl::GetInstance().SelectAudioDeviceById(-1, false);
    EXPECT_NE(ret, RESULT_OK);
    ret = AudioSystemManagerAdapterImpl::GetInstance().SelectAudioDeviceById(
        (int32_t)ADAPTER_AUDIO_UNDEFINED_DEVICEID, false);
    EXPECT_NE(ret, RESULT_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_011.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_011, TestSize.Level1)
{
    std::array<AudioAdapterStreamType, 16> streamArray = {
        AudioAdapterStreamType::STREAM_DEFAULT,
        AudioAdapterStreamType::STREAM_VOICE_CALL,
        AudioAdapterStreamType::STREAM_MUSIC,
        AudioAdapterStreamType::STREAM_RING,
        AudioAdapterStreamType::STREAM_MEDIA,
        AudioAdapterStreamType::STREAM_VOICE_ASSISTANT,
        AudioAdapterStreamType::STREAM_SYSTEM,
        AudioAdapterStreamType::STREAM_ALARM,
        AudioAdapterStreamType::STREAM_NOTIFICATION,
        AudioAdapterStreamType::STREAM_BLUETOOTH_SCO,
        AudioAdapterStreamType::STREAM_ENFORCED_AUDIBLE,
        AudioAdapterStreamType::STREAM_DTMF,
        AudioAdapterStreamType::STREAM_TTS,
        AudioAdapterStreamType::STREAM_ACCESSIBILITY,
        AudioAdapterStreamType::STREAM_RECORDING,
        AudioAdapterStreamType::STREAM_ALL,
    };

    for (auto& stream : streamArray)
        AudioSystemManagerAdapterImpl::GetInstance().GetStreamType(stream);
    AudioStreamType testStream =
        AudioSystemManagerAdapterImpl::GetInstance().GetStreamType(static_cast<AudioAdapterStreamType>(-2));
    EXPECT_EQ(testStream, AudioStreamType::STREAM_DEFAULT);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_012.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_012, TestSize.Level1)
{
    auto callback = std::make_shared<AudioCallbackTest>();
    auto callbackTest = std::make_shared<AudioManagerCallbackAdapterImpl>(callback);
    ASSERT_NE(callbackTest, nullptr);

    InterruptAction interruptAction;

    interruptAction.interruptHint = InterruptHint::INTERRUPT_HINT_PAUSE;
    callbackTest->OnInterrupt(interruptAction);

    interruptAction.interruptHint = InterruptHint::INTERRUPT_HINT_STOP;
    callbackTest->OnInterrupt(interruptAction);

    interruptAction.interruptHint = InterruptHint::INTERRUPT_HINT_RESUME;
    callbackTest->OnInterrupt(interruptAction);

    interruptAction.interruptHint = static_cast<InterruptHint>(-1);
    callbackTest->OnInterrupt(interruptAction);
    callbackTest->cb_ = nullptr;
    callbackTest->OnInterrupt(interruptAction);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_013.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_013, TestSize.Level1)
{
    g_audioRender = std::make_shared<AudioRendererAdapterImpl>();
    ASSERT_NE(g_audioRender, nullptr);

    std::shared_ptr<AudioRendererOptionsAdapterMock> rendererOptions =
        std::make_shared<AudioRendererOptionsAdapterMock>();
    rendererOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions->encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions->format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions->channels = AudioAdapterChannel::STEREO;
    rendererOptions->contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions->streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions->rendererFlags = 0;
    int32_t retNum = g_audioRender->Create(rendererOptions);
    ASSERT_NE(retNum, AudioAdapterCode::AUDIO_OK);

    bool ret = g_audioRender->Start();
    EXPECT_NE(ret, TRUE_OK);

    std::array<uint8_t, 4> bufferArray = { 0, 0, 0, 0 };
    retNum = g_audioRender->Write(bufferArray.data(), bufferArray.size());
    EXPECT_NE(retNum, bufferArray.size());

    uint64_t latency;
    retNum = g_audioRender->GetLatency(latency);
    EXPECT_NE(retNum, RESULT_OK);

    float volume = 0.8;
    retNum = g_audioRender->SetVolume(volume);
    EXPECT_NE(retNum, RESULT_OK);

    float nowVolume = g_audioRender->GetVolume();
    EXPECT_NE(nowVolume, volume);

    ret = g_audioRender->Pause();
    EXPECT_NE(ret, TRUE_OK);

    ret = g_audioRender->Stop();
    EXPECT_NE(ret, TRUE_OK);

    ret = g_audioRender->Release();
    EXPECT_NE(ret, TRUE_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_014.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5RWOG
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_014, TestSize.Level1)
{
    g_applicationContext.reset();
    ApplicationContextMock* contextMock = new ApplicationContextMock();
    EXPECT_NE(contextMock, nullptr);
    EXPECT_CALL(*contextMock, GetCacheDir()).Times(1).WillRepeatedly(::testing::Return(""));
    EXPECT_EQ(g_applicationContext, nullptr);
    g_applicationContext.reset(contextMock);
    EXPECT_NE(g_applicationContext, nullptr);

    std::shared_ptr<AudioRendererAdapterImpl> audioRenderImpl = std::make_shared<AudioRendererAdapterImpl>();
    EXPECT_NE(audioRenderImpl, nullptr);

    std::shared_ptr<AudioRendererOptionsAdapterMock> rendererOptions =
        std::make_shared<AudioRendererOptionsAdapterMock>();
    rendererOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions->encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions->format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions->channels = AudioAdapterChannel::STEREO;
    rendererOptions->contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions->streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions->rendererFlags = 0;
    int32_t retNum = audioRenderImpl->Create(rendererOptions);
    g_applicationContext.reset();
    EXPECT_EQ(g_applicationContext, nullptr);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_ERROR);
    EXPECT_EQ(audioRenderImpl->audio_renderer_, nullptr);
    audioRenderImpl->SetInterruptMode(false);
    audioRenderImpl->SetAudioSilentMode(false);
    retNum = audioRenderImpl->Create(nullptr);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_015.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_015, TestSize.Level1)
{
    ApplicationContextMock* contextMock = new ApplicationContextMock();
    EXPECT_NE(contextMock, nullptr);
    EXPECT_CALL(*contextMock, GetCacheDir()).Times(1).WillRepeatedly(::testing::Return(CACHE_PATH));
    EXPECT_EQ(g_applicationContext, nullptr);
    g_applicationContext.reset(contextMock);
    EXPECT_NE(g_applicationContext, nullptr);

    std::shared_ptr<AudioRendererAdapterImpl> audioRenderImpl = std::make_shared<AudioRendererAdapterImpl>();
    EXPECT_NE(audioRenderImpl, nullptr);

    std::shared_ptr<AudioRendererOptionsAdapterMock> rendererOptions =
        std::make_shared<AudioRendererOptionsAdapterMock>();
    rendererOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions->encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions->format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions->channels = AudioAdapterChannel::STEREO;
    rendererOptions->contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions->streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions->rendererFlags = 0;
    audioRenderImpl->Create(rendererOptions);
    g_applicationContext.reset();
    EXPECT_EQ(g_applicationContext, nullptr);
}

/**
 * @tc.name: NWebAudioAdapterTest_OnInterrupt_016.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_OnInterrupt_016, TestSize.Level1)
{
    std::shared_ptr<AudioRendererCallbackAdapter> cb = std::make_shared<AudioRendererCallbackMock>();
    EXPECT_NE(cb, nullptr);
    auto callBack = std::make_shared<AudioRendererCallbackImpl>(cb);
    EXPECT_NE(callBack, nullptr);
    InterruptEvent interruptEvent;
    interruptEvent.hintType = OHOS::AudioStandard::InterruptHint::INTERRUPT_HINT_PAUSE;
    callBack->OnInterrupt(interruptEvent);
    interruptEvent.hintType = OHOS::AudioStandard::InterruptHint::INTERRUPT_HINT_STOP;
    callBack->OnInterrupt(interruptEvent);
    interruptEvent.hintType = OHOS::AudioStandard::InterruptHint::INTERRUPT_HINT_RESUME;
    callBack->OnInterrupt(interruptEvent);
    interruptEvent.hintType = OHOS::AudioStandard::InterruptHint::INTERRUPT_HINT_NONE;
    callBack->OnInterrupt(interruptEvent);
    callBack->OnStateChange(RendererState::RENDERER_PAUSED, StateChangeCmdType::CMD_FROM_SYSTEM);
    callBack->cb_ = nullptr;
    callBack->OnInterrupt(interruptEvent);
}

/**
 * @tc.name: NWebAudioAdapterTest_SetAudioRendererCallback_017.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_SetAudioRendererCallback_017, TestSize.Level1)
{
    auto audioRender = std::make_shared<AudioRendererAdapterImpl>();
    ASSERT_NE(audioRender, nullptr);
    std::shared_ptr<AudioRendererCallbackAdapter> callback = std::make_shared<AudioRendererCallbackMock>();
    ASSERT_NE(callback, nullptr);
    int32_t retNum = audioRender->SetAudioRendererCallback(callback);
    EXPECT_NE(retNum, 0);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_018.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_018, TestSize.Level1)
{
    g_audioCapturer = std::make_shared<AudioCapturerAdapterImpl>();
    ASSERT_NE(g_audioCapturer, nullptr);
    g_audioCapturer->audio_capturer_ = AudioCapturer::Create(STREAM_MUSIC);
    ASSERT_NE(g_audioCapturer->audio_capturer_, nullptr);
    AudioCapturerParams capturerParams;
    capturerParams.AudioSampleFormat = SAMPLE_S16LE;
    capturerParams.samplingRate = SAMPLE_RATE_44100;
    capturerParams.AudioChannel = STEREO;
    capturerParams.audioEncoding = ENCODING_PCM;
    g_audioCapturer->audio_capturer_->SetParams(capturerParams);
    std::shared_ptr<AudioCapturerReadCallbackAdapter> callback = std::make_shared<AudioCapturerCallbackMock>();
    EXPECT_NE(callback, nullptr);
    g_audioCapturer->SetCapturerReadCallback(callback);
    g_audioCapturer->Start();
    std::shared_ptr<BufferDescAdapterMock> bufferDesc = std::make_shared<BufferDescAdapterMock>();
    EXPECT_NE(bufferDesc, nullptr);
    int32_t retNum = g_audioCapturer->GetBufferDesc(bufferDesc);
    EXPECT_EQ(retNum, 0);
    g_audioCapturer->Enqueue(bufferDesc);
    uint32_t frameCount = 0;
    retNum = g_audioCapturer->GetFrameCount(frameCount);
    EXPECT_EQ(retNum, 0);
    int64_t result = g_audioCapturer->GetAudioTime();
    EXPECT_NE(result, AudioAdapterCode::AUDIO_NULL_ERROR);
    g_audioCapturer->Stop();
    int32_t ret = g_audioCapturer->Release();
    EXPECT_EQ(ret, TRUE_OK);
    retNum = g_audioCapturer->GetBufferDesc(nullptr);
    EXPECT_NE(retNum, 0);
    retNum = g_audioCapturer->Enqueue(nullptr);
    EXPECT_NE(retNum, 0);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_019.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_019, TestSize.Level1)
{
    g_audioCapturer = std::make_shared<AudioCapturerAdapterImpl>();
    ASSERT_NE(g_audioCapturer, nullptr);

    std::shared_ptr<AudioCapturerReadCallbackAdapter> callback = std::make_shared<AudioCapturerCallbackMock>();
    int32_t retNum = g_audioCapturer->SetCapturerReadCallback(callback);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_NULL_ERROR);
    retNum = g_audioCapturer->SetCapturerReadCallback(nullptr);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_NULL_ERROR);

    bool ret = g_audioCapturer->Start();
    EXPECT_NE(ret, TRUE_OK);

    std::shared_ptr<BufferDescAdapterMock> bufferDesc = std::make_shared<BufferDescAdapterMock>();
    EXPECT_NE(bufferDesc, nullptr);
    retNum = g_audioCapturer->GetBufferDesc(bufferDesc);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_NULL_ERROR);

    retNum = g_audioCapturer->Enqueue(bufferDesc);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_NULL_ERROR);

    uint32_t frameCount = 0;
    retNum = g_audioCapturer->GetFrameCount(frameCount);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_NULL_ERROR);

    int64_t result = g_audioCapturer->GetAudioTime();
    EXPECT_EQ(result, AudioAdapterCode::AUDIO_NULL_ERROR);

    ret = g_audioCapturer->Stop();
    EXPECT_NE(ret, TRUE_OK);

    ret = g_audioCapturer->Release();
    EXPECT_NE(ret, TRUE_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_020.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_020, TestSize.Level1)
{
    auto callback = std::make_shared<AudioCapturerCallbackMock>();
    auto callbackTest = std::make_shared<AudioCapturerReadCallbackImpl>(callback);
    EXPECT_NE(callbackTest, nullptr);

    size_t length = 0;
    callbackTest->OnReadData(length);

    auto callbackTestNull = std::make_shared<AudioCapturerReadCallbackImpl>(nullptr);
    EXPECT_EQ(callbackTestNull->cb_, nullptr);
    callbackTest->OnReadData(length);
    callbackTest->cb_ = nullptr;
    callbackTest->OnReadData(length);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_0021.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_021, TestSize.Level1)
{
    std::array<AudioAdapterSamplingRate, 11> samplingArray = {
        AudioAdapterSamplingRate::SAMPLE_RATE_8000,
        AudioAdapterSamplingRate::SAMPLE_RATE_11025,
        AudioAdapterSamplingRate::SAMPLE_RATE_12000,
        AudioAdapterSamplingRate::SAMPLE_RATE_16000,
        AudioAdapterSamplingRate::SAMPLE_RATE_22050,
        AudioAdapterSamplingRate::SAMPLE_RATE_24000,
        AudioAdapterSamplingRate::SAMPLE_RATE_32000,
        AudioAdapterSamplingRate::SAMPLE_RATE_44100,
        AudioAdapterSamplingRate::SAMPLE_RATE_48000,
        AudioAdapterSamplingRate::SAMPLE_RATE_64000,
        AudioAdapterSamplingRate::SAMPLE_RATE_96000,
    };
    for (auto& sampling : samplingArray)
        AudioCapturerAdapterImpl::GetAudioSamplingRate(sampling);

    AudioSamplingRate testSampling =
        AudioCapturerAdapterImpl::GetAudioSamplingRate(static_cast<AudioAdapterSamplingRate>(0));
    EXPECT_EQ(testSampling, AudioSamplingRate::SAMPLE_RATE_44100);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_022.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_022, TestSize.Level1)
{
    AudioCapturerAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_PCM);
    AudioCapturerAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_INVALID);

    AudioEncodingType testEncodingType =
        AudioCapturerAdapterImpl::GetAudioEncodingType(static_cast<AudioAdapterEncodingType>(1));
    EXPECT_EQ(testEncodingType, AudioEncodingType::ENCODING_INVALID);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_023.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_023, TestSize.Level1)
{
    std::array<AudioAdapterSampleFormat, 6> formatArray = {
        AudioAdapterSampleFormat::SAMPLE_U8,
        AudioAdapterSampleFormat::SAMPLE_S16LE,
        AudioAdapterSampleFormat::SAMPLE_S24LE,
        AudioAdapterSampleFormat::SAMPLE_S32LE,
        AudioAdapterSampleFormat::SAMPLE_F32LE,
        AudioAdapterSampleFormat::INVALID_WIDTH,
    };
    for (auto& format : formatArray)
        AudioCapturerAdapterImpl::GetAudioSampleFormat(format);

    AudioSampleFormat testFormat =
        AudioCapturerAdapterImpl::GetAudioSampleFormat(static_cast<AudioAdapterSampleFormat>(-2));
    EXPECT_EQ(testFormat, AudioSampleFormat::INVALID_WIDTH);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_024.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_024, TestSize.Level1)
{
    std::array<AudioAdapterChannel, 8> channelArray = {
        AudioAdapterChannel::MONO,
        AudioAdapterChannel::STEREO,
        AudioAdapterChannel::CHANNEL_3,
        AudioAdapterChannel::CHANNEL_4,
        AudioAdapterChannel::CHANNEL_5,
        AudioAdapterChannel::CHANNEL_6,
        AudioAdapterChannel::CHANNEL_7,
        AudioAdapterChannel::CHANNEL_8,
    };
    for (auto& channel : channelArray)
        AudioCapturerAdapterImpl::GetAudioChannel(channel);

    AudioChannel testChannel = AudioCapturerAdapterImpl::GetAudioChannel(static_cast<AudioAdapterChannel>(0));
    EXPECT_EQ(testChannel, AudioChannel::STEREO);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_025.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_025, TestSize.Level1)
{
    std::array<AudioAdapterSourceType, 6> sourceArray = {
        AudioAdapterSourceType::SOURCE_TYPE_INVALID,
        AudioAdapterSourceType::SOURCE_TYPE_MIC,
        AudioAdapterSourceType::SOURCE_TYPE_VOICE_RECOGNITION,
        AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION,
        AudioAdapterSourceType::SOURCE_TYPE_ULTRASONIC,
    };
    for (auto& source : sourceArray)
        AudioCapturerAdapterImpl::GetAudioSourceType(source);

    SourceType testSource = AudioCapturerAdapterImpl::GetAudioSourceType(static_cast<AudioAdapterSourceType>(-2));
    EXPECT_EQ(testSource, SourceType::SOURCE_TYPE_VOICE_RECOGNITION);
}

/**
 * @tc.name: NWebAudioAdapterTest_Create_026.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_Create_026, TestSize.Level1)
{
    auto audioCapturer = std::make_shared<AudioCapturerAdapterImpl>();
    ASSERT_NE(audioCapturer, nullptr);
    std::shared_ptr<AudioCapturerOptionsAdapterMock> capturerOptions =
        std::make_shared<AudioCapturerOptionsAdapterMock>();
    EXPECT_NE(capturerOptions, nullptr);
    capturerOptions->samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_48000;
    capturerOptions->encoding = AudioAdapterEncodingType::ENCODING_PCM;
    capturerOptions->format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    capturerOptions->channels = AudioAdapterChannel::STEREO;
    capturerOptions->sourceType = AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION;
    capturerOptions->capturerFlags = 0;
    int32_t retNum = audioCapturer->Create(capturerOptions);
    ASSERT_NE(retNum, AudioAdapterCode::AUDIO_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_OnDeviceChange_027.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_OnDeviceChange_027, TestSize.Level1)
{
    auto mock = std::make_shared<AudioManagerDeviceChangeCallbackAdapterMock>();
    ASSERT_NE(mock, nullptr);
    auto callbackAdapter = std::make_shared<AudioManagerDeviceChangeCallbackAdapterImpl>(mock);
    ASSERT_NE(callbackAdapter, nullptr);
    DeviceChangeAction deviceChangeAction = {};
    deviceChangeAction.type = DeviceChangeType::CONNECT;
    deviceChangeAction.flag = DeviceFlag::NONE_DEVICES_FLAG;
    callbackAdapter->OnDeviceChange(deviceChangeAction);
    callbackAdapter->cb_ = nullptr;
    callbackAdapter->OnDeviceChange(deviceChangeAction);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_028.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_028, TestSize.Level1)
{
    std::string deviceName = AudioSystemManagerAdapterImpl::GetInstance().GetDeviceName(DeviceType::DEVICE_TYPE_NONE);
    EXPECT_EQ(deviceName, DEVICE_TYPE_NONE_ZH_CN_TEST);
    bool ret = AudioSystemManagerAdapterImpl::GetInstance().SetLanguage(LANGUAGE_EN);
    EXPECT_EQ(ret, TRUE_OK);
    deviceName = AudioSystemManagerAdapterImpl::GetInstance().GetDeviceName(DeviceType::DEVICE_TYPE_NONE);
    EXPECT_EQ(deviceName, DEVICE_TYPE_NONE_TEST);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_029.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_GetChangeReason_029, TestSize.Level1)
{
    std::array<AudioStreamDeviceChangeReason, 5> reasonArray = {
        AudioStreamDeviceChangeReason::UNKNOWN,
        AudioStreamDeviceChangeReason::NEW_DEVICE_AVAILABLE,
        AudioStreamDeviceChangeReason::OLD_DEVICE_UNAVALIABLE,
        AudioStreamDeviceChangeReason::OVERRODE,
    };

    std::shared_ptr<AudioOutputChangeCallbackAdapter> cb = std::make_shared<AudioOutputChangeCallbackMock>();
    EXPECT_NE(cb, nullptr);
    auto callBack = std::make_shared<AudioOutputChangeCallbackImpl>(cb);
    EXPECT_NE(callBack, nullptr);

    for (auto& reason : reasonArray)
        callBack->GetChangeReason(reason);

    AudioAdapterDeviceChangeReason testReason = callBack->GetChangeReason(
        static_cast<AudioStreamDeviceChangeReason>(-1)
    );
    EXPECT_EQ(testReason, AudioAdapterDeviceChangeReason::UNKNOWN);
}

/**
 * @tc.name: NWebAudioAdapterTest_GetChangeReason_030.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_OnOutputDeviceChange_030, TestSize.Level1)
{
    std::shared_ptr<AudioOutputChangeCallbackAdapter> cb = std::make_shared<AudioOutputChangeCallbackMock>();
    EXPECT_NE(cb, nullptr);
    auto callBack = std::make_shared<AudioOutputChangeCallbackImpl>(cb);
    ASSERT_NE(callBack, nullptr);

    AudioDeviceDescriptor deviceInfo(AudioDeviceDescriptor::DEVICE_INFO);

    AudioStreamDeviceChangeReason reason = AudioStreamDeviceChangeReason::UNKNOWN;
    callBack->OnOutputDeviceChange(deviceInfo, reason);

    reason = AudioStreamDeviceChangeReason::NEW_DEVICE_AVAILABLE;
    callBack->OnOutputDeviceChange(deviceInfo, reason);

    reason = AudioStreamDeviceChangeReason::OLD_DEVICE_UNAVALIABLE;
    callBack->OnOutputDeviceChange(deviceInfo, reason);

    reason = AudioStreamDeviceChangeReason::OVERRODE;
    callBack->OnOutputDeviceChange(deviceInfo, reason);

    reason = static_cast<AudioStreamDeviceChangeReason>(-1);
    callBack->OnOutputDeviceChange(deviceInfo, reason);

    callBack->cb_ = nullptr;
    callBack->OnOutputDeviceChange(deviceInfo, reason);
}

/**
 * @tc.name: NWebAudioAdapterTest_SetAudioOutputChangeCallback_031.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_SetAudioOutputChangeCallback_031, TestSize.Level1)
{
    auto audioOutputChange = std::make_shared<AudioRendererAdapterImpl>();
    ASSERT_NE(audioOutputChange, nullptr);
    int32_t retNum = audioOutputChange->SetAudioOutputChangeCallback(nullptr);
    EXPECT_NE(retNum, 0);

    std::shared_ptr<AudioOutputChangeCallbackAdapter> callback = std::make_shared<AudioOutputChangeCallbackMock>();
    ASSERT_NE(callback, nullptr);
    retNum = audioOutputChange->SetAudioOutputChangeCallback(callback);
    EXPECT_NE(retNum, 0);

    AudioRendererOptions rendererOptions;
    AudioRendererInfo renderInfo;
    rendererOptions.streamInfo.samplingRate = AudioSamplingRate::SAMPLE_RATE_44100;
    rendererOptions.streamInfo.encoding = AudioEncodingType::ENCODING_PCM;
    rendererOptions.streamInfo.format = AudioSampleFormat::SAMPLE_S16LE;
    rendererOptions.streamInfo.channels = AudioChannel::STEREO;
    rendererOptions.rendererInfo = renderInfo;
    audioOutputChange->audio_renderer_ = AudioRenderer::Create(rendererOptions);
    ASSERT_NE(audioOutputChange->audio_renderer_, nullptr);

    retNum = audioOutputChange->SetAudioOutputChangeCallback(callback);
    EXPECT_EQ(retNum, 0);
}

} // namespace NWeb
} // namespace OHOS

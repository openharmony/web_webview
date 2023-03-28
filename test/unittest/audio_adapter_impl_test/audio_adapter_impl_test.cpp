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
#include <string>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "audio_renderer_adapter.h"
#include "audio_renderer_adapter_impl.h"
#include "audio_system_manager_adapter_impl.h"
#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/application_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace {
const int RESULT_OK = 0;
const bool TRUE_OK = true;
const std::string CACHE_PATH = "/data/local/tmp";
std::shared_ptr<NWeb::AudioRendererAdapterImpl> g_audioRender = nullptr;
std::shared_ptr<AbilityRuntime::ApplicationContext> g_applicationContext = nullptr;
} // namespace

namespace AbilityRuntime {
    std::shared_ptr<ApplicationContext> Context::GetApplicationContext()
    {
        return g_applicationContext;
    }
} // namespace OHOS::AbilityRuntime

namespace NWeb {
class NWebAudioAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
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

void NWebAudioAdapterTest::SetUpTestCase(void) {}

void NWebAudioAdapterTest::TearDownTestCase(void) {}

void NWebAudioAdapterTest::SetUp(void) {}

void NWebAudioAdapterTest::TearDown(void) {}

class AudioRendererWriteCallbackAdapterMock : public AudioRendererWriteCallbackAdapter {
public:
    void OnWriteData(size_t length) override {};
};

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_001.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_001, TestSize.Level1)
{
    g_audioRender = std::make_shared<AudioRendererAdapterImpl>();
    ASSERT_NE(g_audioRender, nullptr);

    AudioAdapterRendererOptions rendererOptions;
    rendererOptions.samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions.channels = AudioAdapterChannel::STEREO;
    rendererOptions.contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions.streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions.rendererFlags = 0;
    int32_t retNum = g_audioRender->Create(rendererOptions, CACHE_PATH);
    ASSERT_EQ(retNum, AudioAdapterCode::AUDIO_OK);

    bool ret = g_audioRender->Start();
    EXPECT_EQ(ret, TRUE_OK);

    std::array<uint8_t, 4> bufferArray = {0, 0, 0, 0};
    retNum = g_audioRender->Write(bufferArray.data(), bufferArray.size());
    EXPECT_EQ(retNum, bufferArray.size());

    uint64_t latency;
    retNum = g_audioRender->GetLatency(latency);
    EXPECT_EQ(retNum, RESULT_OK);

    float volume = 0.8;
    retNum = g_audioRender->SetVolume(volume);
    EXPECT_EQ(retNum, RESULT_OK);

    float nowVolume = g_audioRender->GetVolume();
    EXPECT_EQ(nowVolume, volume);

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

    AudioSamplingRate testSampling = AudioRendererAdapterImpl::GetAudioSamplingRate(
        static_cast<AudioAdapterSamplingRate>(0));
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

    AudioEncodingType testEncodingType = AudioRendererAdapterImpl::GetAudioEncodingType(
        static_cast<AudioAdapterEncodingType>(1));
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

    AudioSampleFormat testFormat = AudioRendererAdapterImpl::GetAudioSampleFormat(
        static_cast<AudioAdapterSampleFormat>(-2));
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

    AudioChannel testChannel = AudioRendererAdapterImpl::GetAudioChannel(
        static_cast<AudioAdapterChannel>(0));
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

    ContentType testContent = AudioRendererAdapterImpl::GetAudioContentType(
        static_cast<AudioAdapterContentType>(-1));
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

    StreamUsage testUsage = AudioRendererAdapterImpl::GetAudioStreamUsage(
        static_cast<AudioAdapterStreamUsage>(-1));
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

    ret = AudioSystemManagerAdapterImpl::GetInstance().HasAudioInputDevices();
    EXPECT_EQ(ret, TRUE_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_009.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_009, TestSize.Level1)
{
    AudioAdapterInterrupt interrupt;
    interrupt.streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    interrupt.contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    interrupt.streamType = AudioAdapterStreamType::STREAM_MUSIC;

    int32_t ret = AudioSystemManagerAdapterImpl::GetInstance().RequestAudioFocus(interrupt);
    EXPECT_EQ(ret, RESULT_OK);

    ret = AudioSystemManagerAdapterImpl::GetInstance().AbandonAudioFocus(interrupt);
    EXPECT_EQ(ret, RESULT_OK);
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

    int32_t ret = AudioSystemManagerAdapterImpl::GetInstance().SetAudioManagerInterruptCallback(callback);
    EXPECT_EQ(ret, RESULT_OK);

    ret = AudioSystemManagerAdapterImpl::GetInstance().UnsetAudioManagerInterruptCallback();
    EXPECT_EQ(ret, RESULT_OK);
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
    AudioStreamType testStream = AudioSystemManagerAdapterImpl::GetInstance().GetStreamType(
        static_cast<AudioAdapterStreamType>(-2));
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

    AudioAdapterRendererOptions rendererOptions;
    rendererOptions.samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions.channels = AudioAdapterChannel::STEREO;
    rendererOptions.contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions.streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions.rendererFlags = 0;
    int32_t retNum = g_audioRender->Create(rendererOptions);
    ASSERT_NE(retNum, AudioAdapterCode::AUDIO_OK);

    bool ret = g_audioRender->Start();
    EXPECT_NE(ret, TRUE_OK);

    std::array<uint8_t, 4> bufferArray = {0, 0, 0, 0};
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

    BufferDescAdapter bufDesc;
    retNum = g_audioRender->GetBufferDesc(bufDesc);
    EXPECT_NE(retNum, AudioAdapterCode::AUDIO_OK);
    retNum = g_audioRender->Enqueue(bufDesc);
    EXPECT_NE(retNum, AudioAdapterCode::AUDIO_OK);
    auto writeCallback = std::make_shared<AudioRendererWriteCallbackAdapterMock>();
    EXPECT_NE(writeCallback, nullptr);
    retNum = g_audioRender->SetAudioRendererWriteCallbackAdapter(writeCallback);
    EXPECT_NE(retNum, AudioAdapterCode::AUDIO_OK);

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
    ApplicationContextMock *contextMock = new ApplicationContextMock();
    EXPECT_NE(contextMock, nullptr);
    EXPECT_CALL(*contextMock, GetCacheDir())
        .Times(1)
        .WillRepeatedly(::testing::Return(""));
    EXPECT_EQ(g_applicationContext, nullptr);
    g_applicationContext.reset(contextMock);
    EXPECT_NE(g_applicationContext, nullptr);

    std::shared_ptr<AudioRendererAdapterImpl> audioRenderImpl = std::make_shared<AudioRendererAdapterImpl>();
    EXPECT_NE(audioRenderImpl, nullptr);

    AudioAdapterRendererOptions rendererOptions;
    rendererOptions.samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions.channels = AudioAdapterChannel::STEREO;
    rendererOptions.contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions.streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions.rendererFlags = 0;
    int32_t retNum = audioRenderImpl->Create(rendererOptions);
    g_applicationContext.reset();
    EXPECT_EQ(g_applicationContext, nullptr);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_ERROR);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_015.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_015, TestSize.Level1)
{
    ApplicationContextMock *contextMock = new ApplicationContextMock();
    EXPECT_NE(contextMock, nullptr);
    EXPECT_CALL(*contextMock, GetCacheDir())
        .Times(1)
        .WillRepeatedly(::testing::Return(CACHE_PATH));
    EXPECT_EQ(g_applicationContext, nullptr);
    g_applicationContext.reset(contextMock);
    EXPECT_NE(g_applicationContext, nullptr);

    std::shared_ptr<AudioRendererAdapterImpl> audioRenderImpl = std::make_shared<AudioRendererAdapterImpl>();
    EXPECT_NE(audioRenderImpl, nullptr);

    AudioAdapterRendererOptions rendererOptions;
    rendererOptions.samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions.channels = AudioAdapterChannel::STEREO;
    rendererOptions.contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions.streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions.rendererFlags = 0;
    int32_t retNum = audioRenderImpl->Create(rendererOptions);
    g_applicationContext.reset();
    EXPECT_EQ(g_applicationContext, nullptr);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_GetBufferDesc_016.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5HRX9
 */
HWTEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_GetBufferDesc_016, TestSize.Level1)
{
    std::shared_ptr<AudioRendererAdapterImpl> audioRenderImpl = std::make_shared<AudioRendererAdapterImpl>();
    EXPECT_NE(audioRenderImpl, nullptr);

    AudioAdapterRendererOptions rendererOptions;
    rendererOptions.samplingRate = AudioAdapterSamplingRate::SAMPLE_RATE_44100;
    rendererOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
    rendererOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
    rendererOptions.channels = AudioAdapterChannel::STEREO;
    rendererOptions.contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    rendererOptions.streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    rendererOptions.rendererFlags = 0;
    int32_t retNum = audioRenderImpl->Create(rendererOptions, CACHE_PATH);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);

    auto writeCallback = std::make_shared<AudioRendererWriteCallbackAdapterMock>();
    EXPECT_NE(writeCallback, nullptr);
    retNum = audioRenderImpl->SetAudioRendererWriteCallbackAdapter(nullptr);
    EXPECT_NE(retNum, AudioAdapterCode::AUDIO_OK);
    retNum = audioRenderImpl->SetAudioRendererWriteCallbackAdapter(writeCallback);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);
    BufferDescAdapter bufDesc;
    retNum = audioRenderImpl->GetBufferDesc(bufDesc);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);
    retNum = audioRenderImpl->Enqueue(bufDesc);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);

    AudioRendererWriteCallbackImpl writeCallbackImplNull(nullptr);
    writeCallbackImplNull.OnWriteData(0);
    AudioRendererWriteCallbackImpl writeCallbackImpl(writeCallback);
    writeCallbackImpl.OnWriteData(0);
}
}  // namespace OHOS::NWeb
}  // namespace OHOS
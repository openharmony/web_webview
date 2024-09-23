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
#include <testing/gmock/include/gmock/gmock.h>
#include <testing/gtest/include/gtest/gtest.h>

#define private public
#include <accesstoken/ability_access_control.h>

#include "audio_capturer_adapter.h"
#include "audio_capturer_adapter_impl.h"
#include "audio_renderer_adapter.h"
#include "audio_renderer_adapter_impl.h"
#include "audio_system_manager_adapter_impl.h"

using namespace testing;

namespace OHOS {
namespace {
const int RESULT_OK = 0;
const bool TRUE_OK = true;
const std::string LANGUAGE_EN = "en";
const std::string DEVICE_TYPE_INVALID_TEST = "device/invalid";
const std::string DEVICE_TYPE_INVALID_ZH_CN_TEST = "未知设备";
const std::string CACHE_PATH = "/data/local/tmp";
std::shared_ptr<NWeb::AudioCapturerAdapterImpl> g_audioCapturer = nullptr;
const int32_t ADAPTER_AUDIO_UNDEFINED_DEVICEID = 1000001;
} // namespace

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

    int32_t GetFrameSize()
    {
        return frame_size_;
    }
    AudioAdapterSamplingRate samplingRate;
    AudioAdapterEncodingType encoding;
    AudioAdapterSampleFormat format;
    AudioAdapterChannel channels;
    AudioAdapterContentType contentType;
    AudioAdapterStreamUsage streamUsage;
    int32_t rendererFlags;
    int32_t frame_size_;
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

    int32_t OnWriteDataCallback(void* buffer, int32_t length) override
    {
        return AUDIO_DATA_CALLBACK_RESULT_INVALID;
    };
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
    std::string permissionName = "ohos.permission.MICROPHONE";
    OH_AT_CheckSelfPermission(permissionName.c_str());
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
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_001)
{
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
    rendererOptions->frame_size_ = 4096; //  4096: frame size of pcm data
    int32_t retNum = audioRenderImpl->Create(rendererOptions);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);

    bool ret = audioRenderImpl->Start();
    EXPECT_EQ(ret, TRUE_OK);

    uint64_t latency;
    retNum = audioRenderImpl->GetLatency(latency);
    EXPECT_EQ(retNum, RESULT_OK);

    float volume = 0.8;
    retNum = audioRenderImpl->SetVolume(volume);
    EXPECT_EQ(retNum, RESULT_OK);

    float nowVolume = audioRenderImpl->GetVolume();
    EXPECT_EQ(nowVolume, volume);
    retNum = audioRenderImpl->SetAudioRendererCallback(nullptr);
    EXPECT_NE(retNum, 0);
    std::shared_ptr<AudioRendererCallbackAdapter> callback = std::make_shared<AudioRendererCallbackMock>();
    EXPECT_NE(callback, nullptr);
    retNum = audioRenderImpl->SetAudioRendererCallback(callback);
    EXPECT_EQ(retNum, 0);
    audioRenderImpl->SetInterruptMode(true);
    audioRenderImpl->SetInterruptMode(false);
    ret = audioRenderImpl->IsRendererStateRunning();
    EXPECT_EQ(ret, TRUE_OK);
    ret = audioRenderImpl->Pause();
    EXPECT_EQ(ret, TRUE_OK);
    ret = audioRenderImpl->Stop();
    EXPECT_EQ(ret, TRUE_OK);
    ret = audioRenderImpl->Release();
    EXPECT_EQ(ret, TRUE_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_002.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_002)
{
    std::array<AudioAdapterSamplingRate, 11U> samplingArray = {
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

    int32_t testSampling = AudioRendererAdapterImpl::GetAudioSamplingRate(static_cast<AudioAdapterSamplingRate>(0));
    EXPECT_EQ(testSampling, 44100U); // default sampling rate 44100
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_003.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_003)
{
    AudioRendererAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_PCM);
    AudioRendererAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_INVALID);

    OH_AudioStream_EncodingType testEncodingType =
        AudioRendererAdapterImpl::GetAudioEncodingType(static_cast<AudioAdapterEncodingType>(1));
    EXPECT_EQ(testEncodingType, AUDIOSTREAM_ENCODING_TYPE_AUDIOVIVID);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_004.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_004)
{
    std::array<AudioAdapterSampleFormat, 6U> formatArray = {
        AudioAdapterSampleFormat::SAMPLE_U8,
        AudioAdapterSampleFormat::SAMPLE_S16LE,
        AudioAdapterSampleFormat::SAMPLE_S24LE,
        AudioAdapterSampleFormat::SAMPLE_S32LE,
        AudioAdapterSampleFormat::SAMPLE_F32LE,
        AudioAdapterSampleFormat::INVALID_WIDTH,
    };
    for (auto& format : formatArray)
        AudioRendererAdapterImpl::GetAudioSampleFormat(format);

    OH_AudioStream_SampleFormat testFormat =
        AudioRendererAdapterImpl::GetAudioSampleFormat(static_cast<AudioAdapterSampleFormat>(-2));
    EXPECT_EQ(testFormat, AUDIOSTREAM_SAMPLE_S16LE);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_005.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_005)
{
    std::array<AudioAdapterChannel, 8U> channelArray = {
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

    int32_t testChannel = AudioRendererAdapterImpl::GetAudioChannel(static_cast<AudioAdapterChannel>(0));
    EXPECT_EQ(testChannel, 2U); // default audio channels 2
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_007.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_007)
{
    std::array<AudioAdapterStreamUsage, 5U> usageArray = {
        AudioAdapterStreamUsage::STREAM_USAGE_UNKNOWN,
        AudioAdapterStreamUsage::STREAM_USAGE_MEDIA,
        AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION,
        AudioAdapterStreamUsage::STREAM_USAGE_VOICE_ASSISTANT,
        AudioAdapterStreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE,
    };
    for (auto& usage : usageArray)
        AudioRendererAdapterImpl::GetAudioStreamUsage(usage);

    OH_AudioStream_Usage testUsage =
        AudioRendererAdapterImpl::GetAudioStreamUsage(static_cast<AudioAdapterStreamUsage>(-1));
    EXPECT_EQ(testUsage, AUDIOSTREAM_USAGE_MUSIC);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_008.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_008)
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

    OH_AudioDeviceDescriptorArray* audioDeviceArray = nullptr;
    OH_AudioRoutingManager* audioRoutingManager;
    OH_AudioManager_GetAudioRoutingManager(&audioRoutingManager);
    OH_AudioCommon_Result ohRet = OH_AudioRoutingManager_GetPreferredOutputDevice(
        audioRoutingManager, AUDIOSTREAM_USAGE_VOICE_COMMUNICATION, &audioDeviceArray);
    EXPECT_EQ(ohRet, AUDIOCOMMON_RESULT_SUCCESS);
    int32_t select = AudioSystemManagerAdapterImpl::GetInstance().SelectAudioOutputDevice(false, *audioDeviceArray);
    EXPECT_NE(select, 0);
    select = AudioSystemManagerAdapterImpl::GetInstance().SelectAudioOutputDevice(true, *audioDeviceArray);
    EXPECT_NE(select, 0);
    OH_AudioRoutingManager_ReleaseDevices(audioRoutingManager, audioDeviceArray);
    AudioSystemManagerAdapterImpl::GetInstance().GetDefaultInputDevice();
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_009.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_009)
{
    std::shared_ptr<AudioInterruptAdapterMock> interrupt = std::make_shared<AudioInterruptAdapterMock>();
    EXPECT_NE(interrupt, nullptr);
    interrupt->streamUsage = AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
    interrupt->contentType = AudioAdapterContentType::CONTENT_TYPE_MUSIC;
    interrupt->streamType = AudioAdapterStreamType::STREAM_DEFAULT;

    int32_t ret = AudioSystemManagerAdapterImpl::GetInstance().RequestAudioFocus(interrupt);
    EXPECT_EQ(ret, RESULT_OK); // invalid interface
    ret = AudioSystemManagerAdapterImpl::GetInstance().AbandonAudioFocus(interrupt);
    EXPECT_EQ(ret, RESULT_OK); // invalid interface

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
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_010)
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
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_013.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_013)
{
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
    rendererOptions->frame_size_ = 4096; //  4096: frame size of pcm data
    int32_t retNum = audioRenderImpl->Create(rendererOptions);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);

    bool ret = audioRenderImpl->Start();
    EXPECT_EQ(ret, TRUE_OK);

    uint64_t latency;
    retNum = audioRenderImpl->GetLatency(latency);
    EXPECT_EQ(retNum, RESULT_OK);

    float volume = 0.8;
    retNum = audioRenderImpl->SetVolume(volume);
    EXPECT_EQ(retNum, RESULT_OK);

    float nowVolume = audioRenderImpl->GetVolume();
    EXPECT_EQ(nowVolume, volume);

    ret = audioRenderImpl->Pause();
    EXPECT_EQ(ret, TRUE_OK);

    ret = audioRenderImpl->Stop();
    EXPECT_EQ(ret, TRUE_OK);

    ret = audioRenderImpl->Release();
    EXPECT_EQ(ret, TRUE_OK);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_014.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5RWOG
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_014)
{
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
    rendererOptions->frame_size_ = 4096; //  4096: frame size of pcm data
    int32_t retNum = audioRenderImpl->Create(rendererOptions);
    EXPECT_EQ(retNum, AudioAdapterCode::AUDIO_OK);
    EXPECT_NE(audioRenderImpl->audio_renderer_, nullptr);
    audioRenderImpl->SetInterruptMode(false);
    retNum = audioRenderImpl->Create(nullptr);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_015.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_015)
{
    // No need to configure cached path
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
    rendererOptions->frame_size_ = 4096; //  4096: frame size of pcm data
    audioRenderImpl->Create(rendererOptions);
}

/**
 * @tc.name: NWebAudioAdapterTest_SetAudioRendererCallback_017.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_SetAudioRendererCallback_017)
{
    auto audioRender = std::make_shared<AudioRendererAdapterImpl>();
    ASSERT_NE(audioRender, nullptr);
    std::shared_ptr<AudioRendererCallbackAdapter> callback = std::make_shared<AudioRendererCallbackMock>();
    ASSERT_NE(callback, nullptr);
    int32_t retNum = audioRender->SetAudioRendererCallback(callback);
    EXPECT_EQ(retNum, 0);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_018.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_018)
{
    // delete test for inner interface
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_019.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_019)
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
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_020)
{
    // delete test for class AudioCapturerReadCallbackImpl
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_0021.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_021)
{
    std::array<AudioAdapterSamplingRate, 11U> samplingArray = {
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

    int32_t testSampling = AudioCapturerAdapterImpl::GetAudioSamplingRate(static_cast<AudioAdapterSamplingRate>(0));
    EXPECT_EQ(testSampling, 44100U);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_022.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_022)
{
    AudioCapturerAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_PCM);
    AudioCapturerAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType::ENCODING_INVALID);

    OH_AudioStream_EncodingType testEncodingType =
        AudioCapturerAdapterImpl::GetAudioEncodingType(static_cast<AudioAdapterEncodingType>(1));
    EXPECT_EQ(testEncodingType, AUDIOSTREAM_ENCODING_TYPE_RAW);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_023.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_023)
{
    std::array<AudioAdapterSampleFormat, 6U> formatArray = {
        AudioAdapterSampleFormat::SAMPLE_U8,
        AudioAdapterSampleFormat::SAMPLE_S16LE,
        AudioAdapterSampleFormat::SAMPLE_S24LE,
        AudioAdapterSampleFormat::SAMPLE_S32LE,
        AudioAdapterSampleFormat::SAMPLE_F32LE,
        AudioAdapterSampleFormat::INVALID_WIDTH,
    };
    for (auto& format : formatArray)
        AudioCapturerAdapterImpl::GetAudioSampleFormat(format);

    OH_AudioStream_SampleFormat testFormat =
        AudioCapturerAdapterImpl::GetAudioSampleFormat(static_cast<AudioAdapterSampleFormat>(-2));
    EXPECT_EQ(testFormat, AUDIOSTREAM_SAMPLE_U8);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_024.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_024)
{
    std::array<AudioAdapterChannel, 8U> channelArray = {
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

    int32_t testChannel = AudioCapturerAdapterImpl::GetAudioChannel(static_cast<AudioAdapterChannel>(0));
    EXPECT_EQ(testChannel, 2U);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_025.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_025)
{
    std::array<AudioAdapterSourceType, 6U> sourceArray = {
        AudioAdapterSourceType::SOURCE_TYPE_INVALID,
        AudioAdapterSourceType::SOURCE_TYPE_MIC,
        AudioAdapterSourceType::SOURCE_TYPE_VOICE_RECOGNITION,
        AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION,
        AudioAdapterSourceType::SOURCE_TYPE_ULTRASONIC,
    };
    for (auto& source : sourceArray)
        AudioCapturerAdapterImpl::GetAudioSourceType(source);

    OH_AudioStream_SourceType testSource =
        AudioCapturerAdapterImpl::GetAudioSourceType(static_cast<AudioAdapterSourceType>(-2));
    EXPECT_EQ(testSource, AUDIOSTREAM_SOURCE_TYPE_VOICE_RECOGNITION);
}

/**
 * @tc.name: NWebAudioAdapterTest_Create_026.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:AR000I7I7N
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_Create_026)
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
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_028.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_AudioAdapterImpl_028)
{
    std::string deviceName = AudioSystemManagerAdapterImpl::GetInstance().GetDeviceName(AUDIO_DEVICE_TYPE_INVALID);
    EXPECT_EQ(deviceName, DEVICE_TYPE_INVALID_ZH_CN_TEST);
    bool ret = AudioSystemManagerAdapterImpl::GetInstance().SetLanguage(LANGUAGE_EN);
    EXPECT_EQ(ret, TRUE_OK);
    deviceName = AudioSystemManagerAdapterImpl::GetInstance().GetDeviceName(AUDIO_DEVICE_TYPE_INVALID);
    EXPECT_EQ(deviceName, DEVICE_TYPE_INVALID_TEST);
}

/**
 * @tc.name: NWebAudioAdapterTest_AudioAdapterImpl_029.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_GetChangeReason_029)
{
    std::array<OH_AudioStream_DeviceChangeReason, 5U> reasonArray = {
        REASON_UNKNOWN,
        REASON_NEW_DEVICE_AVAILABLE,
        REASON_OLD_DEVICE_UNAVAILABLE,
        REASON_OVERRODE,
    };

    for (auto& reason : reasonArray)
        AudioRendererAdapterImpl::GetChangeReason(reason);

    AudioAdapterDeviceChangeReason testReason = AudioRendererAdapterImpl::GetChangeReason(REASON_UNKNOWN);
    EXPECT_EQ(testReason, AudioAdapterDeviceChangeReason::UNKNOWN);
}

/**
 * @tc.name: NWebAudioAdapterTest_SetAudioOutputChangeCallback_031.
 * @tc.desc: Audio adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5HRX9
 */
TEST_F(NWebAudioAdapterTest, NWebAudioAdapterTest_SetAudioOutputChangeCallback_031)
{
    auto audioOutputChange = std::make_shared<AudioRendererAdapterImpl>();
    ASSERT_NE(audioOutputChange, nullptr);
    int32_t retNum = audioOutputChange->SetAudioOutputChangeCallback(nullptr);
    EXPECT_NE(retNum, 0);

    std::shared_ptr<AudioOutputChangeCallbackAdapter> callback = std::make_shared<AudioOutputChangeCallbackAdapter>();
    ASSERT_NE(callback, nullptr);
    retNum = audioOutputChange->SetAudioOutputChangeCallback(callback);
    EXPECT_EQ(retNum, 0);
}

} // namespace NWeb
} // namespace OHOS

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

#include "audio_renderer_adapter_impl.h"

#include <unordered_map>

// #include "nweb_log.h"

namespace OHOS::NWeb {
constexpr int64_t NS_TO_S = 1000000000; // 1s = 1000000000ns
constexpr int32_t DEFAULT_SAMPLING_RATE = 44100; // 44100:see SAMPLING_RATE_MAP
constexpr int32_t DEFAULT_AUDIO_CHANNELS = 2; // 2:see AUDIO_CHANNEL_MAP

const std::unordered_map<AudioAdapterSamplingRate, int32_t> SAMPLING_RATE_MAP = {
    { AudioAdapterSamplingRate::SAMPLE_RATE_8000, 8000 },   // Common sampling rate 8000
    { AudioAdapterSamplingRate::SAMPLE_RATE_11025, 11025 }, // Common sampling rate 11025
    { AudioAdapterSamplingRate::SAMPLE_RATE_12000, 12000 }, // Common sampling rate 12000
    { AudioAdapterSamplingRate::SAMPLE_RATE_16000, 16000 }, // Common sampling rate 16000
    { AudioAdapterSamplingRate::SAMPLE_RATE_22050, 22050 }, // Common sampling rate 22050
    { AudioAdapterSamplingRate::SAMPLE_RATE_24000, 24000 }, // Common sampling rate 24000
    { AudioAdapterSamplingRate::SAMPLE_RATE_32000, 32000 }, // Common sampling rate 32000
    { AudioAdapterSamplingRate::SAMPLE_RATE_44100, 44100 }, // Common sampling rate 44100
    { AudioAdapterSamplingRate::SAMPLE_RATE_48000, 48000 }, // Common sampling rate 48000
    { AudioAdapterSamplingRate::SAMPLE_RATE_64000, 64000 }, // Common sampling rate 64000
    { AudioAdapterSamplingRate::SAMPLE_RATE_96000, 96000 }, // Common sampling rate 96000
};

const std::unordered_map<AudioAdapterEncodingType, OH_AudioStream_EncodingType> ENCODING_TYPE_MAP = {
    { AudioAdapterEncodingType::ENCODING_PCM, AUDIOSTREAM_ENCODING_TYPE_RAW },
    { AudioAdapterEncodingType::ENCODING_INVALID, AUDIOSTREAM_ENCODING_TYPE_AUDIOVIVID }
};

const std::unordered_map<AudioAdapterSampleFormat, OH_AudioStream_SampleFormat> SAMPLE_FORMAT_MAP = {
    { AudioAdapterSampleFormat::SAMPLE_U8, AUDIOSTREAM_SAMPLE_U8 },
    { AudioAdapterSampleFormat::SAMPLE_S16LE, AUDIOSTREAM_SAMPLE_S16LE },
    { AudioAdapterSampleFormat::SAMPLE_S24LE, AUDIOSTREAM_SAMPLE_S24LE },
    { AudioAdapterSampleFormat::SAMPLE_S32LE, AUDIOSTREAM_SAMPLE_S32LE },
};

const std::unordered_map<AudioAdapterChannel, int32_t> AUDIO_CHANNEL_MAP = {
    { AudioAdapterChannel::MONO, 1 },      // 1 audio channels
    { AudioAdapterChannel::STEREO, 2 },    // 2 audio channels
    { AudioAdapterChannel::CHANNEL_3, 3 }, // 3 audio channels
    { AudioAdapterChannel::CHANNEL_4, 4 }, // 4 audio channels
    { AudioAdapterChannel::CHANNEL_5, 5 }, // 5 audio channels
    { AudioAdapterChannel::CHANNEL_6, 6 }, // 6 audio channels
    { AudioAdapterChannel::CHANNEL_7, 7 }, // 7 audio channels
    { AudioAdapterChannel::CHANNEL_8, 8 }, // 8 audio channels
};

const std::unordered_map<AudioAdapterStreamUsage, OH_AudioStream_Usage> STREAM_USAGE_MAP = {
    { AudioAdapterStreamUsage::STREAM_USAGE_UNKNOWN, AUDIOSTREAM_USAGE_UNKNOWN },
    { AudioAdapterStreamUsage::STREAM_USAGE_MEDIA, AUDIOSTREAM_USAGE_MOVIE },
    { AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION, AUDIOSTREAM_USAGE_VIDEO_COMMUNICATION },
    { AudioAdapterStreamUsage::STREAM_USAGE_VOICE_ASSISTANT, AUDIOSTREAM_USAGE_VOICE_ASSISTANT },
    { AudioAdapterStreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE, AUDIOSTREAM_USAGE_RINGTONE },
};

const std::unordered_map<int32_t, OH_AudioStream_LatencyMode> LATENCY_MODE_MAP = {
    { 0, AUDIOSTREAM_LATENCY_MODE_NORMAL }, // 0:ohaudio/native_audiostream_base.h
    { 1, AUDIOSTREAM_LATENCY_MODE_FAST }, // 1:ohaudio/native_audiostream_base.h
};

const std::unordered_map<OH_AudioStream_DeviceChangeReason, AudioAdapterDeviceChangeReason> CHANGE_REASON_MAP = {
    { REASON_UNKNOWN, AudioAdapterDeviceChangeReason::UNKNOWN },
    { REASON_NEW_DEVICE_AVAILABLE, AudioAdapterDeviceChangeReason::NEW_DEVICE_AVAILABLE },
    { REASON_OLD_DEVICE_UNAVAILABLE, AudioAdapterDeviceChangeReason::OLD_DEVICE_UNAVALIABLE },
    { REASON_OVERRODE, AudioAdapterDeviceChangeReason::OVERRODE },
};

static int64_t GetCurrentRealTimeNs()
{
    struct timespec ts = { 0, 0 };
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return (ts.tv_sec * NS_TO_S + ts.tv_nsec);
}

static OH_AudioData_Callback_Result AudioRendererOnWriteDataCallback(
    OH_AudioRenderer* renderer,
    void* userData,
    void* buffer,
    int32_t length) {
    if (userData) {
        return ((AudioRendererAdapterImpl*)(userData))->OnWriteDataCallback(buffer, length);
    }
    return AUDIO_DATA_CALLBACK_RESULT_INVALID;
}

static int32_t AudioRendererOnError(OH_AudioRenderer* renderer,
                                    void* userData,
                                    OH_AudioStream_Result error)
{
    // WVLOG_E("audio renderer error:%{public}d", error);
    if (userData) {
        return ((AudioRendererAdapterImpl*)(userData))->OnError(error);
    }
    return 0;
}

static int32_t AudioRendererOnInterruptEvent(OH_AudioRenderer* renderer,
                                             void* userData,
                                             OH_AudioInterrupt_ForceType type,
                                             OH_AudioInterrupt_Hint hint)
{
    // WVLOG_I("audio render interrupt event, type:%{public}d, hint:%{public}d", type, hint);
    if (userData) {
        return ((AudioRendererAdapterImpl*)(userData))->OnInterruptEvent(type, hint);
    }
    return 0;
}

static void AudioRendererOnOutputDeviceChange(
    OH_AudioRenderer* renderer,
    void* userData,
    OH_AudioStream_DeviceChangeReason reason)
{
    // WVLOG_I("audio renderer device change, reason:%{public}d", reason);
    if (userData) {
        ((AudioRendererAdapterImpl*)(userData))->OnOutputDeviceChange(reason);
    }
}

AudioRendererAdapterImpl::~AudioRendererAdapterImpl()
{
    // Make sure to stop the audio playback and release the audio instance
    // WVLOG_I("audio renderer destructor");
    if (audio_renderer_ != nullptr) {
        if (IsRendererStateRunning()) {
            if (OH_AudioRenderer_Stop(audio_renderer_) != AUDIOSTREAM_SUCCESS) {
                // WVLOG_E("Failed to stop renderer");
            }
        }
        if (OH_AudioRenderer_Release(audio_renderer_) != AUDIOSTREAM_SUCCESS) {
            // WVLOG_E("Failed to release renderer");
        }
        audio_renderer_ = nullptr;
    }

    if (audio_stream_builder_ != nullptr) {
        if (OH_AudioStreamBuilder_Destroy(audio_stream_builder_) != AUDIOSTREAM_SUCCESS) {
            // WVLOG_E("Failed to release stream builder");
        }
        audio_stream_builder_ = nullptr;
    }
}

int32_t AudioRendererAdapterImpl::OnError(OH_AudioStream_Result error)
{
    // WVLOG_E("audio renderer error:%{public}d", error);
    return 0;
}

int32_t AudioRendererAdapterImpl::OnInterruptEvent(OH_AudioInterrupt_ForceType type, OH_AudioInterrupt_Hint hint)
{
    if (!callback_) {
        // WVLOG_E("callback is nullptr!");
        return 0;
    }
    switch (hint) {
        case AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
            callback_->OnSuspend();
            break;
        case AUDIOSTREAM_INTERRUPT_HINT_STOP:
            callback_->OnSuspend();
            break;
        case AUDIOSTREAM_INTERRUPT_HINT_RESUME:
            callback_->OnResume();
            break;
        default:
            // WVLOG_E("audio renderer interrupt hint not foud, code: %{public}d", hint);
            break;
    }
    return 0;
}

OH_AudioData_Callback_Result AudioRendererAdapterImpl::OnWriteDataCallback(void* buffer, int32_t length)
{
    if (!callback_) {
        // WVLOG_E("callback is nullptr!");
        return AUDIO_DATA_CALLBACK_RESULT_INVALID;
    }
    int32_t ret = callback_->OnWriteDataCallback(buffer, length);
    if (ret <= 0) {
        // WVLOG_I("audio renderer write length: %{public}d", ret);
        return AUDIO_DATA_CALLBACK_RESULT_INVALID;
    }
    return AUDIO_DATA_CALLBACK_RESULT_VALID;
}

void AudioRendererAdapterImpl::OnOutputDeviceChange(OH_AudioStream_DeviceChangeReason reason)
{
    if (!ouputChangeCallback_) {
        // WVLOG_E("callback is nullptr!");
        return;
    }
    AudioAdapterDeviceChangeReason reasonAdapter = GetChangeReason(reason);
    // WVLOG_I("OnOutputDeviceChange reason: %{public}d", static_cast<int32_t>(reasonAdapter));
    ouputChangeCallback_->OnOutputDeviceChange(static_cast<int32_t>(reasonAdapter));
}

void AudioRendererAdapterImpl::SetStream(
    const std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions,
    OH_AudioStreamBuilder* audioStreamBuilder)
{
    OHAudioRendererOptions audioOptions;
    TransformToAudioRendererOptions(audioOptions, rendererOptions);
    nsPerFrame_ = NS_TO_S /
                    static_cast<double>(audioOptions.samplingRate);
    // set params
    OH_AudioStreamBuilder_SetSamplingRate(audioStreamBuilder,
                                          audioOptions.samplingRate);
    OH_AudioStreamBuilder_SetEncodingType(audioStreamBuilder,
                                          audioOptions.encodingType);
    OH_AudioStreamBuilder_SetSampleFormat(audioStreamBuilder,
                                          audioOptions.sampleFormat);
    OH_AudioStreamBuilder_SetChannelCount(audioStreamBuilder,
                                          audioOptions.channelCount);
    OH_AudioStreamBuilder_SetLatencyMode(audioStreamBuilder,
                                         audioOptions.latencyMode);
    OH_AudioStreamBuilder_SetFrameSizeInCallback(audioStreamBuilder,
                                                 audioOptions.frameSize);

    // When AUDIOSTREAM_USAGE_VIDEO_COMMUNICATION is used, RendererInfo needs to be set, and
    // SetOffloadAllowed is set to false by default. In other cases, you do not need to set
    // RendererInfo. The default value of SetOffloadAllowed is false.
    if (audioOptions.streamUsage == AUDIOSTREAM_USAGE_VIDEO_COMMUNICATION) {
        OH_AudioStreamBuilder_SetRendererInfo(audioStreamBuilder,
                                              audioOptions.streamUsage);
    }
}

int32_t AudioRendererAdapterImpl::Create(
    const std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions, std::string cachePath)
{
    (void)cachePath; // No need to configure cached path
    if (audio_stream_builder_ != nullptr) {
        // WVLOG_E("audio stream builder already exist!");
        return AUDIO_ERROR;
    }

    OH_AudioStream_Result ret = OH_AudioStreamBuilder_Create(
        &audio_stream_builder_, AUDIOSTREAM_TYPE_RENDERER);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Create audio stream builder failed!");
        return AUDIO_ERROR;
    }

    SetStream(rendererOptions, audio_stream_builder_);

    // set callback
    OH_AudioRenderer_Callbacks callbacks = {0};
    callbacks.OH_AudioRenderer_OnError = AudioRendererOnError;
    callbacks.OH_AudioRenderer_OnInterruptEvent = AudioRendererOnInterruptEvent;
    callbacks.OH_AudioRenderer_OnWriteData = nullptr;
    callbacks.OH_AudioRenderer_OnStreamEvent = nullptr;
    ret = OH_AudioStreamBuilder_SetRendererCallback(audio_stream_builder_, callbacks,
                                                    this);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Create audio render callback failed!");
        return AUDIO_ERROR;
    }
    ret = OH_AudioStreamBuilder_SetRendererWriteDataCallback(audio_stream_builder_,
        AudioRendererOnWriteDataCallback, this);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Create audio render write data calback failed!");
        return AUDIO_ERROR;
    }

    ret = OH_AudioStreamBuilder_SetRendererOutputDeviceChangeCallback(audio_stream_builder_,
        AudioRendererOnOutputDeviceChange, this);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Create audio rendeir output device change callback failed!");
        return AUDIO_ERROR;
    }

    // create audio render
    ret = OH_AudioStreamBuilder_GenerateRenderer(audio_stream_builder_,
                                                 &audio_renderer_);
    if (ret != AUDIOSTREAM_SUCCESS || audio_renderer_ == nullptr) {
        // WVLOG_E("Create audio render failed!");
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

bool AudioRendererAdapterImpl::Start()
{
    // WVLOG_I("audio renderer start");
    if (audio_renderer_ == nullptr) {
        // WVLOG_E("audio renderer is nullptr");
        return false;
    }
    OH_AudioStream_Result ret = OH_AudioRenderer_Start(audio_renderer_);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to start renderer");
        return false;
    }
    return true;
}

bool AudioRendererAdapterImpl::Pause()
{
    // WVLOG_I("audio renderer pause");
    if (audio_renderer_ == nullptr) {
        // WVLOG_E("audio renderer is nullptr");
        return false;
    }
    OH_AudioStream_Result ret = OH_AudioRenderer_Pause(audio_renderer_);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to pause renderer");
        return false;
    }
    return true;
}

bool AudioRendererAdapterImpl::Stop()
{
    // WVLOG_I("audio  renderer stop");
    if (audio_renderer_ == nullptr) {
        // WVLOG_E("audio renderer is nullptr");
        return false;
    }
    OH_AudioStream_Result ret = OH_AudioRenderer_Stop(audio_renderer_);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to stop renderer");
        return false;
    }
    return true;
}

bool AudioRendererAdapterImpl::Release()
{
    // WVLOG_I("audio renderer release");
    if (audio_renderer_ == nullptr) {
        // WVLOG_E("audio renderer is nullptr");
        return false;
    }
    OH_AudioStream_Result ret = OH_AudioRenderer_Release(audio_renderer_);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to release renderer");
        return false;
    }
    audio_renderer_ = nullptr;
    return true;
}

int32_t AudioRendererAdapterImpl::Write(uint8_t* buffer, size_t bufferSize)
{
    // Replace this interface with OnWriteDatacallback.
    (void)buffer;
    (void)bufferSize;
    // WVLOG_E("Failed to write data");
    return AUDIO_ERROR;
}

int32_t AudioRendererAdapterImpl::GetLatency(uint64_t& latency)
{
    if (audio_renderer_ == nullptr) {
        // WVLOG_E("audio renderer is nullptr");
        return AUDIO_NULL_ERROR;
    }

    int64_t existingFrameIndex;
    int64_t existingFramePts;
    OH_AudioStream_Result result =
        OH_AudioRenderer_GetTimestamp(audio_renderer_, CLOCK_MONOTONIC,
                                      &existingFrameIndex, &existingFramePts);
    if (result != OH_AudioStream_Result::AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to get timestamp");
        return AUDIO_ERROR;
    }

    int64_t frames;
    result = OH_AudioRenderer_GetFramesWritten(audio_renderer_, &frames);
    if (result != OH_AudioStream_Result::AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to get frames written");
        return AUDIO_ERROR;
    }

    if (existingFramePts == 0) {
        latency = frames * nsPerFrame_;
        return AUDIO_OK;
    }

    int64_t frameIndexDelta = frames - existingFrameIndex;
    int64_t nextFramePts = existingFramePts + frameIndexDelta * nsPerFrame_;
    latency = nextFramePts - GetCurrentRealTimeNs();
    latency = latency > 0 ? latency : 0;
    return AUDIO_OK;
}

int32_t AudioRendererAdapterImpl::SetVolume(float volume)
{
    if (audio_renderer_ == nullptr) {
        // WVLOG_E("audio renderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    OH_AudioStream_Result ret = OH_AudioRenderer_SetVolume(audio_renderer_, volume);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to set volume");
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

float AudioRendererAdapterImpl::GetVolume()
{
    if (audio_renderer_ == nullptr) {
        // WVLOG_E("audio renderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    float volume = 0;
    OH_AudioStream_Result ret = OH_AudioRenderer_GetVolume(audio_renderer_, &volume);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to set volume");
        return AUDIO_ERROR;
    }
    return volume;
}

int32_t AudioRendererAdapterImpl::SetAudioRendererCallback(
    const std::shared_ptr<AudioRendererCallbackAdapter>& callback)
{
    if (callback == nullptr) {
        // WVLOG_E("set audio manager interrupt callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    callback_ = callback;
    return AUDIO_OK;
}

int32_t AudioRendererAdapterImpl::SetAudioOutputChangeCallback(
    const std::shared_ptr<AudioOutputChangeCallbackAdapter>& callback)
{
    // WVLOG_I("AudioRendererAdapterImpl::SetAudioOutputChangeCallback");
    if (callback == nullptr) {
        // WVLOG_E("set audio manager interrupt callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    ouputChangeCallback_ = callback;
    return AUDIO_OK;
}

void AudioRendererAdapterImpl::SetInterruptMode(bool audioExclusive)
{
    if (audio_stream_builder_ == nullptr) {
        // WVLOG_E("audiostream builder is nullptr");
        return;
    }
    OH_AudioInterrupt_Mode mode =
        audioExclusive ? AUDIOSTREAM_INTERRUPT_MODE_INDEPENDENT : AUDIOSTREAM_INTERRUPT_MODE_SHARE;
    OH_AudioStream_Result ret = OH_AudioStreamBuilder_SetRendererInterruptMode(audio_stream_builder_, mode);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to set interrupt mode");
    }
    return;
}

bool AudioRendererAdapterImpl::IsRendererStateRunning()
{
    if (audio_renderer_ == nullptr) {
        // WVLOG_I("audio renderer is nullptr");
        return false;
    }

    OH_AudioStream_State state;
    OH_AudioStream_Result ret = OH_AudioRenderer_GetCurrentState(audio_renderer_, &state);
    if (ret != AUDIOSTREAM_SUCCESS) {
        // WVLOG_E("Failed to set interrupt mode");
        return false;
    }
    return (state == AUDIOSTREAM_STATE_RUNNING) ? true : false;
}

int32_t AudioRendererAdapterImpl::GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate)
{
    auto item = SAMPLING_RATE_MAP.find(samplingRate);
    if (item == SAMPLING_RATE_MAP.end()) {
        // WVLOG_E("audio sampling rate not found");
        return DEFAULT_SAMPLING_RATE;
    }
    return item->second;
}

OH_AudioStream_EncodingType AudioRendererAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType encodingType)
{
    auto item = ENCODING_TYPE_MAP.find(encodingType);
    if (item == ENCODING_TYPE_MAP.end()) {
        // WVLOG_E("audio encoding type not found");
        return AUDIOSTREAM_ENCODING_TYPE_AUDIOVIVID;
    }
    return item->second;
}

OH_AudioStream_SampleFormat AudioRendererAdapterImpl::GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat)
{
    auto item = SAMPLE_FORMAT_MAP.find(sampleFormat);
    if (item == SAMPLE_FORMAT_MAP.end()) {
        // WVLOG_E("audio sample format not found");
        return AUDIOSTREAM_SAMPLE_S16LE;
    }
    return item->second;
}

int32_t AudioRendererAdapterImpl::GetAudioChannel(AudioAdapterChannel channel)
{
    auto item = AUDIO_CHANNEL_MAP.find(channel);
    if (item == AUDIO_CHANNEL_MAP.end()) {
        // WVLOG_E("audio channel not found");
        return DEFAULT_AUDIO_CHANNELS;
    }
    return item->second;
}

OH_AudioStream_Usage AudioRendererAdapterImpl::GetAudioStreamUsage(AudioAdapterStreamUsage streamUsage)
{
    auto item = STREAM_USAGE_MAP.find(streamUsage);
    if (item == STREAM_USAGE_MAP.end()) {
        // WVLOG_E("audio stream usage not found");
        return AUDIOSTREAM_USAGE_MUSIC;
    }
    return item->second;
}

OH_AudioStream_LatencyMode AudioRendererAdapterImpl::GetAudioLatency(int32_t renderflag)
{
    auto item = LATENCY_MODE_MAP.find(renderflag);
    if (item == LATENCY_MODE_MAP.end()) {
        // WVLOG_E("audio stream usage not found");
        return AUDIOSTREAM_LATENCY_MODE_NORMAL;
    }
    return item->second;
}

AudioAdapterDeviceChangeReason AudioRendererAdapterImpl::GetChangeReason(
    OH_AudioStream_DeviceChangeReason reason)
{
    auto item = CHANGE_REASON_MAP.find(reason);
    if (item == CHANGE_REASON_MAP.end()) {
        // WVLOG_E("device change reason not found");
        return AudioAdapterDeviceChangeReason::UNKNOWN;
    }
    return item->second;
}

void AudioRendererAdapterImpl::TransformToAudioRendererOptions(
    OHAudioRendererOptions& out, const std::shared_ptr<AudioRendererOptionsAdapter>& in)
{
    out.samplingRate = GetAudioSamplingRate(in->GetSamplingRate());
    out.encodingType = GetAudioEncodingType(in->GetEncodingType());
    out.sampleFormat = GetAudioSampleFormat(in->GetSampleFormat());
    out.channelCount = GetAudioChannel(in->GetChannel());
    out.streamUsage = GetAudioStreamUsage(in->GetStreamUsage());
    out.latencyMode = GetAudioLatency(in->GetRenderFlags());
    out.frameSize = in->GetFrameSize();
}

} // namespace OHOS::NWeb

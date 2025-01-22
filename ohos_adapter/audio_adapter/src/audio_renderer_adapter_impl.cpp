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

#include "application_context.h"
#include "audio_errors.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

const std::unordered_map<AudioAdapterSamplingRate, AudioSamplingRate> SAMPLING_RATE_MAP = {
    { AudioAdapterSamplingRate::SAMPLE_RATE_8000, AudioSamplingRate::SAMPLE_RATE_8000 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_11025, AudioSamplingRate::SAMPLE_RATE_11025 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_12000, AudioSamplingRate::SAMPLE_RATE_12000 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_16000, AudioSamplingRate::SAMPLE_RATE_16000 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_22050, AudioSamplingRate::SAMPLE_RATE_22050 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_24000, AudioSamplingRate::SAMPLE_RATE_24000 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_32000, AudioSamplingRate::SAMPLE_RATE_32000 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_44100, AudioSamplingRate::SAMPLE_RATE_44100 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_48000, AudioSamplingRate::SAMPLE_RATE_48000 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_64000, AudioSamplingRate::SAMPLE_RATE_64000 },
    { AudioAdapterSamplingRate::SAMPLE_RATE_96000, AudioSamplingRate::SAMPLE_RATE_96000 },
};

const std::unordered_map<AudioAdapterEncodingType, AudioEncodingType> ENCODING_TYPE_MAP = {
    { AudioAdapterEncodingType::ENCODING_PCM, AudioEncodingType::ENCODING_PCM },
    { AudioAdapterEncodingType::ENCODING_INVALID, AudioEncodingType::ENCODING_INVALID }
};

const std::unordered_map<AudioAdapterSampleFormat, AudioSampleFormat> SAMPLE_FORMAT_MAP = {
    { AudioAdapterSampleFormat::SAMPLE_U8, AudioSampleFormat::SAMPLE_U8 },
    { AudioAdapterSampleFormat::SAMPLE_S16LE, AudioSampleFormat::SAMPLE_S16LE },
    { AudioAdapterSampleFormat::SAMPLE_S24LE, AudioSampleFormat::SAMPLE_S24LE },
    { AudioAdapterSampleFormat::SAMPLE_S32LE, AudioSampleFormat::SAMPLE_S32LE },
    { AudioAdapterSampleFormat::SAMPLE_F32LE, AudioSampleFormat::SAMPLE_F32LE },
};

const std::unordered_map<AudioAdapterChannel, AudioChannel> AUDIO_CHANNEL_MAP = {
    { AudioAdapterChannel::MONO, AudioChannel::MONO },
    { AudioAdapterChannel::STEREO, AudioChannel::STEREO },
    { AudioAdapterChannel::CHANNEL_3, AudioChannel::CHANNEL_3 },
    { AudioAdapterChannel::CHANNEL_4, AudioChannel::CHANNEL_4 },
    { AudioAdapterChannel::CHANNEL_5, AudioChannel::CHANNEL_5 },
    { AudioAdapterChannel::CHANNEL_6, AudioChannel::CHANNEL_6 },
    { AudioAdapterChannel::CHANNEL_7, AudioChannel::CHANNEL_7 },
    { AudioAdapterChannel::CHANNEL_8, AudioChannel::CHANNEL_8 },
};

const std::unordered_map<AudioAdapterContentType, ContentType> CONTENT_TYPE_MAP = {
    { AudioAdapterContentType::CONTENT_TYPE_UNKNOWN, ContentType::CONTENT_TYPE_UNKNOWN },
    { AudioAdapterContentType::CONTENT_TYPE_SPEECH, ContentType::CONTENT_TYPE_SPEECH },
    { AudioAdapterContentType::CONTENT_TYPE_MUSIC, ContentType::CONTENT_TYPE_MUSIC },
    { AudioAdapterContentType::CONTENT_TYPE_MOVIE, ContentType::CONTENT_TYPE_MOVIE },
    { AudioAdapterContentType::CONTENT_TYPE_SONIFICATION, ContentType::CONTENT_TYPE_SONIFICATION },
    { AudioAdapterContentType::CONTENT_TYPE_RINGTONE, ContentType::CONTENT_TYPE_RINGTONE },
};

const std::unordered_map<AudioAdapterStreamUsage, StreamUsage> STREAM_USAGE_MAP = {
    { AudioAdapterStreamUsage::STREAM_USAGE_UNKNOWN, StreamUsage::STREAM_USAGE_UNKNOWN },
    { AudioAdapterStreamUsage::STREAM_USAGE_MEDIA, StreamUsage::STREAM_USAGE_MEDIA },
    { AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION, StreamUsage::STREAM_USAGE_VIDEO_COMMUNICATION },
    { AudioAdapterStreamUsage::STREAM_USAGE_VOICE_ASSISTANT, StreamUsage::STREAM_USAGE_VOICE_ASSISTANT },
    { AudioAdapterStreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE, StreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE },
};

const std::unordered_map<AudioStreamDeviceChangeReason, AudioAdapterDeviceChangeReason> CHANGE_REASON_MAP = {
    { AudioStreamDeviceChangeReason::UNKNOWN, AudioAdapterDeviceChangeReason::UNKNOWN },
    { AudioStreamDeviceChangeReason::NEW_DEVICE_AVAILABLE, AudioAdapterDeviceChangeReason::NEW_DEVICE_AVAILABLE },
    { AudioStreamDeviceChangeReason::OLD_DEVICE_UNAVALIABLE, AudioAdapterDeviceChangeReason::OLD_DEVICE_UNAVALIABLE },
    { AudioStreamDeviceChangeReason::OVERRODE, AudioAdapterDeviceChangeReason::OVERRODE },
};

const std::unordered_map<AudioAdapterConcurrencyMode, AudioConcurrencyMode> AUDIO_CONCURRENCY_MAP = {
    { AudioAdapterConcurrencyMode::INVALID, AudioConcurrencyMode::INVALID },
    { AudioAdapterConcurrencyMode::DEFAULT, AudioConcurrencyMode::DEFAULT },
    { AudioAdapterConcurrencyMode::MIX_WITH_OTHERS, AudioConcurrencyMode::MIX_WITH_OTHERS },
    { AudioAdapterConcurrencyMode::DUCK_OTHERS, AudioConcurrencyMode::DUCK_OTHERS },
    { AudioAdapterConcurrencyMode::PAUSE_OTHERS, AudioConcurrencyMode::PAUSE_OTHERS },
    { AudioAdapterConcurrencyMode::SLIENT, AudioConcurrencyMode::SLIENT },
};

AudioRendererCallbackImpl::AudioRendererCallbackImpl(std::shared_ptr<AudioRendererCallbackAdapter> cb) : cb_(cb) {};

void AudioRendererCallbackImpl::OnInterrupt(const InterruptEvent& interruptEvent)
{
    if (!cb_) {
        return;
    }
    switch (interruptEvent.hintType) {
        case InterruptHint::INTERRUPT_HINT_PAUSE:
            cb_->OnSuspend();
            break;
        case InterruptHint::INTERRUPT_HINT_STOP:
            cb_->OnSuspend();
            break;
        case InterruptHint::INTERRUPT_HINT_RESUME:
            cb_->OnResume();
            break;
        default:
            WVLOG_E("audio renderer interrupt hint not foud, code: %{public}d", interruptEvent.hintType);
            break;
    }
}

void AudioRendererCallbackImpl::OnStateChange(const RendererState state, const StateChangeCmdType cmdType) {}
AudioAdapterDeviceChangeReason AudioOutputChangeCallbackImpl::GetChangeReason(AudioStreamDeviceChangeReason reason)
{
    auto item = CHANGE_REASON_MAP.find(reason);
    if (item == CHANGE_REASON_MAP.end()) {
        WVLOG_E("device change reason not found");
        return AudioAdapterDeviceChangeReason::UNKNOWN;
    }
    return item->second;
}

AudioOutputChangeCallbackImpl::AudioOutputChangeCallbackImpl(std::shared_ptr<AudioOutputChangeCallbackAdapter> cb)
    : cb_(cb) {};
void AudioOutputChangeCallbackImpl::OnOutputDeviceChange(
    const AudioDeviceDescriptor& deviceInfo, const AudioStreamDeviceChangeReason reason)
{
    if (!cb_) {
        return;
    }
    AudioAdapterDeviceChangeReason reasonAdapter = GetChangeReason(reason);
    WVLOG_I("OnOutputDeviceChange reason: %{public}d", (int32_t)reasonAdapter);
    cb_->OnOutputDeviceChange((int32_t)reasonAdapter);
}

int32_t AudioRendererAdapterImpl::Create(
    const std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions, std::string cachePath)
{
    std::string audioCachePath = cachePath;
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!context) {
        WVLOG_E("application context get failed");
        return AUDIO_ERROR;
    }
    if (audioCachePath.empty()) {
        audioCachePath = context->GetCacheDir();
        if (audioCachePath.empty()) {
            WVLOG_E("application cache path get failed");
            return AUDIO_ERROR;
        }
    }
    if (!rendererOptions) {
        WVLOG_E("rendererOptions is nullptr");
        return AUDIO_ERROR;
    }

    AudioRendererOptions audioOptions;
    TransformToAudioRendererOptions(audioOptions, rendererOptions);
    audio_renderer_ = AudioRenderer::Create(audioCachePath, audioOptions);
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer create failed");
        return AUDIO_NULL_ERROR;
    }
    audio_renderer_->SetOffloadAllowed(false);
    return AUDIO_OK;
}

bool AudioRendererAdapterImpl::Start()
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return false;
    }
    return audio_renderer_->Start();
}

bool AudioRendererAdapterImpl::Pause()
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return false;
    }
    return audio_renderer_->Pause();
}

bool AudioRendererAdapterImpl::Stop()
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return false;
    }
    return audio_renderer_->Stop();
}

bool AudioRendererAdapterImpl::Release()
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return false;
    }
    return audio_renderer_->Release();
}

int32_t AudioRendererAdapterImpl::Write(uint8_t* buffer, size_t bufferSize)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->Write(buffer, bufferSize);
}

int32_t AudioRendererAdapterImpl::GetLatency(uint64_t& latency)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->GetLatency(latency);
}

int32_t AudioRendererAdapterImpl::SetVolume(float volume)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->SetVolume(volume);
}

float AudioRendererAdapterImpl::GetVolume()
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->GetVolume();
}

int32_t AudioRendererAdapterImpl::SetAudioRendererCallback(
    const std::shared_ptr<AudioRendererCallbackAdapter>& callback)
{
    if (callback == nullptr) {
        WVLOG_E("set audio manager interrupt callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    callback_ = std::make_shared<AudioRendererCallbackImpl>(callback);

    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    int32_t ret = audio_renderer_->SetRendererCallback(callback_);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio renderer set callback failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioRendererAdapterImpl::SetAudioOutputChangeCallback(
    const std::shared_ptr<AudioOutputChangeCallbackAdapter>& callback)
{
    WVLOG_I("AudioRendererAdapterImpl::SetAudioOutputChangeCallback");
    if (callback == nullptr) {
        WVLOG_E("set audio manager interrupt callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    ouputChangeCallback_ = std::make_shared<AudioOutputChangeCallbackImpl>(callback);
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    int32_t ret = audio_renderer_->RegisterOutputDeviceChangeWithInfoCallback(ouputChangeCallback_);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio renderer set output device change callback failed, code: %{public}d", ret);
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

void AudioRendererAdapterImpl::SetInterruptMode(bool audioExclusive)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return;
    }
    InterruptMode interruptMode = audioExclusive ? InterruptMode::INDEPENDENT_MODE : InterruptMode::SHARE_MODE;
    WVLOG_D("AudioRendererAdapterImpl::SetInterruptMode audioExclusive: %{public}d", audioExclusive);
    audio_renderer_->SetInterruptMode(interruptMode);
}

void AudioRendererAdapterImpl::SetAudioSilentMode(bool isSilentMode)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return;
    }
    audio_renderer_->SetSilentModeAndMixWithOthers(isSilentMode);
    WVLOG_D("AudioRendererAdapterImpl::SetAudioSilentMode isSilentMode: %{public}d", isSilentMode);
}

bool AudioRendererAdapterImpl::IsRendererStateRunning()
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return false;
    }
    return audio_renderer_->GetStatus() == OHOS::AudioStandard::RendererState::RENDERER_RUNNING;
}

AudioSamplingRate AudioRendererAdapterImpl::GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate)
{
    auto item = SAMPLING_RATE_MAP.find(samplingRate);
    if (item == SAMPLING_RATE_MAP.end()) {
        WVLOG_E("audio sampling rate not found");
        return AudioSamplingRate::SAMPLE_RATE_44100;
    }
    return item->second;
}

AudioEncodingType AudioRendererAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType encodingType)
{
    auto item = ENCODING_TYPE_MAP.find(encodingType);
    if (item == ENCODING_TYPE_MAP.end()) {
        WVLOG_E("audio encoding type not found");
        return AudioEncodingType::ENCODING_INVALID;
    }
    return item->second;
}

AudioSampleFormat AudioRendererAdapterImpl::GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat)
{
    auto item = SAMPLE_FORMAT_MAP.find(sampleFormat);
    if (item == SAMPLE_FORMAT_MAP.end()) {
        WVLOG_E("audio sample format not found");
        return AudioSampleFormat::INVALID_WIDTH;
    }
    return item->second;
}

AudioChannel AudioRendererAdapterImpl::GetAudioChannel(AudioAdapterChannel channel)
{
    auto item = AUDIO_CHANNEL_MAP.find(channel);
    if (item == AUDIO_CHANNEL_MAP.end()) {
        WVLOG_E("audio channel not found");
        return AudioChannel::STEREO;
    }
    return item->second;
}

ContentType AudioRendererAdapterImpl::GetAudioContentType(AudioAdapterContentType contentType)
{
    auto item = CONTENT_TYPE_MAP.find(contentType);
    if (item == CONTENT_TYPE_MAP.end()) {
        WVLOG_E("audio content type not found");
        return ContentType::CONTENT_TYPE_MUSIC;
    }
    return item->second;
}

StreamUsage AudioRendererAdapterImpl::GetAudioStreamUsage(AudioAdapterStreamUsage streamUsage)
{
    auto item = STREAM_USAGE_MAP.find(streamUsage);
    if (item == STREAM_USAGE_MAP.end()) {
        WVLOG_E("audio stream usage not found");
        return StreamUsage::STREAM_USAGE_MEDIA;
    }
    return item->second;
}

AudioSessionStrategy AudioRendererAdapterImpl::GetAudioAudioStrategy(AudioAdapterConcurrencyMode concurrencyMode)
{
    struct AudioSessionStrategy strategy;
    auto item = AUDIO_CONCURRENCY_MAP.find(concurrencyMode);
    if (item == AUDIO_CONCURRENCY_MAP.end()) {
        WVLOG_E("audio concurrency mode not found");
        strategy.concurrencyMode = AudioConcurrencyMode::INVALID;
        return strategy;
    }
    strategy.concurrencyMode = item->second;
    return strategy;
}

void AudioRendererAdapterImpl::TransformToAudioRendererOptions(
    AudioRendererOptions& out, const std::shared_ptr<AudioRendererOptionsAdapter>& in)
{
    out.streamInfo.samplingRate = GetAudioSamplingRate(in->GetSamplingRate());
    out.streamInfo.encoding = GetAudioEncodingType(in->GetEncodingType());
    out.streamInfo.format = GetAudioSampleFormat(in->GetSampleFormat());
    out.streamInfo.channels = GetAudioChannel(in->GetChannel());
    out.rendererInfo.contentType = GetAudioContentType(in->GetContentType());
    out.rendererInfo.streamUsage = GetAudioStreamUsage(in->GetStreamUsage());
    out.rendererInfo.rendererFlags = in->GetRenderFlags();
    out.strategy = GetAudioAudioStrategy(in->GetConcurrencyMode());
}

} // namespace OHOS::NWeb

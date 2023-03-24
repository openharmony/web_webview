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

#include "audio_errors.h"
#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/application_context.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
const std::unordered_map<AudioAdapterSamplingRate, AudioSamplingRate> SAMPLING_RATE_MAP = {
    {AudioAdapterSamplingRate::SAMPLE_RATE_8000, AudioSamplingRate::SAMPLE_RATE_8000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_11025, AudioSamplingRate::SAMPLE_RATE_11025},
    {AudioAdapterSamplingRate::SAMPLE_RATE_12000, AudioSamplingRate::SAMPLE_RATE_12000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_16000, AudioSamplingRate::SAMPLE_RATE_16000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_22050, AudioSamplingRate::SAMPLE_RATE_22050},
    {AudioAdapterSamplingRate::SAMPLE_RATE_24000, AudioSamplingRate::SAMPLE_RATE_24000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_32000, AudioSamplingRate::SAMPLE_RATE_32000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_44100, AudioSamplingRate::SAMPLE_RATE_44100},
    {AudioAdapterSamplingRate::SAMPLE_RATE_48000, AudioSamplingRate::SAMPLE_RATE_48000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_64000, AudioSamplingRate::SAMPLE_RATE_64000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_96000, AudioSamplingRate::SAMPLE_RATE_96000},
};

const std::unordered_map<AudioAdapterEncodingType, AudioEncodingType> ENCODING_TYPE_MAP = {
    {AudioAdapterEncodingType::ENCODING_PCM, AudioEncodingType::ENCODING_PCM},
    {AudioAdapterEncodingType::ENCODING_INVALID, AudioEncodingType::ENCODING_INVALID}
};

const std::unordered_map<AudioAdapterSampleFormat, AudioSampleFormat> SAMPLE_FORMAT_MAP = {
    {AudioAdapterSampleFormat::SAMPLE_U8, AudioSampleFormat::SAMPLE_U8},
    {AudioAdapterSampleFormat::SAMPLE_S16LE, AudioSampleFormat::SAMPLE_S16LE},
    {AudioAdapterSampleFormat::SAMPLE_S24LE, AudioSampleFormat::SAMPLE_S24LE},
    {AudioAdapterSampleFormat::SAMPLE_S32LE, AudioSampleFormat::SAMPLE_S32LE},
    {AudioAdapterSampleFormat::SAMPLE_F32LE, AudioSampleFormat::SAMPLE_F32LE},
};

const std::unordered_map<AudioAdapterChannel, AudioChannel> AUDIO_CHANNEL_MAP = {
    {AudioAdapterChannel::MONO, AudioChannel::MONO},
    {AudioAdapterChannel::STEREO, AudioChannel::STEREO},
    {AudioAdapterChannel::CHANNEL_3, AudioChannel::CHANNEL_3},
    {AudioAdapterChannel::CHANNEL_4, AudioChannel::CHANNEL_4},
    {AudioAdapterChannel::CHANNEL_5, AudioChannel::CHANNEL_5},
    {AudioAdapterChannel::CHANNEL_6, AudioChannel::CHANNEL_6},
    {AudioAdapterChannel::CHANNEL_7, AudioChannel::CHANNEL_7},
    {AudioAdapterChannel::CHANNEL_8, AudioChannel::CHANNEL_8},
};

const std::unordered_map<AudioAdapterContentType, ContentType> CONTENT_TYPE_MAP = {
    {AudioAdapterContentType::CONTENT_TYPE_UNKNOWN, ContentType::CONTENT_TYPE_UNKNOWN},
    {AudioAdapterContentType::CONTENT_TYPE_SPEECH, ContentType::CONTENT_TYPE_SPEECH},
    {AudioAdapterContentType::CONTENT_TYPE_MUSIC, ContentType::CONTENT_TYPE_MUSIC},
    {AudioAdapterContentType::CONTENT_TYPE_MOVIE, ContentType::CONTENT_TYPE_MOVIE},
    {AudioAdapterContentType::CONTENT_TYPE_SONIFICATION, ContentType::CONTENT_TYPE_SONIFICATION},
    {AudioAdapterContentType::CONTENT_TYPE_RINGTONE, ContentType::CONTENT_TYPE_RINGTONE},
};

const std::unordered_map<AudioAdapterStreamUsage, StreamUsage> STREAM_USAGE_MAP = {
    {AudioAdapterStreamUsage::STREAM_USAGE_UNKNOWN, StreamUsage::STREAM_USAGE_UNKNOWN},
    {AudioAdapterStreamUsage::STREAM_USAGE_MEDIA, StreamUsage::STREAM_USAGE_MEDIA},
    {AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION, StreamUsage::STREAM_USAGE_VOICE_COMMUNICATION},
    {AudioAdapterStreamUsage::STREAM_USAGE_VOICE_ASSISTANT, StreamUsage::STREAM_USAGE_VOICE_ASSISTANT},
    {AudioAdapterStreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE, StreamUsage::STREAM_USAGE_NOTIFICATION_RINGTONE},
};

AudioRendererWriteCallbackImpl::AudioRendererWriteCallbackImpl(
    std::shared_ptr<AudioRendererWriteCallbackAdapter> callback) : callback_(callback) {}

void AudioRendererWriteCallbackImpl::OnWriteData(size_t length)
{
    if (callback_) {
        callback_->OnWriteData(length);
    }
}

int32_t AudioRendererAdapterImpl::Create(const AudioAdapterRendererOptions &rendererOptions,
    std::string cachePath)
{
    std::string audioCachePath = cachePath;
    if (audioCachePath.empty()) {
        std::shared_ptr<AbilityRuntime::ApplicationContext> context =
            AbilityRuntime::ApplicationContext::GetApplicationContext();
        if (!context) {
            WVLOG_E("application context get failed");
            return AUDIO_ERROR;
        }
        audioCachePath = context->GetCacheDir();
        if (audioCachePath.empty()) {
            WVLOG_E("application cache path get failed");
            return AUDIO_ERROR;
        }
    }

    AudioRendererOptions audioOptions;
    audioOptions.streamInfo.samplingRate = GetAudioSamplingRate(rendererOptions.samplingRate);
    audioOptions.streamInfo.encoding = GetAudioEncodingType(rendererOptions.encoding);
    audioOptions.streamInfo.format = GetAudioSampleFormat(rendererOptions.format);
    audioOptions.streamInfo.channels = GetAudioChannel(rendererOptions.channels);
    audioOptions.rendererInfo.contentType = GetAudioContentType(rendererOptions.contentType);
    audioOptions.rendererInfo.streamUsage = GetAudioStreamUsage(rendererOptions.streamUsage);
    audioOptions.rendererInfo.rendererFlags = rendererOptions.rendererFlags;

    audio_renderer_ = AudioRenderer::Create(audioCachePath, audioOptions);
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer create failed");
        return AUDIO_NULL_ERROR;
    }
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

int32_t AudioRendererAdapterImpl::Write(uint8_t *buffer, size_t bufferSize)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->Write(buffer, bufferSize);
}

int32_t AudioRendererAdapterImpl::GetLatency(uint64_t &latency) const
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->GetLatency(latency);
}

int32_t AudioRendererAdapterImpl::SetVolume(float volume) const
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->SetVolume(volume);
}

float AudioRendererAdapterImpl::GetVolume() const
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio rendderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    return audio_renderer_->GetVolume();
}

int32_t AudioRendererAdapterImpl::Enqueue(const BufferDescAdapter& bufDesc)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio renderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    BufferDesc bufInfo;
    bufInfo.buffer =  bufDesc.buffer;
    bufInfo.bufLength =  bufDesc.bufLength;
    bufInfo.dataLength =  bufDesc.dataLength;
    int32_t ret = audio_renderer_->Enqueue(bufInfo);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio renderer enqueue faild");
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
}

int32_t AudioRendererAdapterImpl::GetBufferDesc(BufferDescAdapter& bufDesc)
{
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio renderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    BufferDesc bufInfo;
    int32_t ret = audio_renderer_->GetBufferDesc(bufInfo);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio renderer get buffer desc faild");
        return AUDIO_ERROR;
    }
    bufDesc.buffer = bufInfo.buffer;
    bufDesc.bufLength = bufInfo.bufLength;
    bufDesc.dataLength = bufInfo.dataLength;
    return AUDIO_OK;
}

int32_t AudioRendererAdapterImpl::SetAudioRendererWriteCallbackAdapter(
    const std::shared_ptr<AudioRendererWriteCallbackAdapter>& callback)
{
    if (callback == nullptr) {
        WVLOG_E("audio renderer write callback is nullptr");
        return AUDIO_NULL_ERROR;
    }
    callback_ = std::make_shared<AudioRendererWriteCallbackImpl>(callback);
    if (audio_renderer_ == nullptr) {
        WVLOG_E("audio renderer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    int32_t ret = audio_renderer_->SetRenderMode(RENDER_MODE_CALLBACK);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio renderer set render mode faild");
        return AUDIO_ERROR;
    }
    ret = audio_renderer_->SetRendererWriteCallback(callback_);
    if (ret != AudioStandard::SUCCESS) {
        WVLOG_E("audio renderer set renderer write callback faild");
        return AUDIO_ERROR;
    }
    return AUDIO_OK;
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
}  // namespace OHOS::NWeb
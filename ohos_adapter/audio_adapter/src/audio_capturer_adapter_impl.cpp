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

#include "audio_capturer_adapter_impl.h"

#include <unordered_map>

#include "nweb_log.h"

namespace OHOS::NWeb {

const std::unordered_map<AudioAdapterSamplingRate, int32_t> SAMPLING_RATE_MAP = {
    {AudioAdapterSamplingRate::SAMPLE_RATE_8000, 8000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_11025, 11025},
    {AudioAdapterSamplingRate::SAMPLE_RATE_12000, 12000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_16000, 16000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_22050, 22050},
    {AudioAdapterSamplingRate::SAMPLE_RATE_24000, 24000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_32000, 32000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_44100, 44100},
    {AudioAdapterSamplingRate::SAMPLE_RATE_48000, 48000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_64000, 64000},
    {AudioAdapterSamplingRate::SAMPLE_RATE_96000, 96000},
};

const std::unordered_map<AudioAdapterEncodingType, OH_AudioStream_EncodingType> ENCODING_TYPE_MAP = {
    {AudioAdapterEncodingType::ENCODING_PCM, AUDIOSTREAM_ENCODING_TYPE_RAW},
};

const std::unordered_map<AudioAdapterSampleFormat, OH_AudioStream_SampleFormat> SAMPLE_FORMAT_MAP = {
    {AudioAdapterSampleFormat::SAMPLE_U8, AUDIOSTREAM_SAMPLE_U8},
    {AudioAdapterSampleFormat::SAMPLE_S16LE, AUDIOSTREAM_SAMPLE_S16LE},
    {AudioAdapterSampleFormat::SAMPLE_S24LE, AUDIOSTREAM_SAMPLE_S24LE},
    {AudioAdapterSampleFormat::SAMPLE_S32LE, AUDIOSTREAM_SAMPLE_S32LE},
};

const std::unordered_map<AudioAdapterChannel, int32_t> AUDIO_CHANNEL_MAP = {
    {AudioAdapterChannel::MONO, 1},
    {AudioAdapterChannel::STEREO, 2},
    {AudioAdapterChannel::CHANNEL_3, 3},
    {AudioAdapterChannel::CHANNEL_4, 4},
    {AudioAdapterChannel::CHANNEL_5, 5},
    {AudioAdapterChannel::CHANNEL_6, 6},
    {AudioAdapterChannel::CHANNEL_7, 7},
    {AudioAdapterChannel::CHANNEL_8, 8},
};

const std::unordered_map<AudioAdapterSourceType, OH_AudioStream_SourceType> SOURCE_TYPE_MAP = {
    {AudioAdapterSourceType::SOURCE_TYPE_INVALID, AUDIOSTREAM_SOURCE_TYPE_INVALID},
    {AudioAdapterSourceType::SOURCE_TYPE_MIC, AUDIOSTREAM_SOURCE_TYPE_MIC},
    {AudioAdapterSourceType::SOURCE_TYPE_VOICE_RECOGNITION, AUDIOSTREAM_SOURCE_TYPE_VOICE_RECOGNITION},
    {AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION, AUDIOSTREAM_SOURCE_TYPE_VOICE_COMMUNICATION},
};

namespace {
const int32_t DEFAULT_SAMPLING_RATE = 44100;
const OH_AudioStream_EncodingType DEFAULT_ENCODETYPE = AUDIOSTREAM_ENCODING_TYPE_RAW;
const OH_AudioStream_SampleFormat DEFAULT_SAMPLE_FORMAT = AUDIOSTREAM_SAMPLE_U8;
const int32_t DEFAULT_AUDIO_CHANNEL = 2;
const OH_AudioStream_SourceType DEFAULT_SOURCETYPE = AUDIOSTREAM_SOURCE_TYPE_VOICE_RECOGNITION;

int32_t OnReadData(OH_AudioCapturer* capturer, void* userData, void* buffer, int32_t length)
{
    if (userData == nullptr) {
        return -1;
    }

    std::shared_ptr<UserDataCallBack> userDataCallback = *(static_cast<std::shared_ptr<UserDataCallBack>*>(userData));
    if (userDataCallback->callback == nullptr) {
        return -1;
    }
    userDataCallback->buffer = static_cast<uint8_t*>(buffer);
    userDataCallback->length = length;
    userDataCallback->callback->OnReadData(length);
    return 0;
}
} // namespace

bool AudioCapturerAdapterImpl::AudioStreamBuilderSet(
    OH_AudioStreamBuilder* builder,
    const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions)
{
    auto ret = OH_AudioStreamBuilder_SetSamplingRate(builder, GetAudioSamplingRate(capturerOptions->GetSamplingRate()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio stream capturer builder set samplerate failed");
        return false;
    }

    ret = OH_AudioStreamBuilder_SetEncodingType(builder, GetAudioEncodingType(capturerOptions->GetEncoding()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio stream capturer builder set encoding type failed");
        return false;
    }

    ret = OH_AudioStreamBuilder_SetSampleFormat(builder, GetAudioSampleFormat(capturerOptions->GetSampleFormat()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio stream capturer builder set sample format failed");
        return false;
    }

    ret = OH_AudioStreamBuilder_SetChannelCount(builder, GetAudioChannel(capturerOptions->GetChannels()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio stream capturer builder set channel count failed");
        return false;
    }

    ret = OH_AudioStreamBuilder_SetCapturerInfo(builder, GetAudioSourceType(capturerOptions->GetSourceType()));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio stream capturer builder set capturer info failed");
        return false;
    }

    if (capturerOptions->GetCapturerFlags() != 0) {
        OH_AudioStream_LatencyMode latencyMode = AUDIOSTREAM_LATENCY_MODE_FAST;
        ret = OH_AudioStreamBuilder_SetLatencyMode(builder, latencyMode);
        if (ret != AUDIOSTREAM_SUCCESS) {
            // continue when set latencyMode failed
            WVLOG_E("audio stream capturer builder set latencyMode failed");
        }
    }
    return true;
}

int32_t AudioCapturerAdapterImpl::Create(
    const std::shared_ptr<AudioCapturerOptionsAdapter> capturerOptions,
    std::string cachePath)
{
    (void) cachePath;

    if (!capturerOptions) {
        WVLOG_E("capturerOptions is nullptr");
        return AUDIO_ERROR;
    }

    OH_AudioStreamBuilder* builder;
    auto ret = OH_AudioStreamBuilder_Create(&builder, AUDIOSTREAM_TYPE_CAPTURER);
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("create audio stream capturer builder failed");
        return AUDIO_NULL_ERROR;
    }

    if (!AudioStreamBuilderSet(builder, capturerOptions)) {
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }

    OH_AudioCapturer_Callbacks callbacks;
    callbacks.OH_AudioCapturer_OnReadData = OnReadData;
    callbacks.OH_AudioCapturer_OnStreamEvent = nullptr;
    callbacks.OH_AudioCapturer_OnInterruptEvent = nullptr;
    callbacks.OH_AudioCapturer_OnError = nullptr;

    userDataCallBack_ = std::make_shared<UserDataCallBack>();
    ret = OH_AudioStreamBuilder_SetCapturerCallback(builder, callbacks, static_cast<void*>(&userDataCallBack_));
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("create audio stream set callback failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }

    ret = OH_AudioStreamBuilder_GenerateCapturer(builder, &audio_capturer_);
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("create audio stream capturer failed");
        OH_AudioStreamBuilder_Destroy(builder);
        return AUDIO_ERROR;
    }
    OH_AudioStreamBuilder_Destroy(builder);
    return AUDIO_OK;
}

bool AudioCapturerAdapterImpl::Start()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    auto ret = OH_AudioCapturer_Start(audio_capturer_);
    return ret == AUDIOSTREAM_SUCCESS;
}

bool AudioCapturerAdapterImpl::Stop()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    auto ret = OH_AudioCapturer_Stop(audio_capturer_);
    return ret == AUDIOSTREAM_SUCCESS;
}

bool AudioCapturerAdapterImpl::Release()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return false;
    }
    auto ret = OH_AudioCapturer_Release(audio_capturer_);
    return ret == AUDIOSTREAM_SUCCESS;
}

int32_t AudioCapturerAdapterImpl::SetCapturerReadCallback(
    std::shared_ptr<AudioCapturerReadCallbackAdapter> callback)
{
    if (callback == nullptr) {
        WVLOG_E("audio capturer callback is nullptr");
        return AUDIO_NULL_ERROR;
    }

    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }

    if (userDataCallBack_ == nullptr) {
        WVLOG_E("userDataCallBack is nullptr");
        return AUDIO_NULL_ERROR;
    }

    userDataCallBack_->callback = callback;
    return AUDIO_OK;
}

int32_t AudioCapturerAdapterImpl::GetBufferDesc(std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    if (!bufferDesc || !userDataCallBack_ || userDataCallBack_->buffer == nullptr) {
        WVLOG_E("bufferDesc is nullptr");
        return AUDIO_NULL_ERROR;
    }
    WVLOG_D("GetBufferDesc %{public}p, %{public}zd", userDataCallBack_->buffer, userDataCallBack_->length);
    bufferDesc->SetBuffer(userDataCallBack_->buffer);
    bufferDesc->SetBufLength(userDataCallBack_->length);
    bufferDesc->SetDataLength(userDataCallBack_->length);
    return AUDIO_OK;
}

int32_t AudioCapturerAdapterImpl::Enqueue(const std::shared_ptr<BufferDescAdapter> bufferDesc)
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
 
    // no need to do enqueue
    WVLOG_D("[adapter mock] AudioCapturerAdapterImpl::Enqueue");
    return AUDIO_OK;
}

int32_t AudioCapturerAdapterImpl::GetFrameCount(uint32_t &frameCount)
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    int32_t frameCountValue;
    auto ret = OH_AudioCapturer_GetFrameSizeInCallback(audio_capturer_, &frameCountValue);
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio capturer get frame size failed");
        return AUDIO_ERROR;
    }
    frameCount = frameCountValue;
    return AUDIO_OK;
}

int64_t AudioCapturerAdapterImpl::GetAudioTime()
{
    if (audio_capturer_ == nullptr) {
        WVLOG_E("audio capturer is nullptr");
        return AUDIO_NULL_ERROR;
    }
    int64_t framePosition;
    int64_t timestamp;
    auto ret = OH_AudioCapturer_GetTimestamp(audio_capturer_, CLOCK_MONOTONIC, &framePosition, &timestamp);
    if (ret != AUDIOSTREAM_SUCCESS) {
        WVLOG_E("audio capturer get timestamp failed");
        return AUDIO_ERROR;
    }
    return timestamp;
}

int32_t AudioCapturerAdapterImpl::GetAudioSamplingRate(AudioAdapterSamplingRate samplingRate)
{
    auto item = SAMPLING_RATE_MAP.find(samplingRate);
    if (item == SAMPLING_RATE_MAP.end()) {
        WVLOG_E("audio sampling rate not found");
        return DEFAULT_SAMPLING_RATE;
    }
    return item->second;
}

OH_AudioStream_EncodingType AudioCapturerAdapterImpl::GetAudioEncodingType(AudioAdapterEncodingType encodingType)
{
    auto item = ENCODING_TYPE_MAP.find(encodingType);
    if (item == ENCODING_TYPE_MAP.end()) {
        WVLOG_E("audio encoding type not found");
        return DEFAULT_ENCODETYPE;
    }
    return item->second;
}

OH_AudioStream_SampleFormat AudioCapturerAdapterImpl::GetAudioSampleFormat(AudioAdapterSampleFormat sampleFormat)
{
    auto item = SAMPLE_FORMAT_MAP.find(sampleFormat);
    if (item == SAMPLE_FORMAT_MAP.end()) {
        WVLOG_E("audio sample format not found");
        return DEFAULT_SAMPLE_FORMAT;
    }
    return item->second;
}

int32_t AudioCapturerAdapterImpl::GetAudioChannel(AudioAdapterChannel channel)
{
    auto item = AUDIO_CHANNEL_MAP.find(channel);
    if (item == AUDIO_CHANNEL_MAP.end()) {
        WVLOG_E("audio channel not found");
        return DEFAULT_AUDIO_CHANNEL;
    }
    return item->second;
}

OH_AudioStream_SourceType AudioCapturerAdapterImpl::GetAudioSourceType(AudioAdapterSourceType sourceType)
{
    auto item = SOURCE_TYPE_MAP.find(sourceType);
    if (item == SOURCE_TYPE_MAP.end()) {
        WVLOG_E("audio source type not found");
        return DEFAULT_SOURCETYPE;
    }
    return item->second;
}
}  // namespace OHOS::NWeb

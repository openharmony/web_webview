/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "audio_codec_decoder_adapter_impl.h"

#include "native_avcodec_audiocodec.h"
#include "native_avcapability.h"
#include "native_avbuffer.h"
#include <multimedia/native_audio_channel_layout.h>
#include "native_drm_err.h"
#include "native_mediakeysession.h"
#include <cinttypes>
#include "audio_cenc_info_adapter_impl.h"

#define MIME_SIZE 256

namespace OHOS::NWeb {

static const std::unordered_map<OHOS::MediaAVCodec::AVCodecBufferFlag, BufferFlag> BUFFER_FLAG_MAP = {
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE, BufferFlag::CODEC_BUFFER_FLAG_NONE },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS, BufferFlag::CODEC_BUFFER_FLAG_EOS },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_SYNC_FRAME, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_PARTIAL_FRAME,
        BufferFlag::CODEC_BUFFER_FLAG_PARTIAL_FRAME },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_CODEC_DATA, BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA }
};

static const std::unordered_map<const char *, AudioMimeType>  MIME_TYPE_MAP = {
    { OH_AVCODEC_MIMETYPE_AUDIO_AAC, AudioMimeType::MIMETYPE_AUDIO_AAC },
    { OH_AVCODEC_MIMETYPE_AUDIO_FLAC, AudioMimeType::MIMETYPE_AUDIO_FLAC },
    { OH_AVCODEC_MIMETYPE_AUDIO_VORBIS, AudioMimeType::MIMETYPE_AUDIO_VORBIS },
    { OH_AVCODEC_MIMETYPE_AUDIO_MPEG, AudioMimeType::MIMETYPE_AUDIO_MPEG },
    { OH_AVCODEC_MIMETYPE_AUDIO_AMR_NB, AudioMimeType::MIMETYPE_AUDIO_AMR_NB },
    { OH_AVCODEC_MIMETYPE_AUDIO_AMR_WB, AudioMimeType::MIMETYPE_AUDIO_AMR_WB },
    { OH_AVCODEC_MIMETYPE_AUDIO_G711MU, AudioMimeType::MIMETYPE_AUDIO_G711MU },
    { OH_AVCODEC_MIMETYPE_AUDIO_APE, AudioMimeType::MIMETYPE_AUDIO_APE }
};

AudioDecoderFormatAdapterImpl::~AudioDecoderFormatAdapterImpl() {}
int32_t AudioDecoderFormatAdapterImpl::GetSampleRate()
{
    return sampleRate_;
}

int32_t AudioDecoderFormatAdapterImpl::GetChannelCount()
{
    return channelCount_;
}

int64_t AudioDecoderFormatAdapterImpl::GetBitRate()
{
    return bitRate_;
}

int32_t AudioDecoderFormatAdapterImpl::GetMaxInputSize()
{
    return maxInputSize_;
}

int32_t AudioDecoderFormatAdapterImpl::GetAudioSampleFormat()
{
    return audioSampleFormat_;
}

bool AudioDecoderFormatAdapterImpl::GetAACIsAdts()
{
    return aacIsAdts_;
}

int32_t AudioDecoderFormatAdapterImpl::GetIdentificationHeader()
{
    return idHeader_;
}

int32_t AudioDecoderFormatAdapterImpl::GetSetupHeader()
{
    return setupHeader_;
}

uint8_t* AudioDecoderFormatAdapterImpl::GetCodecConfig()
{
    return codecConfig_;
}

uint32_t AudioDecoderFormatAdapterImpl::GetCodecConfigSize()
{
    return codecConfigSize_;
}

void AudioDecoderFormatAdapterImpl::SetSampleRate(int32_t sampleRate)
{
    sampleRate_ = sampleRate;
}

void AudioDecoderFormatAdapterImpl::SetChannelCount(int32_t channelCount)
{
    channelCount_ = channelCount;
}

void AudioDecoderFormatAdapterImpl::SetBitRate(int64_t bitRate)
{
    bitRate_ = bitRate;
}

void AudioDecoderFormatAdapterImpl::SetMaxInputSize(int32_t maxInputSize)
{
    maxInputSize_ = maxInputSize;
}

void AudioDecoderFormatAdapterImpl::SetAACIsAdts(bool isAdts)
{
    aacIsAdts_ = isAdts;
}

void AudioDecoderFormatAdapterImpl::SetAudioSampleFormat(int32_t audioSampleFormat)
{
    audioSampleFormat_ = audioSampleFormat;
}

void AudioDecoderFormatAdapterImpl::SetIdentificationHeader(int32_t idHeader)
{
    idHeader_ = idHeader;
}

void AudioDecoderFormatAdapterImpl::SetSetupHeader(int32_t setupHeader)
{
    setupHeader_ = setupHeader;
}

void AudioDecoderFormatAdapterImpl::SetCodecConfig(uint8_t* codecConfig)
{
    codecConfig_ = codecConfig;
}

void AudioDecoderFormatAdapterImpl::SetCodecConfigSize(uint32_t size)
{
    codecConfigSize_ = size;
}

void AudioDecoderFormatAdapterImpl::PrintFormatData(std::shared_ptr<AudioDecoderFormatAdapter> format)
{
    WVLOG_I("PrintFormatData:SampleRate[%{public}d], ChannelCount[%{public}d],"
            "MaxInputSize[%{public}d], AACIsAdts[%{public}d], AudioSampleFormat[%{public}d],"
            "BitRate[%{public}" PRId64 "], IDHeader[%{public}d], SetupHeader[%{public}d],",
            format->GetSampleRate(), format->GetChannelCount(), format->GetMaxInputSize(),
            static_cast<int32_t>(format->GetAACIsAdts()), format->GetAudioSampleFormat(),
            format->GetBitRate(), format->GetIdentificationHeader(), format->GetSetupHeader());
}

AudioDecoderCallbackAdapterImpl::AudioDecoderCallbackAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> cb) :cb_(cb) {};

void AudioDecoderCallbackAdapterImpl::OnError(int32_t errorCode)
{
    if (!cb_) {
        WVLOG_E("callback is nullptr.");
        return;
    }

    cb_->OnError(errorCode);
}

void AudioDecoderCallbackAdapterImpl::OnOutputFormatChanged()
{
    if (!cb_) {
        WVLOG_E("callback is nullptr.");
        return;
    }

    cb_->OnOutputFormatChanged();
}

void AudioDecoderCallbackAdapterImpl::OnInputBufferAvailable(uint32_t index)
{
    WVLOG_I("inputBuffer[%{public}u] available .", index);
    if (!cb_) {
        WVLOG_E("callback is nullptr.");
        return;
    }

    cb_->OnInputBufferAvailable(index);
}

void AudioDecoderCallbackAdapterImpl::OnOutputBufferAvailable(
    uint32_t index, uint8_t *bufferData, int32_t size, int64_t pts, int32_t offset, uint32_t flags)
{
    WVLOG_I("outputBuffer[%{public}u] available, buffer size[%{public}d], pts[%{public}" PRId64 "],"
        "offset[%{public}d], flags[%{public}u].", index, size, pts, offset, flags);
    if (!cb_) {
        WVLOG_E("callback is nullptr.");
        return;
    }

    cb_->OnOutputBufferAvailable(index, bufferData, size, pts, offset, flags);
}

std::mutex AudioCodecDecoderAdapterImpl::decoderMutex_;

std::mutex& AudioCodecDecoderAdapterImpl::GetDecoderMutex()
{
    return decoderMutex_;
}

AudioCodecDecoderAdapterImpl::AudioCodecDecoderAdapterImpl() {}

AudioCodecDecoderAdapterImpl::~AudioCodecDecoderAdapterImpl()
{
    std::unique_lock<std::mutex> lock(decoderMutex_);
    WVLOG_D("~AudioCodecDecoderAdapterImpl enter.");
    callback_ = nullptr;
    if (decoder_ != nullptr) {
        AudioDecoderCallbackManager::DeleteAudioDecoder(decoder_);
        OH_AVErrCode errCode = OH_AudioCodec_Destroy(decoder_);
        if (errCode != AV_ERR_OK) {
            WVLOG_E("destroy decoder_ fail, errCode = %{public}u.", uint32_t(errCode));
        }
        decoder_ = nullptr;
    }
}

OH_AVCodec* AudioCodecDecoderAdapterImpl::GetAVCodec()
{
    return decoder_;
}

AudioDecoderCallbackAdapterImpl* AudioCodecDecoderAdapterImpl::GetAudioDecoderCallBack()
{
    return callback_.get();
}

void AudioCodecDecoderAdapterImpl::SetInputBuffer(uint32_t index, OH_AVBuffer *buffer)
{
    WVLOG_D("AudioCodecDecoderAdapterImpl %{public}s[%{public}u].", __FUNCTION__, index);
    std::unique_lock<std::mutex> lock(inMutex_);
    if (inputBuffers_.find(index) != inputBuffers_.end()) {
        WVLOG_I("AudioCodecDecoderAdapterImpl %{public}s[%{public}u].", __FUNCTION__, index);
        return;
    }
    inputBuffers_.insert(std::make_pair(index, buffer));
}

void AudioCodecDecoderAdapterImpl::SetOutputBuffer(uint32_t index, OH_AVBuffer *buffer)
{
    std::unique_lock<std::mutex> lock(outMutex_);
    if (outputBuffers_.find(index) != outputBuffers_.end()) {
        return;
    }
    outputBuffers_.insert(std::make_pair(index, buffer));
}

OH_AVBuffer* AudioCodecDecoderAdapterImpl::GetInputBuffer(uint32_t index)
{
    std::unique_lock<std::mutex> lock(inMutex_);
    if (inputBuffers_.find(index) == inputBuffers_.end()) {
        return nullptr;
    }
    return inputBuffers_.at(index);
}

OH_AVBuffer* AudioCodecDecoderAdapterImpl::GetOutputBuffer(uint32_t index)
{
    std::unique_lock<std::mutex> lock(outMutex_);
    if (outputBuffers_.find(index) == outputBuffers_.end()) {
        return nullptr;
    }
    return outputBuffers_.at(index);
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::CreateAudioDecoderByMime(const std::string& mimetype)
{
    WVLOG_I("%{public}s, mimetype[%{public}s].", __FUNCTION__, mimetype.c_str());
    if (decoder_ != nullptr) {
        WVLOG_I("decoder_ already has decoder.");
        ReleaseDecoder();
    }
    decoder_ = OH_AudioCodec_CreateByMime(mimetype.c_str(), false);
    if (decoder_ == nullptr) {
        WVLOG_E("create decoder by mine[%{public}s] failed.", mimetype.c_str());
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    mimeType_ = AudioMimeType::MIMETYPE_UNKNOW;
    for (auto it = MIME_TYPE_MAP.begin(); it != MIME_TYPE_MAP.end(); it++) {
        if (strcmp(it->first, mimetype.c_str()) == 0) {
            mimeType_ = it->second;
        }
    }

    AudioDecoderCallbackManager::AddAudioDecoder(this);
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::CreateAudioDecoderByName(const std::string& name)
{
    WVLOG_I("%{public}s, name[%{public}s].", __FUNCTION__, name.c_str());
    if (decoder_ != nullptr) {
        WVLOG_E("create decoder by name[%{public}s] failed.", name.c_str());
        ReleaseDecoder();
    }

    decoder_ = OH_AudioCodec_CreateByName(name.c_str());
    if (decoder_ == nullptr) {
        WVLOG_E("create decoder failed.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    AudioDecoderCallbackManager::AddAudioDecoder(this);
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::ConfigureDecoder(
    const std::shared_ptr<AudioDecoderFormatAdapter> format)
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat *avFormat = OH_AVFormat_Create();
    if (avFormat == nullptr) {
        WVLOG_E("create avformat fail.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat_SetIntValue(avFormat, OH_MD_KEY_AUD_SAMPLE_RATE, format->GetSampleRate());
    OH_AVFormat_SetIntValue(avFormat, OH_MD_KEY_AUD_CHANNEL_COUNT, format->GetChannelCount());
    OH_AVErrCode errCode = OH_AudioCodec_Configure(decoder_, avFormat);
    OH_AVFormat_Destroy(avFormat);
    avFormat = nullptr;
    if (errCode != AV_ERR_OK) {
        WVLOG_E("set config fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::SetParameterDecoder(
    const std::shared_ptr<AudioDecoderFormatAdapter> format)
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat *avFormat = OH_AVFormat_Create();
    if (avFormat == nullptr) {
        WVLOG_E("create avformat fail.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat_SetIntValue(avFormat, OH_MD_KEY_AUD_SAMPLE_RATE, format->GetSampleRate());
    OH_AVFormat_SetIntValue(avFormat, OH_MD_KEY_AUD_CHANNEL_COUNT, format->GetChannelCount());
    OH_AVErrCode errCode = OH_AudioCodec_SetParameter(decoder_, avFormat);
    OH_AVFormat_Destroy(avFormat);
    avFormat = nullptr;
    if (errCode != AV_ERR_OK) {
        WVLOG_E("set config fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::PrepareDecoder()
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode errCode = OH_AudioCodec_Prepare(decoder_);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("prepare decoder fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::StartDecoder()
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode errCode = OH_AudioCodec_Start(decoder_);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("start decoder fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::StopDecoder()
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode errCode = OH_AudioCodec_Stop(decoder_);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("stop decoder fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::FlushDecoder()
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode errCode = OH_AudioCodec_Flush(decoder_);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("flush decoder fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    // clear input and output buffers
    {
        std::unique_lock<std::mutex> lock(inMutex_);
        inputBuffers_.clear();
    }
    {
        std::unique_lock<std::mutex> lock(outMutex_);
        outputBuffers_.clear();
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::ResetDecoder()
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode errCode = OH_AudioCodec_Reset(decoder_);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("start reset fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    // clear input and output buffers
    {
        std::unique_lock<std::mutex> lock(inMutex_);
        inputBuffers_.clear();
    }
    {
        std::unique_lock<std::mutex> lock(outMutex_);
        outputBuffers_.clear();
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::ReleaseDecoder()
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_I("decoder_ already released.");
        return AudioDecoderAdapterCode::DECODER_OK;
    }

    AudioDecoderCallbackManager::DeleteAudioDecoder(decoder_);
    OH_AVErrCode errCode = OH_AudioCodec_Destroy(decoder_);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("destroy decoder_ fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    decoder_ = nullptr;

    // clear input and output buffers
    {
        std::unique_lock<std::mutex> lock(inMutex_);
        inputBuffers_.clear();
    }
    {
        std::unique_lock<std::mutex> lock(outMutex_);
        outputBuffers_.clear();
    }

    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::SetAVCencInfo(
    OH_AVCencInfo *avCencInfo, std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    OH_AVErrCode errNo = OH_AVCencInfo_SetAlgorithm(avCencInfo, static_cast<DrmCencAlgorithm>(cencInfo->GetAlgo()));
    if (errNo != AV_ERR_OK) {
        WVLOG_E("set AVCencInfo Algorithm fail, errNo = %{public}u", static_cast<uint32_t>(errNo));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    errNo = OH_AVCencInfo_SetKeyIdAndIv(
        avCencInfo, cencInfo->GetKeyId(), cencInfo->GetKeyIdLen(), cencInfo->GetIv(), cencInfo->GetIvLen());
    if (errNo != AV_ERR_OK) {
        WVLOG_E("set AVCencInfo keyid and iv fail, errNo = %{public}u",
            static_cast<uint32_t>(errNo));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    DrmSubsample subSamples[cencInfo->GetClearHeaderLens().size()];
    for (uint32_t i = 0; i < cencInfo->GetClearHeaderLens().size(); i++) {
        subSamples[i].clearHeaderLen = cencInfo->GetClearHeaderLens()[i];
        subSamples[i].payLoadLen = cencInfo->GetPayLoadLens()[i];
    }
    errNo = OH_AVCencInfo_SetSubsampleInfo(
        avCencInfo, cencInfo->GetEncryptedBlockCount(), cencInfo->GetSkippedBlockCount(),
        cencInfo->GetFirstEncryptedOffset(), cencInfo->GetClearHeaderLens().size(), subSamples);
    if (errNo != AV_ERR_OK) {
        WVLOG_E("set AVCencInfo subsampleInfo fail, errNo = %{public}u",
            static_cast<uint32_t>(errNo));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    errNo = OH_AVCencInfo_SetMode(avCencInfo, DRM_CENC_INFO_KEY_IV_SUBSAMPLES_SET);
    if (errNo != AV_ERR_OK) {
        WVLOG_E("set AVCencInfo mode fail, errNo = %{public}u", static_cast<uint32_t>(errNo));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::SetBufferCencInfo(
    uint32_t index, std::shared_ptr<AudioCencInfoAdapter> cencInfo)
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    AudioCencInfoAdapterImpl::PrintCencInfoData(cencInfo);
    if (cencInfo == nullptr) {
        WVLOG_E("cencInfo is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVCencInfo *avCencInfo = OH_AVCencInfo_Create();
    if (avCencInfo == nullptr) {
        WVLOG_E("create AVCencInfo fail.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    AudioDecoderAdapterCode ret = SetAVCencInfo(avCencInfo, cencInfo);
    if (ret != AudioDecoderAdapterCode::DECODER_OK) {
        return ret;
    }
    // set CencInfo to AVBuffer
    OH_AVBuffer *avBuffer = GetInputBuffer(index);
    if (avBuffer == nullptr) {
        WVLOG_E("set AVCencInfo fail, not find inputbuffererr[%{public}u]", index);
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode errNo = OH_AVCencInfo_SetAVBuffer(avCencInfo, avBuffer);
    if (errNo != AV_ERR_OK) {
        WVLOG_E("set AVCencInfo fail, errNo = %{public}u", static_cast<uint32_t>(errNo));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    errNo = OH_AVCencInfo_Destroy(avCencInfo);
    if (errNo != AV_ERR_OK) {
        WVLOG_E("destroy cencInfo fail, errNo = %{public}u", static_cast<uint32_t>(errNo));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs,
    uint8_t *bufferData, int32_t bufferSize, std::shared_ptr<AudioCencInfoAdapter> cencInfo,
    bool isEncrypted, BufferFlag flag)
{
    WVLOG_I("%{public}s index[%{public}u],  buffer size[%{public}d], isEncrypted[%{public}d],"
        "flag[%{public}d].", __FUNCTION__, index, bufferSize, static_cast<uint32_t>(isEncrypted),
        static_cast<uint32_t>(flag));

    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    if (isEncrypted && SetBufferCencInfo(index, cencInfo) != AudioDecoderAdapterCode::DECODER_OK) {
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVBuffer *avBuffer = GetInputBuffer(index);
    int32_t bufferCapacity = OH_AVBuffer_GetCapacity(avBuffer);
    if (avBuffer == nullptr || bufferCapacity < bufferSize) {
        WVLOG_E("QueueInputBufferDec fail, inputbuffer[%{public}u] not find.", index);
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    uint8_t *addr = OH_AVBuffer_GetAddr(avBuffer);
    if (flag != GetBufferFlag(OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS)) {
        if (bufferData == nullptr) {
            WVLOG_E("index[%{public}u] bufferData is nullptr.", index);
            return AudioDecoderAdapterCode::DECODER_ERROR;
        }
        if (memcpy_s(addr, bufferCapacity, bufferData, bufferSize) != EOK) {
            WVLOG_E(" index[%{public}u] memcpy_s buffer fail.", index);
            return AudioDecoderAdapterCode::DECODER_ERROR;
        }
    }

    // The size is the length of each frame of data to be decoded. The pts is the timestamp for
    // each frame, indicating when the audio should be played. The values of size and pts are
    // obtained from the audio/video resource file or the data stream to be decoded.
    OH_AVCodecBufferAttr attr = {0};
    if (flag == GetBufferFlag(OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS)) {
        attr.size = 0;
        attr.flags = OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS;
    } else {
        attr.size = bufferSize;
        attr.flags = OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE;
    }
    attr.pts = presentationTimeUs;
    OH_AVBuffer_SetBufferAttr(avBuffer, &attr);
    OH_AVErrCode errCode = OH_AudioCodec_PushInputBuffer(decoder_, index);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("index[%{public}u] push input buffer fail, errCode = %{public}u.",
            index, uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    std::unique_lock<std::mutex> lock(inMutex_);
    inputBuffers_.erase(index);
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::GetOutputFormatDec(
    std::shared_ptr<AudioDecoderFormatAdapter> format)
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder_ is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat *avFormat = OH_AudioCodec_GetOutputDescription(decoder_);
    if (avFormat == nullptr) {
        WVLOG_E("get output description fail.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    AudioDecoderAdapterCode ret = GetParamFromAVFormat(avFormat, format);
    if (ret != AudioDecoderAdapterCode::DECODER_OK) {
        return ret;
    }
    OH_AVFormat_Destroy(avFormat);
    avFormat = nullptr;

    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::ReleaseOutputBufferDec(uint32_t index)
{
    WVLOG_I("%{public}s index[%{public}u] .", __FUNCTION__, index);
    if (decoder_ == nullptr) {
        WVLOG_E("index[%{public}u] is nullptr.", index);
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVBuffer *avBuffer = GetOutputBuffer(index);
    if (avBuffer == nullptr) {
        WVLOG_E("ReleaseOutputBufferDec fail, outputbuffer[%{public}u] not find.", index);
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVCodecBufferAttr attr = {0};
    OH_AVErrCode errCode = OH_AVBuffer_GetBufferAttr(avBuffer, &attr);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("index[%{public}u] get output buffer attr fail, errCode = %{public}u.",
            index, uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    errCode = OH_AudioCodec_FreeOutputBuffer(decoder_, index);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("index[%{public}u] free output buffer fail, errCode = %{public}u.",
            index, uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    {
        std::unique_lock<std::mutex> lock(outMutex_);
        outputBuffers_.erase(index);
    }

    if (attr.flags == OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS) {
        WVLOG_I("index[%{public}u] free output buffer, buffer flag is eos.", index);
    }
    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::SetCallbackDec(
    const std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> callback)
{
    WVLOG_I("%{public}s enter.", __FUNCTION__);
    if (decoder_ == nullptr) {
        WVLOG_E("decoder is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }
    if (callback == nullptr) {
        WVLOG_E("calllback is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    callback_ = std::make_shared<AudioDecoderCallbackAdapterImpl>(callback);
    if (callback_ == nullptr) {
        WVLOG_E("Create Callback failed.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    struct OH_AVCodecCallback cb = {&AudioDecoderCallbackManager::OnError,
        &AudioDecoderCallbackManager::OnOutputFormatChanged, &AudioDecoderCallbackManager::OnInputBufferAvailable,
        &AudioDecoderCallbackManager::OnOutputBufferAvailable};

    // Instead of using the ADecBufferSignal structure recommended by the media framework for buffer rotation,
    // implement it in the chromium kernel to reduce the business logic at the webview layer.
    OH_AVErrCode errCode = OH_AudioCodec_RegisterCallback(decoder_, cb, nullptr);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("register callback fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    return AudioDecoderAdapterCode::DECODER_OK;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::SetDecryptionConfig(void *session, bool secureAudio)
{
    WVLOG_I("%{public}s, secureAudio[%{public}d].", __FUNCTION__, static_cast<uint32_t>(secureAudio));
    if (session == nullptr) {
        WVLOG_E("session is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    if (decoder_ == nullptr) {
        WVLOG_E("decoder is nullptr.");
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    // The media audio codec module currently only supports non-secure decoding mode.
    secureAudio = false;

    MediaKeySession *mediaKeySession = static_cast<MediaKeySession*>(session);
    OH_AVErrCode errCode = OH_AudioCodec_SetDecryptionConfig(decoder_, mediaKeySession, secureAudio);
    if (errCode != AV_ERR_OK) {
        WVLOG_E("set decryption config fail, errCode = %{public}u.", uint32_t(errCode));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    return AudioDecoderAdapterCode::DECODER_OK;
}

BufferFlag AudioCodecDecoderAdapterImpl::GetBufferFlag(OHOS::MediaAVCodec::AVCodecBufferFlag codecBufferFlag)
{
    auto flag = BUFFER_FLAG_MAP.find(codecBufferFlag);
    if (flag == BUFFER_FLAG_MAP.end()) {
        WVLOG_E("buffer flag not found.");
        return BufferFlag::CODEC_BUFFER_FLAG_NONE;
    }
    return flag->second;
}

AudioDecoderAdapterCode AudioCodecDecoderAdapterImpl::GetParamFromAVFormat(
    OH_AVFormat *avFormat, std::shared_ptr<AudioDecoderFormatAdapter> format)
{
    if (avFormat == nullptr || format == nullptr) {
        WVLOG_E("avFormat or format is nullptr, avFormat is %{public}d, format is. %{public}d",
            int32_t(avFormat == nullptr), int32_t(format == nullptr));
        return AudioDecoderAdapterCode::DECODER_ERROR;
    }

    int32_t sampleRate = 0;
    int32_t channels = 0;
    int32_t maxInputSize = 0;
    int32_t aacIsAdts = 0;
    int32_t sampleFormat = 0;
    int64_t bitRate = 0;
    int32_t idHeader = 0;
    int32_t setupHeader = 0;
    uint8_t *codecConfig = nullptr;
    size_t codecConfigSize = 0;
    OH_AVFormat_GetIntValue(avFormat, OH_MD_KEY_AUD_SAMPLE_RATE, &sampleRate);
    OH_AVFormat_GetIntValue(avFormat, OH_MD_KEY_AUD_CHANNEL_COUNT, &channels);
    OH_AVFormat_GetIntValue(avFormat, OH_MD_KEY_MAX_INPUT_SIZE, &maxInputSize);
    OH_AVFormat_GetIntValue(avFormat, OH_MD_KEY_AAC_IS_ADTS, &aacIsAdts);
    OH_AVFormat_GetIntValue(avFormat, OH_MD_KEY_AUDIO_SAMPLE_FORMAT, &sampleFormat);
    OH_AVFormat_GetLongValue(avFormat, OH_MD_KEY_BITRATE, &bitRate);
    OH_AVFormat_GetIntValue(avFormat, OH_MD_KEY_IDENTIFICATION_HEADER, &idHeader);
    OH_AVFormat_GetIntValue(avFormat, OH_MD_KEY_SETUP_HEADER, &setupHeader);
    OH_AVFormat_GetBuffer(avFormat, OH_MD_KEY_CODEC_CONFIG, &codecConfig, &codecConfigSize);

    format->SetSampleRate(sampleRate);
    format->SetChannelCount(channels);
    format->SetMaxInputSize(maxInputSize);
    if (aacIsAdts != 0) {
        format->SetAACIsAdts(true);
    } else {
        format->SetAACIsAdts(false);
    }
    format->SetAudioSampleFormat(sampleFormat);
    format->SetBitRate(bitRate);
    format->SetIdentificationHeader(idHeader);
    format->SetSetupHeader(setupHeader);
    format->SetCodecConfig(codecConfig);
    format->SetCodecConfigSize(uint32_t(codecConfigSize));
    AudioDecoderFormatAdapterImpl::PrintFormatData(format);
    return AudioDecoderAdapterCode::DECODER_OK;
}

std::map<OH_AVCodec*, OHOS::NWeb::AudioCodecDecoderAdapterImpl *> AudioDecoderCallbackManager::decoders_;

std::mutex AudioDecoderCallbackManager::decodersMapMutex_;

OHOS::NWeb::AudioCodecDecoderAdapterImpl *AudioDecoderCallbackManager::FindAudioDecoder(
    OH_AVCodec *codec)
{
    std::unique_lock<std::mutex> lock(decodersMapMutex_);
    if (decoders_.find(codec) != decoders_.end()) {
        return decoders_.at(codec);
    }
    return nullptr;
}

void AudioDecoderCallbackManager::AddAudioDecoder(OHOS::NWeb::AudioCodecDecoderAdapterImpl *decoder)
{
    if (decoder == nullptr || decoder->GetAVCodec() == nullptr) {
        return;
    }
    std::unique_lock<std::mutex> lock(decodersMapMutex_);
    if (decoders_.find(decoder->GetAVCodec()) != decoders_.end()) {
        return;
    }
    decoders_[decoder->GetAVCodec()] = decoder;
}

void AudioDecoderCallbackManager::DeleteAudioDecoder(OH_AVCodec *codec)
{
    if (codec == nullptr) {
        return;
    }
    std::unique_lock<std::mutex> lock(decodersMapMutex_);
    auto it = decoders_.find(codec);
    if (it != decoders_.end()) {
        decoders_.erase(codec);
    }
}

void AudioDecoderCallbackManager::OnError(OH_AVCodec *codec, int32_t errorCode, void *userData)
{
    (void)userData;
    WVLOG_I("AudioDecoderCallbackManager %{public}s.", __FUNCTION__);
    if (codec == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager avcodec is nullptr.");
        return;
    }

    std::unique_lock<std::mutex> lock(AudioCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::AudioCodecDecoderAdapterImpl *impl = FindAudioDecoder(codec);
    if (impl == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager not find decoder.");
        return;
    }
    if (impl->GetAudioDecoderCallBack() == nullptr) {
        WVLOG_E("audio decoder callback is nullptr.");
        return;
    }

    impl->GetAudioDecoderCallBack()->OnError(errorCode);
}

void AudioDecoderCallbackManager::OnOutputFormatChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData)
{
    WVLOG_I("AudioDecoderCallbackManager %{public}s.", __FUNCTION__);
    if (codec == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager avcodec is nullptr.");
        return;
    }
    std::unique_lock<std::mutex> lock(AudioCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::AudioCodecDecoderAdapterImpl *impl = FindAudioDecoder(codec);
    if (impl == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager not find decoder.");
        return;
    }
    if (impl->GetAudioDecoderCallBack() == nullptr) {
        WVLOG_E("audio decoder callback is nullptr.");
        return;
    }

    impl->GetAudioDecoderCallBack()->OnOutputFormatChanged();
}

void AudioDecoderCallbackManager::OnInputBufferAvailable(
    OH_AVCodec *codec, uint32_t index, OH_AVBuffer *data, void *userData)
{
    WVLOG_D("AudioDecoderCallbackManager %{public}s[%{public}u].", __FUNCTION__, index);
    if (codec == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager::OnInputBufferAvailable avcodec is nullptr.");
        return;
    }

    if (data == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager::OnInputBufferAvailable avbuffer is nullptr.");
        return;
    }

    std::unique_lock<std::mutex> lock(AudioCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::AudioCodecDecoderAdapterImpl *impl = FindAudioDecoder(codec);
    if (impl == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager::OnInputBufferAvailable not find decoder.");
        return;
    }
    if (impl->GetAudioDecoderCallBack() == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager::OnInputBufferAvailable audio decoder callback is nullptr.");
        return;
    }
    impl->SetInputBuffer(index, data);
    impl->GetAudioDecoderCallBack()->OnInputBufferAvailable(index);
}

void AudioDecoderCallbackManager::OnOutputBufferAvailable(
    OH_AVCodec *codec, uint32_t index, OH_AVBuffer *data, void *userData)
{
    (void)userData;
    WVLOG_I("AudioDecoderCallbackManager %{public}s.", __FUNCTION__);
    if (codec == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager avcodec is nullptr.");
        return;
    }

    std::unique_lock<std::mutex> lock(AudioCodecDecoderAdapterImpl::GetDecoderMutex());
    OHOS::NWeb::AudioCodecDecoderAdapterImpl *impl = FindAudioDecoder(codec);
    if (impl == nullptr) {
        WVLOG_E("AudioDecoderCallbackManager not find decoder.");
        return;
    }
    if (impl->GetAudioDecoderCallBack() == nullptr) {
        WVLOG_E("audio decoder callback is nullptr.");
        return;
    }
    impl->SetOutputBuffer(index, data);

    OH_AVCodecBufferAttr attr = {0};
    OH_AVErrCode errCode = OH_AVBuffer_GetBufferAttr(data, &attr);
    if (errCode != AV_ERR_OK || attr.size < 0) {
        WVLOG_E(" get buffer attr fail.");
        return;
    }

    uint8_t bufferData[attr.size];
    if (memcpy_s(bufferData, sizeof(bufferData), reinterpret_cast<uint8_t *>(OH_AVBuffer_GetAddr(data)),
        attr.size) != EOK) {
        WVLOG_E(" memcpy_s buffer fail.");
        return;
    }
    // Copy the buffer data from the data.
    impl->GetAudioDecoderCallBack()->OnOutputBufferAvailable(
        index, bufferData, attr.size, attr.pts, attr.offset, attr.flags);
}

} // namespace OHOS::NWeb

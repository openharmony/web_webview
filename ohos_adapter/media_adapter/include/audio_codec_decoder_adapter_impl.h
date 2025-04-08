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

#ifndef AUDIO_CODEC_DECODER_ADAPTER_IMPL_H
#define AUDIO_CODEC_DECODER_ADAPTER_IMPL_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <map>

#include "native_averrors.h"
#include "native_avcodec_audiodecoder.h"
#include "native_avcodec_base.h"
#include "native_cencinfo.h"
#include "native_avformat.h"
#include "avcodec_common.h"

#include "audio_codec_decoder_adapter.h"
#include "audio_cenc_info_adapter.h"

namespace OHOS::NWeb {

enum class AudioMimeType {
    MIMETYPE_UNKNOW = 0,
    MIMETYPE_AUDIO_AAC = 1, // "audio/mp4a-latm"
    MIMETYPE_AUDIO_FLAC = 2, // "audio/flac"
    MIMETYPE_AUDIO_VORBIS = 3, // "audio/vorbis"
    MIMETYPE_AUDIO_MPEG = 4, // "audio/mpeg"
    MIMETYPE_AUDIO_AMR_NB = 5, // "audio/3gpp"
    MIMETYPE_AUDIO_AMR_WB = 6, // "audio/amr-wb"
    MIMETYPE_AUDIO_G711MU = 7, // "audio/g711mu"
    MIMETYPE_AUDIO_APE = 8 // "audio/x-ape"
};

class AudioDecoderFormatAdapterImpl : public AudioDecoderFormatAdapter {
public:
    AudioDecoderFormatAdapterImpl() = default;

    ~AudioDecoderFormatAdapterImpl() override;

    int32_t GetSampleRate() override;

    int32_t GetChannelCount() override;

    int64_t GetBitRate() override;

    int32_t GetMaxInputSize() override;

    bool GetAACIsAdts() override;

    int32_t GetAudioSampleFormat() override;

    int32_t GetIdentificationHeader() override;

    int32_t GetSetupHeader() override;

    uint8_t* GetCodecConfig() override;

    uint32_t GetCodecConfigSize() override;

    void SetSampleRate(int32_t sampleRate) override;

    void SetChannelCount(int32_t channelCount) override;

    void SetBitRate(int64_t bitRate) override;

    void SetMaxInputSize(int32_t maxInputSize) override;

    void SetAACIsAdts(bool isAdts) override;

    void SetAudioSampleFormat(int32_t audioSampleFormat) override;

    void SetIdentificationHeader(int32_t idHeader) override;

    void SetSetupHeader(int32_t setupHeader) override;

    void SetCodecConfig(uint8_t* codecConfig) override;

    void SetCodecConfigSize(uint32_t size) override;

    static void PrintFormatData(std::shared_ptr<AudioDecoderFormatAdapter> format);

private:
    int32_t sampleRate_ = 0;
    int32_t channelCount_ = 0;
    int32_t bitRate_ = 0;
    int32_t maxInputSize_ = 0;
    bool aacIsAdts_ = false;
    int32_t audioSampleFormat_ = 0;
    int32_t idHeader_ = 0;
    int32_t setupHeader_ = 0;
    uint8_t* codecConfig_ = nullptr;
    uint32_t codecConfigSize_ = 0;
};

class AudioDecoderCallbackAdapterImpl {
public:
    explicit AudioDecoderCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> cb);

    ~AudioDecoderCallbackAdapterImpl() = default;

    void OnError(int32_t errorCode);

    void OnOutputFormatChanged();

    void OnInputBufferAvailable(uint32_t index);

    void OnOutputBufferAvailable(
        uint32_t index, uint8_t *bufferData, int32_t size, int64_t pts, int32_t offset, uint32_t flags);

private:
    std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> cb_ = nullptr;
};

class AudioCodecDecoderAdapterImpl : public AudioCodecDecoderAdapter {
public:
    AudioCodecDecoderAdapterImpl();

    ~AudioCodecDecoderAdapterImpl() override;

    AudioDecoderAdapterCode CreateAudioDecoderByMime(const std::string& mimetype) override;

    AudioDecoderAdapterCode CreateAudioDecoderByName(const std::string& name) override;

    AudioDecoderAdapterCode ConfigureDecoder(const std::shared_ptr<AudioDecoderFormatAdapter> format) override;

    AudioDecoderAdapterCode SetParameterDecoder(const std::shared_ptr<AudioDecoderFormatAdapter> format) override;

    AudioDecoderAdapterCode PrepareDecoder() override;

    AudioDecoderAdapterCode StartDecoder() override;

    AudioDecoderAdapterCode StopDecoder() override;

    AudioDecoderAdapterCode FlushDecoder() override;

    AudioDecoderAdapterCode ResetDecoder() override;

    AudioDecoderAdapterCode ReleaseDecoder() override;

    AudioDecoderAdapterCode QueueInputBufferDec(uint32_t index, int64_t presentationTimeUs, uint8_t *bufferData,
        int32_t bufferSize, std::shared_ptr<AudioCencInfoAdapter> cencInfo, bool isEncrypted,
        BufferFlag flag) override;

    AudioDecoderAdapterCode GetOutputFormatDec(std::shared_ptr<AudioDecoderFormatAdapter> format) override;

    AudioDecoderAdapterCode ReleaseOutputBufferDec(uint32_t index) override;

    AudioDecoderAdapterCode SetCallbackDec(
        const std::shared_ptr<OHOS::NWeb::AudioDecoderCallbackAdapter> callback) override;

    AudioDecoderAdapterCode SetDecryptionConfig(void *session, bool secureAudio) override;

    OH_AVCodec* GetAVCodec();

    AudioDecoderCallbackAdapterImpl* GetAudioDecoderCallBack();

    void SetInputBuffer(uint32_t index, OH_AVBuffer *buffer);

    void SetOutputBuffer(uint32_t index, OH_AVBuffer *buffer);

    static BufferFlag GetBufferFlag(OHOS::MediaAVCodec::AVCodecBufferFlag codecBufferFlag);

    static std::mutex& GetDecoderMutex();

private:

    AudioDecoderAdapterCode GetParamFromAVFormat(
        OH_AVFormat *avFormat, std::shared_ptr<AudioDecoderFormatAdapter> format);

    AudioDecoderAdapterCode SetBufferCencInfo(uint32_t index, std::shared_ptr<AudioCencInfoAdapter> cencInfo);

    AudioDecoderAdapterCode SetAVCencInfo(OH_AVCencInfo *avCencInfo, std::shared_ptr<AudioCencInfoAdapter> cencInfo);

    OH_AVBuffer* GetInputBuffer(uint32_t index);

    OH_AVBuffer* GetOutputBuffer(uint32_t index);

    AudioMimeType mimeType_ = AudioMimeType::MIMETYPE_UNKNOW;
    OH_AVCodec *decoder_  = nullptr;
    std::shared_ptr<AudioDecoderCallbackAdapterImpl> callback_ = nullptr;
    std::mutex inMutex_;
    std::mutex outMutex_;
    std::map<uint32_t, OH_AVBuffer *> inputBuffers_;
    std::map<uint32_t, OH_AVBuffer *> outputBuffers_;
    static std::mutex decoderMutex_;
};

class AudioDecoderCallbackManager {
public:
    static void OnError(OH_AVCodec *codec, int32_t errorCode, void *userData);

    static void OnOutputFormatChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData);

    static void OnInputBufferAvailable(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *data, void *userData);

    static void OnOutputBufferAvailable(OH_AVCodec *codec, uint32_t index, OH_AVBuffer *data, void *userData);

    static OHOS::NWeb::AudioCodecDecoderAdapterImpl *FindAudioDecoder(OH_AVCodec *decoder);

    static void DeleteAudioDecoder(OH_AVCodec *decoder);

    static void AddAudioDecoder(OHOS::NWeb::AudioCodecDecoderAdapterImpl *decoder);

private:
    static std::map<OH_AVCodec *, OHOS::NWeb::AudioCodecDecoderAdapterImpl *> decoders_;

    static std::mutex decodersMapMutex_;
};
} // namespace OHOS::NWeb

#endif // MEDIA_CODEC_DECODER_ADAPTER_H

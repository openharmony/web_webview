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

#ifndef MEDIA_CODEC_DECODER_ADAPTER_H
#define MEDIA_CODEC_DECODER_ADAPTER_H

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

#include "graphic_adapter.h"
#include "media_codec_adapter.h"
#include "audio_cenc_info_adapter.h"

namespace OHOS::NWeb {

enum class DecoderAdapterCode : int32_t { DECODER_OK = 0, DECODER_ERROR = 1, DECODER_RETRY = 2 };

class DecoderFormatAdapter {
public:
    DecoderFormatAdapter() = default;

    virtual ~DecoderFormatAdapter() = default;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual double GetFrameRate() = 0;

    virtual void SetWidth(int32_t width) = 0;

    virtual void SetHeight(int32_t height) = 0;

    virtual void SetFrameRate(double frameRate) = 0;
};

class DecoderCallbackAdapter {
public:
    DecoderCallbackAdapter() = default;

    virtual ~DecoderCallbackAdapter() = default;

    virtual void OnError(ErrorType errorType, int32_t errorCode) = 0;

    virtual void OnStreamChanged(int32_t width, int32_t height, double frameRate) = 0;

    virtual void OnNeedInputData(uint32_t index, std::shared_ptr<OhosBufferAdapter> buffer) = 0;

    virtual void OnNeedOutputData(uint32_t index, std::shared_ptr<BufferInfoAdapter> info, BufferFlag flag) = 0;
};

class MediaCodecDecoderAdapter {
public:
    MediaCodecDecoderAdapter() = default;

    virtual ~MediaCodecDecoderAdapter() = default;

    virtual DecoderAdapterCode CreateVideoDecoderByMime(const std::string& mimetype) = 0;

    virtual DecoderAdapterCode CreateVideoDecoderByName(const std::string& name) = 0;

    virtual DecoderAdapterCode ConfigureDecoder(const std::shared_ptr<DecoderFormatAdapter> format) = 0;

    virtual DecoderAdapterCode SetParameterDecoder(const std::shared_ptr<DecoderFormatAdapter> format) = 0;

    virtual DecoderAdapterCode SetOutputSurface(void* window) = 0;

    virtual DecoderAdapterCode PrepareDecoder() = 0;

    virtual DecoderAdapterCode StartDecoder() = 0;

    virtual DecoderAdapterCode StopDecoder() = 0;

    virtual DecoderAdapterCode FlushDecoder() = 0;

    virtual DecoderAdapterCode ResetDecoder() = 0;

    virtual DecoderAdapterCode ReleaseDecoder() = 0;

    virtual DecoderAdapterCode QueueInputBufferDec(
        uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, BufferFlag flag) = 0;

    virtual DecoderAdapterCode GetOutputFormatDec(std::shared_ptr<DecoderFormatAdapter> format) = 0;

    virtual DecoderAdapterCode ReleaseOutputBufferDec(uint32_t index, bool isRender) = 0;

    virtual DecoderAdapterCode SetCallbackDec(const std::shared_ptr<DecoderCallbackAdapter> callback) = 0;

    virtual DecoderAdapterCode SetDecryptionConfig(void *session, bool isSecure) = 0;

    virtual DecoderAdapterCode SetAVCencInfo(uint32_t index, const std::shared_ptr<AudioCencInfoAdapter> cencInfo) = 0;
};

} // namespace OHOS::NWeb

#endif // MEDIA_CODEC_DECODER_ADAPTER_H

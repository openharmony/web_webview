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

#ifndef MEDIA_CODEC_DECODER_ADAPTER_IMPL_H
#define MEDIA_CODEC_DECODER_ADAPTER_IMPL_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <thread>

#include "avcodec_video_decoder.h"
#include "media_description.h"
#include "media_codec_decoder_adapter.h"

namespace OHOS::NWeb {
using namespace OHOS::MediaAVCodec;

class DecoderFormatImpl final : public DecoderFormat {
public:
    DecoderFormatImpl(int32_t width, int32_t height) : DecoderFormat(width, height) {};

    ~DecoderFormatImpl() override = default;

    void SetFormatWidth(int32_t width) override;

    void SetFormatHeight(int32_t height) override;
};

class DecoderCallbackImpl : public MediaAVCodec::AVCodecCallback {
public:
    DecoderCallbackImpl(std::shared_ptr<DecoderCallbackAdapter> cb);

    ~DecoderCallbackImpl() override = default;

    void OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode) override;

    void OnOutputFormatChanged(const MediaAVCodec::Format& format) override;

    void OnInputBufferAvailable(uint32_t index, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;

    void OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
        MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<MediaAVCodec::AVSharedMemory> buffer) override;

private:
    std::shared_ptr<DecoderCallbackAdapter> cb_ = nullptr;
};

class MediaCodecDecoderAdapterImpl : public MediaCodecDecoderAdapter {
public:
    MediaCodecDecoderAdapterImpl() = default;

    ~MediaCodecDecoderAdapterImpl() override = default;

    DecoderAdapterCode CreateVideoDecoderByMime(const std::string& mimetype) override;

    DecoderAdapterCode CreateVideoDecoderByName(const std::string& name) override;

    DecoderAdapterCode ConfigureDecoder(int32_t width, int32_t height, double framerate) override;

    DecoderAdapterCode SetParameterDecoder(const DecoderFormat& format) override;

    DecoderAdapterCode SetOutputSurface(void* window) override;

    DecoderAdapterCode PrepareDecoder() override;

    DecoderAdapterCode StartDecoder() override;

    DecoderAdapterCode StopDecoder() override;

    DecoderAdapterCode FlushDecoder() override;

    DecoderAdapterCode ResetDecoder() override;

    DecoderAdapterCode ReleaseDecoder() override;

    DecoderAdapterCode QueueInputBufferDec(uint32_t index, BufferInfo info, BufferFlag flag) override;

    DecoderAdapterCode GetOutputFormatDec(int32_t& width, int32_t& height) override;

    DecoderAdapterCode ReleaseOutputBufferDec(uint32_t index, bool isRender) override;

    DecoderAdapterCode SetCallbackDec(const std::shared_ptr<DecoderCallbackAdapter>& callback) override;

    static ErrorType GetErrorType(MediaAVCodec::AVCodecErrorType codecErrorType);

    static BufferFlag GetBufferFlag(MediaAVCodec::AVCodecBufferFlag codecBufferFlag);

    static MediaAVCodec::AVCodecBufferFlag GetAVBufferFlag(BufferFlag bufferFlag);

private:
    std::shared_ptr<OHOS::MediaAVCodec::AVCodecVideoDecoder> decoder_ = nullptr;
    std::shared_ptr<DecoderCallbackImpl> callback_ = nullptr;
};

} // namespace OHOS::NWeb
#endif
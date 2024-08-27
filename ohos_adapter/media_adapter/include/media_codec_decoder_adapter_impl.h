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

#include <map>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avformat.h>
#include "media_codec_decoder_adapter.h"

namespace OHOS::NWeb {

class MediaCodecDecoderAdapterImpl : public MediaCodecDecoderAdapter {
public:
    MediaCodecDecoderAdapterImpl() = default;

    ~MediaCodecDecoderAdapterImpl() override;

    DecoderAdapterCode CreateVideoDecoderByMime(const std::string& mimetype) override;

    DecoderAdapterCode CreateVideoDecoderByName(const std::string& name) override;

    DecoderAdapterCode ConfigureDecoder(const std::shared_ptr<DecoderFormatAdapter> format) override;

    DecoderAdapterCode SetParameterDecoder(const std::shared_ptr<DecoderFormatAdapter> format) override;

    DecoderAdapterCode SetOutputSurface(void* window) override;

    DecoderAdapterCode PrepareDecoder() override;

    DecoderAdapterCode StartDecoder() override;

    DecoderAdapterCode StopDecoder() override;

    DecoderAdapterCode FlushDecoder() override;

    DecoderAdapterCode ResetDecoder() override;

    DecoderAdapterCode ReleaseDecoder() override;

    DecoderAdapterCode QueueInputBufferDec(
        uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, BufferFlag flag) override;

    DecoderAdapterCode GetOutputFormatDec(std::shared_ptr<DecoderFormatAdapter> format) override;

    DecoderAdapterCode ReleaseOutputBufferDec(uint32_t index, bool isRender) override;

    DecoderAdapterCode SetCallbackDec(const std::shared_ptr<DecoderCallbackAdapter> callback) override;

    static BufferFlag GetBufferFlag(OH_AVCodecBufferFlags codecBufferFlag);

    static OH_AVCodecBufferFlags GetAVBufferFlag(BufferFlag bufferFlag);

    void OnError(int32_t errorCode);

    void OnOutputFormatChanged(OH_AVFormat* format);

    void OnInputBufferAvailable(uint32_t index, OH_AVBuffer* buffer);

    void OnOutputBufferAvailable(uint32_t index, OH_AVBuffer* buffer);

private:
    OH_AVCodec* decoder_ = nullptr;
    std::shared_ptr<DecoderCallbackAdapter> callback_ = nullptr;
    std::map<uint32_t, OH_AVBuffer*> bufferMap_;
};
} // namespace OHOS::NWeb

#endif // MEDIA_CODEC_DECODER_ADAPTER_IMPL_H

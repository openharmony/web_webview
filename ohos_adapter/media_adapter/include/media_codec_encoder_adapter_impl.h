/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H
#define MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H

#include "media_codec_adapter.h"
#include <multimedia/player_framework/native_avcodec_videoencoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>
#include "native_window_adapter_impl.h"

namespace OHOS::NWeb {
class MediaCodecEncoderAdapterImpl;

class EncoderCallbackImpl {
public:
    EncoderCallbackImpl(std::shared_ptr<CodecCallbackAdapter> cb);
    ~EncoderCallbackImpl() = default;
    void OnError(int32_t errorCode);
    void OnOutputFormatChanged(OH_AVFormat *format);
    void OnInputBufferAvailable(uint32_t index, OH_AVBuffer *buffer);
    void OnOutputBufferAvailable(uint32_t index, OH_AVBuffer *buffer);

private:
    std::shared_ptr<CodecCallbackAdapter> cb_ = nullptr;
};

class MediaCodecEncoderAdapterImpl : public MediaCodecAdapter {
public:
    MediaCodecEncoderAdapterImpl() = default;

    ~MediaCodecEncoderAdapterImpl() override = default;

    CodecCodeAdapter CreateVideoCodecByMime(const std::string mimetype) override;
    CodecCodeAdapter CreateVideoCodecByName(const std::string name) override;
    CodecCodeAdapter SetCodecCallback(const std::shared_ptr<CodecCallbackAdapter> callback) override;
    CodecCodeAdapter Configure(const std::shared_ptr<CodecConfigParaAdapter> config) override;
    CodecCodeAdapter Prepare() override;
    CodecCodeAdapter Start() override;
    CodecCodeAdapter Stop() override;
    CodecCodeAdapter Reset() override;
    CodecCodeAdapter Release() override;
    std::shared_ptr<ProducerSurfaceAdapter> CreateInputSurface() override;
    CodecCodeAdapter ReleaseOutputBuffer(uint32_t index, bool isRender) override;
    CodecCodeAdapter RequestKeyFrameSoon() override;
    static BufferFlag GetBufferFlag(OH_AVCodecBufferFlags codecBufferFlag);

private:
    OH_AVCodec* encoder_ = nullptr;
    std::shared_ptr<EncoderCallbackImpl> callback_ = nullptr;
};
} // namespace OHOS::NWeb
#endif // MEDIA_CODEC_ENCODER_ADAPTER_IMPL_H

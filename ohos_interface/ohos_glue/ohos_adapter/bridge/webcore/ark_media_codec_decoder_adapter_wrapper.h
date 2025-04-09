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

#ifndef ARK_MEDIA_CODEC_DECODER_ADAPTER_WRAPPER_H
#define ARK_MEDIA_CODEC_DECODER_ADAPTER_WRAPPER_H
#pragma once

#include "media_codec_decoder_adapter.h"
#include "audio_cenc_info_adapter.h"
#include "ohos_adapter/include/ark_media_codec_decoder_adapter.h"

namespace OHOS::ArkWeb {

class ArkMediaCodecDecoderAdapterWrapper : public OHOS::NWeb::MediaCodecDecoderAdapter {
public:
    explicit ArkMediaCodecDecoderAdapterWrapper(ArkWebRefPtr<ArkMediaCodecDecoderAdapter>);

    OHOS::NWeb::DecoderAdapterCode CreateVideoDecoderByMime(const std::string& mimetype) override;

    OHOS::NWeb::DecoderAdapterCode CreateVideoDecoderByName(const std::string& name) override;

    OHOS::NWeb::DecoderAdapterCode ConfigureDecoder(
        const std::shared_ptr<OHOS::NWeb::DecoderFormatAdapter> format) override;

    OHOS::NWeb::DecoderAdapterCode SetParameterDecoder(
        const std::shared_ptr<OHOS::NWeb::DecoderFormatAdapter> format) override;

    OHOS::NWeb::DecoderAdapterCode SetOutputSurface(void* window) override;

    OHOS::NWeb::DecoderAdapterCode PrepareDecoder() override;

    OHOS::NWeb::DecoderAdapterCode StartDecoder() override;

    OHOS::NWeb::DecoderAdapterCode StopDecoder() override;

    OHOS::NWeb::DecoderAdapterCode FlushDecoder() override;

    OHOS::NWeb::DecoderAdapterCode ResetDecoder() override;

    OHOS::NWeb::DecoderAdapterCode ReleaseDecoder() override;

    OHOS::NWeb::DecoderAdapterCode QueueInputBufferDec(
        uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, OHOS::NWeb::BufferFlag flag) override;

    OHOS::NWeb::DecoderAdapterCode GetOutputFormatDec(
        std::shared_ptr<OHOS::NWeb::DecoderFormatAdapter> format) override;

    OHOS::NWeb::DecoderAdapterCode ReleaseOutputBufferDec(uint32_t index, bool isRender) override;

    OHOS::NWeb::DecoderAdapterCode SetCallbackDec(
        const std::shared_ptr<OHOS::NWeb::DecoderCallbackAdapter> callback) override;

    OHOS::NWeb::DecoderAdapterCode SetDecryptionConfig(void *session, bool isSecure) override;

    OHOS::NWeb::DecoderAdapterCode SetAVCencInfo(
        uint32_t index, const std::shared_ptr<NWeb::AudioCencInfoAdapter> cencInfo) override;

private:
    ArkWebRefPtr<ArkMediaCodecDecoderAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_CODEC_DECODER_ADAPTER_WRAPPER_H

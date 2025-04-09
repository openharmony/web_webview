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

#ifndef ARK_MEDIA_CODEC_DECODER_ADAPTER_IMPL_H
#define ARK_MEDIA_CODEC_DECODER_ADAPTER_IMPL_H
#pragma once

#include "media_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_media_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_audio_cenc_info_adapter.h"

namespace OHOS::ArkWeb {

class ArkMediaCodecDecoderAdapterImpl : public ArkMediaCodecDecoderAdapter {
public:
    explicit ArkMediaCodecDecoderAdapterImpl(std::shared_ptr<OHOS::NWeb::MediaCodecDecoderAdapter>);

    int32_t CreateVideoDecoderByMime(const ArkWebString& mimetype) override;

    int32_t CreateVideoDecoderByName(const ArkWebString& name) override;

    int32_t ConfigureDecoder(const ArkWebRefPtr<ArkDecoderFormatAdapter> format) override;

    int32_t SetParameterDecoder(const ArkWebRefPtr<ArkDecoderFormatAdapter> format) override;

    int32_t SetOutputSurface(void* window) override;

    int32_t PrepareDecoder() override;

    int32_t StartDecoder() override;

    int32_t StopDecoder() override;

    int32_t FlushDecoder() override;

    int32_t ResetDecoder() override;

    int32_t ReleaseDecoder() override;

    int32_t QueueInputBufferDec(
        uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, uint32_t flag) override;

    int32_t GetOutputFormatDec(ArkWebRefPtr<ArkDecoderFormatAdapter> format) override;

    int32_t ReleaseOutputBufferDec(uint32_t index, bool isRender) override;

    int32_t SetCallbackDec(const ArkWebRefPtr<ArkDecoderCallbackAdapter> callback) override;

    int32_t SetDecryptionConfig(void *session, bool isSecure) override;

    int32_t SetAVCencInfo(uint32_t index, const ArkWebRefPtr<ArkAudioCencInfoAdapter> cencInfo) override;

private:
    std::shared_ptr<OHOS::NWeb::MediaCodecDecoderAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkMediaCodecDecoderAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_CODEC_DECODER_ADAPTER_IMPL_H

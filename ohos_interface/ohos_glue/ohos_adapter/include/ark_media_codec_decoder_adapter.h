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

#ifndef ARK_MEDIA_CODEC_DECODER_ADAPTER_H
#define ARK_MEDIA_CODEC_DECODER_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_media_codec_adapter.h"
#include "ohos_adapter/include/ark_audio_cenc_info_adapter.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkDecoderFormatAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual double GetFrameRate() = 0;

    /*--ark web()--*/
    virtual void SetWidth(int32_t width) = 0;

    /*--ark web()--*/
    virtual void SetHeight(int32_t height) = 0;

    /*--ark web()--*/
    virtual void SetFrameRate(double frameRate) = 0;
};

/*--ark web(source=webcore)--*/
class ArkDecoderCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnError(int32_t errorType, int32_t errorCode) = 0;

    /*--ark web()--*/
    virtual void OnStreamChanged(int32_t width, int32_t height, double frameRate) = 0;

    /*--ark web()--*/
    virtual void OnNeedInputData(uint32_t index, ArkWebRefPtr<ArkOhosBufferAdapter> buffer) = 0;

    /*--ark web()--*/
    virtual void OnNeedOutputData(uint32_t index, ArkWebRefPtr<ArkBufferInfoAdapter> info, uint32_t flag) = 0;
};

/*--ark web(source=webview)--*/
class ArkMediaCodecDecoderAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t CreateVideoDecoderByMime(const ArkWebString& mimetype) = 0;

    /*--ark web()--*/
    virtual int32_t CreateVideoDecoderByName(const ArkWebString& name) = 0;

    /*--ark web()--*/
    virtual int32_t ConfigureDecoder(const ArkWebRefPtr<ArkDecoderFormatAdapter> format) = 0;

    /*--ark web()--*/
    virtual int32_t SetParameterDecoder(const ArkWebRefPtr<ArkDecoderFormatAdapter> format) = 0;

    /*--ark web()--*/
    virtual int32_t SetOutputSurface(void* window) = 0;

    /*--ark web()--*/
    virtual int32_t PrepareDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t StartDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t StopDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t FlushDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t ResetDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseDecoder() = 0;

    /*--ark web()--*/
    virtual int32_t QueueInputBufferDec(
        uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, uint32_t flag) = 0;

    /*--ark web()--*/
    virtual int32_t GetOutputFormatDec(ArkWebRefPtr<ArkDecoderFormatAdapter> format) = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseOutputBufferDec(uint32_t index, bool isRender) = 0;

    /*--ark web()--*/
    virtual int32_t SetCallbackDec(const ArkWebRefPtr<ArkDecoderCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual int32_t SetDecryptionConfig(void *session, bool isSecure) = 0;

    /*--ark web()--*/
    virtual int32_t SetAVCencInfo(uint32_t index, const ArkWebRefPtr<ArkAudioCencInfoAdapter> cencInfo) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_CODEC_DECODER_ADAPTER_H

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

#include "ohos_adapter/bridge/ark_media_codec_decoder_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_decoder_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_decoder_format_adapter_impl.h"
#include "ohos_adapter/bridge/ark_audio_cenc_info_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaCodecDecoderAdapterWrapper::ArkMediaCodecDecoderAdapterWrapper(ArkWebRefPtr<ArkMediaCodecDecoderAdapter> ref)
    : ctocpp_(ref)
{}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::CreateVideoDecoderByMime(const std::string& mimetype)
{
    ArkWebString str = ArkWebStringClassToStruct(mimetype);
    int32_t result = ctocpp_->CreateVideoDecoderByMime(str);
    ArkWebStringStructRelease(str);
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::CreateVideoDecoderByName(const std::string& name)
{
    ArkWebString str = ArkWebStringClassToStruct(name);
    int32_t result = ctocpp_->CreateVideoDecoderByName(str);
    ArkWebStringStructRelease(str);
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::ConfigureDecoder(
    const std::shared_ptr<OHOS::NWeb::DecoderFormatAdapter> format)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(format)) {
        result = ctocpp_->ConfigureDecoder(nullptr);
    } else {
        result = ctocpp_->ConfigureDecoder(new ArkDecoderFormatAdapterImpl(format));
    }
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::SetParameterDecoder(
    const std::shared_ptr<OHOS::NWeb::DecoderFormatAdapter> format)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(format)) {
        result = ctocpp_->SetParameterDecoder(nullptr);
    } else {
        result = ctocpp_->SetParameterDecoder(new ArkDecoderFormatAdapterImpl(format));
    }

    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::SetOutputSurface(void* window)
{
    int32_t result = ctocpp_->SetOutputSurface(window);
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::PrepareDecoder()
{
    int32_t result = ctocpp_->PrepareDecoder();
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::StartDecoder()
{
    int32_t result = ctocpp_->StartDecoder();
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::StopDecoder()
{
    int32_t result = ctocpp_->StopDecoder();
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::FlushDecoder()
{
    int32_t result = ctocpp_->FlushDecoder();
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::ResetDecoder()
{
    int32_t result = ctocpp_->ResetDecoder();
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::ReleaseDecoder()
{
    int32_t result = ctocpp_->ReleaseDecoder();
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::QueueInputBufferDec(
    uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, OHOS::NWeb::BufferFlag flag)
{
    int32_t result = ctocpp_->QueueInputBufferDec(index, presentationTimeUs, size, offset, (uint32_t)flag);
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::GetOutputFormatDec(
    std::shared_ptr<OHOS::NWeb::DecoderFormatAdapter> format)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(format)) {
        result = ctocpp_->GetOutputFormatDec(nullptr);
    } else {
        result = ctocpp_->GetOutputFormatDec(new ArkDecoderFormatAdapterImpl(format));
    }

    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::ReleaseOutputBufferDec(uint32_t index, bool isRender)
{
    int32_t result = ctocpp_->ReleaseOutputBufferDec(index, isRender);
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::SetCallbackDec(
    const std::shared_ptr<OHOS::NWeb::DecoderCallbackAdapter> callback)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        result = ctocpp_->SetCallbackDec(nullptr);
    } else {
        result = ctocpp_->SetCallbackDec(new ArkDecoderCallbackAdapterImpl(callback));
    }

    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::SetDecryptionConfig(void *session, bool isSecure)
{
    int32_t result = ctocpp_->SetDecryptionConfig(session, isSecure);
    return (OHOS::NWeb::DecoderAdapterCode)result;
}

OHOS::NWeb::DecoderAdapterCode ArkMediaCodecDecoderAdapterWrapper::SetAVCencInfo(
    uint32_t index, const std::shared_ptr<NWeb::AudioCencInfoAdapter> cencInfo)
{
    int32_t result;
    if (CHECK_SHARED_PTR_IS_NULL(cencInfo)) {
        result = ctocpp_->SetAVCencInfo(index, nullptr);
    } else {
        result = ctocpp_->SetAVCencInfo(index, new ArkAudioCencInfoAdapterImpl(cencInfo));
    }

    return (OHOS::NWeb::DecoderAdapterCode)result;
}
} // namespace OHOS::ArkWeb

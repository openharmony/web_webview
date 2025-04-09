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

#include "ohos_adapter/bridge/ark_media_codec_decoder_adapter_impl.h"

#include "ohos_adapter/bridge/ark_decoder_callback_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_decoder_format_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_cenc_info_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaCodecDecoderAdapterImpl::ArkMediaCodecDecoderAdapterImpl(
    std::shared_ptr<OHOS::NWeb::MediaCodecDecoderAdapter> ref)
    : real_(ref)
{}

int32_t ArkMediaCodecDecoderAdapterImpl::CreateVideoDecoderByMime(const ArkWebString& mimetype)
{
    return (int32_t)real_->CreateVideoDecoderByMime(ArkWebStringStructToClass(mimetype));
}

int32_t ArkMediaCodecDecoderAdapterImpl::CreateVideoDecoderByName(const ArkWebString& name)
{
    return (int32_t)real_->CreateVideoDecoderByName(ArkWebStringStructToClass(name));
}

int32_t ArkMediaCodecDecoderAdapterImpl::ConfigureDecoder(const ArkWebRefPtr<ArkDecoderFormatAdapter> format)
{
    if (CHECK_REF_PTR_IS_NULL(format)) {
        return (int32_t)real_->ConfigureDecoder(nullptr);
    }
    return (int32_t)real_->ConfigureDecoder(std::make_shared<ArkDecoderFormatAdapterWrapper>(format));
}

int32_t ArkMediaCodecDecoderAdapterImpl::SetParameterDecoder(const ArkWebRefPtr<ArkDecoderFormatAdapter> format)
{
    if (CHECK_REF_PTR_IS_NULL(format)) {
        return (int32_t)real_->SetParameterDecoder(nullptr);
    }
    return (int32_t)real_->SetParameterDecoder(std::make_shared<ArkDecoderFormatAdapterWrapper>(format));
}

int32_t ArkMediaCodecDecoderAdapterImpl::SetOutputSurface(void* window)
{
    return (int32_t)real_->SetOutputSurface(window);
}

int32_t ArkMediaCodecDecoderAdapterImpl::PrepareDecoder()
{
    return (int32_t)real_->PrepareDecoder();
}

int32_t ArkMediaCodecDecoderAdapterImpl::StartDecoder()
{
    return (int32_t)real_->StartDecoder();
}

int32_t ArkMediaCodecDecoderAdapterImpl::StopDecoder()
{
    return (int32_t)real_->StopDecoder();
}

int32_t ArkMediaCodecDecoderAdapterImpl::FlushDecoder()
{
    return (int32_t)real_->FlushDecoder();
}

int32_t ArkMediaCodecDecoderAdapterImpl::ResetDecoder()
{
    return (int32_t)real_->ResetDecoder();
}

int32_t ArkMediaCodecDecoderAdapterImpl::ReleaseDecoder()
{
    return (int32_t)real_->ReleaseDecoder();
}

int32_t ArkMediaCodecDecoderAdapterImpl::QueueInputBufferDec(
    uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, uint32_t flag)
{
    return (int32_t)real_->QueueInputBufferDec(index, presentationTimeUs, size, offset, (NWeb::BufferFlag)flag);
}

int32_t ArkMediaCodecDecoderAdapterImpl::GetOutputFormatDec(ArkWebRefPtr<ArkDecoderFormatAdapter> format)
{
    if (CHECK_REF_PTR_IS_NULL(format)) {
        return (int32_t)real_->GetOutputFormatDec(nullptr);
    }
    return (int32_t)real_->GetOutputFormatDec(std::make_shared<ArkDecoderFormatAdapterWrapper>(format));
}

int32_t ArkMediaCodecDecoderAdapterImpl::ReleaseOutputBufferDec(uint32_t index, bool isRender)
{
    return (int32_t)real_->ReleaseOutputBufferDec(index, isRender);
}

int32_t ArkMediaCodecDecoderAdapterImpl::SetCallbackDec(const ArkWebRefPtr<ArkDecoderCallbackAdapter> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        return (int32_t)real_->SetCallbackDec(nullptr);
    }

    return (int32_t)real_->SetCallbackDec(std::make_shared<ArkDecoderCallbackAdapterWrapper>(callback));
}

int32_t ArkMediaCodecDecoderAdapterImpl::SetDecryptionConfig(void *session, bool isSecure)
{
    return (int32_t)real_->SetDecryptionConfig(session, isSecure);
}

int32_t ArkMediaCodecDecoderAdapterImpl::SetAVCencInfo(
    uint32_t index, const ArkWebRefPtr<ArkAudioCencInfoAdapter> cencInfo)
{
    if (CHECK_REF_PTR_IS_NULL(cencInfo)) {
        return (int32_t)real_->SetAVCencInfo(index, nullptr);
    }

    return (int32_t)real_->SetAVCencInfo(index, std::make_shared<ArkAudioCencInfoAdapterWrapper>(cencInfo));
}

} // namespace OHOS::ArkWeb

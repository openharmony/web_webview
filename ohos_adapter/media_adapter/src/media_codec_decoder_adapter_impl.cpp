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

#include "media_codec_decoder_adapter_impl.h"
#include <native_buffer/native_buffer.h>
#include <unordered_map>
#include "buffer_info_adapter_impl.h"
#include "ohos_buffer_adapter_impl.h"

using namespace OHOS::NWeb;

namespace {
const std::unordered_map<OH_AVCodecBufferFlags, BufferFlag> BUFFER_FLAG_MAP = {
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE, BufferFlag::CODEC_BUFFER_FLAG_NONE },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS, BufferFlag::CODEC_BUFFER_FLAG_EOS },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_INCOMPLETE_FRAME,
        BufferFlag::CODEC_BUFFER_FLAG_PARTIAL_FRAME },
    { OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_CODEC_DATA, BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA }
};

const std::unordered_map<BufferFlag, OH_AVCodecBufferFlags> AV_BUFFER_FLAG_MAP = {
    { BufferFlag::CODEC_BUFFER_FLAG_NONE, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE },
    { BufferFlag::CODEC_BUFFER_FLAG_EOS, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS },
    { BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_SYNC_FRAME },
    { BufferFlag::CODEC_BUFFER_FLAG_PARTIAL_FRAME,
        OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_INCOMPLETE_FRAME },
    { BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA, OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_CODEC_DATA }
};
} // namespace

MediaCodecDecoderAdapterImpl::~MediaCodecDecoderAdapterImpl()
{
    if (decoder_ != nullptr) {
        ReleaseDecoder();
    }
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::CreateVideoDecoderByMime(const std::string& mimetype)
{
    decoder_ = OH_VideoDecoder_CreateByMime(mimetype.c_str());
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName(const std::string& name)
{
    decoder_ = OH_VideoDecoder_CreateByName(name.c_str());
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ConfigureDecoder(const std::shared_ptr<DecoderFormatAdapter> format)
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat* codecFormat = OH_AVFormat_Create();
    if (codecFormat == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_WIDTH, format->GetWidth());
    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_HEIGHT, format->GetHeight());

    OH_AVErrCode ret = OH_VideoDecoder_Configure(decoder_, codecFormat);
    OH_AVFormat_Destroy(codecFormat);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetParameterDecoder(const std::shared_ptr<DecoderFormatAdapter> format)
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat* codecFormat = OH_AVFormat_Create();
    if (codecFormat == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_WIDTH, format->GetWidth());
    OH_AVFormat_SetIntValue(codecFormat, OH_MD_KEY_HEIGHT, format->GetHeight());

    OH_AVErrCode ret = OH_VideoDecoder_SetParameter(decoder_, codecFormat);
    OH_AVFormat_Destroy(codecFormat);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetOutputSurface(void* window)
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OHNativeWindow* window_ = reinterpret_cast<OHNativeWindow*>(window);
    if (window_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    int32_t usage = NATIVEBUFFER_USAGE_MEM_DMA;
    OH_NativeWindow_NativeWindowHandleOpt(window_, SET_USAGE, usage);

    OH_AVErrCode ret = OH_VideoDecoder_SetSurface(decoder_, window_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::PrepareDecoder()
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode ret = OH_VideoDecoder_Prepare(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::StartDecoder()
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Start(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::StopDecoder()
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Stop(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::FlushDecoder()
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Flush(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ResetDecoder()
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Reset(decoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ReleaseDecoder()
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    OH_AVErrCode ret = OH_VideoDecoder_Destroy(decoder_);
    decoder_ = nullptr;
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::QueueInputBufferDec(
    uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, BufferFlag flag)
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVCodecBufferAttr attr;
    attr.pts = presentationTimeUs;
    attr.size = size;
    attr.offset = offset;
    attr.flags = MediaCodecDecoderAdapterImpl::GetAVBufferFlag(flag);

    OH_AVBuffer_SetBufferAttr(bufferMap_[index], &attr);
    OH_AVErrCode ret = OH_VideoDecoder_PushInputBuffer(decoder_, index);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::GetOutputFormatDec(std::shared_ptr<DecoderFormatAdapter> format)
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVFormat* codecFormat = OH_VideoDecoder_GetOutputDescription(decoder_);
    if (codecFormat == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t width = 0;
    int32_t height = 0;
    OH_AVFormat_GetIntValue(codecFormat, OH_MD_KEY_VIDEO_PIC_WIDTH, &width);
    OH_AVFormat_GetIntValue(codecFormat, OH_MD_KEY_VIDEO_PIC_HEIGHT, &height);
    format->SetWidth(width);
    format->SetHeight(height);
    OH_AVFormat_Destroy(codecFormat);

    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ReleaseOutputBufferDec(uint32_t index, bool isRender)
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OH_AVErrCode ret;
    if (isRender) {
        ret = OH_VideoDecoder_RenderOutputBuffer(decoder_, index);
    } else {
        ret = OH_VideoDecoder_FreeOutputBuffer(decoder_, index);
    }
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetCallbackDec(const std::shared_ptr<DecoderCallbackAdapter> callback)
{
    if (decoder_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (callback == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    callback_ = callback;
    if (callback_ == nullptr) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    struct OH_AVCodecCallback cb;
    cb.onError = [] (OH_AVCodec *codec, int32_t errorCode, void *userData) {
        (void)codec;
        static_cast<MediaCodecDecoderAdapterImpl*>(userData)->OnError(errorCode);
    };
    cb.onStreamChanged = [] (OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
        (void)codec;
        static_cast<MediaCodecDecoderAdapterImpl*>(userData)->OnOutputFormatChanged(format);
    };
    cb.onNeedInputBuffer = [] (OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
        (void)codec;
        static_cast<MediaCodecDecoderAdapterImpl*>(userData)->OnInputBufferAvailable(index, buffer);
    };
    cb.onNewOutputBuffer = [] (OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
        (void)codec;
        static_cast<MediaCodecDecoderAdapterImpl*>(userData)->OnOutputBufferAvailable(index, buffer);
    };

    OH_AVErrCode ret = OH_VideoDecoder_RegisterCallback(decoder_, cb, this);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

BufferFlag MediaCodecDecoderAdapterImpl::GetBufferFlag(OH_AVCodecBufferFlags codecBufferFlag)
{
    auto flag = BUFFER_FLAG_MAP.find(codecBufferFlag);
    if (flag == BUFFER_FLAG_MAP.end()) {
        return BufferFlag::CODEC_BUFFER_FLAG_NONE;
    }
    return flag->second;
}

OH_AVCodecBufferFlags MediaCodecDecoderAdapterImpl::GetAVBufferFlag(BufferFlag bufferFlag)
{
    auto flag = AV_BUFFER_FLAG_MAP.find(bufferFlag);
    if (flag == AV_BUFFER_FLAG_MAP.end()) {
        return OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_NONE;
    }
    return flag->second;
}

void MediaCodecDecoderAdapterImpl::OnError(int32_t errorCode)
{
    if (!callback_) {
        return;
    }

    callback_->OnError(ErrorType::CODEC_ERROR_INTERNAL, errorCode);
}

void MediaCodecDecoderAdapterImpl::OnOutputFormatChanged(OH_AVFormat* format)
{
    if (!callback_) {
        return;
    }

    int32_t width = 0;
    int32_t height = 0;
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_WIDTH, &width);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_HEIGHT, &height);

    callback_->OnStreamChanged(width, height, 0);
}

void MediaCodecDecoderAdapterImpl::OnInputBufferAvailable(uint32_t index, OH_AVBuffer* buffer)
{
    if (!callback_) {
        return;
    }

    if (buffer == nullptr || OH_AVBuffer_GetAddr(buffer) == nullptr) {
        return;
    }

    std::shared_ptr<OhosBufferAdapterImpl> ohosBuffer = std::make_shared<OhosBufferAdapterImpl>();
    if (ohosBuffer == nullptr) {
        return;
    }

    bufferMap_[index] = buffer;

    ohosBuffer->SetAddr(OH_AVBuffer_GetAddr(buffer));
    ohosBuffer->SetBufferSize(OH_AVBuffer_GetCapacity(buffer));
    callback_->OnNeedInputData(index, ohosBuffer);
}

void MediaCodecDecoderAdapterImpl::OnOutputBufferAvailable(uint32_t index, OH_AVBuffer* buffer)
{
    if (!callback_) {
        return;
    }

    OH_AVCodecBufferAttr attr;
    OH_AVErrCode ret = OH_AVBuffer_GetBufferAttr(buffer, &attr);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return;
    }

    std::shared_ptr<BufferInfoAdapterImpl> info_ = std::make_shared<BufferInfoAdapterImpl>();
    if (!info_) {
        return;
    }

    info_->SetPresentationTimeUs(attr.pts);
    info_->SetSize(attr.size);
    info_->SetOffset(attr.offset);

    BufferFlag flag_;

    flag_ = MediaCodecDecoderAdapterImpl::GetBufferFlag(static_cast<OH_AVCodecBufferFlags>(attr.flags));
    callback_->OnNeedOutputData(index, info_, flag_);
}

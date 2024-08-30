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

#include "media_codec_encoder_adapter_impl.h"
#include "ohos_buffer_adapter_impl.h"
#include "buffer_info_adapter_impl.h"
#include "capability_data_adapter_impl.h"
#include "codec_format_adapter_impl.h"

#include <unordered_map>

#include "nweb_log.h"

namespace OHOS::NWeb {
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

CodecCodeAdapter MediaCodecEncoderAdapterImpl::CreateVideoCodecByMime(const std::string mimetype)
{
    encoder_ = OH_VideoEncoder_CreateByMime(mimetype.c_str());
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder create by mime failed.");
        return CodecCodeAdapter::ERROR;
    }

    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::CreateVideoCodecByName(const std::string name)
{
    encoder_ = OH_VideoEncoder_CreateByName(name.c_str());
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder create by name failed.");
        return CodecCodeAdapter::ERROR;
    }

    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::SetCodecCallback(const std::shared_ptr<CodecCallbackAdapter> callback)
{
    if (callback == nullptr) {
        WVLOG_E("Media Callback is NULL.");
        return CodecCodeAdapter::ERROR;
    }

    callback_ = std::make_shared<EncoderCallbackImpl>(callback);
    if (callback_ == nullptr) {
        WVLOG_E("create Callback failed.");
        return CodecCodeAdapter::ERROR;
    }

    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr.");
        return CodecCodeAdapter::ERROR;
    }

    struct OH_AVCodecCallback cb;
    cb.onError = [] (OH_AVCodec *codec, int32_t errorCode, void *userData) {
        (void)codec;
        static_cast<EncoderCallbackImpl*>(userData)->OnError(errorCode);
    };
    cb.onStreamChanged = [] (OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
        (void)codec;
        static_cast<EncoderCallbackImpl*>(userData)->OnOutputFormatChanged(format);
    };
    cb.onNeedInputBuffer = [] (OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
        (void)codec;
        static_cast<EncoderCallbackImpl*>(userData)->OnInputBufferAvailable(index, buffer);
    };
    cb.onNewOutputBuffer = [] (OH_AVCodec *codec, uint32_t index, OH_AVBuffer *buffer, void *userData) {
        (void)codec;
        static_cast<EncoderCallbackImpl*>(userData)->OnOutputBufferAvailable(index, buffer);
    };

    OH_AVErrCode ret = OH_VideoEncoder_RegisterCallback(encoder_, cb, callback_.get());
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("MediaCodecEncoder set callback failed.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::Configure(const std::shared_ptr<CodecConfigParaAdapter> config)
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when ConfigureEncoder.");
        return CodecCodeAdapter::ERROR;
    }

    if (config == nullptr) {
        WVLOG_E("config is nullptr when ConfigureEncoder.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVFormat *avCodecFormat = OH_AVFormat_Create();
    if (avCodecFormat == nullptr) {
        return CodecCodeAdapter::ERROR;
    }

    OH_AVFormat_SetIntValue(avCodecFormat, OH_MD_KEY_WIDTH, config->GetWidth());
    OH_AVFormat_SetIntValue(avCodecFormat, OH_MD_KEY_HEIGHT, config->GetHeight());
    OH_AVFormat_SetDoubleValue(avCodecFormat, OH_MD_KEY_FRAME_RATE, config->GetFrameRate());
    OH_AVFormat_SetLongValue(avCodecFormat, OH_MD_KEY_BITRATE, config->GetBitRate());
    OH_AVFormat_SetIntValue(avCodecFormat, OH_MD_KEY_VIDEO_ENCODE_BITRATE_MODE, OH_VideoEncodeBitrateMode::VBR);
    OH_AVFormat_SetIntValue(avCodecFormat, OH_MD_KEY_PIXEL_FORMAT, (int32_t)OH_AVPixelFormat::AV_PIXEL_FORMAT_YUVI420);
    WVLOG_I("Configure width: %{public}d, height: %{public}d, bitRate: %{public}d, framerate: %{public}lf,",
        config->GetWidth(), config->GetHeight(), (int32_t)config->GetBitRate(), config->GetFrameRate());
    OH_AVErrCode ret = OH_VideoEncoder_Configure(encoder_, avCodecFormat);
    OH_AVFormat_Destroy(avCodecFormat);

    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder config error.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::Prepare()
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when PrepareEncoder.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVErrCode ret = OH_VideoEncoder_Prepare(encoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder PrepareEncoder error.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::Start()
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when StartEncoder.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVErrCode ret = OH_VideoEncoder_Start(encoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder Start error.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::Stop()
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when StopEncoder.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVErrCode ret = OH_VideoEncoder_Stop(encoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder Stop error.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::Reset()
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when ResetEncoder.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVErrCode ret = OH_VideoEncoder_Reset(encoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder Reset error.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::Release()
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when ReleaseEncoder.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVErrCode ret = OH_VideoEncoder_Destroy(encoder_);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder Release error.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

std::shared_ptr<ProducerSurfaceAdapter> MediaCodecEncoderAdapterImpl::CreateInputSurface()
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when CreateInputSurface.");
        return nullptr;
    }

    OHNativeWindow *window = nullptr;
    OH_AVErrCode ret = OH_VideoEncoder_GetSurface(encoder_, &window);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder create input surface error.");
        return nullptr;
    }

    return std::make_shared<ProducerNativeAdapterImpl>(window);
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::ReleaseOutputBuffer(uint32_t index, bool isRender)
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVErrCode ret = OH_VideoEncoder_FreeOutputBuffer(encoder_, index);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("release buffer failed.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

CodecCodeAdapter MediaCodecEncoderAdapterImpl::RequestKeyFrameSoon()
{
    if (encoder_ == nullptr) {
        WVLOG_E("MediaCodecEncoder is nullptr when RequestKeyFrameSoon.");
        return CodecCodeAdapter::ERROR;
    }

    OH_AVFormat *avCodecFormat = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(avCodecFormat, OH_MD_KEY_REQUEST_I_FRAME, true);

    OH_AVErrCode ret = OH_VideoEncoder_SetParameter(encoder_, avCodecFormat);
    OH_AVFormat_Destroy(avCodecFormat);

    if (ret != OH_AVErrCode::AV_ERR_OK) {
        WVLOG_E("encoder SetParameter error.");
        return CodecCodeAdapter::ERROR;
    }
    return CodecCodeAdapter::OK;
}

BufferFlag MediaCodecEncoderAdapterImpl::GetBufferFlag(OH_AVCodecBufferFlags codecBufferFlag)
{
    auto flag = BUFFER_FLAG_MAP.find(codecBufferFlag);
    if (flag == BUFFER_FLAG_MAP.end()) {
        WVLOG_E("buffer flag not found.");
        return BufferFlag::CODEC_BUFFER_FLAG_NONE;
    }
    return flag->second;
}

EncoderCallbackImpl::EncoderCallbackImpl(std::shared_ptr<CodecCallbackAdapter> cb) : cb_(cb) {};

void EncoderCallbackImpl::OnError(int32_t errorCode)
{
    if (!cb_) {
        WVLOG_E("callback is null.");
        return;
    }
    cb_->OnError(ErrorType::CODEC_ERROR_INTERNAL, errorCode);
}

void EncoderCallbackImpl::OnOutputFormatChanged(OH_AVFormat *format)
{
    if (!cb_) {
        WVLOG_E("callback is null.");
        return;
    }

    std::shared_ptr<CodecFormatAdapterImpl> formatAdapter = std::make_shared<CodecFormatAdapterImpl>();
    if (!formatAdapter) {
        WVLOG_E("formatAdapter is null");
        return;
    }

    int32_t width = 0;
    int32_t height = 0;
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_WIDTH, &width);
    OH_AVFormat_GetIntValue(format, OH_MD_KEY_HEIGHT, &height);
    formatAdapter->SetWidth(width);
    formatAdapter->SetHeight(height);
    cb_->OnStreamChanged(formatAdapter);
}

void EncoderCallbackImpl::OnInputBufferAvailable(uint32_t index, OH_AVBuffer *buffer)
{
    if (!cb_) {
        WVLOG_E("callback is null.");
        return;
    }

    if (buffer == nullptr || OH_AVBuffer_GetAddr(buffer) == nullptr) {
        WVLOG_E("callback input buffer is null");
        return;
    }

    std::shared_ptr<OhosBufferAdapterImpl> ohosBuffer = std::make_shared<OhosBufferAdapterImpl>();
    if (!ohosBuffer) {
        WVLOG_E("new OhosBufferAdapterImpl failed");
        return;
    }

    ohosBuffer->SetAddr(OH_AVBuffer_GetAddr(buffer));
    ohosBuffer->SetBufferSize(OH_AVBuffer_GetCapacity(buffer));
    cb_->OnNeedInputData(index, ohosBuffer);
}

void EncoderCallbackImpl::OnOutputBufferAvailable(
    uint32_t index, OH_AVBuffer *buffer)
{
    if (!cb_) {
        WVLOG_E("callback is null.");
        return;
    }

    OH_AVCodecBufferAttr attr;
    OH_AVErrCode ret = OH_AVBuffer_GetBufferAttr(buffer, &attr);
    if (ret != OH_AVErrCode::AV_ERR_OK) {
        return;
    }

    if (buffer == nullptr || OH_AVBuffer_GetAddr(buffer) == nullptr) {
        WVLOG_E("callback output buffer is null");
        return;
    }

    std::shared_ptr<BufferInfoAdapterImpl> bufferInfo = std::make_shared<BufferInfoAdapterImpl>();
    if (bufferInfo == nullptr) {
        WVLOG_E("new bufferInfoAdapterImpl failed");
        return;
    }

    std::shared_ptr<OhosBufferAdapterImpl> ohosBuffer = std::make_shared<OhosBufferAdapterImpl>();
    if (ohosBuffer == nullptr) {
        WVLOG_E("new OhosBufferAdapterImpl failed");
        return;
    }

    bufferInfo->SetPresentationTimeUs(attr.pts);
    bufferInfo->SetSize(attr.size);
    bufferInfo->SetOffset(attr.offset);

    auto attrflags = static_cast<OH_AVCodecBufferFlags>(attr.flags);
    BufferFlag flagAdapter = MediaCodecEncoderAdapterImpl::GetBufferFlag(attrflags);

    ohosBuffer->SetAddr(OH_AVBuffer_GetAddr(buffer));
    ohosBuffer->SetBufferSize(attr.size);
    cb_->OnNeedOutputData(index, bufferInfo, flagAdapter, ohosBuffer);
}
} // namespace OHOS::NWeb

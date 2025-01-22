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
#include "buffer_info_adapter_impl.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <system_error>
#include <unordered_map>
#include <utility>

#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/window.h"
#include "native_window.h"
#include "nweb_log.h"
#include "ohos_buffer_adapter_impl.h"
#include "avcodec_list.h"

using namespace OHOS::NWeb;
using namespace std;
namespace {
const std::unordered_map<OHOS::MediaAVCodec::AVCodecErrorType, ErrorType> ERROR_TYPE_MAP = {
    { OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_INTERNAL, ErrorType::CODEC_ERROR_INTERNAL },
    { OHOS::MediaAVCodec::AVCodecErrorType::AVCODEC_ERROR_EXTEND_START, ErrorType::CODEC_ERROR_EXTEND_START }
};

const std::unordered_map<OHOS::MediaAVCodec::AVCodecBufferFlag, BufferFlag> BUFFER_FLAG_MAP = {
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE, BufferFlag::CODEC_BUFFER_FLAG_NONE },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS, BufferFlag::CODEC_BUFFER_FLAG_EOS },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_SYNC_FRAME, BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_PARTIAL_FRAME,
        BufferFlag::CODEC_BUFFER_FLAG_PARTIAL_FRAME },
    { OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_CODEC_DATA, BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA }
};

const std::unordered_map<BufferFlag, OHOS::MediaAVCodec::AVCodecBufferFlag> AV_BUFFER_FLAG_MAP = {
    { BufferFlag::CODEC_BUFFER_FLAG_NONE, OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE },
    { BufferFlag::CODEC_BUFFER_FLAG_EOS, OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_EOS },
    { BufferFlag::CODEC_BUFFER_FLAG_SYNC_FRAME, OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_SYNC_FRAME },
    { BufferFlag::CODEC_BUFFER_FLAG_PARTIAL_FRAME,
        OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_PARTIAL_FRAME },
    { BufferFlag::CODEC_BUFFER_FLAG_CODEC_DATA, OHOS::MediaAVCodec::AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_CODEC_DATA }
};
} // namespace

DecoderCallbackImpl::DecoderCallbackImpl(std::shared_ptr<DecoderCallbackAdapter> cb) : cb_(cb) {};

DecoderAdapterCode MediaCodecDecoderAdapterImpl::CreateVideoDecoderByMime(const std::string& mimetype)
{
    decoder_ = VideoDecoderFactory::CreateByMime(mimetype);
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder create failed.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::CreateVideoDecoderByName(const std::string& name)
{
    std::shared_ptr<OHOS::MediaAVCodec::AVCodecList> codecCapability = AVCodecListFactory::CreateAVCodecList();
    if (codecCapability == nullptr) {
        WVLOG_E("MediaCodecDecoder CreateAVCodecList failed.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    capabilityData_ = codecCapability->GetCapability(name, false, AVCodecCategory::AVCODEC_HARDWARE);
    if (capabilityData_ != nullptr) {
        decoder_ = VideoDecoderFactory::CreateByName(capabilityData_->codecName);
        if (decoder_ != nullptr) {
            isHardwareDecode_ = true;
            WVLOG_I("MediaCodecDecoder create hardware decoder.");
            return DecoderAdapterCode::DECODER_OK;
        }
    }

    capabilityData_ = codecCapability->GetCapability(name, false, AVCodecCategory::AVCODEC_SOFTWARE);
    if (capabilityData_ != nullptr) {
        decoder_ = VideoDecoderFactory::CreateByName(capabilityData_->codecName);
        if (decoder_ != nullptr) {
            isHardwareDecode_ = false;
            WVLOG_I("MediaCodecDecoder create software decoder.");
            return DecoderAdapterCode::DECODER_OK;
        }
    }

    WVLOG_E("MediaCodecDecoder create decoder failed.");
    return DecoderAdapterCode::DECODER_ERROR;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ConfigureDecoder(const std::shared_ptr<DecoderFormatAdapter> format)
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (capabilityData_ != nullptr) {
        OHOS::MediaAVCodec::VideoCaps videoCaps(capabilityData_);
        if (!videoCaps.IsSizeSupported(format->GetWidth(), format->GetHeight())) {
            WVLOG_E("MediaCodecDecoder video size is not supported.");
            return DecoderAdapterCode::DECODER_ERROR;
        }
    }

    OHOS::Media::Format codecFormat;

    codecFormat.PutIntValue(OHOS::MediaAVCodec::MediaDescriptionKey::MD_KEY_WIDTH, format->GetWidth());
    codecFormat.PutIntValue(OHOS::MediaAVCodec::MediaDescriptionKey::MD_KEY_HEIGHT, format->GetHeight());

    int32_t ret = decoder_->Configure(codecFormat);
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetParameterDecoder(const std::shared_ptr<DecoderFormatAdapter> format)
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OHOS::Media::Format codecFormat;

    codecFormat.PutIntValue(OHOS::MediaAVCodec::MediaDescriptionKey::MD_KEY_WIDTH, format->GetWidth());
    codecFormat.PutIntValue(OHOS::MediaAVCodec::MediaDescriptionKey::MD_KEY_HEIGHT, format->GetHeight());

    int32_t ret = decoder_->SetParameter(codecFormat);
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetOutputSurface(void* window)
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    OHNativeWindow* window_ = reinterpret_cast<OHNativeWindow*>(window);
    if (window_ == nullptr || window_->surface == nullptr) {
        WVLOG_E("Window is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (isHardwareDecode_) {
        uint64_t usage = BUFFER_USAGE_MEM_DMA;
        NativeWindowHandleOpt(window_, SET_USAGE, usage);
        WVLOG_I("MediaCodecDecoder default to opening Hebc.");
    }

    int32_t ret = decoder_->SetOutputSurface(window_->surface);
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::PrepareDecoder()
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t ret = decoder_->Prepare();
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::StartDecoder()
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    int32_t ret = decoder_->Start();
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::StopDecoder()
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    int32_t ret = decoder_->Stop();
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::FlushDecoder()
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    int32_t ret = decoder_->Flush();
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ResetDecoder()
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    int32_t ret = decoder_->Reset();
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ReleaseDecoder()
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }
    int32_t ret = decoder_->Release();
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::QueueInputBufferDec(
    uint32_t index, int64_t presentationTimeUs, int32_t size, int32_t offset, BufferFlag flag)
{
    struct OHOS::MediaAVCodec::AVCodecBufferInfo bufferInfo;

    bufferInfo.presentationTimeUs = presentationTimeUs;
    bufferInfo.size = size;
    bufferInfo.offset = offset;

    AVCodecBufferFlag bufferFlag = MediaCodecDecoderAdapterImpl::GetAVBufferFlag(flag);

    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t ret = decoder_->QueueInputBuffer(index, bufferInfo, bufferFlag);
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::GetOutputFormatDec(std::shared_ptr<DecoderFormatAdapter> format)
{
    OHOS::Media::Format codecFormat;

    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (format == nullptr) {
        WVLOG_E("format is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t ret = decoder_->GetOutputFormat(codecFormat);
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t width = 0;
    int32_t height = 0;
    codecFormat.GetIntValue(OHOS::Media::Tag::VIDEO_PIC_WIDTH, width);
    codecFormat.GetIntValue(OHOS::Media::Tag::VIDEO_PIC_HEIGHT, height);
    format->SetWidth(width);
    format->SetHeight(height);

    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::ReleaseOutputBufferDec(uint32_t index, bool isRender)
{
    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t ret = decoder_->ReleaseOutputBuffer(index, isRender);
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

DecoderAdapterCode MediaCodecDecoderAdapterImpl::SetCallbackDec(const std::shared_ptr<DecoderCallbackAdapter> callback)
{
    if (callback == nullptr) {
        WVLOG_E("Media Callback is NULL.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    callback_ = std::make_shared<DecoderCallbackImpl>(callback);
    if (callback_ == nullptr) {
        WVLOG_E("Create Callback failed.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    if (decoder_ == nullptr) {
        WVLOG_E("MediaCodecDecoder is nullptr.");
        return DecoderAdapterCode::DECODER_ERROR;
    }

    int32_t ret = decoder_->SetCallback(callback_);
    if (ret != AVCodecServiceErrCode::AVCS_ERR_OK) {
        return DecoderAdapterCode::DECODER_ERROR;
    }
    return DecoderAdapterCode::DECODER_OK;
}

ErrorType MediaCodecDecoderAdapterImpl::GetErrorType(AVCodecErrorType codecErrorType)
{
    auto type = ERROR_TYPE_MAP.find(codecErrorType);
    if (type == ERROR_TYPE_MAP.end()) {
        WVLOG_E("error type not found.");
        return ErrorType::CODEC_ERROR_INTERNAL;
    }
    return type->second;
}

BufferFlag MediaCodecDecoderAdapterImpl::GetBufferFlag(AVCodecBufferFlag codecBufferFlag)
{
    auto flag = BUFFER_FLAG_MAP.find(codecBufferFlag);
    if (flag == BUFFER_FLAG_MAP.end()) {
        WVLOG_E("buffer flag not found.");
        return BufferFlag::CODEC_BUFFER_FLAG_NONE;
    }
    return flag->second;
}

AVCodecBufferFlag MediaCodecDecoderAdapterImpl::GetAVBufferFlag(BufferFlag bufferFlag)
{
    auto flag = AV_BUFFER_FLAG_MAP.find(bufferFlag);
    if (flag == AV_BUFFER_FLAG_MAP.end()) {
        WVLOG_E("buffer flag not found.");
        return AVCodecBufferFlag::AVCODEC_BUFFER_FLAG_NONE;
    }
    return flag->second;
}

void DecoderCallbackImpl::OnError(AVCodecErrorType errorType, int32_t errorCode)
{
    if (!cb_) {
        WVLOG_E("callback is NULL.");
        return;
    }

    ErrorType errType = MediaCodecDecoderAdapterImpl::GetErrorType(errorType);

    cb_->OnError(errType, errorCode);
}

void DecoderCallbackImpl::OnOutputFormatChanged(const Media::Format& format)
{
    if (!cb_) {
        WVLOG_E("callback is NULL.");
        return;
    }

    int32_t width = 0;
    int32_t height = 0;
    format.GetIntValue(OHOS::MediaAVCodec::MediaDescriptionKey::MD_KEY_WIDTH, width);
    format.GetIntValue(OHOS::MediaAVCodec::MediaDescriptionKey::MD_KEY_HEIGHT, height);

    cb_->OnStreamChanged(width, height, 0);
}

void DecoderCallbackImpl::OnInputBufferAvailable(uint32_t index, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    if (!cb_) {
        WVLOG_E("callback is NULL.");
        return;
    }

    if (buffer == nullptr || buffer->GetBase() == nullptr) {
        return;
    }

    std::shared_ptr<OhosBufferAdapterImpl> ohosBuffer = std::make_shared<OhosBufferAdapterImpl>();
    if (ohosBuffer == nullptr) {
        WVLOG_E("new OhosBufferAdapterImpl failed");
        return;
    }

    ohosBuffer->SetAddr(buffer->GetBase());
    ohosBuffer->SetBufferSize(buffer->GetSize());
    cb_->OnNeedInputData(index, ohosBuffer);
}

void DecoderCallbackImpl::OnOutputBufferAvailable(
    uint32_t index, AVCodecBufferInfo info, AVCodecBufferFlag flag, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    if (!cb_) {
        WVLOG_E("callback is NULL.");
        return;
    }

    std::shared_ptr<BufferInfoAdapterImpl> info_ = std::make_shared<BufferInfoAdapterImpl>();
    if (!info_) {
        WVLOG_E("new BufferInfoAdapterImpl failed");
        return;
    }

    info_->SetPresentationTimeUs(info.presentationTimeUs);
    info_->SetSize(info.size);
    info_->SetOffset(info.offset);

    BufferFlag flag_;

    flag_ = MediaCodecDecoderAdapterImpl::GetBufferFlag(flag);
    cb_->OnNeedOutputData(index, info_, flag_);
}

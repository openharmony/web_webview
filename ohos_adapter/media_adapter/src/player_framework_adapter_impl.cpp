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

#include "player_framework_adapter_impl.h"

#include "foundation/multimedia/player_framework/interfaces/inner_api/native/media_errors.h"
#include "nweb_log.h"
#include "surface_adapter_impl.h"

namespace OHOS::NWeb {
namespace {
bool IsUnsupportType(int32_t errorCode)
{
    switch (errorCode) {
        case Media::MSERR_UNSUPPORT:
        case Media::MSERR_UNSUPPORT_AUD_SRC_TYPE:
        case Media::MSERR_UNSUPPORT_AUD_SAMPLE_RATE:
        case Media::MSERR_UNSUPPORT_AUD_CHANNEL_NUM:
        case Media::MSERR_UNSUPPORT_AUD_ENC_TYPE:
        case Media::MSERR_UNSUPPORT_AUD_PARAMS:
        case Media::MSERR_UNSUPPORT_VID_SRC_TYPE:
        case Media::MSERR_UNSUPPORT_VID_ENC_TYPE:
        case Media::MSERR_UNSUPPORT_VID_PARAMS:
        case Media::MSERR_UNSUPPORT_CONTAINER_TYPE:
        case Media::MSERR_UNSUPPORT_PROTOCOL_TYPE:
        case Media::MSERR_UNSUPPORT_VID_DEC_TYPE:
        case Media::MSERR_UNSUPPORT_AUD_DEC_TYPE:
        case Media::MSERR_UNSUPPORT_STREAM:
        case Media::MSERR_UNSUPPORT_FILE:
        case Media::MSERR_UNSUPPORT_SOURCE:
            return true;
        default:
            return false;
    }
}

bool IsFatalError(int32_t errorCode)
{
    switch (errorCode) {
        case Media::MSERR_EXT_NO_MEMORY:
        case Media::MSERR_EXT_SERVICE_DIED:
        case Media::MSERR_CREATE_PLAYER_ENGINE_FAILED:
        case Media::MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED:
        case Media::MSERR_AUD_DEC_FAILED:
        case Media::MSERR_VID_DEC_FAILED:
        case Media::MSERR_FILE_ACCESS_FAILED:
        case Media::MSERR_DATA_SOURCE_IO_ERROR:
            return true;
        default:
            return false;
    }
}

NWeb::PlayerOnInfoType ConvertInfoType(Media::PlayerOnInfoType infoType)
{
    NWeb::PlayerOnInfoType ret = NWeb::PlayerOnInfoType::INFO_TYPE_UNSET;
    switch (infoType) {
        case Media::INFO_TYPE_SEEKDONE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_SEEKDONE;
            break;
        case Media::INFO_TYPE_EOS:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_EOS;
            break;
        case Media::INFO_TYPE_STATE_CHANGE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_STATE_CHANGE;
            break;
        case Media::INFO_TYPE_POSITION_UPDATE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_POSITION_UPDATE;
            break;
        case Media::INFO_TYPE_MESSAGE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_MESSAGE;
            break;
        case Media::INFO_TYPE_INTERRUPT_EVENT:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_INTERRUPT_EVENT;
            break;
        default:
            break;
    }
    return ret;
}

Media::PlayerSeekMode ConvertSeekMode(NWeb::PlayerSeekMode mode)
{
    Media::PlayerSeekMode ret = Media::SEEK_CLOSEST;
    switch (mode) {
        case NWeb::PlayerSeekMode::SEEK_NEXT_SYNC:
            ret = Media::SEEK_NEXT_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_PREVIOUS_SYNC:
            ret = Media::SEEK_PREVIOUS_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_CLOSEST_SYNC:
            ret = Media::SEEK_CLOSEST_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_CLOSEST:
            ret = Media::SEEK_CLOSEST;
            break;
        default:
            break;
    }
    return ret;
}

Media::PlaybackRateMode ConvertRateMode(NWeb::PlaybackRateMode mode)
{
    Media::PlaybackRateMode ret = Media::SPEED_FORWARD_1_00_X;
    switch (mode) {
        case NWeb::PlaybackRateMode::SPEED_FORWARD_0_75_X:
            ret = Media::SPEED_FORWARD_0_75_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X:
            ret = Media::SPEED_FORWARD_1_00_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_25_X:
            ret = Media::SPEED_FORWARD_1_25_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_75_X:
            ret = Media::SPEED_FORWARD_1_75_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_2_00_X:
            ret = Media::SPEED_FORWARD_2_00_X;
            break;
        default:
            break;
    }
    return ret;
}
} // namespace

PlayerCallbackImpl::PlayerCallbackImpl(std::shared_ptr<PlayerCallbackAdapter> callback)
    : callbackAdapter_(std::move(callback))
{}

void PlayerCallbackImpl::OnInfo(Media::PlayerOnInfoType type, int32_t extra, const Media::Format& infoBody)
{
    int32_t hintValue = -1;
    if (type == Media::INFO_TYPE_INTERRUPT_EVENT) {
        infoBody.GetIntValue(OHOS::Media::PlayerKeys::AUDIO_INTERRUPT_HINT, hintValue);
    }
    if (callbackAdapter_) {
        callbackAdapter_->OnInfo(ConvertInfoType(type), extra, hintValue);
    }
}

void PlayerCallbackImpl::OnError(int32_t errorCode, const std::string& errorMsg)
{
    (void)errorMsg;
    WVLOG_E("media player error, errorCode=%{public}d", errorCode);
    if (callbackAdapter_) {
        auto errorType = PlayerAdapterErrorType::INVALID_CODE;
        if (IsUnsupportType(errorCode)) {
            errorType = PlayerAdapterErrorType::UNSUPPORT_TYPE;
        } else if (IsFatalError(errorCode)) {
            errorType = PlayerAdapterErrorType::FATAL_ERROR;
        }
        callbackAdapter_->OnError(errorType);
    }
}

PlayerAdapterImpl::PlayerAdapterImpl()
{
    player_ = Media::PlayerFactory::CreatePlayer();
}

PlayerAdapterImpl::~PlayerAdapterImpl()
{
    if (player_) {
        player_->Release();
        WVLOG_I("player release");
    }
}

int32_t PlayerAdapterImpl::SetPlayerCallback(std::shared_ptr<PlayerCallbackAdapter> callbackAdapter)
{
    if (!player_ || !callbackAdapter) {
        WVLOG_E("player_ or callbackAdapter is nullptr");
        return -1;
    }
    auto callback = std::make_shared<PlayerCallbackImpl>(std::move(callbackAdapter));
    return player_->SetPlayerCallback(callback);
}

int32_t PlayerAdapterImpl::SetSource(const std::string& url)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetSource(url);
}

int32_t PlayerAdapterImpl::SetSource(int32_t fd, int64_t offset, int64_t size)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetSource(fd, offset, size);
}

int32_t PlayerAdapterImpl::SetVideoSurface(std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter)
{
    if (!player_ || !cSurfaceAdapter) {
        WVLOG_E("player_  or cSurfaceAdapter is nullptr");
        return -1;
    }
    auto cSurface = std::static_pointer_cast<ConsumerSurfaceAdapterImpl>(cSurfaceAdapter)->GetConsumerSurface();
    cSurface->SetDefaultUsage(BUFFER_USAGE_CPU_READ);
    sptr<IBufferProducer> producer = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    return player_->SetVideoSurface(pSurface);
}

int32_t PlayerAdapterImpl::SetVolume(float leftVolume, float rightVolume)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetVolume(leftVolume, rightVolume);
}

int32_t PlayerAdapterImpl::Seek(int32_t mSeconds, PlayerSeekMode mode)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->Seek(mSeconds, ConvertSeekMode(mode));
}

int32_t PlayerAdapterImpl::Play()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->Play();
}

int32_t PlayerAdapterImpl::Pause()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->Pause();
}

int32_t PlayerAdapterImpl::PrepareAsync()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->PrepareAsync();
}

int32_t PlayerAdapterImpl::GetCurrentTime(int32_t& currentTime)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->GetCurrentTime(currentTime);
}

int32_t PlayerAdapterImpl::GetDuration(int32_t& duration)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->GetDuration(duration);
}

int32_t PlayerAdapterImpl::SetPlaybackSpeed(PlaybackRateMode mode)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetPlaybackSpeed(ConvertRateMode(mode));
}

int32_t PlayerAdapterImpl::SetMediaSourceHeader(const std::string& url,
    const std::map<std::string, std::string>& header)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    std::shared_ptr<Media::AVMediaSource> mediaSource = std::make_shared<Media::AVMediaSource>(url, header);
    Media::AVPlayStrategy strategy;
    return player_->SetMediaSource(mediaSource, strategy);
}

} // namespace OHOS::NWeb

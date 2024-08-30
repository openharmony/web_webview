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

#include "nweb_log.h"
#include "native_window_adapter_impl.h"

namespace OHOS::NWeb {
namespace {
constexpr int32_t INVALID = -1;

int32_t ConverterState(int32_t ndkState) {
    switch(ndkState) {
        case AVPlayerState::AV_IDLE:
            return PlayerAdapter::PlayerStates::PLAYER_IDLE;
        case AVPlayerState::AV_INITIALIZED:
            return PlayerAdapter::PlayerStates::PLAYER_INITIALIZED;
        case AVPlayerState::AV_PREPARED:
            return PlayerAdapter::PlayerStates::PLAYER_PREPARED;
        case AVPlayerState::AV_PLAYING:
            return PlayerAdapter::PlayerStates::PLAYER_STARTED;
        case AVPlayerState::AV_PAUSED:
            return PlayerAdapter::PlayerStates::PLAYER_PAUSED;
        case AVPlayerState::AV_STOPPED:
            return PlayerAdapter::PlayerStates::PLAYER_STOPPED;
        case AVPlayerState::AV_COMPLETED:
            return PlayerAdapter::PlayerStates::PLAYER_PLAYBACK_COMPLETE;
        case AVPlayerState::AV_RELEASED:
            return PlayerAdapter::PlayerStates::PLAYER_RELEASED;
        case AVPlayerState::AV_ERROR:
            return PlayerAdapter::PlayerStates::PLAYER_STATE_ERROR;
        default:
            WVLOG_E("could not find state = %{public}d", ndkState);
            return ndkState;
    }
}

bool IsUnsupportType(int32_t errorCode)
{
    switch (errorCode) {
        case OH_AVErrCode::AV_ERR_UNSUPPORT:
            return true;
        default:
            return false;
    }
}

bool IsFatalError(int32_t errorCode)
{
    switch (errorCode) {
        case OH_AVErrCode::AV_ERR_NO_MEMORY:
        case OH_AVErrCode::AV_ERR_OPERATE_NOT_PERMIT:
        case OH_AVErrCode::AV_ERR_IO:
        case OH_AVErrCode::AV_ERR_TIMEOUT:
        case OH_AVErrCode::AV_ERR_UNKNOWN:
        case OH_AVErrCode::AV_ERR_SERVICE_DIED:
        case OH_AVErrCode::AV_ERR_EXTEND_START:
        case OH_AVErrCode::AV_ERR_DRM_BASE:
        case OH_AVErrCode::AV_ERR_DRM_DECRYPT_FAILED:
        case OH_AVErrCode::AV_ERR_VIDEO_BASE:
        case OH_AVErrCode::AV_ERR_VIDEO_UNSUPPORTED_COLOR_SPACE_CONVERSION:
            return true;
        default:
            return false;
    }
}

NWeb::PlayerOnInfoType ConvertInfoType(AVPlayerOnInfoType infoType)
{
    NWeb::PlayerOnInfoType ret = NWeb::PlayerOnInfoType::INFO_TYPE_UNSET;
    switch (infoType) {
        case AVPlayerOnInfoType::AV_INFO_TYPE_SEEKDONE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_SEEKDONE;
            break;
        case AVPlayerOnInfoType::AV_INFO_TYPE_EOS:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_EOS;
            break;
        case AVPlayerOnInfoType::AV_INFO_TYPE_STATE_CHANGE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_STATE_CHANGE;
            break;
        case AVPlayerOnInfoType::AV_INFO_TYPE_POSITION_UPDATE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_POSITION_UPDATE;
            break;
        case AVPlayerOnInfoType::AV_INFO_TYPE_MESSAGE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_MESSAGE;
            break;
        case AVPlayerOnInfoType::AV_INFO_TYPE_INTERRUPT_EVENT:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_INTERRUPT_EVENT;
            break;
        default:
            break;
    }
    return ret;
}

AVPlayerSeekMode ConvertSeekMode(NWeb::PlayerSeekMode mode)
{
    AVPlayerSeekMode ret = AVPlayerSeekMode::AV_SEEK_CLOSEST;
    switch (mode) {
        case NWeb::PlayerSeekMode::SEEK_NEXT_SYNC:
            ret = AVPlayerSeekMode::AV_SEEK_NEXT_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_PREVIOUS_SYNC:
            ret = AVPlayerSeekMode::AV_SEEK_PREVIOUS_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_CLOSEST_SYNC:
            ret = AVPlayerSeekMode::AV_SEEK_CLOSEST;
            break;
        case NWeb::PlayerSeekMode::SEEK_CLOSEST:
            ret = AVPlayerSeekMode::AV_SEEK_CLOSEST;
            break;
        default:
            break;
    }
    return ret;
}

AVPlaybackSpeed ConvertRateMode(NWeb::PlaybackRateMode mode)
{
    AVPlaybackSpeed ret = AVPlaybackSpeed::AV_SPEED_FORWARD_1_00_X;
    switch (mode) {
        case NWeb::PlaybackRateMode::SPEED_FORWARD_0_75_X:
            ret = AVPlaybackSpeed::AV_SPEED_FORWARD_0_75_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X:
            ret = AVPlaybackSpeed::AV_SPEED_FORWARD_1_00_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_25_X:
            ret = AVPlaybackSpeed::AV_SPEED_FORWARD_1_25_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_75_X:
            ret = AVPlaybackSpeed::AV_SPEED_FORWARD_1_75_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_2_00_X:
            ret = AVPlaybackSpeed::AV_SPEED_FORWARD_2_00_X;
            break;
        default:
            break;
    }
    return ret;
}

int GetErrorCode(OH_AVErrCode errorCode)
{
    int32_t code = static_cast<int32_t>(errorCode);
    if (code == OH_AVErrCode::AV_ERR_OK) {
        WVLOG_D("return success, code = %{public}d", code);
    } else {
        WVLOG_E("return failed, code = %{public}d", code);
    }
    return code;
}
} // namespace

void PlayerInfoCallback(OH_AVPlayer *player, AVPlayerOnInfoType type, OH_AVFormat* infoBody, void *userData)
{
    if (userData == nullptr) {
        WVLOG_E("userData is null");
        return;
    }
    int32_t state = 0;
    if (type == AVPlayerOnInfoType::AV_INFO_TYPE_STATE_CHANGE) {
        OH_AVFormat_GetIntValue(infoBody, OH_PLAYER_STATE, &state);
        state = ConverterState(state);
    }
    int32_t hintValue = INVALID;
    if (type == AVPlayerOnInfoType::AV_INFO_TYPE_INTERRUPT_EVENT) {
        OH_AVFormat_GetIntValue(infoBody, OH_PLAYER_AUDIO_INTERRUPT_HINT, &hintValue);
    }
    PlayerCallbackAdapter* callback = (PlayerCallbackAdapter*)(userData);
    callback->OnInfo(ConvertInfoType(type), state, hintValue);
}

void PlayerErrorCallback(OH_AVPlayer *player, int32_t errorCode, const char *errorMsg, void *userData)
{
    if (userData == nullptr) {
        WVLOG_E("PlayerErrorCallback, userData is nullptr");
        return;
    }
    auto errorType = PlayerAdapterErrorType::INVALID_CODE;
    if (IsUnsupportType(errorCode)) {
        errorType = PlayerAdapterErrorType::UNSUPPORT_TYPE;
    } else if(IsFatalError(errorCode)) {
        errorType = PlayerAdapterErrorType::FATAL_ERROR;
    }
    PlayerCallbackAdapter* callback = (PlayerCallbackAdapter*)(userData);
    callback->OnError(errorType);
}

PlayerAdapterImpl::PlayerAdapterImpl()
{
    player_ = OH_AVPlayer_Create();
}

PlayerAdapterImpl::~PlayerAdapterImpl()
{
    if (player_) {
        int ret = GetErrorCode(OH_AVPlayer_Release(player_));
        WVLOG_I("player release, ret = %{public}d", ret);
    }
}

int32_t PlayerAdapterImpl::SetPlayerCallback(std::shared_ptr<PlayerCallbackAdapter> callbackAdapter)
{
    if (!player_ || !callbackAdapter) {
        WVLOG_E("player_ or callbackAdapter is nullptr");
        return -1;
    }
    callbackAdapter_ = std::move(callbackAdapter);
    int ret1 = GetErrorCode(OH_AVPlayer_SetOnInfoCallback(player_, PlayerInfoCallback, callbackAdapter_.get()));
    int ret2 = GetErrorCode(OH_AVPlayer_SetOnErrorCallback(player_, PlayerErrorCallback, callbackAdapter_.get()));
    return ret1 + ret2;
}

int32_t PlayerAdapterImpl::SetSource(const std::string& url)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_SetURLSource(player_, url.c_str()));
}

int32_t PlayerAdapterImpl::SetSource(int32_t fd, int64_t offset, int64_t size)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_SetFDSource(player_, fd, offset, size));
}

int32_t PlayerAdapterImpl::SetVideoSurface(std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter)
{
    if (!player_ || !cSurfaceAdapter) {
        WVLOG_E("player_  or cSurfaceAdapter is nullptr");
        return -1;
    }
    OH_NativeImage* cImage = std::static_pointer_cast<ConsumerNativeAdapterImpl>(cSurfaceAdapter)->GetConsumerSurface();
    OHNativeWindow* nativeWindow = OH_NativeImage_AcquireNativeWindow(cImage);
    OH_NativeWindow_NativeWindowHandleOpt(nativeWindow, SET_USAGE, NATIVEBUFFER_USAGE_CPU_READ);
    return GetErrorCode(OH_AVPlayer_SetVideoSurface(player_, nativeWindow));
}

int32_t PlayerAdapterImpl::SetVolume(float leftVolume, float rightVolume)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_SetVolume(player_, leftVolume, rightVolume));
}

int32_t PlayerAdapterImpl::Seek(int32_t mSeconds, PlayerSeekMode mode)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_Seek(player_, mSeconds, ConvertSeekMode(mode)));
}

int32_t PlayerAdapterImpl::Play()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_Play(player_));
}

int32_t PlayerAdapterImpl::Pause()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_Pause(player_));
}

int32_t PlayerAdapterImpl::PrepareAsync()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_Prepare(player_));
}

int32_t PlayerAdapterImpl::GetCurrentTime(int32_t& currentTime)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_GetCurrentTime(player_, &currentTime));
}

int32_t PlayerAdapterImpl::GetDuration(int32_t& duration)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_GetDuration(player_, &duration));
}

int32_t PlayerAdapterImpl::SetPlaybackSpeed(PlaybackRateMode mode)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return GetErrorCode(OH_AVPlayer_SetPlaybackSpeed(player_, ConvertRateMode(mode)));
}
} // namespace OHOS::NWeb

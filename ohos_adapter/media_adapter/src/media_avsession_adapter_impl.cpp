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

#include "media_avsession_adapter_impl.h"

#include <bundle/native_interface_bundle.h>

#include "nweb_log.h"

namespace OHOS::NWeb {

std::unordered_map<std::string, OH_AVSession *> MediaAVSessionAdapterImpl::avSessionMap;

MediaAVSessionCallbackImpl::MediaAVSessionCallbackImpl(
    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter)
    : callbackAdapter_(callbackAdapter) {
}

void MediaAVSessionCallbackImpl::OnPlay() {
    if (callbackAdapter_) {
        callbackAdapter_->Play();
    }
}

void MediaAVSessionCallbackImpl::OnPause() {
    if (callbackAdapter_) {
        callbackAdapter_->Pause();
    }
}

void MediaAVSessionCallbackImpl::OnStop() {
    if (callbackAdapter_) {
        callbackAdapter_->Stop();
    }
}

void MediaAVSessionCallbackImpl::OnPlayNext() {
}

void MediaAVSessionCallbackImpl::OnPlayPrevious() {
}

void MediaAVSessionCallbackImpl::OnFastForward(int64_t time) {
}

void MediaAVSessionCallbackImpl::OnRewind(int64_t time) {
}

void MediaAVSessionCallbackImpl::OnSeek(int64_t time) {
    if (callbackAdapter_) {
        callbackAdapter_->SeekTo(time);
    }
}

void MediaAVSessionCallbackImpl::OnSetSpeed(double speed) {
}

void MediaAVSessionCallbackImpl::OnSetLoopMode(int32_t loopMode) {
}

void MediaAVSessionCallbackImpl::OnToggleFavorite(const std::string& assertId) {
}

void MediaAVSessionCallbackImpl::OnSkipToQueueItem(int32_t itemId) {
}

void MediaAVSessionCallbackImpl::OnAVCallAnswer() {
}

void MediaAVSessionCallbackImpl::OnAVCallHangUp() {
}

void MediaAVSessionCallbackImpl::OnAVCallToggleCallMute() {
}

void MediaAVSessionCallbackImpl::OnPlayFromAssetId(int64_t assetId) {
}

void MediaAVSessionKey::Init() {
    pid_ = getpid();
    OH_NativeBundle_ElementName bundleInfo = OH_NativeBundle_GetMainElementName();
    bundleName_ = std::string(bundleInfo.bundleName);
    type_ = MediaAVSessionType::MEDIA_TYPE_INVALID;
    abilityName_ = std::string(bundleInfo.abilityName);
    return;
}

int32_t MediaAVSessionKey::GetPID() {
    return pid_;
}

std::string &MediaAVSessionKey::GetBundleName() {
    return bundleName_;
}

std::string &MediaAVSessionKey::GetAbilityName() {
    return abilityName_;
}

void MediaAVSessionKey::SetType(MediaAVSessionType type) {
    type_ = type;
}

MediaAVSessionType MediaAVSessionKey::GetType() {
    return type_;
}

std::string MediaAVSessionKey::ToString() {
    return (std::to_string(pid_) + "_" + bundleName_ + "_" + abilityName_);
}

MediaAVSessionAdapterImpl::MediaAVSessionAdapterImpl() {
    avSessionKey_ = std::make_shared<MediaAVSessionKey>();
    avSessionKey_->Init();
    AVMetadata_Result ret = OH_AVMetadataBuilder_Create(&builder_);
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("create metadata builder failed, ret=%{public}d", ret);
    }

    ret = OH_AVMetadataBuilder_SetAssetId(builder_, std::to_string(avSessionKey_->GetPID()).c_str());
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("set assert id failed, ret=%{public}d", ret);
    }

    ret = OH_AVMetadataBuilder_GenerateAVMetadata(builder_, &avMetadata_);
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("generate avmetadata failed, ret=%{public}d", ret);
    }

    avPlaybackState_ = PLAYBACK_STATE_INITIAL;
}

MediaAVSessionAdapterImpl::~MediaAVSessionAdapterImpl() {
    WVLOG_I("destroy");

    if (avMetadata_) {
        AVMetadata_Result ret = OH_AVMetadata_Destroy(avMetadata_);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("destroy avmetadata failed, ret=%{public}d", ret);
        }
    }

    if (builder_) {
        AVMetadata_Result ret = OH_AVMetadataBuilder_Destroy(builder_);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("destroy avmetadata builder failed, ret=%{public}d", ret);
        }
    }

    DestroyAVSession();
}

bool MediaAVSessionAdapterImpl::CreateAVSession(MediaAVSessionType type) {
    WVLOG_I("CreateAVSession in, type=%{public}d", int32_t(type));
    if (MediaAVSessionType::MEDIA_TYPE_INVALID == type) {
        WVLOG_E("CreateAVSession, type invalid return false");
        return false;
    }
    if (avSession_ && (type != avSessionKey_->GetType())) {
        DestroyAVSession();
    }
    auto findIter = avSessionMap.find(avSessionKey_->ToString());
    if (!avSession_) {
        if (findIter != avSessionMap.end()) {
            DestroyAndEraseSession();
        }
        return CreateNewSession(type);
    } else {
        if (findIter != avSessionMap.end()) {
            if (findIter->second != avSession_) {
                DestroyAndEraseSession();
                DestroyAVSession();
            } else {
                WVLOG_E("CreateAVSession, return false");
                return false;
            }
        }
        return CreateNewSession(type);
    }
}

void MediaAVSessionAdapterImpl::DestroyAVSession() {
    WVLOG_I("DestroyAVSession in");
    if (avSession_) {
        AVSession_ErrCode ret = OH_AVSession_Destroy(avSession_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("DestroyAVSession Destroy() failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("DestroyAVSession Destroy() success, ret: %{public}d", ret);
        }
    }
    auto iter = avSessionMap.find(avSessionKey_->ToString());
    if (iter != avSessionMap.end()) {
        avSessionMap.erase(iter);
    }
    WVLOG_I("DestroyAVSession out");
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(OH_AVSession *session,
    AVSession_ControlCommand command, void *userData) {
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);
    
    switch (command) {
        case CONTROL_CMD_PLAY:
            media->OnPlay();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;
        
        case CONTROL_CMD_PAUSE:
            media->OnPause();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;
        
        case CONTROL_CMD_STOP:
            media->OnStop();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;

        case CONTROL_CMD_PLAY_NEXT:
            media->OnPlayNext();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;

        case CONTROL_CMD_PLAY_PREVIOUS:
            media->OnPlayPrevious();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;

        default:
            WVLOG_E("invalid cmd: %{public}d", command);
    }

    return AVSESSION_CALLBACK_RESULT_FAILURE;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnFastForwardCallback(OH_AVSession *session,
    uint32_t seekTime, void *userData) {
    WVLOG_I("FastForwardCallback seekTime: %{public}u", seekTime);
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);

    media->OnFastForward(seekTime);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnRewindCallback(OH_AVSession *session,
    uint32_t seekTime, void *userData) {
    WVLOG_I("RewindCallback seekTime: %{public}u", seekTime);
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);

    media->OnRewind(seekTime);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnSeekCallback(OH_AVSession *session,
    uint64_t seekTime, void *userData) {
    WVLOG_I("SeekCallback seekTime: %{public}lu", seekTime);
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);

    media->OnSeek(seekTime);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnSetSpeedCallback(OH_AVSession *session,
    uint32_t speed, void *userData) {
    WVLOG_I("SetSpeedCallback seekTime: %{public}u", speed);
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);

    media->OnSetSpeed(speed);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnSetLoopModeCallback(OH_AVSession *session,
    AVSession_LoopMode curLoopMode, void *userData) {
    WVLOG_I("SetLoopModeCallback curLoopMode: %{public}u", static_cast<uint32_t>(curLoopMode));
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);

    media->OnSetLoopMode(curLoopMode);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnToggleFavoriteCallback(OH_AVSession *session,
    const char *assertId, void *userData) {
    WVLOG_I("OnToggleFavorite assertId: %{public}s", assertId);
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);

    media->OnToggleFavorite(assertId);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnPlayFromAssertIdCallback(OH_AVSession *session,
    const char *assertId, void *userData) {
    WVLOG_I("OnPlayFromAssertId assertId: %{public}s", assertId);
    MediaAVSessionCallbackImpl *media = reinterpret_cast<MediaAVSessionCallbackImpl *>(userData);

    media->OnPlayFromAssetId(std::stoi(assertId));
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

bool MediaAVSessionAdapterImpl::DoOtherRegistCallback(std::shared_ptr<MediaAVSessionCallbackImpl> callback)
{
    AVSession_ErrCode ret = OH_AVSession_RegisterRewindCallback(avSession_,
        &MediaAVSessionAdapterImpl::AVSessionOnRewindCallback, reinterpret_cast<void *>(callback.get()));
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("RegisterRewindCallback failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_RegisterSeekCallback(avSession_,
        &MediaAVSessionAdapterImpl::AVSessionOnSeekCallback, reinterpret_cast<void *>(callback.get()));
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("RegisterSeekCallback failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_RegisterSpeedCallback(avSession_,
        &MediaAVSessionAdapterImpl::AVSessionOnSetSpeedCallback, reinterpret_cast<void *>(callback.get()));
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("RegisterSpeedCallback failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_RegisterSetLoopModeCallback(avSession_,
        &MediaAVSessionAdapterImpl::AVSessionOnSetLoopModeCallback, reinterpret_cast<void *>(callback.get()));
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("SetLoopModeCallback failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_RegisterToggleFavoriteCallback(avSession_,
        &MediaAVSessionAdapterImpl::AVSessionOnToggleFavoriteCallback, reinterpret_cast<void *>(callback.get()));
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("ToggleFavoriteCallback failed. ret: %{public}d", ret);
        return false;
    }

    ret = OH_AVSession_RegisterPlayFromAssetIdCallback(avSession_,
        &MediaAVSessionAdapterImpl::AVSessionOnPlayFromAssertIdCallback, reinterpret_cast<void *>(callback.get()));
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("PlayFromAssertIdCallback failed. ret: %{public}d", ret);
        return false;
    }

    return true;
}

bool MediaAVSessionAdapterImpl::RegistCallback(
    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter) {
    WVLOG_I("RegistCallback in");
    if (avSession_ && Activate()) {
        auto callback = std::make_shared<MediaAVSessionCallbackImpl>(callbackAdapter);
        for (AVSession_ControlCommand command = CONTROL_CMD_PLAY;
            command <= CONTROL_CMD_PLAY_PREVIOUS;
            command = (AVSession_ControlCommand)(command + 1)) {
            AVSession_ErrCode ret = OH_AVSession_RegisterCommandCallback(avSession_,
                command, &MediaAVSessionAdapterImpl::AVSessionOnCommandCallback,
                reinterpret_cast<void *>(callback.get()));
            if (ret != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("RegisterCommandCallback failed. ret: %{public}d", ret);
                return false;
            }
        }
        
        return DoOtherRegistCallback(callback);
    }

    WVLOG_I("RegistCallback out return false");
    return false;
}

bool MediaAVSessionAdapterImpl::IsActivated() {
    WVLOG_I("IsActivated in");
    if (avSession_) {
        return isActived_;
    }
    WVLOG_E("IsActivated out avSession is null, return false");
    return false;
}

bool MediaAVSessionAdapterImpl::Activate() {
    if (!avSession_) {
        WVLOG_E("Activate avSession_ is null, return false");
        return false;
    }

    AVSession_ErrCode ret = OH_AVSession_Activate(avSession_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("Activate failed. ret: %{public}d", ret);
        return false;
    }

    isActived_ = true;
    WVLOG_I("Activate success.");
    return true;
}

void MediaAVSessionAdapterImpl::DeActivate() {
    if (avSession_) {
        AVSession_ErrCode ret = OH_AVSession_Deactivate(avSession_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("DeActivate failed. ret: %{public}d", ret);
            return;
        }
        isActived_ = false;
    }
}

void MediaAVSessionAdapterImpl::SetMetadata(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) {
    UpdateMetaDataCache(metadata);
    if (avSession_) {
        Activate();
        AVSession_ErrCode ret = OH_AVSession_SetAVMetadata(avSession_, avMetadata_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("SetMetadata failed. ret: %{public}d", ret);
        } else {
            WVLOG_I("SetMetadata success");
        }
    }
}

void MediaAVSessionAdapterImpl::SetPlaybackState(MediaAVSessionPlayState state) {
    if (UpdatePlaybackStateCache(state) && avSession_) {
        Activate();
        AVSession_ErrCode ret = OH_AVSession_SetPlaybackState(avSession_, avPlaybackState_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("SetPlaybackState failed. ret: %{public}d", ret);
        } else {
            WVLOG_I("SetPlaybackState success");
        }
    }
}

void MediaAVSessionAdapterImpl::SetPlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position) {
    if (UpdateMetaDataCache(position) && avSession_) {
        Activate();
        AVSession_ErrCode ret = OH_AVSession_SetAVMetadata(avSession_, avMetadata_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("SetAVMetadata failed. ret: %{public}d", ret);
        } else {
            WVLOG_I("SetAVMetadata success");
        }
    }
    if (UpdatePlaybackStateCache(position) && avSession_) {
        Activate();
        AVSession_ErrCode ret = OH_AVSession_SetPlaybackState(avSession_, avPlaybackState_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("SetPlaybackPosition failed. ret: %{public}d", ret);
        } else {
            WVLOG_I("SetPlaybackPosition success");
        }
    }
}

bool MediaAVSessionAdapterImpl::UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) {
    bool updated = false;
    AVMetadata_Result ret = OH_AVMetadataBuilder_SetTitle(builder_, metadata->GetTitle().c_str());
    if (ret == AVMETADATA_SUCCESS) {
        updated = true;
    }

    ret = OH_AVMetadataBuilder_SetArtist(builder_, metadata->GetArtist().c_str());
    if (ret == AVMETADATA_SUCCESS) {
        updated = true;
    }

    ret = OH_AVMetadataBuilder_SetAlbum(builder_, metadata->GetAlbum().c_str());
    if (ret == AVMETADATA_SUCCESS) {
        updated = true;
    }
    WVLOG_I("UpdateMetaDataCache return updated: %{public}d", updated);
    return updated;
}

bool MediaAVSessionAdapterImpl::UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionPositionAdapter> position) {
    AVMetadata_Result ret = OH_AVMetadataBuilder_SetDuration(builder_, position->GetDuration());
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("UpdateMetaDataCache failed. ret: %{public}d", ret);
        return false;
    }
    WVLOG_E("UpdateMetaDataCache success.");
    return true;
}

bool MediaAVSessionAdapterImpl::UpdatePlaybackStateCache(MediaAVSessionPlayState state) {
    switch (state) {
        case MediaAVSessionPlayState::STATE_PLAY:
            avPlaybackState_ = PLAYBACK_STATE_PLAYING;
            break;
        case MediaAVSessionPlayState::STATE_PAUSE:
            avPlaybackState_ = PLAYBACK_STATE_PAUSED;
            break;
        case MediaAVSessionPlayState::STATE_INITIAL:
        default:
            avPlaybackState_ = PLAYBACK_STATE_PAUSED;
            break;
    }

    AVSession_ErrCode ret = OH_AVSession_SetPlaybackState(avSession_, avPlaybackState_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("UpdatePlaybackStateCache failed. ret: %{public}d", ret);
        return false;
    }
    WVLOG_I("UpdatePlaybackStateCache success.");
    return true;
}

bool MediaAVSessionAdapterImpl::UpdatePlaybackStateCache(
    const std::shared_ptr<MediaAVSessionPositionAdapter> position) {
    bool updated = false;
    auto duration = static_cast<int32_t>(position->GetDuration());
    AVMetadata_Result ret = OH_AVMetadataBuilder_SetDuration(builder_, duration);
    if (ret == AVMETADATA_SUCCESS) {
        duration = true;
    }

    AVSession_PlaybackPosition playbackPosition = {
        .elapsedTime = position->GetElapsedTime(),
        .updateTime = position->GetUpdateTime(),
    };

    AVSession_ErrCode err = OH_AVSession_SetPlaybackPosition(avSession_, &playbackPosition);
    if (err == AV_SESSION_ERR_SUCCESS) {
        updated = true;
    }
    WVLOG_I("UpdatePlaybackStateCache return updated: %{public}d", updated);
    return updated;
}

void MediaAVSessionAdapterImpl::DestroyAndEraseSession() {
    WVLOG_I("DestroyAndEraseSession in");
    auto iter = avSessionMap.find(avSessionKey_->ToString());
    if (iter == avSessionMap.end()) {
        WVLOG_E("DestroyAndEraseSession invalid iterator return");
        return;
    }
    if (!iter->second) {
        avSessionMap.erase(iter);
        WVLOG_E("DestroyAndEraseSession avsession is null pointer return");
        return;
    }

    AVSession_ErrCode ret = OH_AVSession_Destroy(iter->second);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_E("DestroyAndEraseSession Destroy failed, ret: %{public}d", ret);
    } else {
        WVLOG_I("DestroyAndEraseSession Destroy success");
    }
    avSessionMap.erase(iter);
    WVLOG_I("DestroyAndEraseSession out");
}

bool MediaAVSessionAdapterImpl::CreateNewSession(const MediaAVSessionType& type) {
    WVLOG_I("CreateNewSession in type: %{public}d", type);
    AVSession_Type sessionType;

    switch (type) {
        case MediaAVSessionType::MEDIA_TYPE_AUDIO:
            sessionType = SESSION_TYPE_AUDIO;
            break;
        
        case MediaAVSessionType::MEDIA_TYPE_VIDEO:
            sessionType = SESSION_TYPE_VIDEO;
            break;
        
        default:
            return false;
    }

    AVSession_ErrCode ret = OH_AVSession_Create(sessionType, "av_media_session",
        avSessionKey_->GetBundleName().c_str(), avSessionKey_->GetAbilityName().c_str(),
        &avSession_);
    if (ret != AV_SESSION_ERR_SUCCESS) {
        WVLOG_I("OH_AVSession_Create failed. ret: %{public}d", ret);
        return false;
    }

    avSessionKey_->SetType(type);
    avSessionMap.insert(std::pair<std::string, OH_AVSession *>((avSessionKey_->ToString()),
        avSession_));
    return true;
}

} // namespace OHOS::NWeb
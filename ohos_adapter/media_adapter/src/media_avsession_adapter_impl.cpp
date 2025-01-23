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

std::unordered_map<std::string, MediaAVSessionAdapterImpl*> MediaAVSessionAdapterImpl::avSessionMap;

void MediaAVSessionKey::Init() {
    pid_ = getpid();
    OH_NativeBundle_ElementName bundleInfo = OH_NativeBundle_GetMainElementName();
    if (bundleInfo.bundleName) {
        bundleName_ = std::string(bundleInfo.bundleName);
    } else {
        bundleName_ = std::string("dummy");
    }

    type_ = MediaAVSessionType::MEDIA_TYPE_INVALID;
    if (bundleInfo.abilityName) {
        abilityName_ = std::string(bundleInfo.abilityName);
    } else {
        abilityName_ = std::string("dummy");
    }

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

    avMetadata_ = nullptr;
    builder_ = nullptr;
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
            if (findIter->second != this) {
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
    WVLOG_I("DestroyAVSession in actived: %d", isActived_);
    if (avSession_) {
        AVSession_ErrCode ret = OH_AVSession_Destroy(avSession_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("DestroyAVSession Destroy() failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("DestroyAVSession Destroy() success, ret: %{public}d", ret);
        }
        avSession_ = nullptr;
    }
    auto iter = avSessionMap.find(avSessionKey_->ToString());
    if (iter != avSessionMap.end() && iter->second == this) {
        avSessionMap.erase(iter);
    }
    WVLOG_I("DestroyAVSession out");
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(OH_AVSession *session,
    AVSession_ControlCommand command, void *userData) {
    MediaAVSessionCallbackAdapter *media = reinterpret_cast<MediaAVSessionCallbackAdapter *>(userData);

    if (!media) {
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }

    switch (command) {
        case CONTROL_CMD_PLAY:
            media->Play();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;
        
        case CONTROL_CMD_PAUSE:
            media->Pause();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;
        
        case CONTROL_CMD_STOP:
            media->Stop();
            return AVSESSION_CALLBACK_RESULT_SUCCESS;

        default:
            WVLOG_E("invalid cmd: %{public}d", command);
    }

    return AVSESSION_CALLBACK_RESULT_FAILURE;
}

AVSessionCallback_Result MediaAVSessionAdapterImpl::AVSessionOnSeekCallback(OH_AVSession *session,
    uint64_t seekTime, void *userData) {
    WVLOG_I("SeekCallback seekTime: %{public}lu", seekTime);
    MediaAVSessionCallbackAdapter *media = reinterpret_cast<MediaAVSessionCallbackAdapter *>(userData);
    if (!media) {
        return AVSESSION_CALLBACK_RESULT_FAILURE;
    }

    media->SeekTo(seekTime);
    return AVSESSION_CALLBACK_RESULT_SUCCESS;
}

bool MediaAVSessionAdapterImpl::RegistCallback(
    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter) {
    WVLOG_I("RegistCallback in");
    if (avSession_ && Activate()) {
        AVSession_ErrCode ret;
        callbackAdapter_ = callbackAdapter;
        for (AVSession_ControlCommand command = CONTROL_CMD_PLAY;
            command <= CONTROL_CMD_STOP;
            command = (AVSession_ControlCommand)(command + 1)) {
            ret = OH_AVSession_RegisterCommandCallback(avSession_,
                command, &MediaAVSessionAdapterImpl::AVSessionOnCommandCallback,
                reinterpret_cast<void *>(callbackAdapter_.get()));
            if (ret != AV_SESSION_ERR_SUCCESS) {
                WVLOG_E("RegisterCommandCallback failed. ret: %{public}d", ret);
                return false;
            }
        }

        ret = OH_AVSession_RegisterSeekCallback(avSession_,
            &MediaAVSessionAdapterImpl::AVSessionOnSeekCallback, reinterpret_cast<void *>(callbackAdapter_.get()));
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("RegisterSeekCallback failed. ret: %{public}d", ret);
            return false;
        }

        return true;
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

AVMetadata_Result MediaAVSessionAdapterImpl::UpdateAVMetadata(void) {
    AVMetadata_Result ret = AVMETADATA_SUCCESS;

    if (avMetadata_) {
        ret = OH_AVMetadata_Destroy(avMetadata_);
        if (ret != AVMETADATA_SUCCESS) {
            WVLOG_E("OH_AVMetadata_Destroy failed. ret: %{public}d", ret);
        }
    }

    avMetadata_ = nullptr;
    ret = OH_AVMetadataBuilder_GenerateAVMetadata(builder_, &avMetadata_);
    if (ret != AVMETADATA_SUCCESS) {
        WVLOG_E("generate avmetadata failed, ret=%{public}d", ret);
    }

    return ret;
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

    if (updated) {
        ret = UpdateAVMetadata();
        if (ret != AVMETADATA_SUCCESS) {
           return false;
        }
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

    ret = UpdateAVMetadata();
    if (ret != AVMETADATA_SUCCESS) {
        return false;
    }
    WVLOG_E("UpdateMetaDataCache success.");
    return true;
}

bool MediaAVSessionAdapterImpl::UpdatePlaybackStateCache(MediaAVSessionPlayState state) {
    if (!avSession_) {
        return false;
    }

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

    if (!avSession_) {
        return false;
    }

    AVMetadata_Result ret = OH_AVMetadataBuilder_SetDuration(builder_, duration);
    if (ret == AVMETADATA_SUCCESS) {
        ret = UpdateAVMetadata();
        if (ret != AVMETADATA_SUCCESS) {
           return false;
        }
        updated = true;
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
    WVLOG_I("DestroyAndEraseSession in actived: %d", isActived_);
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

    if (iter->second->avSession_ != nullptr) {
        AVSession_ErrCode ret = OH_AVSession_Destroy(iter->second->avSession_);
        if (ret != AV_SESSION_ERR_SUCCESS) {
            WVLOG_E("DestroyAndEraseSession Destroy failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("DestroyAndEraseSession Destroy success");
        }
    }
    iter->second->avSession_ = nullptr;
    avSessionMap.erase(iter);
    WVLOG_I("DestroyAndEraseSession out");
}

bool MediaAVSessionAdapterImpl::CreateNewSession(const MediaAVSessionType& type) {
    WVLOG_I("CreateNewSession in type: %{public}d actived: %d", type, isActived_);
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
    avSessionMap.insert(std::pair<std::string, MediaAVSessionAdapterImpl*>((avSessionKey_->ToString()), this));
    return true;
}

} // namespace OHOS::NWeb
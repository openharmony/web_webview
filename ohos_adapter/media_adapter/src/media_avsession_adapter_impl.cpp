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

#include "ability_manager_client.h"
#include "avsession_errors.h"
#include "avsession_manager.h"
#include "bundle_mgr_proxy.h"
#include "element_name.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "native_interface_bundle.h"
#include "nweb_log.h"
#include "parameters.h"
#include "system_ability_definition.h"

namespace OHOS::NWeb {

std::unordered_map<std::string, std::shared_ptr<AVSession::AVSession>> MediaAVSessionAdapterImpl::avSessionMap;

constexpr int64_t LIVE_STREAM_INFINITE_DURATION = -1;

MediaAVSessionCallbackImpl::MediaAVSessionCallbackImpl(
    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter)
    : callbackAdapter_(callbackAdapter)
{
}

void MediaAVSessionCallbackImpl::OnPlay(const AVSession::AVControlCommand& cmd)
{
    if (callbackAdapter_) {
        callbackAdapter_->Play();
    }
}

void MediaAVSessionCallbackImpl::OnPause()
{
    if (callbackAdapter_) {
        callbackAdapter_->Pause();
    }
}

void MediaAVSessionCallbackImpl::OnStop()
{
    if (callbackAdapter_) {
        callbackAdapter_->Stop();
    }
}

void MediaAVSessionCallbackImpl::OnPlayNext(const AVSession::AVControlCommand& cmd)
{
}

void MediaAVSessionCallbackImpl::OnPlayPrevious(const AVSession::AVControlCommand& cmd)
{
}

void MediaAVSessionCallbackImpl::OnFastForward(int64_t time, const AVSession::AVControlCommand& cmd)
{
}

void MediaAVSessionCallbackImpl::OnRewind(int64_t time, const AVSession::AVControlCommand& cmd)
{
}

void MediaAVSessionCallbackImpl::OnSeek(int64_t time)
{
    if (callbackAdapter_) {
        callbackAdapter_->SeekTo(time);
    }
}

void MediaAVSessionCallbackImpl::OnSetSpeed(double speed)
{
}

void MediaAVSessionCallbackImpl::OnSetLoopMode(int32_t loopMode)
{
}

void MediaAVSessionCallbackImpl::OnToggleFavorite(const std::string& assertId)
{
}

void MediaAVSessionCallbackImpl::OnMediaKeyEvent(const MMI::KeyEvent& keyEvent)
{
}

void MediaAVSessionCallbackImpl::OnOutputDeviceChange(const int32_t connectionState,
    const AVSession::OutputDeviceInfo& outputDeviceInfo)
{
}

void MediaAVSessionCallbackImpl::OnCommonCommand(
    const std::string& commonCommand,
    const AAFwk::WantParams& commandArgs)
{
}

void MediaAVSessionCallbackImpl::OnSkipToQueueItem(int32_t itemId)
{
}

void MediaAVSessionCallbackImpl::OnAVCallAnswer()
{
}

void MediaAVSessionCallbackImpl::OnAVCallHangUp()
{
}

void MediaAVSessionCallbackImpl::OnAVCallToggleCallMute()
{
}

void MediaAVSessionCallbackImpl::OnPlayFromAssetId(int64_t assetId)
{
}

void MediaAVSessionCallbackImpl::OnCastDisplayChange(
    const AVSession::CastDisplayInfo& castDisplayInfo)
{
}

void MediaAVSessionKey::Init()
{
    pid_ = getprocpid();
    element_ = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();
    WVLOG_I("media avsession adapter Init AAFwk BundleName=%{public}s, AbilityName=%{public}s",
            element_.GetBundleName().c_str(), element_.GetAbilityName().c_str());
    auto context = AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (context) {
        element_.SetBundleName(context->GetBundleName());
        WVLOG_I("media avsession adapter Init context BundleName()=%{public}s", context->GetBundleName().c_str());
    }
    type_ = MediaAVSessionType::MEDIA_TYPE_INVALID;

    // SA application can get AbilityName by GetTopAbility, but others cannnot.
    if (!element_.GetAbilityName().empty()) {
        WVLOG_I("media avsession adapter Init element_.GetAbilityName()=%{public}s", element_.GetAbilityName().c_str());
        return;
    }

    OH_NativeBundle_ElementName bundleInfo = OH_NativeBundle_GetMainElementName();
    WVLOG_I("OH_NativeBundle_GetMainElementName, bundleName=%{public}s, abilityName=%{public}s", bundleInfo.bundleName,
        bundleInfo.abilityName);
    if (bundleInfo.abilityName) {
        element_.SetAbilityName(std::string(bundleInfo.abilityName));
    }
}

int32_t MediaAVSessionKey::GetPID()
{
    return pid_;
}

AppExecFwk::ElementName MediaAVSessionKey::GetElement()
{
    return element_;
}

void MediaAVSessionKey::SetType(MediaAVSessionType type)
{
    type_ = type;
}

MediaAVSessionType MediaAVSessionKey::GetType()
{
    return type_;
}

std::string MediaAVSessionKey::ToString()
{
    return (std::to_string(pid_) + "_" + element_.GetBundleName() + "_" + element_.GetAbilityName());
}

MediaAVSessionAdapterImpl::MediaAVSessionAdapterImpl()
{
    avSessionKey_ = std::make_shared<MediaAVSessionKey>();
    avSessionKey_->Init();
    avMetadata_ = std::make_shared<AVSession::AVMetaData>();
    avMetadata_->SetAssetId(std::to_string(avSessionKey_->GetPID()));
    avPlaybackState_ = std::make_shared<AVSession::AVPlaybackState>();
}

MediaAVSessionAdapterImpl::~MediaAVSessionAdapterImpl()
{
    DestroyAVSession();
}

bool MediaAVSessionAdapterImpl::CreateAVSession(MediaAVSessionType type)
{
    WVLOG_I("media avsession adapter CreateAVSession in, type=%{public}d", int32_t(type));
    if (MediaAVSessionType::MEDIA_TYPE_INVALID == type) {
        WVLOG_E("media avsession adapter CreateAVSession, type invalid return false");
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
            if (findIter->second.get() != avSession_.get()) {
                DestroyAndEraseSession();
                DestroyAVSession();
            } else {
                WVLOG_E("media avsession adapter CreateAVSession, return false");
                return false;
            }
        }
        return CreateNewSession(type);
    }
}

void MediaAVSessionAdapterImpl::DestroyAVSession()
{
    WVLOG_I("media avsession adapter DestroyAVSession in");
    if (avSession_) {
        int32_t ret = avSession_->Destroy();
        if (ret != AVSession::AVSESSION_SUCCESS) {
            WVLOG_E("media avsession adapter DestroyAVSession Destroy() failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("media avsession adapter DestroyAVSession Destroy() success, ret: %{public}d", ret);
        }
        avSession_.reset();
    }
    auto iter = avSessionMap.find(avSessionKey_->ToString());
    if (iter != avSessionMap.end()) {
        avSessionMap.erase(iter);
    }
    WVLOG_I("media avsession adapter DestroyAVSession out");
}

bool MediaAVSessionAdapterImpl::RegistCallback(
    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter)
{
    WVLOG_I("media avsession adapter RegistCallback in");

    if (avSession_ && Activate()) {
        auto callback = std::make_shared<MediaAVSessionCallbackImpl>(callbackAdapter);
        int32_t ret = avSession_->RegisterCallback(callback);
        if (ret != AVSession::AVSESSION_SUCCESS) {
            WVLOG_E("media avsession adapter RegistCallback RegisterCallback() failed, ret: %{public}d", ret);
            return false;
        }
        static const std::vector<int32_t> commands = {
            AVSession::AVControlCommand::SESSION_CMD_PLAY,
            AVSession::AVControlCommand::SESSION_CMD_PAUSE,
            AVSession::AVControlCommand::SESSION_CMD_STOP,
            AVSession::AVControlCommand::SESSION_CMD_SEEK
        };
        for (auto command : commands) {
            ret = avSession_->AddSupportCommand(command);
            if (ret != AVSession::AVSESSION_SUCCESS) {
                WVLOG_E("media avsession adapter RegistCallback AddSupportCommand() '%{public}d' failed", command);
            } else {
                WVLOG_I("media avsession adapter RegistCallback AddSupportCommand() '%{public}d' success", command);
            }
        }
        WVLOG_I("media avsession adapter RegistCallback out return true");
        return true;
    }
    WVLOG_I("media avsession adapter RegistCallback out return false");
    return false;
}

bool MediaAVSessionAdapterImpl::IsActivated()
{
    WVLOG_I("media avsession adapter IsActivated in");
    if (avSession_) {
        bool ret = avSession_->IsActive();
        WVLOG_I("media avsession adapter IsActive out ret: %{public}d", ret);
        return ret;
    }
    WVLOG_E("media avsession adapter IsActivated out avSession is null, return false");
    return false;
}

bool MediaAVSessionAdapterImpl::Activate()
{
    if (!avSession_) {
        WVLOG_E("media avsession adapter Activate avSession_ is null, return false");
        return false;
    } else if (avSession_->IsActive()) {
        WVLOG_I("media avsession adapter Activate IsActive() is true, return true");
        return true;
    }
    int32_t ret = avSession_->Activate();
    if (ret != AVSession::AVSESSION_SUCCESS) {
        WVLOG_E("media avsession adapter Activate Activate() failed, ret: %{public}d", ret);
        return false;
    }
    WVLOG_I("media avsession adapter Activate return true");
    return true;
}

void MediaAVSessionAdapterImpl::DeActivate()
{
    if (avSession_ && avSession_->IsActive()) {
        int32_t ret = avSession_->Deactivate();
        if (ret != AVSession::AVSESSION_SUCCESS) {
            WVLOG_E("media avsession adapter deactivate avsession failed, ret: %{public}d", ret);
        }
    }
}

void MediaAVSessionAdapterImpl::SetMetadata(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata)
{
    UpdateMetaDataCache(metadata);
    if (avSession_) {
        Activate();
        auto avMetadata = avMetadata_.get();
        int32_t ret = avSession_->SetAVMetaData(*avMetadata);
        if (ret != AVSession::AVSESSION_SUCCESS) {
            WVLOG_E("media avsession adapter SetMetadata SetAVMetaData() failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("media avsession adapter SetMetadata SetAVMetaData() success, ret: %{public}d", ret);
        }
    }
}

void MediaAVSessionAdapterImpl::SetPlaybackState(MediaAVSessionPlayState state)
{
    if (UpdatePlaybackStateCache(state) && avSession_) {
        Activate();
        auto avPlaybackState = avPlaybackState_.get();
        int32_t ret = avSession_->SetAVPlaybackState(*avPlaybackState);
        if (ret != AVSession::AVSESSION_SUCCESS) {
            WVLOG_E("media avsession adapter SetPlaybackState SetAVPlaybackState() failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("media avsession adapter SetPlaybackState SetAVPlaybackState() success, ret: %{public}d", ret);
        }
    }
}

void MediaAVSessionAdapterImpl::SetPlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position)
{
    if (UpdateMetaDataCache(position) && avSession_) {
        Activate();
        auto avMetadata = avMetadata_.get();
        int32_t ret = avSession_->SetAVMetaData(*avMetadata);
        if (ret != AVSession::AVSESSION_SUCCESS) {
            WVLOG_E("media avsession adapter SetPlaybackPosition SetAVMetaData() failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("media avsession adapter SetPlaybackPosition SetAVMetaData() success, ret: %{public}d", ret);
        }
    }
    if (UpdatePlaybackStateCache(position) && avSession_) {
        Activate();
        auto avPlaybackState = avPlaybackState_.get();
        int32_t ret = avSession_->SetAVPlaybackState(*avPlaybackState);
        if (ret != AVSession::AVSESSION_SUCCESS) {
            WVLOG_E("media avsession adapter SetPlaybackPosition SetAVPlaybackState() failed, ret: %{public}d", ret);
        } else {
            WVLOG_I("media avsession adapter SetPlaybackPosition SetAVPlaybackState() success, ret: %{public}d", ret);
        }
    }
}

bool MediaAVSessionAdapterImpl::UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata)
{
    bool updated = false;
    if (avMetadata_->GetTitle() != metadata->GetTitle()) {
        avMetadata_->SetTitle(metadata->GetTitle());
        updated = true;
    }
    if (avMetadata_->GetArtist() != metadata->GetArtist()) {
        avMetadata_->SetArtist(metadata->GetArtist());
        updated = true;
    }
    if (avMetadata_->GetAlbum() != metadata->GetAlbum()) {
        avMetadata_->SetAlbum(metadata->GetAlbum());
        updated = true;
    }
    if (avMetadata_->GetMediaImageUri() != metadata->GetImageUrl()) {
        avMetadata_->SetMediaImageUri(metadata->GetImageUrl());
        updated = true;
    }
    WVLOG_I("media avsession adapter UpdateMetaDataCache return updated: %{public}d", updated);
    return updated;
}

bool MediaAVSessionAdapterImpl::UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionPositionAdapter> position)
{
    if (!position) {
        WVLOG_E("position is nullptr, media avsession adapter UpdateMetaDataCache return false");
        return false;
    }
    int64_t getDuration = position->GetDuration();
    if (avMetadata_->GetDuration() != getDuration) {
        if (getDuration < INT64_MAX) {
            avMetadata_->SetDuration(getDuration);
        } else {
            avMetadata_->SetDuration(LIVE_STREAM_INFINITE_DURATION);
        }
        return true;
    }
    WVLOG_E("media avsession adapter UpdateMetaDataCache return false");
    return false;
}

bool MediaAVSessionAdapterImpl::UpdatePlaybackStateCache(MediaAVSessionPlayState state)
{
    int32_t currentState;
    switch (state) {
        case MediaAVSessionPlayState::STATE_PLAY:
            currentState = AVSession::AVPlaybackState::PLAYBACK_STATE_PLAY;
            break;
        case MediaAVSessionPlayState::STATE_PAUSE:
            currentState = AVSession::AVPlaybackState::PLAYBACK_STATE_PAUSE;
            break;
        case MediaAVSessionPlayState::STATE_INITIAL:
        default:
            currentState = AVSession::AVPlaybackState::PLAYBACK_STATE_INITIAL;
            break;
    }
    if (avPlaybackState_->GetState() != currentState) {
        avPlaybackState_->SetState(currentState);
        WVLOG_I("media avsession adapter UpdatePlaybackStateCache return true");
        return true;
    }
    WVLOG_E("media avsession adapter UpdatePlaybackStateCache return false");
    return false;
}

bool MediaAVSessionAdapterImpl::UpdatePlaybackStateCache(
    const std::shared_ptr<MediaAVSessionPositionAdapter> position)
{
    bool updated = false;
    auto duration = static_cast<int32_t>(position->GetDuration());
    if (avPlaybackState_->GetDuration() != duration) {
        avPlaybackState_->SetDuration(duration);
        updated = true;
    }
    auto avPosition = avPlaybackState_->GetPosition();
    if ((avPosition.elapsedTime_ != position->GetElapsedTime()) ||
        (avPosition.updateTime_ != position->GetUpdateTime())) {
        avPosition.elapsedTime_ = position->GetElapsedTime();
        avPosition.updateTime_ = position->GetUpdateTime();
        avPlaybackState_->SetPosition(avPosition);
        updated = true;
    }
    WVLOG_I("media avsession adapter UpdatePlaybackStateCache return updated: %{public}d", updated);
    return updated;
}

void MediaAVSessionAdapterImpl::DestroyAndEraseSession()
{
    WVLOG_I("media avsession adapter DestroyAndEraseSession in");
    auto iter = avSessionMap.find(avSessionKey_->ToString());
    if (iter == avSessionMap.end()) {
        WVLOG_E("media avsession adapter DestroyAndEraseSession invalid iterator return");
        return;
    }
    if (!iter->second) {
        avSessionMap.erase(iter);
        WVLOG_E("media avsession adapter DestroyAndEraseSession avsession is null pointer return");
        return;
    }
    int32_t ret = iter->second->Destroy();
    if (ret != AVSession::AVSESSION_SUCCESS) {
        WVLOG_E("media avsession adapter DestroyAndEraseSession Destroy failed, ret: %{public}d", ret);
    } else {
        WVLOG_I("media avsession adapter DestroyAndEraseSession Destroy success, ret: %{public}d", ret);
    }
    iter->second.reset();
    avSessionMap.erase(iter);
    WVLOG_I("media avsession adapter DestroyAndEraseSession out");
}

bool MediaAVSessionAdapterImpl::CreateNewSession(const MediaAVSessionType& type)
{
    WVLOG_I("media avsession adapter CreateNewSession in");
    avSession_ = AVSession::AVSessionManager::GetInstance().CreateSession(
        avSessionKey_->GetElement().GetBundleName(), static_cast<int32_t>(type), avSessionKey_->GetElement());
    if (avSession_) {
        avSessionKey_->SetType(type);
        avSessionMap.insert(
            std::pair<std::string, std::shared_ptr<AVSession::AVSession>>(avSessionKey_->ToString(), avSession_));
        return true;
    } else {
        WVLOG_E("media avsession adapter CreateNewSession Fail, out return false");
        return false;
    }
}

} // namespace OHOS::NWeb
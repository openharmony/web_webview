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

#ifndef MEDIA_AVSESSION_ADAPTER_IMPL_H
#define MEDIA_AVSESSION_ADAPTER_IMPL_H

#include <unordered_map>

#include "media_avsession_adapter.h"
#include "av_session.h"

namespace OHOS::NWeb {

class MediaAVSessionCallbackImpl : public AVSession::AVSessionCallback {
public:
    MediaAVSessionCallbackImpl(std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter);
    ~MediaAVSessionCallbackImpl() = default;

    void OnPlay() override;
    void OnPause() override;
    void OnStop() override;
    void OnPlayNext() override;
    void OnPlayPrevious() override;
    void OnFastForward(int64_t time) override;
    void OnRewind(int64_t time) override;
    void OnSeek(int64_t time) override;
    void OnSetSpeed(double speed) override;
    void OnSetLoopMode(int32_t loopMode) override;
    void OnToggleFavorite(const std::string& assertId) override;
    void OnMediaKeyEvent(const MMI::KeyEvent& keyEvent) override;
    void OnOutputDeviceChange(const int32_t connectionState,
             const AVSession::OutputDeviceInfo& outputDeviceInfo) override;
    void OnCommonCommand(const std::string& commonCommand, const AAFwk::WantParams& commandArgs) override;
    void OnSkipToQueueItem(int32_t itemId) override;
    void OnAVCallAnswer() override;
    void OnAVCallHangUp() override;
    void OnAVCallToggleCallMute() override;
    void OnPlayFromAssetId(int64_t assetId) override;
    void OnCastDisplayChange(const AVSession::CastDisplayInfo& castDisplayInfo) override;

private:
    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter_;
};

class MediaAVSessionKey {
public:
    MediaAVSessionKey() = default;
    ~MediaAVSessionKey() = default;

    void Init();
    int32_t GetPID();
    AppExecFwk::ElementName GetElement();
    void SetType(MediaAVSessionType type);
    MediaAVSessionType GetType();
    std::string ToString();

private:
    int32_t pid_;
    AppExecFwk::ElementName element_;
    MediaAVSessionType type_;
};

class MediaAVSessionAdapterImpl : public MediaAVSessionAdapter {
public:
    MediaAVSessionAdapterImpl();
    ~MediaAVSessionAdapterImpl() override;

    bool CreateAVSession(MediaAVSessionType type) override;
    void DestroyAVSession() override;
    bool RegistCallback(std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter) override;
    bool IsActivated() override;
    bool Activate() override;
    void DeActivate() override;
    void SetMetadata(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) override;
    void SetPlaybackState(MediaAVSessionPlayState state) override;
    void SetPlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position) override;

private:
    bool UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata);
    bool UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionPositionAdapter> position);
    bool UpdatePlaybackStateCache(MediaAVSessionPlayState state);
    bool UpdatePlaybackStateCache(const std::shared_ptr<MediaAVSessionPositionAdapter> position);
    void DestroyAndEraseSession();
    bool CreateNewSession(const MediaAVSessionType& type);

    std::shared_ptr<MediaAVSessionKey> avSessionKey_;
    std::shared_ptr<AVSession::AVMetaData> avMetadata_;
    std::shared_ptr<AVSession::AVPlaybackState> avPlaybackState_;
    std::shared_ptr<AVSession::AVSession> avSession_;
    static std::unordered_map<std::string, std::shared_ptr<AVSession::AVSession>> avSessionMap;
};
} // namespace OHOS::NWeb

#endif // MEDIA_AVSESSION_ADAPTER_IMPL_H
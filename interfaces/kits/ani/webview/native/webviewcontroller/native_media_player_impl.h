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

#ifndef ANI_NATIVE_MEDIA_PLAYER_IMPL_H
#define ANI_NATIVE_MEDIA_PLAYER_IMPL_H

#include <ani.h>

#include "napi/native_api.h"
#include "nweb_native_media_player.h"

namespace OHOS::NWeb {

class AniNativeMediaPlayerHandlerImpl {
public:
    AniNativeMediaPlayerHandlerImpl(int32_t nwebId, std::shared_ptr<NWebNativeMediaPlayerHandler> handler);
    ~AniNativeMediaPlayerHandlerImpl() = default;

    void HandleStatusChanged(PlaybackStatus status);

    void HandleVolumeChanged(double volume);

    void HandleMutedChanged(bool isMuted);

    void HandlePlaybackRateChanged(double playbackRate);

    void HandleDurationChanged(double duration);

    void HandleTimeUpdate(double playTime);

    void HandleBufferedEndTimeChanged(double bufferedEndTime);

    void HandleEnded();

    void HandleNetworkStateChanged(NetworkState state);

    void HandleReadyStateChanged(ReadyState state);

    void HandleFullScreenChanged(bool isFullScreen);

    void HandleSeeking();

    void HandleSeekFinished();

    void HandleError(MediaError error, const std::string& message);

    void HandleVideoSizeChanged(double width, double height);

private:
    int32_t nwebId_ = -1;
    std::shared_ptr<NWebNativeMediaPlayerHandler> handler_ = nullptr;
};

class NWebNativeMediaPlayerBridgeImpl : public NWebNativeMediaPlayerBridge {
public:
    NWebNativeMediaPlayerBridgeImpl(int32_t nwebId, ani_vm* vm, ani_ref value);
    ~NWebNativeMediaPlayerBridgeImpl();

    void UpdateRect(double x, double y, double width, double height) override;

    void Play() override;

    void Pause() override;

    void Seek(double time) override;

    void SetVolume(double volume) override;

    void SetMuted(bool isMuted) override;

    void SetPlaybackRate(double playbackRate) override;

    void Release() override;

    void EnterFullScreen() override;

    void ExitFullScreen() override;

    void ResumeMediaPlayer() override;

    void SuspendMediaPlayer(SuspendType type) override;

private:
    int32_t nwebId_ = -1;
    ani_vm* vm_ = nullptr;
    ani_ref value_ = nullptr;
};

class NWebCreateNativeMediaPlayerCallbackImpl : public NWebCreateNativeMediaPlayerCallback {
public:
    explicit NWebCreateNativeMediaPlayerCallbackImpl(int32_t nwebId, ani_vm* vm, ani_fn_object callback);
    ~NWebCreateNativeMediaPlayerCallbackImpl();

    std::shared_ptr<NWebNativeMediaPlayerBridge> OnCreate(
        std::shared_ptr<NWebNativeMediaPlayerHandler> handler, std::shared_ptr<NWebMediaInfo> mediaInfo) override;

private:
    ani_object ConstructRect(std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo);

    ani_object ConstructHandler(std::shared_ptr<NWebNativeMediaPlayerHandler> handler);

    ani_object ConstructControls(const std::vector<std::string>& controls);

    ani_object ConstructHeaders(const std::map<std::string, std::string>& headers);

    ani_object ConstructAttributes(const std::map<std::string, std::string>& attributes);

    ani_object ConstructMediaInfo(std::shared_ptr<NWebMediaInfo> mediaInfo);

    ani_object ConstructSourceInfos(const std::vector<std::shared_ptr<NWebMediaSourceInfo>>& sourceInfos);

    ani_object ConstructSurfaceInfo(std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo);

private:
    int32_t nwebId_ = -1;
    ani_vm* vm_ = nullptr;
    ani_ref callback_ = nullptr;
};

} // namespace OHOS::NWeb

#endif // ANI_NATIVE_MEDIA_PLAYER_IMPL_H

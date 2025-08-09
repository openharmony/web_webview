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

#ifndef NATIVE_MEDIA_PLAYER_IMPL_H
#define NATIVE_MEDIA_PLAYER_IMPL_H

#include "napi/native_api.h"
#include "nweb_native_media_player.h"
#include "napi_native_mediaplayer_handler_impl.h"

namespace OHOS::NWeb {

class NWebNativeMediaPlayerBridgeImpl : public NWebNativeMediaPlayerBridge {
public:
    NWebNativeMediaPlayerBridgeImpl(int32_t nwebId, napi_env env, napi_value value);
    ~NWebNativeMediaPlayerBridgeImpl() = default;

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
    napi_env env_ = nullptr;
    napi_value value_ = nullptr;
};

class NWebCreateNativeMediaPlayerCallbackImpl : public NWebCreateNativeMediaPlayerCallback {
public:
    explicit NWebCreateNativeMediaPlayerCallbackImpl(int32_t nwebId, napi_env env, napi_ref callback);
    ~NWebCreateNativeMediaPlayerCallbackImpl();

    std::shared_ptr<NWebNativeMediaPlayerBridge> OnCreate(
        std::shared_ptr<NWebNativeMediaPlayerHandler> handler, std::shared_ptr<NWebMediaInfo> mediaInfo) override;

private:
    void ConstructRect(napi_value* value, std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo);

    void ConstructHandler(napi_value* value, std::shared_ptr<NWebNativeMediaPlayerHandler> handler);

    void ConstructControls(napi_value* value, const std::vector<std::string>& controls);

    void ConstructHeaders(napi_value* value, const std::map<std::string, std::string>& headers);

    void ConstructAttributes(napi_value* value, const std::map<std::string, std::string>& attributes);

    void ConstructMediaInfo(napi_value* value, std::shared_ptr<NWebMediaInfo> mediaInfo);

    void ConstructSourceInfos(napi_value* value, const std::vector<std::shared_ptr<NWebMediaSourceInfo>>& sourceInfos);

    void ConstructSurfaceInfo(napi_value* value, std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo);

private:
    int32_t nwebId_ = -1;
    napi_env env_ = nullptr;
    napi_ref callback_ = nullptr;
};

} // namespace OHOS::NWeb

#endif // NATIVE_MEDIA_PLAYER_IMPL_H

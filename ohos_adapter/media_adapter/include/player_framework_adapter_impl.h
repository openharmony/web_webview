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

#ifndef PLAYER_FRAMEWORK_ADAPTER_H
#define PLAYER_FRAMEWORK_ADAPTER_H

#include "media_adapter.h"
#include "player.h"
#include "loading_request.h"

#include <mutex>

namespace OHOS::NWeb {
class PlayerCallbackImpl : public Media::PlayerCallback {
public:
    explicit PlayerCallbackImpl(std::shared_ptr<PlayerCallbackAdapter> callback);

    ~PlayerCallbackImpl() = default;

    void OnInfo(Media::PlayerOnInfoType type, int32_t extra, const Media::Format& infoBody) override;

    void OnError(int32_t errorCode, const std::string& errorMsg) override;

private:
    std::shared_ptr<PlayerCallbackAdapter> callbackAdapter_ = nullptr;
};

// LoaderCallbackImpl: implements Media::LoaderCallback to bridge
// player framework's data requests to Chromium's network stack via MediaSourceDataHandler.
class LoaderCallbackImpl : public Media::LoaderCallback {
public:
    explicit LoaderCallbackImpl(std::shared_ptr<MediaSourceDataHandler> handler);

    ~LoaderCallbackImpl() = default;

    // Called by PlayerServer IPC when HiStreamer needs data
    int64_t Open(std::shared_ptr<Media::LoadingRequest>& request) override;
    void Read(int64_t uuid, int64_t offset, int64_t length) override;
    void Close(int64_t uuid) override;

    // Called by PlayerAdapterImpl when Chromium pushes data back
    void OnRespondHeader(int64_t uuid, const std::map<std::string, std::string>& header,
                         const std::string& redirectUrl);
    void OnRespondData(int64_t uuid, int64_t offset, const std::vector<uint8_t>& data);
    void OnFinishLoading(int64_t uuid, int32_t errorCode);

private:
    std::mutex mutex_;
    std::map<int64_t, std::shared_ptr<Media::LoadingRequest>> requests_;
    std::shared_ptr<MediaSourceDataHandler> handler_;
};

class PlayerAdapterImpl : public PlayerAdapter {
public:
    PlayerAdapterImpl();

    ~PlayerAdapterImpl() override;

    int32_t SetPlayerCallback(std::shared_ptr<PlayerCallbackAdapter> callbackAdapter) override;

    int32_t SetSource(const std::string& url) override;

    int32_t SetSource(int32_t fd, int64_t offset = 0, int64_t size = 0) override;

    int32_t SetVideoSurface(std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter) override;

    int32_t SetVolume(float leftVolume, float rightVolume) override;

    int32_t Seek(int32_t mSeconds, PlayerSeekMode mode) override;

    int32_t Play() override;

    int32_t Pause() override;

    int32_t PrepareAsync() override;

    int32_t GetCurrentTime(int32_t& currentTime) override;

    int32_t GetDuration(int32_t& duration) override;

    int32_t SetPlaybackSpeed(PlaybackRateMode mode) override;

    int32_t SetVideoSurfaceNew(void* native_window) override;

    int32_t SetMediaSourceHeader(const std::string& url, const std::map<std::string, std::string>& header) override;

    // HLS proxy download: creates AVMediaSource with LoaderCallback
    int32_t SetMediaSourceHeaderForHls(
        const std::string& url,
        const std::map<std::string, std::string>& header,
        std::shared_ptr<MediaSourceDataHandler> handler) override;

    // Data response from Chromium
    void OnDataRespondHeader(int64_t uuid,
        const std::map<std::string, std::string>& header,
        const std::string& redirectUrl) override;
    void OnDataRespondData(int64_t uuid, int64_t offset,
        const std::vector<uint8_t>& data) override;
    void OnDataFinishLoading(int64_t uuid, int32_t errorCode) override;

private:
    std::shared_ptr<Media::Player> player_ = nullptr;
    std::shared_ptr<LoaderCallbackImpl> loader_callback_;
};
} // namespace OHOS::NWeb

#endif // PLAYER_FRAMEWORK_ADAPTER_H

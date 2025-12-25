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

#ifndef MEDIA_AVSESSION_ADAPTER_H
#define MEDIA_AVSESSION_ADAPTER_H

#include <string>
#include <memory>

namespace OHOS::NWeb {

enum class MediaAVSessionType : int32_t { MEDIA_TYPE_INVALID = -1, MEDIA_TYPE_AUDIO = 0, MEDIA_TYPE_VIDEO = 1 };

enum class MediaAVSessionPlayState : int32_t { STATE_INITIAL = 0, STATE_PLAY = 2, STATE_PAUSE = 3 };

class MediaAVSessionMetadataAdapter {
public:
    MediaAVSessionMetadataAdapter() = default;

    virtual ~MediaAVSessionMetadataAdapter() = default;

    virtual void SetTitle(const std::string& title) = 0;

    virtual std::string GetTitle() = 0;

    virtual void SetArtist(const std::string& artist) = 0;

    virtual std::string GetArtist() = 0;

    virtual void SetAlbum(const std::string& album) = 0;

    virtual std::string GetAlbum() = 0;

    virtual void SetImageUrl(const std::string& imageUrl) {}

    virtual std::string GetImageUrl() { return ""; }
};

struct MediaAVSessionPositionAdapter {
public:
    MediaAVSessionPositionAdapter() = default;

    virtual ~MediaAVSessionPositionAdapter() = default;

    virtual void SetDuration(int64_t duration) = 0;

    virtual int64_t GetDuration() = 0;

    virtual void SetElapsedTime(int64_t elapsedTime) = 0;

    virtual int64_t GetElapsedTime() = 0;

    virtual void SetUpdateTime(int64_t updateTime) = 0;

    virtual int64_t GetUpdateTime() = 0;
};

class MediaAVSessionCallbackAdapter {
public:
    virtual ~MediaAVSessionCallbackAdapter() = default;

    virtual void Play() = 0;

    virtual void Pause() = 0;

    virtual void Stop() = 0;

    virtual void SeekTo(int64_t millisTime) = 0;

    virtual int32_t GetMediaCastCurrentTime() = 0;

    virtual void PullUpCastBackGround(Const std::string& device_name) = 0;

    virtual void SetAvCast(bool is_avcast) = 0;

    virtual void UpdateUiPlayState(bool is_playing) = 0;

    virtual void UpdateUiPlayPosition(int64_t position) = 0;

    virtual void MediaCastStopped() = 0;
};

class MediaAVSessionAdapter {
public:
    MediaAVSessionAdapter() = default;

    virtual ~MediaAVSessionAdapter() = default;

    virtual bool CreateAVSession(MediaAVSessionType type) = 0;

    virtual void DestroyAVSession() = 0;

    virtual bool RegistCallback(std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter) = 0;

    virtual bool IsActivated() = 0;

    virtual bool Activate() = 0;

    virtual void DeActivate() = 0;

    virtual void SetMetadata(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) = 0;

    virtual void SetPlaybackState(MediaAVSessionPlayState state) = 0;

    virtual void SetPlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position) = 0;

    virtual void SetMediaCastUri(const std::string& mediaUri) = 0;

    virtual void CreateAVCastAdapter() = 0;

    virtual void SetRemoteCastEnabled(bool enabled) = 0;

    virtual void PrepareMediaCastDescription() = 0;

    virtual void HandleStopMediaCast() = 0;

    virtual void UpdateRemotePlayState(bool is_playing) = 0;

    virtual void UpdateRemotePlayPosition(int64_t position) = 0; 
};

} // namespace OHOS::NWeb

#endif // MEDIA_AVSESSION_ADAPTER_H
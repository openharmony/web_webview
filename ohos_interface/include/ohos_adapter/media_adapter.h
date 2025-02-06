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

#ifndef MEDIA_ADAPTER_H
#define MEDIA_ADAPTER_H

#include <string>
#include <map>

#include "graphic_adapter.h"

namespace OHOS::NWeb {

enum class PlayerAdapterErrorType : int32_t {
    INVALID_CODE = -1,
    UNSUPPORT_TYPE,
    FATAL_ERROR,
};

enum class PlayerOnInfoType : int32_t {
    INFO_TYPE_UNSET = -1,
    INFO_TYPE_SEEKDONE = 1,
    INFO_TYPE_EOS = 4,
    INFO_TYPE_STATE_CHANGE,
    INFO_TYPE_POSITION_UPDATE,
    INFO_TYPE_MESSAGE,
    INFO_TYPE_INTERRUPT_EVENT,
};

enum class PlayerSeekMode : int32_t {
    SEEK_NEXT_SYNC = 0,
    SEEK_PREVIOUS_SYNC,
    SEEK_CLOSEST_SYNC,
    SEEK_CLOSEST,
};

enum class PlaybackRateMode : int32_t {
    SPEED_FORWARD_0_75_X,
    SPEED_FORWARD_1_00_X,
    SPEED_FORWARD_1_25_X,
    SPEED_FORWARD_1_75_X,
    SPEED_FORWARD_2_00_X,
};

class PlayerCallbackAdapter {
public:
    virtual ~PlayerCallbackAdapter() = default;

    virtual void OnInfo(PlayerOnInfoType type, int32_t extra, int32_t value) = 0;

    virtual void OnError(PlayerAdapterErrorType errorType) = 0;
};

class PlayerAdapter {
public:
    enum PlayerStates : int32_t {
        PLAYER_STATE_ERROR = 0,
        PLAYER_IDLE = 1,
        PLAYER_INITIALIZED = 2,
        PLAYER_PREPARING = 3,
        PLAYER_PREPARED = 4,
        PLAYER_STARTED = 5,
        PLAYER_PAUSED = 6,
        PLAYER_STOPPED = 7,
        PLAYER_PLAYBACK_COMPLETE = 8,
        PLAYER_RELEASED = 9,
    };

    PlayerAdapter() = default;

    virtual ~PlayerAdapter() = default;

    virtual int32_t SetPlayerCallback(std::shared_ptr<PlayerCallbackAdapter> callbackAdapter) = 0;

    virtual int32_t SetSource(const std::string& url) = 0;

    virtual int32_t SetSource(int32_t fd, int64_t offset = 0, int64_t size = 0) = 0;

    virtual int32_t SetVideoSurface(std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter) = 0;

    virtual int32_t SetVolume(float leftVolume, float rightVolume) = 0;

    virtual int32_t Seek(int32_t mSeconds, PlayerSeekMode mode) = 0;

    virtual int32_t Play() = 0;

    virtual int32_t Pause() = 0;

    virtual int32_t PrepareAsync() = 0;

    virtual int32_t GetCurrentTime(int32_t& currentTime) = 0;

    virtual int32_t GetDuration(int32_t& duration) = 0;

    virtual int32_t SetPlaybackSpeed(PlaybackRateMode mode) = 0;

    virtual int32_t SetMediaSourceHeader(const std::string& url, const std::map<std::string, std::string>& header)
    {
        return -1;
    }
};

} // namespace OHOS::NWeb

#endif // MEDIA_ADAPTER_H

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NAPI_NATIVE_MEDIAPLAYER_HANDLER_IMPL_H
#define NAPI_NATIVE_MEDIAPLAYER_HANDLER_IMPL_H

#include "nweb_native_media_player.h"
#include "transfer_referenced_count.h"

namespace OHOS::NWeb {

class NapiNativeMediaPlayerHandlerImpl : public TransferReferencedCount {
public:
    NapiNativeMediaPlayerHandlerImpl(int32_t nwebId, std::shared_ptr<NWebNativeMediaPlayerHandler> handler);
    ~NapiNativeMediaPlayerHandlerImpl() = default;

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

} // namespace OHOS::NWeb

#endif // NAPI_NATIVE_MEDIAPLAYER_HANDLER_IMPL_H

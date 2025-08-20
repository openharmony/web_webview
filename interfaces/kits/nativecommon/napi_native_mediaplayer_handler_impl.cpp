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

#include "napi_native_mediaplayer_handler_impl.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

NapiNativeMediaPlayerHandlerImpl::NapiNativeMediaPlayerHandlerImpl(
    int32_t nwebId, std::shared_ptr<NWebNativeMediaPlayerHandler> handler)
    : nwebId_(nwebId), handler_(handler)
{}

void NapiNativeMediaPlayerHandlerImpl::HandleStatusChanged(PlaybackStatus status)
{
    WVLOG_D("begin to handle status changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleStatusChanged(status);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleVolumeChanged(double volume)
{
    WVLOG_D("begin to handle volume changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleVolumeChanged(volume);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleMutedChanged(bool isMuted)
{
    WVLOG_D("begin to handle muted changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleMutedChanged(isMuted);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandlePlaybackRateChanged(double playbackRate)
{
    WVLOG_D("begin to handle playback rate changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandlePlaybackRateChanged(playbackRate);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleDurationChanged(double duration)
{
    WVLOG_D("begin to handle duration changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleDurationChanged(duration);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleTimeUpdate(double playTime)
{
    if (handler_) {
        handler_->HandleTimeUpdate(playTime);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleBufferedEndTimeChanged(double bufferedEndTime)
{
    WVLOG_D("begin to handle buffered end time changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleBufferedEndTimeChanged(bufferedEndTime);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleEnded()
{
    WVLOG_D("begin to handle end,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleEnded();
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleNetworkStateChanged(NetworkState state)
{
    WVLOG_D("begin to handle network state changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleNetworkStateChanged(state);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleReadyStateChanged(ReadyState state)
{
    WVLOG_D("begin to handle ready state changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleReadyStateChanged(state);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleFullScreenChanged(bool isFullScreen)
{
    WVLOG_D("begin to handle full screen changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleFullScreenChanged(isFullScreen);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleSeeking()
{
    WVLOG_D("begin to handle seeking,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleSeeking();
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleSeekFinished()
{
    WVLOG_D("begin to handle seek finished,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleSeekFinished();
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleError(MediaError error, const std::string& message)
{
    WVLOG_D("begin to handle error,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleError(error, message);
    }
}

void NapiNativeMediaPlayerHandlerImpl::HandleVideoSizeChanged(double width, double height)
{
    WVLOG_D("begin to handle video size changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        handler_->HandleVideoSizeChanged(width, height);
    }
}

} // namespace OHOS::NWeb

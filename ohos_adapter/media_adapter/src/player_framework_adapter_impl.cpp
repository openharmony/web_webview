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

#include "player_framework_adapter_impl.h"

#include "../native/media_errors.h"
#include "nweb_log.h"
#include "surface_adapter_impl.h"
#include "surface/window.h"
#include "native_window.h"
#include <cstdint>
#include <securec.h>

namespace OHOS::NWeb {
namespace {
bool IsUnsupportType(int32_t errorCode)
{
    switch (errorCode) {
        case Media::MSERR_UNSUPPORT:
        case Media::MSERR_UNSUPPORT_AUD_SRC_TYPE:
        case Media::MSERR_UNSUPPORT_AUD_SAMPLE_RATE:
        case Media::MSERR_UNSUPPORT_AUD_CHANNEL_NUM:
        case Media::MSERR_UNSUPPORT_AUD_ENC_TYPE:
        case Media::MSERR_UNSUPPORT_AUD_PARAMS:
        case Media::MSERR_UNSUPPORT_VID_SRC_TYPE:
        case Media::MSERR_UNSUPPORT_VID_ENC_TYPE:
        case Media::MSERR_UNSUPPORT_VID_PARAMS:
        case Media::MSERR_UNSUPPORT_CONTAINER_TYPE:
        case Media::MSERR_UNSUPPORT_PROTOCOL_TYPE:
        case Media::MSERR_UNSUPPORT_VID_DEC_TYPE:
        case Media::MSERR_UNSUPPORT_AUD_DEC_TYPE:
        case Media::MSERR_UNSUPPORT_STREAM:
        case Media::MSERR_UNSUPPORT_FILE:
        case Media::MSERR_UNSUPPORT_SOURCE:
            return true;
        default:
            return false;
    }
}

bool IsFatalError(int32_t errorCode)
{
    switch (errorCode) {
        case Media::MSERR_EXT_NO_MEMORY:
        case Media::MSERR_EXT_SERVICE_DIED:
        case Media::MSERR_CREATE_PLAYER_ENGINE_FAILED:
        case Media::MSERR_CREATE_AVMETADATAHELPER_ENGINE_FAILED:
        case Media::MSERR_AUD_DEC_FAILED:
        case Media::MSERR_VID_DEC_FAILED:
        case Media::MSERR_FILE_ACCESS_FAILED:
        case Media::MSERR_DATA_SOURCE_IO_ERROR:
        case Media::MSERR_IO_NETWORK_ACCESS_DENIED:
            return true;
        default:
            return false;
    }
}

NWeb::PlayerOnInfoType ConvertInfoType(Media::PlayerOnInfoType infoType)
{
    NWeb::PlayerOnInfoType ret = NWeb::PlayerOnInfoType::INFO_TYPE_UNSET;
    switch (infoType) {
        case Media::INFO_TYPE_SEEKDONE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_SEEKDONE;
            break;
        case Media::INFO_TYPE_EOS:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_EOS;
            break;
        case Media::INFO_TYPE_STATE_CHANGE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_STATE_CHANGE;
            break;
        case Media::INFO_TYPE_POSITION_UPDATE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_POSITION_UPDATE;
            break;
        case Media::INFO_TYPE_MESSAGE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_MESSAGE;
            break;
        case Media::INFO_TYPE_INTERRUPT_EVENT:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_INTERRUPT_EVENT;
            break;
        case Media::INFO_TYPE_RESOLUTION_CHANGE:
            ret = NWeb::PlayerOnInfoType::INFO_TYPE_RESOLUTION_CHANGE;
            break;
        default:
            break;
    }
    return ret;
}

Media::PlayerSeekMode ConvertSeekMode(NWeb::PlayerSeekMode mode)
{
    Media::PlayerSeekMode ret = Media::SEEK_CLOSEST;
    switch (mode) {
        case NWeb::PlayerSeekMode::SEEK_NEXT_SYNC:
            ret = Media::SEEK_NEXT_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_PREVIOUS_SYNC:
            ret = Media::SEEK_PREVIOUS_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_CLOSEST_SYNC:
            ret = Media::SEEK_CLOSEST_SYNC;
            break;
        case NWeb::PlayerSeekMode::SEEK_CLOSEST:
            ret = Media::SEEK_CLOSEST;
            break;
        default:
            break;
    }
    return ret;
}

Media::PlaybackRateMode ConvertRateMode(NWeb::PlaybackRateMode mode)
{
    Media::PlaybackRateMode ret = Media::SPEED_FORWARD_1_00_X;
    switch (mode) {
        case NWeb::PlaybackRateMode::SPEED_FORWARD_0_75_X:
            ret = Media::SPEED_FORWARD_0_75_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_00_X:
            ret = Media::SPEED_FORWARD_1_00_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_25_X:
            ret = Media::SPEED_FORWARD_1_25_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_1_75_X:
            ret = Media::SPEED_FORWARD_1_75_X;
            break;
        case NWeb::PlaybackRateMode::SPEED_FORWARD_2_00_X:
            ret = Media::SPEED_FORWARD_2_00_X;
            break;
        default:
            break;
    }
    return ret;
}
} // namespace

PlayerCallbackImpl::PlayerCallbackImpl(std::shared_ptr<PlayerCallbackAdapter> callback)
    : callbackAdapter_(std::move(callback))
{}

void PlayerCallbackImpl::OnInfo(Media::PlayerOnInfoType type, int32_t extra, const Media::Format& infoBody)
{
    int32_t hintValue = -1;
    if (type == Media::INFO_TYPE_INTERRUPT_EVENT) {
        infoBody.GetIntValue(OHOS::Media::PlayerKeys::AUDIO_INTERRUPT_HINT, hintValue);
    }
    if (type == Media::INFO_TYPE_RESOLUTION_CHANGE) {
        infoBody.GetIntValue(OHOS::Media::PlayerKeys::PLAYER_WIDTH, extra);
        infoBody.GetIntValue(OHOS::Media::PlayerKeys::PLAYER_HEIGHT, hintValue);
    }
    if (callbackAdapter_) {
        callbackAdapter_->OnInfo(ConvertInfoType(type), extra, hintValue);
    }
}

void PlayerCallbackImpl::OnError(int32_t errorCode, const std::string& errorMsg)
{
    (void)errorMsg;
    WVLOG_E("media player error, errorCode=%{public}d", errorCode);
    if (callbackAdapter_) {
        auto errorType = PlayerAdapterErrorType::INVALID_CODE;
        if (IsUnsupportType(errorCode)) {
            errorType = PlayerAdapterErrorType::UNSUPPORT_TYPE;
        } else if (IsFatalError(errorCode)) {
            errorType = PlayerAdapterErrorType::FATAL_ERROR;
        }
        callbackAdapter_->OnError(errorType);
    }
}

PlayerAdapterImpl::PlayerAdapterImpl()
{
    player_ = Media::PlayerFactory::CreatePlayer();
}

PlayerAdapterImpl::~PlayerAdapterImpl()
{
    if (player_) {
        player_->Release();
        WVLOG_I("player release");
    }
}

int32_t PlayerAdapterImpl::SetPlayerCallback(std::shared_ptr<PlayerCallbackAdapter> callbackAdapter)
{
    if (!player_ || !callbackAdapter) {
        WVLOG_E("player_ or callbackAdapter is nullptr");
        return -1;
    }
    auto callback = std::make_shared<PlayerCallbackImpl>(std::move(callbackAdapter));
    return player_->SetPlayerCallback(callback);
}

int32_t PlayerAdapterImpl::SetSource(const std::string& url)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetSource(url);
}

int32_t PlayerAdapterImpl::SetSource(int32_t fd, int64_t offset, int64_t size)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetSource(fd, offset, size);
}

int32_t PlayerAdapterImpl::SetVideoSurface(std::shared_ptr<IConsumerSurfaceAdapter> cSurfaceAdapter)
{
    if (!player_ || !cSurfaceAdapter) {
        WVLOG_E("player_  or cSurfaceAdapter is nullptr");
        return -1;
    }
    auto cSurface = std::static_pointer_cast<ConsumerSurfaceAdapterImpl>(cSurfaceAdapter)->GetConsumerSurface();
    cSurface->SetDefaultUsage(BUFFER_USAGE_CPU_READ);
    sptr<IBufferProducer> producer = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    return player_->SetVideoSurface(pSurface);
}

int32_t PlayerAdapterImpl::SetVolume(float leftVolume, float rightVolume)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetVolume(leftVolume, rightVolume);
}

int32_t PlayerAdapterImpl::Seek(int32_t mSeconds, PlayerSeekMode mode)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->Seek(mSeconds, ConvertSeekMode(mode));
}

int32_t PlayerAdapterImpl::Play()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->Play();
}

int32_t PlayerAdapterImpl::Pause()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->Pause();
}

int32_t PlayerAdapterImpl::PrepareAsync()
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->PrepareAsync();
}

int32_t PlayerAdapterImpl::GetCurrentTime(int32_t& currentTime)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->GetCurrentTime(currentTime);
}

int32_t PlayerAdapterImpl::GetDuration(int32_t& duration)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->GetDuration(duration);
}

int32_t PlayerAdapterImpl::SetPlaybackSpeed(PlaybackRateMode mode)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    return player_->SetPlaybackSpeed(ConvertRateMode(mode));
}

int32_t PlayerAdapterImpl::SetVideoSurfaceNew(void* native_window)
{
    WVLOG_I("PlayerAdapterImpl::SetVideoSurfaceNew");
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    OHNativeWindow* ohNativeWindow = reinterpret_cast<OHNativeWindow*>(native_window);
    return player_->SetVideoSurface(ohNativeWindow->surface);
}

int32_t PlayerAdapterImpl::SetMediaSourceHeader(const std::string& url,
    const std::map<std::string, std::string>& header)
{
    if (!player_) {
        WVLOG_E("player_ is nullptr");
        return -1;
    }
    std::shared_ptr<Media::AVMediaSource> mediaSource = std::make_shared<Media::AVMediaSource>(url, header);
    Media::AVPlayStrategy strategy;
    return player_->SetMediaSource(mediaSource, strategy);
}

// --- LoaderCallbackImpl ---

LoaderCallbackImpl::LoaderCallbackImpl(std::shared_ptr<MediaSourceDataHandler> handler)
    : handler_(std::move(handler)) {}

int64_t LoaderCallbackImpl::Open(std::shared_ptr<Media::LoadingRequest>& request)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!handler_ || !request) {
        WVLOG_E("LoaderCallbackImpl::Open handler_ request is nullptr");
        return -1;
    }
    int64_t handle = handler_->HandleDataOpen(request->GetUrl(), request->GetHeader());
    requests_[handle] = request;
    return handle;
}

void LoaderCallbackImpl::Read(int64_t uuid, int64_t offset, int64_t length)
{
    if (!handler_) {
        WVLOG_E("LoaderCallbackImpl::Read handler_ is nullptr");
    }
    handler_->HandleDataRead(uuid, offset, length);
}

void LoaderCallbackImpl::Close(int64_t uuid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!handler_) {
        WVLOG_E("LoaderCallbackImpl::Close handler_ is nullptr");
    }
    handler_->HandleDataClose(uuid);
    requests_.erase(uuid);
}

void LoaderCallbackImpl::OnRespondHeader(int64_t uuid,
    std::map<std::string, std::string> header, std::string redirectUrl)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = requests_.find(uuid);
    if (it != requests_.end()) {
        if (!it->second) {
            WVLOG_E("LoaderCallbackImpl::OnRespondHeader it->second is nullptr");
            return;
        }
        it->second->RespondHeader(uuid, header, redirectUrl);
    }
}

void LoaderCallbackImpl::OnRespondData(int64_t uuid, int64_t offset,
    const std::vector<uint8_t>& data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = requests_.find(uuid);
    if (it == requests_.end() || data.empty()) {
        return;
    }
    if (data.size() > INT32_MAX) {
        WVLOG_E("data size exceeds INT32_MAX");
        return;
    }
    auto mem = std::make_shared<Media::AVSharedMemoryBase>(
        static_cast<int32_t>(data.size()), Media::AVSharedMemory::FLAGS_READ_WRITE, "hlsProxyData");
    if (mem->Init() != 0 || mem->GetBase() == nullptr) {
        WVLOG_E("AVSharedMemoryBase Init failed, size=%{public}zu", data.size());
        return;
    }
    if (memcpy_s(mem->GetBase(), data.size(), data.data(), data.size()) != EOK) {
        WVLOG_E("memcpy_s failed, size=%{public}zu", data.size());
        return;
    }
    if (!it->second) {
        WVLOG_E("LoaderCallbackImpl::OnRespondData it->second is nullptr");
        return;
    }
    it->second->RespondData(uuid, offset, mem);
}

void LoaderCallbackImpl::OnFinishLoading(int64_t uuid, int32_t errorCode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = requests_.find(uuid);
    if (it != requests_.end()) {
        if (!it->second) {
            WVLOG_E("LoaderCallbackImpl::OnFinishLoading it->second is nullptr");
            return;
        }
        it->second->FinishLoading(uuid, errorCode);
        requests_.erase(uuid);
    }
}

// --- PlayerAdapterImpl HLS methods ---

int32_t PlayerAdapterImpl::SetMediaSourceHeaderForHls(const std::string& url,
    const std::map<std::string, std::string>& header,
    std::shared_ptr<MediaSourceDataHandler> handler)
{
    if (!player_ || !handler) {
        WVLOG_E("player_ or handler is nullptr");
        return -1;
    }
    loader_callback_ = std::make_shared<LoaderCallbackImpl>(std::move(handler));
    auto mediaSource = std::make_shared<Media::AVMediaSource>(url, header);
    mediaSource->mediaSourceLoaderCb_ = loader_callback_;
    Media::AVPlayStrategy strategy;
    return player_->SetMediaSource(mediaSource, strategy);
}

void PlayerAdapterImpl::OnDataRespondHeader(int64_t uuid,
    const std::map<std::string, std::string>& header,
    const std::string& redirectUrl)
{
    if (loader_callback_) {
        loader_callback_->OnRespondHeader(uuid, header, redirectUrl);
    }
}

void PlayerAdapterImpl::OnDataRespondData(int64_t uuid, int64_t offset,
    const std::vector<uint8_t>& data)
{
    if (loader_callback_) {
        loader_callback_->OnRespondData(uuid, offset, data);
    }
}

void PlayerAdapterImpl::OnDataFinishLoading(int64_t uuid, int32_t errorCode)
{
    if (loader_callback_) {
        loader_callback_->OnFinishLoading(uuid, errorCode);
    }
}

} // namespace OHOS::NWeb

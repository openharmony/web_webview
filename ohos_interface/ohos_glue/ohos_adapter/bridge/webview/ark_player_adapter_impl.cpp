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

#include "ohos_adapter/bridge/ark_player_adapter_impl.h"

#include "ohos_adapter/bridge/ark_iconsumer_surface_adapter_impl.h"
#include "ohos_adapter/bridge/ark_player_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPlayerAdapterImpl::ArkPlayerAdapterImpl(std::shared_ptr<OHOS::NWeb::PlayerAdapter> ref) : real_(ref) {}

int32_t ArkPlayerAdapterImpl::SetPlayerCallback(ArkWebRefPtr<ArkPlayerCallbackAdapter> callbackAdapter)
{
    if (CHECK_REF_PTR_IS_NULL(callbackAdapter)) {
        return real_->SetPlayerCallback(nullptr);
    }

    return real_->SetPlayerCallback(std::make_shared<ArkPlayerCallbackAdapterWrapper>(callbackAdapter));
}

int32_t ArkPlayerAdapterImpl::SetSource(const ArkWebString& url)
{
    std::string surl = ArkWebStringStructToClass(url);
    return real_->SetSource(surl);
}

int32_t ArkPlayerAdapterImpl::SetSource(int32_t fd, int64_t offset, int64_t size)
{
    return real_->SetSource(fd, offset, size);
}

int32_t ArkPlayerAdapterImpl::SetVideoSurface(ArkWebRefPtr<ArkIConsumerSurfaceAdapter> cSurfaceAdapter)
{
    ArkIConsumerSurfaceAdapterImpl* imp = static_cast<ArkIConsumerSurfaceAdapterImpl*>(cSurfaceAdapter.get());
    return real_->SetVideoSurface(imp->real_);
}

int32_t ArkPlayerAdapterImpl::SetVolume(float leftVolume, float rightVolume)
{
    return real_->SetVolume(leftVolume, rightVolume);
}

int32_t ArkPlayerAdapterImpl::Seek(int32_t mSeconds, int32_t mode)
{
    return real_->Seek(mSeconds, (OHOS::NWeb::PlayerSeekMode)mode);
}

int32_t ArkPlayerAdapterImpl::Play()
{
    return real_->Play();
}

int32_t ArkPlayerAdapterImpl::Pause()
{
    return real_->Pause();
}

int32_t ArkPlayerAdapterImpl::PrepareAsync()
{
    return real_->PrepareAsync();
}

int32_t ArkPlayerAdapterImpl::GetCurrentTime(int32_t& currentTime)
{
    return real_->GetCurrentTime(currentTime);
}

int32_t ArkPlayerAdapterImpl::GetDuration(int32_t& duration)
{
    return real_->GetDuration(duration);
}

int32_t ArkPlayerAdapterImpl::SetPlaybackSpeed(int32_t mode)
{
    return real_->SetPlaybackSpeed((OHOS::NWeb::PlaybackRateMode)mode);
}

int32_t ArkPlayerAdapterImpl::SetMediaSourceHeader(const ArkWebString& url,
    const ArkWebStringMap& header)
{
    std::string surl = ArkWebStringStructToClass(url);
    std::map<std::string, std::string> sheader = ArkWebStringMapStructToClass(header);
    return real_->SetMediaSourceHeader(surl, sheader);
}

int32_t ArkPlayerAdapterImpl::SetVideoSurfaceNew(void* native_window)
{
    return real_->SetVideoSurfaceNew(native_window);
}

} // namespace OHOS::ArkWeb

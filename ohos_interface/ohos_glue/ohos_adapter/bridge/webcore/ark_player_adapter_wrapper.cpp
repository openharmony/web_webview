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

#include "ohos_adapter/bridge/ark_player_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_iconsumer_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_player_callback_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPlayerAdapterWrapper::ArkPlayerAdapterWrapper(ArkWebRefPtr<ArkPlayerAdapter> ref) : ctocpp_(ref) {}

int32_t ArkPlayerAdapterWrapper::SetPlayerCallback(std::shared_ptr<OHOS::NWeb::PlayerCallbackAdapter> callbackAdapter)
{
    if (CHECK_SHARED_PTR_IS_NULL(callbackAdapter)) {
        return ctocpp_->SetPlayerCallback(nullptr);
    }

    return ctocpp_->SetPlayerCallback(new ArkPlayerCallbackAdapterImpl(callbackAdapter));
}

int32_t ArkPlayerAdapterWrapper::SetSource(const std::string& url)
{
    ArkWebString str = ArkWebStringClassToStruct(url);
    return ctocpp_->SetSource(str);
}

int32_t ArkPlayerAdapterWrapper::SetSource(int32_t fd, int64_t offset, int64_t size)
{
    return ctocpp_->SetSource(fd, offset, size);
}

int32_t ArkPlayerAdapterWrapper::SetVideoSurface(std::shared_ptr<OHOS::NWeb::IConsumerSurfaceAdapter> cSurfaceAdapter)
{
    std::shared_ptr<ArkIConsumerSurfaceAdapterWrapper> wrapper =
        std::static_pointer_cast<ArkIConsumerSurfaceAdapterWrapper>(cSurfaceAdapter);
    return ctocpp_->SetVideoSurface(wrapper->ctocpp_);
}

int32_t ArkPlayerAdapterWrapper::SetVolume(float leftVolume, float rightVolume)
{
    return ctocpp_->SetVolume(leftVolume, rightVolume);
}

int32_t ArkPlayerAdapterWrapper::Seek(int32_t mSeconds, OHOS::NWeb::PlayerSeekMode mode)
{
    return ctocpp_->Seek(mSeconds, (int32_t)mode);
}

int32_t ArkPlayerAdapterWrapper::Play()
{
    return ctocpp_->Play();
}

int32_t ArkPlayerAdapterWrapper::Pause()
{
    return ctocpp_->Pause();
}

int32_t ArkPlayerAdapterWrapper::PrepareAsync()
{
    return ctocpp_->PrepareAsync();
}

int32_t ArkPlayerAdapterWrapper::GetCurrentTime(int32_t& currentTime)
{
    return ctocpp_->GetCurrentTime(currentTime);
}

int32_t ArkPlayerAdapterWrapper::GetDuration(int32_t& duration)
{
    return ctocpp_->GetDuration(duration);
}

int32_t ArkPlayerAdapterWrapper::SetPlaybackSpeed(OHOS::NWeb::PlaybackRateMode mode)
{
    return ctocpp_->SetPlaybackSpeed((int32_t)mode);
}

int32_t ArkPlayerAdapterWrapper::SetMediaSourceHeader(const std::string& url,
    const std::map<std::string, std::string>& header)
{
    ArkWebString surl = ArkWebStringClassToStruct(url);
    ArkWebStringMap sheader = ArkWebStringMapClassToStruct(header);
    int32_t result = ctocpp_->SetMediaSourceHeader(surl, sheader);
    ArkWebStringStructRelease(surl);
    ArkWebStringMapStructRelease(sheader);
    return result;
}

} // namespace OHOS::ArkWeb

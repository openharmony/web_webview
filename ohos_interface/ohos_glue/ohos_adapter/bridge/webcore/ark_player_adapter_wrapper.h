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

#ifndef ARK_PLAYER_ADAPTER_WRAPPER_H
#define ARK_PLAYER_ADAPTER_WRAPPER_H
#pragma once

#include "media_adapter.h"
#include "ohos_adapter/include/ark_media_adapter.h"

namespace OHOS::ArkWeb {

class ArkPlayerAdapterWrapper : public OHOS::NWeb::PlayerAdapter {
public:
    explicit ArkPlayerAdapterWrapper(ArkWebRefPtr<ArkPlayerAdapter>);

    int32_t SetPlayerCallback(std::shared_ptr<OHOS::NWeb::PlayerCallbackAdapter> callbackAdapter) override;

    int32_t SetSource(const std::string& url) override;

    int32_t SetSource(int32_t fd, int64_t offset, int64_t size) override;

    int32_t SetVideoSurface(std::shared_ptr<OHOS::NWeb::IConsumerSurfaceAdapter> cSurfaceAdapter) override;

    int32_t SetVolume(float leftVolume, float rightVolume) override;

    int32_t Seek(int32_t mSeconds, OHOS::NWeb::PlayerSeekMode mode) override;

    int32_t Play() override;

    int32_t Pause() override;

    int32_t PrepareAsync() override;

    int32_t GetCurrentTime(int32_t& currentTime) override;

    int32_t GetDuration(int32_t& duration) override;

    int32_t SetPlaybackSpeed(OHOS::NWeb::PlaybackRateMode mode) override;

    int32_t SetMediaSourceHeader(const std::string& url, const std::map<std::string, std::string>& header) override;

    int32_t SetVideoSurfaceNew(void* native_window) override;

private:
    ArkWebRefPtr<ArkPlayerAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PLAYER_ADAPTER_WRAPPER_H

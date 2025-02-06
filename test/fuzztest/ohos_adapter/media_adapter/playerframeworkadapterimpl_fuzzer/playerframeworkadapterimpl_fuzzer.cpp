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

#include "playerframeworkadapterimpl_fuzzer.h"

#include "ohos_adapter_helper.h"
#include "player_framework_adapter_impl.h"
using namespace OHOS::NWeb;

namespace OHOS {
class PlayerCallbackTest : public PlayerCallbackAdapter {
public:
    PlayerCallbackTest() = default;
    ~PlayerCallbackTest() override = default;
    void OnInfo(PlayerOnInfoType type, int32_t extra, int32_t value) override
    {
        infoType_ = type;
    }
    void OnError(PlayerAdapterErrorType errorType) override
    {
        errorType_ = errorType;
    }
    PlayerOnInfoType infoType_ = PlayerOnInfoType::INFO_TYPE_UNSET;
    PlayerAdapterErrorType errorType_ = PlayerAdapterErrorType::INVALID_CODE;
};

bool PlayerFrameworkAdapterImpl_fuzzerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    int32_t currentTime = static_cast<int32_t>(size);
    int32_t duration = static_cast<int32_t>(size);

    std::shared_ptr<PlayerCallbackAdapter> callbackAdapter = nullptr;
    PlayerCallbackImpl playerCallback(callbackAdapter);

    playerCallback.OnInfo(Media::PlayerOnInfoType::INFO_TYPE_EOS, 0, Media::Format());
    playerCallback.OnError(0, "");
    PlayerAdapterImpl playerAdapter;
    playerAdapter.SetPlayerCallback(nullptr);
    playerAdapter.SetSource("");
    playerAdapter.SetSource(0, 0, 0);
    playerAdapter.SetVideoSurface(nullptr);
    playerAdapter.SetVolume(0.0, 0.0);
    playerAdapter.Seek(0, PlayerSeekMode::SEEK_PREVIOUS_SYNC);
    playerAdapter.Play();
    playerAdapter.Pause();
    playerAdapter.PrepareAsync();
    playerAdapter.GetCurrentTime(currentTime);
    playerAdapter.GetDuration(duration);
    playerAdapter.SetPlaybackSpeed(PlaybackRateMode::SPEED_FORWARD_1_00_X);
    std::map<std::string, std::string> header;
    playerAdapter.SetMediaSourceHeader("", header);

    auto callbackTest = std::make_unique<PlayerCallbackTest>();
    std::shared_ptr<PlayerCallbackImpl> playerCallbackMock =
        std::make_shared<PlayerCallbackImpl>(std::move(callbackTest));
    std::unique_ptr<PlayerAdapterImpl> playerAdapterNormal = std::make_unique<PlayerAdapterImpl>();
    ;
    playerAdapterNormal->SetPlayerCallback(std::move(callbackTest));
    auto surfaceAdapter = NWeb::OhosAdapterHelper::GetInstance().CreateConsumerSurfaceAdapter();
    playerAdapterNormal->SetVideoSurface(std::move(surfaceAdapter));
    playerAdapterNormal->SetSource("");
    playerAdapterNormal->SetSource(-1, 0, 0);
    playerAdapterNormal->SetVolume(0.0, 0.0);
    playerAdapterNormal->Seek(0, PlayerSeekMode::SEEK_PREVIOUS_SYNC);
    playerAdapterNormal->Play();
    playerAdapterNormal->Pause();
    playerAdapterNormal->PrepareAsync();
    playerAdapterNormal->GetCurrentTime(currentTime);
    playerAdapterNormal->GetDuration(duration);
    playerAdapter.SetPlaybackSpeed(PlaybackRateMode::SPEED_FORWARD_1_00_X);
    playerAdapter.SetMediaSourceHeader("", header);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::PlayerFrameworkAdapterImpl_fuzzerFuzzTest(data, size);
    return 0;
}
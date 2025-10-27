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

#include "mediaavsessionadapter_fuzzer.h"

#define private public
#include <cstdlib>
#include <ctime>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#include "media_avsession_adapter.h"
#include "media_avsession_adapter_impl.h"
#include "avcast_controller.h"

using namespace OHOS::NWeb;

namespace OHOS {
namespace AVSession {
class MediaAVSessionMetadataAdapterMock : public MediaAVSessionMetadataAdapter {
public:
    MediaAVSessionMetadataAdapterMock() = default;
    void SetTitle(const std::string& title) {}

    std::string GetTitle() { return ""; }

    void SetArtist(const std::string& artist) {}

    std::string GetArtist() { return ""; }

    void SetAlbum(const std::string& album) {}

    std::string GetAlbum() { return ""; }

    void SetImageUrl(const std::string& imageUrl) {}

    std::string GetImageUrl() { return ""; }
};

class MediaAVSessionPositionAdapterMock : public MediaAVSessionPositionAdapter {
public:
    MediaAVSessionPositionAdapterMock() = default;

    void SetDuration(int64_t duration) {}

    int64_t GetDuration() { return 0; }

    void SetElapsedTime(int64_t elapsedTime) {}

    int64_t GetElapsedTime() { return 0; }

    void SetUpdateTime(int64_t updateTime) {}

    int64_t GetUpdateTime() { return 0; }
};

class AVSessionMock : public AVSession::AVSession {
public:
    std::string GetSessionId() { return ""; }

    std::string GetSessionType() { return ""; }

    int32_t GetAVMetaData(AVMetaData& meta) { return 0; }

    int32_t SetAVMetaData(const AVMetaData& meta) { return 0; }

    int32_t SetAVCallMetaData(const AVCallMetaData& meta) { return 0; }

    int32_t SetAVCallState(const AVCallState& avCallState) { return 0; }

    int32_t GetAVPlaybackState(AVPlaybackState& state) { return 0; }

    int32_t SetAVPlaybackState(const AVPlaybackState& state) { return 0; }

    int32_t GetAVQueueItems(std::vector<AVQueueItem>& items) { return 0; }

    int32_t SetAVQueueItems(const std::vector<AVQueueItem>& items) { return 0; }

    int32_t GetAVQueueTitle(std::string& title) { return 0; }

    int32_t SetAVQueueTitle(const std::string& title) { return 0; }

    int32_t SetLaunchAbility(const AbilityRuntime::WantAgent::WantAgent& ability) { return 0; }

    int32_t GetExtras(AAFwk::WantParams& extras) { return 0; }

    int32_t SetExtras(const AAFwk::WantParams& extras) { return 0; }

    int32_t SendCustomData(const AAFwk::WantParams& data) { return 0; }

    std::shared_ptr<AVSessionController> GetController() { return nullptr; }

    int32_t RegisterCallback(const std::shared_ptr<AVSessionCallback>& callback) { return 0; }

    int32_t Activate() { return 0; }

    int32_t Deactivate() { return 0; }

    bool IsActive() { return true; }

    int32_t Destroy() { return 0; }

    int32_t AddSupportCommand(const int32_t cmd) { return 0; }

    int32_t DeleteSupportCommand(const int32_t cmd) { return 0; }

    int32_t SetSessionEvent(const std::string& event, const AAFwk::WantParams& args) { return 0; }

    int32_t UpdateAVQueueInfo(const AVQueueInfo& info) { return 0; }

    std::shared_ptr<AVCastController> GetAVCastController() { return nullptr; }

    int32_t ReleaseCast(bool continuePlay = false) { return 0; }

    int32_t StartCastDisplayListener() { return 0; }

    int32_t StopCastDisplayListener() { return 0; }

    int32_t GetAllCastDisplays(std::vector<CastDisplayInfo>& castDisplays) { return 0; }
};

bool MediaAvsessionAdapterFuzzTest_01(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<MediaAVSessionMetadataAdapterMock> metadataadapter =
        std::make_shared<MediaAVSessionMetadataAdapterMock>();
    std::shared_ptr<MediaAVSessionPositionAdapterMock> pointeradapter =
        std::make_shared<MediaAVSessionPositionAdapterMock>();
    std::shared_ptr<MediaAVSessionAdapterImpl> avSessionAdapter = std::make_shared<MediaAVSessionAdapterImpl>();
    std::shared_ptr<AVSession> avSessionMock = std::make_shared<AVSessionMock>();

    avSessionAdapter->avSessionKey_->Init();
    avSessionAdapter->avSession_ = avSessionMock;

    auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    avSessionAdapter->CreateAVSession(type);
    type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    avSessionAdapter->CreateAVSession(type);
    avSessionAdapter->IsActivated();
    avSessionAdapter->Activate();

    avSessionAdapter->SetMetadata(metadataadapter);

    auto rawValue = fdp.ConsumeIntegralInRange<int64_t>(0, 3);
    auto playState = static_cast<MediaAVSessionPlayState>(rawValue);
    avSessionAdapter->SetPlaybackState(playState);

    avSessionAdapter->SetPlaybackPosition(pointeradapter);
    avSessionAdapter->UpdateMetaDataCache(metadataadapter);
    avSessionAdapter->UpdateMetaDataCache(pointeradapter);

    avSessionAdapter->UpdatePlaybackStateCache(playState);
    avSessionAdapter->DeActivate();
    avSessionAdapter->DestroyAVSession();
    avSessionAdapter->DestroyAndEraseSession();
    return true;
}

bool MediaAvsessionAdapterFuzzTest_02(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<MediaAVSessionMetadataAdapterMock> metadataadapter =
        std::make_shared<MediaAVSessionMetadataAdapterMock>();
    std::shared_ptr<MediaAVSessionPositionAdapterMock> pointeradapter =
        std::make_shared<MediaAVSessionPositionAdapterMock>();
    std::shared_ptr<MediaAVSessionAdapterImpl> avSessionAdapter = std::make_shared<MediaAVSessionAdapterImpl>();

    avSessionAdapter->avSessionKey_->Init();

    auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
    avSessionAdapter->CreateAVSession(type);
    type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
    avSessionAdapter->CreateAVSession(type);
    avSessionAdapter->IsActivated();
    avSessionAdapter->Activate();

    avSessionAdapter->SetMetadata(metadataadapter);

    auto rawValue = fdp.ConsumeIntegralInRange<int64_t>(0, 3);
    auto playState = static_cast<MediaAVSessionPlayState>(rawValue);
    avSessionAdapter->SetPlaybackState(playState);

    avSessionAdapter->SetPlaybackPosition(pointeradapter);
    avSessionAdapter->UpdateMetaDataCache(metadataadapter);
    avSessionAdapter->UpdateMetaDataCache(pointeradapter);

    avSessionAdapter->UpdatePlaybackStateCache(playState);
    avSessionAdapter->DeActivate();
    avSessionAdapter->DestroyAVSession();
    avSessionAdapter->DestroyAndEraseSession();
    return true;
}

} // namespace AVSession
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AVSession::MediaAvsessionAdapterFuzzTest_01(data, size);
    OHOS::AVSession::MediaAvsessionAdapterFuzzTest_02(data, size);
    return 0;
}
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

#ifndef MEDIA_AVSESSION_ADAPTER_IMPL_H
#define MEDIA_AVSESSION_ADAPTER_IMPL_H

#include <unordered_map>

#include <multimedia/av_session/native_avmetadata.h>
#include <multimedia/av_session/native_avsession.h>
#include <multimedia/av_session/native_avsession_errors.h>

#include "media_avsession_adapter.h"


namespace OHOS::NWeb {

class MediaAVSessionKey {
public:
    MediaAVSessionKey() = default;
    ~MediaAVSessionKey() = default;

    void Init();
    int32_t GetPID();
    std::string &GetBundleName();
    std::string &GetAbilityName();
    void SetType(MediaAVSessionType type);
    MediaAVSessionType GetType();
    std::string ToString();

private:
    int32_t pid_;
    std::string bundleName_;
    std::string abilityName_;
    MediaAVSessionType type_;
};

class MediaAVSessionAdapterImpl : public MediaAVSessionAdapter {
public:
    MediaAVSessionAdapterImpl();
    ~MediaAVSessionAdapterImpl() override;

    bool CreateAVSession(MediaAVSessionType type) override;
    void DestroyAVSession() override;
    bool RegistCallback(std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter) override;
    bool IsActivated() override;
    bool Activate() override;
    void DeActivate() override;
    void SetMetadata(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata) override;
    void SetPlaybackState(MediaAVSessionPlayState state) override;
    void SetPlaybackPosition(const std::shared_ptr<MediaAVSessionPositionAdapter> position) override;

private:
    static AVSessionCallback_Result AVSessionOnCommandCallback(OH_AVSession *session,
        AVSession_ControlCommand command, void *userData);
    static AVSessionCallback_Result AVSessionOnSeekCallback(OH_AVSession *session,
        uint64_t seekTime, void *userData);

    AVMetadata_Result UpdateAVMetadata(void);
    bool UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionMetadataAdapter> metadata);
    bool UpdateMetaDataCache(const std::shared_ptr<MediaAVSessionPositionAdapter> position);
    bool UpdatePlaybackStateCache(MediaAVSessionPlayState state);
    bool UpdatePlaybackStateCache(const std::shared_ptr<MediaAVSessionPositionAdapter> position);
    void DestroyAndEraseSession();
    bool CreateNewSession(const MediaAVSessionType& type);

    std::shared_ptr<MediaAVSessionKey> avSessionKey_ = nullptr;
    AVSession_PlaybackState avPlaybackState_;
    OH_AVMetadataBuilder *builder_ = nullptr;
    OH_AVMetadata *avMetadata_ = nullptr;
    OH_AVSession *avSession_ = nullptr;
    bool isActived_ = false;

    std::shared_ptr<MediaAVSessionCallbackAdapter> callbackAdapter_;
    static std::unordered_map<std::string, MediaAVSessionAdapterImpl*> avSessionMap;
};
} // namespace OHOS::NWeb

#endif // MEDIA_AVSESSION_ADAPTER_IMPL_H
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

#ifndef ARK_MEDIA_ADAPTER_H
#define ARK_MEDIA_ADAPTER_H

#include "ark_graphic_adapter.h"
#include "include/ark_web_base_ref_counted.h"
#include "include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--web engine(source=client)--*/
class ArkPlayerCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--web engine()--*/
    virtual void OnInfo(int32_t type, int32_t extra, int32_t value) = 0;

    /*--web engine()--*/
    virtual void OnError(int32_t errorType) = 0;
};

/*--web engine(source=library)--*/
class ArkPlayerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--web engine()--*/
    ArkPlayerAdapter() = default;

    /*--web engine()--*/
    virtual ~ArkPlayerAdapter() = default;

    /*--web engine()--*/
    virtual int32_t SetPlayerCallback(ArkWebRefPtr<ArkPlayerCallbackAdapter> callbackAdapter) = 0;

    /*--web engine()--*/
    virtual int32_t SetSource(const ArkWebString& url) = 0;

    /*--web engine()--*/
    virtual int32_t SetSource(int32_t fd, int64_t offset, int64_t size) = 0;

    /*--web engine()--*/
    virtual int32_t SetVideoSurface(ArkWebRefPtr<ArkIConsumerSurfaceAdapter> cSurfaceAdapter) = 0;

    /*--web engine()--*/
    virtual int32_t SetVolume(float leftVolume, float rightVolume) = 0;

    /*--web engine()--*/
    virtual int32_t Seek(int32_t mSeconds, int32_t mode) = 0;

    /*--web engine()--*/
    virtual int32_t Play() = 0;

    /*--web engine()--*/
    virtual int32_t Pause() = 0;

    /*--web engine()--*/
    virtual int32_t PrepareAsync() = 0;

    /*--web engine()--*/
    virtual int32_t GetCurrentTime(int32_t& currentTime) = 0;

    /*--web engine()--*/
    virtual int32_t GetDuration(int32_t& duration) = 0;

    /*--web engine()--*/
    virtual int32_t SetPlaybackSpeed(int32_t mode) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_ADAPTER_H
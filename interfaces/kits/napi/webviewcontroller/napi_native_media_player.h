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

#ifndef NAPI_NATIVE_MEDIA_PLAYER_H
#define NAPI_NATIVE_MEDIA_PLAYER_H

#include "napi/native_api.h"

namespace OHOS::NWeb {

class NapiNativeMediaPlayerHandler {
public:
    NapiNativeMediaPlayerHandler() = default;
    ~NapiNativeMediaPlayerHandler() = default;

    static void Init(napi_env env, napi_value value);

    static napi_status DefineProperties(napi_env env, napi_value* value);

private:
    static napi_status ExportEnumPreload(napi_env env, napi_value* value);

    static napi_status ExportEnumMediaType(napi_env env, napi_value* value);

    static napi_status ExportEnumSourceType(napi_env env, napi_value* value);

    static napi_status ExportEnumMediaError(napi_env env, napi_value* value);

    static napi_status ExportEnumReadyState(napi_env env, napi_value* value);

    static napi_status ExportEnumSuspendType(napi_env env, napi_value* value);

    static napi_status ExportEnumNetworkState(napi_env env, napi_value* value);

    static napi_status ExportEnumPlaybackStatus(napi_env env, napi_value* value);

    static napi_value HandleStatusChanged(napi_env env, napi_callback_info info);

    static napi_value HandleVolumeChanged(napi_env env, napi_callback_info info);

    static napi_value HandleMutedChanged(napi_env env, napi_callback_info info);

    static napi_value HandlePlaybackRateChanged(napi_env env, napi_callback_info info);

    static napi_value HandleDurationChanged(napi_env env, napi_callback_info info);

    static napi_value HandleTimeUpdate(napi_env env, napi_callback_info info);

    static napi_value HandleBufferedEndTimeChanged(napi_env env, napi_callback_info info);

    static napi_value HandleEnded(napi_env env, napi_callback_info info);

    static napi_value HandleNetworkStateChanged(napi_env env, napi_callback_info info);

    static napi_value HandleReadyStateChanged(napi_env env, napi_callback_info info);

    static napi_value HandleFullScreenChanged(napi_env env, napi_callback_info info);

    static napi_value HandleSeeking(napi_env env, napi_callback_info info);

    static napi_value HandleSeekFinished(napi_env env, napi_callback_info info);

    static napi_value HandleError(napi_env env, napi_callback_info info);

    static napi_value HandleVideoSizeChanged(napi_env env, napi_callback_info info);
};

} // namespace OHOS::NWeb

#endif // NAPI_NATIVE_MEDIA_PLAYER_H

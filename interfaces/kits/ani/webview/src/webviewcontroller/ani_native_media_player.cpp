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

#include <array>
#include <iostream>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

#include "nweb_native_media_player.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_NATIVE_MEDIA_PLAYER_CLASS_NAME = "L@ohos/web/webview/webview/NativeMediaPlayer;";
}

static void EnterFullScreen(ani_env *env, ani_object object)
{
    WVLOG_I("[rooki] EnterFullScreen");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<NWebNativeMediaPlayerBridge *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->EnterFullScreen();
}

static void ExitFullscreen(ani_env *env, ani_object object)
{
    WVLOG_I("[rooki] ExitFullscreen");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<NWebNativeMediaPlayerBridge *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->ExitFullScreen();
}

ani_status StsNativeMediaPlayerInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webNativeMediaPlayerCls = nullptr;
    ani_status status = env->FindClass(WEB_NATIVE_MEDIA_PLAYER_CLASS_NAME, &webNativeMediaPlayerCls);
    if (status != ANI_OK || !webNativeMediaPlayerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_NATIVE_MEDIA_PLAYER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "enterFullScreen", nullptr, reinterpret_cast<void*>(EnterFullScreen) },
        ani_native_function { "exitFullscreen", nullptr, reinterpret_cast<void*>(ExitFullscreen) },
    };

    status = env->Class_BindNativeMethods(webNativeMediaPlayerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS


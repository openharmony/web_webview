/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include "ani_full_screen_video_overlay_handler.h"

#include <array>
#include <mutex>
#include <string>
#include <unordered_map>

#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "full_screen_video_overlay_handler_wrapper.h"

namespace OHOS {
namespace NWeb {

namespace {
const char* FULL_SCREEN_VIDEO_OVERLAY_HANDLER_INNER_CLASS_NAME =
    "@ohos.web.fullScreenVideoOverlay.FullScreenVideoOverlayHandlerInner";
const char* OVERLAY_CLEANER_CLASS_NAME = "@ohos.web.fullScreenVideoOverlay.OverlayCleaner";

std::mutex g_listenerRefsMutex;
std::unordered_map<FullScreenVideoOverlayHandlerWrapper*, ani_ref> g_listenerRefs;
}

static void SetVideoSurface(ani_env* env, ani_object object, ani_string surfaceId)
{
    WVLOG_D("SetVideoSurface is called");
    if (env == nullptr) {
        WVLOG_E("SetVideoSurface env is nullptr");
        return;
    }
    auto* wrapper = reinterpret_cast<FullScreenVideoOverlayHandlerWrapper*>(
        AniParseUtils::Unwrap(env, object));
    if (!wrapper) {
        WVLOG_E("FullScreenVideoOverlayHandlerWrapper is null");
        return;
    }
    std::string surfaceIdStr;
    if (!AniParseUtils::ParseString(env, surfaceId, surfaceIdStr)) {
        WVLOG_E("ParseString failed for surfaceId");
        return;
    }
    WVLOG_D("SetVideoSurface surfaceId: %{public}s", surfaceIdStr.c_str());
    wrapper->SetVideoSurface(surfaceIdStr);
}

static void RequestMediaControl(ani_env* env, ani_object object, ani_int action, ani_string param)
{
    WVLOG_D("RequestMediaControl is called");
    if (env == nullptr) {
        WVLOG_E("RequestMediaControl env is nullptr");
        return;
    }
    auto* wrapper = reinterpret_cast<FullScreenVideoOverlayHandlerWrapper*>(
        AniParseUtils::Unwrap(env, object));
    if (!wrapper) {
        WVLOG_E("FullScreenVideoOverlayHandlerWrapper is null");
        return;
    }
    int32_t actionInt = static_cast<int32_t>(action);
    std::string paramStr;
    if (!AniParseUtils::ParseString(env, param, paramStr)) {
        WVLOG_E("ParseString failed for param");
        return;
    }
    WVLOG_D("RequestMediaControl action: %{public}d", actionInt);
    wrapper->RequestMediaControl(actionInt, paramStr);
}

static ani_env* GetAniEnv(ani_vm* vm)
{
    ani_env* env = nullptr;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK || env == nullptr) {
        WVLOG_E("GetAniEnv failed");
        return nullptr;
    }
    return env;
}

template<typename... Args>
static void InvokeAniMethod(ani_vm* vm, ani_ref ref, ani_method method, Args... args)
{
    ani_env* envT = GetAniEnv(vm);
    if (!envT || !method) {
        return;
    }
    envT->Object_CallMethod_Void(static_cast<ani_object>(ref), method, args...);
}

static void AddListener(ani_env* env, ani_object object, ani_object listenerObj)
{
    WVLOG_D("AddListener is called");
    if (env == nullptr) {
        WVLOG_E("AddListener env is nullptr");
        return;
    }
    auto* wrapper = reinterpret_cast<FullScreenVideoOverlayHandlerWrapper*>(
        AniParseUtils::Unwrap(env, object));
    if (!wrapper) {
        WVLOG_E("FullScreenVideoOverlayHandlerWrapper is null");
        return;
    }
    if (listenerObj == nullptr) {
        WVLOG_E("listenerObj is null");
        return;
    }

    ani_ref listenerRef;
    if (env->GlobalReference_Create(static_cast<ani_ref>(listenerObj), &listenerRef) != ANI_OK) {
        WVLOG_E("GlobalReference_Create failed for listener");
        return;
    }

    ani_vm* vm = nullptr;
    if (env->GetVM(&vm) != ANI_OK || vm == nullptr) {
        WVLOG_E("GetVM failed");
        env->GlobalReference_Delete(listenerRef);
        return;
    }

    // Cache method references
    ani_type listenerType = nullptr;
    if (env->Object_GetType(listenerObj, &listenerType) != ANI_OK) {
        WVLOG_E("Object_GetType failed for listener");
        env->GlobalReference_Delete(listenerRef);
        return;
    }
    ani_class listenerCls = static_cast<ani_class>(listenerType);

    ani_method onStatusChangedMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onStatusChanged", "i:", &onStatusChangedMethod);
    ani_method onMutedChangedMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onMutedChanged", "z:", &onMutedChangedMethod);
    ani_method onPlaybackRateChangedMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onPlaybackRateChanged", "d:", &onPlaybackRateChangedMethod);
    ani_method onDurationChangedMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onDurationChanged", "d:", &onDurationChangedMethod);
    ani_method onTimeUpdateMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onTimeUpdate", "d:", &onTimeUpdateMethod);
    ani_method onBufferedEndTimeChangedMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onBufferedEndTimeChanged", "d:", &onBufferedEndTimeChangedMethod);
    ani_method onEndedMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onEnded", ":", &onEndedMethod);
    ani_method onVideoSizeChangedMethod = nullptr;
    env->Class_FindMethod(listenerCls, "onVideoSizeChanged", "ii:", &onVideoSizeChangedMethod);

    FullScreenVideoListenerCallbacks callbacks;

    callbacks.OnStatusChanged = [vm, ref = listenerRef, onStatusChangedMethod](int32_t status) {
        InvokeAniMethod(vm, ref, onStatusChangedMethod, static_cast<ani_int>(status));
    };

    callbacks.OnMutedChanged = [vm, ref = listenerRef, onMutedChangedMethod](bool muted) {
        InvokeAniMethod(vm, ref, onMutedChangedMethod, static_cast<ani_boolean>(muted));
    };

    callbacks.OnPlaybackRateChanged = [vm, ref = listenerRef, onPlaybackRateChangedMethod](double rate) {
        InvokeAniMethod(vm, ref, onPlaybackRateChangedMethod, static_cast<ani_double>(rate));
    };

    callbacks.OnDurationChanged = [vm, ref = listenerRef, onDurationChangedMethod](double duration) {
        InvokeAniMethod(vm, ref, onDurationChangedMethod, static_cast<ani_double>(duration));
    };

    callbacks.OnTimeUpdate = [vm, ref = listenerRef, onTimeUpdateMethod](double time) {
        InvokeAniMethod(vm, ref, onTimeUpdateMethod, static_cast<ani_double>(time));
    };

    callbacks.OnBufferedEndTimeChanged = [vm, ref = listenerRef, onBufferedEndTimeChangedMethod](double time) {
        InvokeAniMethod(vm, ref, onBufferedEndTimeChangedMethod, static_cast<ani_double>(time));
    };

    callbacks.OnEnded = [vm, ref = listenerRef, onEndedMethod]() {
        InvokeAniMethod(vm, ref, onEndedMethod);
    };

    callbacks.OnVideoSizeChanged = [vm, ref = listenerRef, onVideoSizeChangedMethod](int32_t w, int32_t h) {
        InvokeAniMethod(vm, ref, onVideoSizeChangedMethod, static_cast<ani_int>(w), static_cast<ani_int>(h));
    };

    wrapper->AddListener(std::move(callbacks));

    {
        std::lock_guard<std::mutex> lock(g_listenerRefsMutex);
        g_listenerRefs[wrapper] = listenerRef;
    }
}

static ani_boolean TransferFullScreenVideoOverlayHandlerToStaticInner(
    ani_env* env, ani_class aniClass, ani_object output, ani_object input)
{
    if (env == nullptr) {
        WVLOG_E("[TRANSFER] env is nullptr");
        return ANI_FALSE;
    }

    void* nativePtr = nullptr;
    if (!arkts_esvalue_unwrap(env, input, &nativePtr) || nativePtr == nullptr) {
        WVLOG_E("[TRANSFER] arkts_esvalue_unwrap failed");
        return ANI_FALSE;
    }

    auto* wrapper = reinterpret_cast<FullScreenVideoOverlayHandlerWrapper*>(nativePtr);
    wrapper->IncRefCount();
    if (!AniParseUtils::Wrap(env, output, FULL_SCREEN_VIDEO_OVERLAY_HANDLER_INNER_CLASS_NAME,
                             reinterpret_cast<ani_long>(wrapper))) {
        wrapper->DecRefCount();
        WVLOG_E("[TRANSFER] FullScreenVideoOverlayHandler wrap failed");
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

ani_status StsFullScreenVideoOverlayHandlerInnerInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }

    ani_class handlerCls = nullptr;
    auto status = env->FindClass(FULL_SCREEN_VIDEO_OVERLAY_HANDLER_INNER_CLASS_NAME, &handlerCls);
    if (status != ANI_OK || !handlerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d",
            FULL_SCREEN_VIDEO_OVERLAY_HANDLER_INNER_CLASS_NAME, status);
        return ANI_ERROR;
    }

    std::array instanceMethods = {
        ani_native_function { "setVideoSurface", nullptr, reinterpret_cast<void*>(SetVideoSurface) },
        ani_native_function { "requestMediaControl", nullptr, reinterpret_cast<void*>(RequestMediaControl) },
        ani_native_function { "addListener", nullptr, reinterpret_cast<void*>(AddListener) },
    };
    status = env->Class_BindNativeMethods(handlerCls, instanceMethods.data(), instanceMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return status;
    }

    std::array staticMethods = {
        ani_native_function { "transferFullScreenVideoOverlayHandlerToStaticInner", nullptr,
            reinterpret_cast<void*>(TransferFullScreenVideoOverlayHandlerToStaticInner) },
    };
    status = env->Class_BindStaticNativeMethods(handlerCls, staticMethods.data(), staticMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindStaticNativeMethods failed status: %{public}d", status);
    }
    return status;
}

static void OverlayClean(ani_env* env, ani_object object)
{
    ani_long ptr = 0;
    env->Object_GetFieldByName_Long(object, "ptr", &ptr);
    if (ptr == 0) {
        return;
    }
    auto* wrapper = reinterpret_cast<FullScreenVideoOverlayHandlerWrapper*>(ptr);
    if (!wrapper) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_listenerRefsMutex);
        auto it = g_listenerRefs.find(wrapper);
        if (it != g_listenerRefs.end()) {
            env->GlobalReference_Delete(it->second);
            g_listenerRefs.erase(it);
        }
    }
    if (wrapper->DecRefCount() <= 0) {
        delete wrapper;
    }
}

ani_status StsOverlayCleanerInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("StsOverlayCleanerInit env is nullptr");
        return ANI_ERROR;
    }

    ani_class cleanerCls = nullptr;
    ani_status status = env->FindClass(OVERLAY_CLEANER_CLASS_NAME, &cleanerCls);
    if (status != ANI_OK || !cleanerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", OVERLAY_CLEANER_CLASS_NAME, status);
        return status;
    }
    std::array cleanerMethods = {
        ani_native_function { "clean", nullptr, reinterpret_cast<void*>(OverlayClean) },
    };
    status = env->Class_BindNativeMethods(cleanerCls, cleanerMethods.data(), cleanerMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return status;
}

} // namespace NWeb
} // namespace OHOS

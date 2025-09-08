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

#include "ani_native_media_player_handler.h"
#include "napi_native_mediaplayer_handler_impl.h"

#include <array>
#include <iostream>
#include <string>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "native_media_player_impl.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "securec.h"
#include "web_errors.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"

namespace OHOS {
namespace NWeb {
const double MAX_VOLUME = 1.0;
const double MAX_PLAYBACK_RATE = 10.0;

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* NATIVE_MEDIA_PLAYER_HANDLER_INNER_CLASS_NAME = "L@ohos/web/webview/webview/NativeMediaPlayerHandlerinner;";
}

bool EnumParseInt32_t(ani_env* env, ani_enum_item enum_item, int32_t& outValue)
{
    ani_class doubleObject;
    if (env->FindClass("Lstd/core/Object;", &doubleObject) != ANI_OK) {
        WVLOG_E("EnumParseInt32 failed - invalid FindClass type");
        return false;
    }
    ani_boolean isObject;
    if (env->Object_InstanceOf(static_cast<ani_object>(enum_item), doubleObject, &isObject) != ANI_OK ||
        isObject != ANI_TRUE) {
        WVLOG_E("EnumParseInt32 failed - invalid int type");
        return false;
    }
    ani_int number = 0;
    if (env->EnumItem_GetValue_Int(enum_item, &number) != ANI_OK) {
        WVLOG_E("EnumParseInt32 failed");
        return false;
    }
    outValue = static_cast<int32_t>(number);
    return true;
}

bool ParseDouble_t(ani_env* env, ani_ref ref, double& outValue)
{
    ani_class doubleClass;
    if (env->FindClass("Lstd/core/Double;", &doubleClass) != ANI_OK) {
        WVLOG_E("ParseDouble failed - invalid FindClass type");
        return false;
    }
    ani_boolean isDouble;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), doubleClass, &isDouble) != ANI_OK ||
        isDouble != ANI_TRUE) {
        WVLOG_E("ParseDouble failed - invalid double type");
        return false;
    }

    ani_double value = 0;
    if (env->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "unboxed", ":d", &value) != ANI_OK) {
        WVLOG_E("ParseDouble failed");
        return false;
    }
    outValue = value;
    return true;
}

bool ParseBoolean_t(ani_env* env, ani_ref ref, bool& outValue)
{
    ani_class boolClass;
    if (env->FindClass("Lstd/core/Boolean;", &boolClass) != ANI_OK) {
        WVLOG_E("ParseBoolean failed - invalid FindClass type");
        return false;
    }
    ani_boolean isBool;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), boolClass, &isBool) != ANI_OK || isBool != ANI_TRUE) {
        WVLOG_E("ParseBoolean failed - invalid boolean type");
        return false;
    }

    ani_boolean boolValue = false;
    if (env->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "unboxed", ":z", &boolValue) != ANI_OK) {
        WVLOG_E("ParseBoolean failed");
        return false;
    }
    outValue = boolValue ? true : false;
    return true;
}

ani_object createObjectDouble(ani_env* env, ani_double status)
{
    static constexpr const char* className = "Lstd/core/Double;";
    ani_class doubleCls {};
    if (env->FindClass(className, &doubleCls) != ANI_OK) {
        WVLOG_E("createObjectDouble failed - invalid FindClass type");
        return nullptr;
    }
    ani_method ctor {};
    if (env->Class_FindMethod(doubleCls, "<ctor>", "d:", &ctor) != ANI_OK) {
        WVLOG_E("createObjectDouble findMethod Failed.");
        return nullptr;
    }
    ani_object obj {};
    if (env->Object_New(doubleCls, ctor, &obj, status) != ANI_OK) {
        WVLOG_E("createObjectDouble failed");
        return nullptr;
    }
    return obj;
}

ani_object createObjectBoolean(ani_env* env, ani_boolean status)
{
    static constexpr const char* className = "Lstd/core/Boolean;";
    ani_class booleanCls {};
    if (env->FindClass(className, &booleanCls) != ANI_OK) {
        WVLOG_E("createObjectBoolean failed - invalid FindClass type");
        return nullptr;
    }
    ani_method ctor {};
    if (env->Class_FindMethod(booleanCls, "<ctor>", "z:", &ctor) != ANI_OK) {
        WVLOG_E("createObjectBoolean findMethod Failed.");
        return nullptr;
    }
    ani_object obj {};
    if (env->Object_New(booleanCls, ctor, &obj, status) != ANI_OK) {
        WVLOG_E("createObjectBoolean failed");
        return nullptr;
    }
    return obj;
}

static void HandleStatusChanged(ani_env* env, ani_object object, ani_enum_item status)
{
    WVLOG_D("handle_status_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    int statusStr;
    if (!EnumParseInt32_t(env, status, statusStr)) {
        WVLOG_E("failed to parse status");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }

    if ((statusStr < static_cast<int>(PlaybackStatus::PAUSED)) ||
        (statusStr > static_cast<int>(PlaybackStatus::PLAYING))) {
        WVLOG_E("status is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }

    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_status_changed is called, status is %{public}d", statusStr);
    handler->HandleStatusChanged(static_cast<PlaybackStatus>(statusStr));
}

static void HandleVolumeChanged(ani_env* env, ani_object object, ani_double volume)
{
    WVLOG_D("handle_volume_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref volumeRef = static_cast<ani_ref>(createObjectDouble(env, volume));
    if (volumeRef == nullptr) {
        WVLOG_E("failed to create volume object");
        return;
    }
    double volumeStr;
    if (!ParseDouble_t(env, volumeRef, volumeStr)) {
        WVLOG_E("failed to parse volume");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if ((volumeStr < 0.0) || (volumeStr > MAX_VOLUME)) {
        WVLOG_E("volume is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_volume_changed is called, volume is %{public}f", volumeStr);
    handler->HandleVolumeChanged(volumeStr);
}

static void HandleMutedChanged(ani_env* env, ani_object object, ani_boolean muted)
{
    WVLOG_D("handle_muted_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref mutedRef = static_cast<ani_ref>(createObjectBoolean(env, muted));
    if (mutedRef == nullptr) {
        WVLOG_E("failed to create muted object");
        return;
    }
    bool mutedStr;
    if (!ParseBoolean_t(env, mutedRef, mutedStr)) {
        WVLOG_E("failed to parse muted");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_muted_changed is called, muted is %{public}d", mutedStr);
    handler->HandleMutedChanged(mutedStr);
}

static void HandlePlaybackRateChanged(ani_env* env, ani_object object, ani_double playbackRate)
{
    WVLOG_D("handle_playback_rate_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref playbackRateRef = static_cast<ani_ref>(createObjectDouble(env, playbackRate));
    if (playbackRateRef == nullptr) {
        WVLOG_E("failed to create playbackRate object");
        return;
    }
    double playbackRateStr;
    if (!ParseDouble_t(env, playbackRateRef, playbackRateStr)) {
        WVLOG_E("failed to parse rate");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if ((playbackRateStr < 0.0) || (playbackRateStr > MAX_PLAYBACK_RATE)) {
        WVLOG_E("rate is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_playback_rate_changed is called, playbackRate is %{public}f", playbackRateStr);
    handler->HandlePlaybackRateChanged(playbackRateStr);
}

static void HandleDurationChanged(ani_env* env, ani_object object, ani_double duration)
{
    WVLOG_D("handle_duration_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref durationRef = static_cast<ani_ref>(createObjectDouble(env, duration));
    if (durationRef == nullptr) {
        WVLOG_E("failed to create duration object");
        return;
    }
    double durationStr;
    if (!ParseDouble_t(env, durationRef, durationStr)) {
        WVLOG_E("failed to parse duration");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if (durationStr < 0.0) {
        WVLOG_E("duration is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_duration_changed is called, duration is %{public}f", durationStr);
    handler->HandleDurationChanged(durationStr);
}

static void HandleTimeUpdate(ani_env* env, ani_object object, ani_double currentPlayTime)
{
    WVLOG_D("handle_time_update is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref currentPlayTimeRef = static_cast<ani_ref>(createObjectDouble(env, currentPlayTime));
    if (currentPlayTimeRef == nullptr) {
        WVLOG_E("failed to create currentPlayTime object");
        return;
    }
    double currentPlayTimeStr;
    if (!ParseDouble_t(env, currentPlayTimeRef, currentPlayTimeStr)) {
        WVLOG_E("failed to parse time");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if (currentPlayTimeStr < 0.0) {
        WVLOG_E("time is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_time_update is called, currentPlayTime is %{public}f", currentPlayTimeStr);
    handler->HandleTimeUpdate(currentPlayTimeStr);
}

static void HandleBufferedEndTimeChanged(ani_env* env, ani_object object, ani_double bufferedEndTime)
{
    WVLOG_D("handle_buffered_end_time_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref bufferedEndTimeRef = static_cast<ani_ref>(createObjectDouble(env, bufferedEndTime));
    if (bufferedEndTimeRef == nullptr) {
        WVLOG_E("failed to create bufferedEndTime object");
        return;
    }
    double bufferedEndTimeStr;
    if (!ParseDouble_t(env, bufferedEndTimeRef, bufferedEndTimeStr)) {
        WVLOG_E("failed to parse time");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if (bufferedEndTimeStr < 0.0) {
        WVLOG_E("time is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_buffered_end_time_changed is called, bufferedEndTime is %{public}f", bufferedEndTimeStr);
    handler->HandleBufferedEndTimeChanged(bufferedEndTimeStr);
}

static void HandleEnded(ani_env* env, ani_object object)
{
    WVLOG_D("handle_ended is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    handler->HandleEnded();
}

static void HandleNetworkStateChanged(ani_env* env, ani_object object, ani_enum_item state)
{
    WVLOG_D("handle_network_state_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    int stateStr;
    if (!EnumParseInt32_t(env, state, stateStr)) {
        WVLOG_E("failed to parse state");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if ((stateStr < static_cast<int>(NetworkState::EMPTY)) ||
        (stateStr > static_cast<int>(NetworkState::NETWORK_ERROR))) {
        WVLOG_E("state is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_network_state_changed is called, state is %{public}d", stateStr);
    handler->HandleNetworkStateChanged(static_cast<NetworkState>(stateStr));
}

static void HandleReadyStateChanged(ani_env* env, ani_object object, ani_enum_item state)
{
    WVLOG_D("handle_ready_state_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    int stateStr;
    if (!EnumParseInt32_t(env, state, stateStr)) {
        WVLOG_E("failed to parse state");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if ((stateStr < static_cast<int>(ReadyState::HAVE_NOTHING)) ||
        (stateStr > static_cast<int>(ReadyState::HAVE_ENOUGH_DATA))) {
        WVLOG_E("state is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_ready_state_changed is called, state is %{public}d", stateStr);
    handler->HandleReadyStateChanged(static_cast<ReadyState>(stateStr));
}

static void HandleFullScreenChanged(ani_env* env, ani_object object, ani_boolean fullscreen)
{
    WVLOG_D("handle_full_screen_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref fullscreenRef = static_cast<ani_ref>(createObjectBoolean(env, fullscreen));
    if (fullscreenRef == nullptr) {
        WVLOG_E("failed to create fullscreen object");
        return;
    }
    bool fullscreenStr;
    if (!ParseBoolean_t(env, fullscreenRef, fullscreenStr)) {
        WVLOG_E("failed to parse flag");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_full_screen_changed is called, fullscreen is %{public}d", fullscreenStr);
    handler->HandleFullScreenChanged(fullscreenStr);
}

static void HandleSeeking(ani_env* env, ani_object object)
{
    WVLOG_D("handle_seeking is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    handler->HandleSeeking();
}

static void HandleSeekFinished(ani_env* env, ani_object object)
{
    WVLOG_D("handle_seek_finished is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    handler->HandleSeekFinished();
}

static void HandleError(ani_env* env, ani_object object, ani_enum_item error, ani_string errorMessage)
{
    WVLOG_D("handle_error is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    int errorStr;
    if (!EnumParseInt32_t(env, error, errorStr)) {
        WVLOG_E("failed to parse error");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    if ((errorStr < static_cast<int>(MediaError::NETWORK_ERROR)) ||
        (errorStr > static_cast<int>(MediaError::DECODE_ERROR))) {
        WVLOG_E("error is invalid");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }

    std::string errorMessageStr;
    if (!AniParseUtils::ParseString(env, static_cast<ani_ref>(errorMessage), errorMessageStr)) {
        WVLOG_E("failed to parse message");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D(
        "handle_error is called, error is %{public}d, errorMessage is %{public}s", errorStr, errorMessageStr.c_str());
    handler->HandleError(static_cast<MediaError>(errorStr), errorMessageStr);
}

static void HandleVideoSizeChanged(ani_env* env, ani_object object, ani_double width, ani_double height)
{
    WVLOG_D("handle_video_size_changed is called");

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref widthRef = static_cast<ani_ref>(createObjectDouble(env, width));
    if (widthRef == nullptr) {
        WVLOG_E("failed to create width object");
        return;
    }
    double widthStr;
    if (!ParseDouble_t(env, widthRef, widthStr)) {
        WVLOG_E("failed to parse width");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    ani_ref heightRef = static_cast<ani_ref>(createObjectDouble(env, height));
    if (heightRef == nullptr) {
        WVLOG_E("failed to create height object");
        return;
    }
    double heightStr;
    if (!ParseDouble_t(env, heightRef, heightStr)) {
        WVLOG_E("failed to parse height");
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    NapiNativeMediaPlayerHandlerImpl* handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(AniParseUtils::Unwrap(env, object));
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return;
    }
    WVLOG_D("handle_video_size_changed is called, width is %{public}f, height is %{public}f",
        widthStr, heightStr);
    handler->HandleVideoSizeChanged(widthStr, heightStr);
}

static ani_boolean TransferNativeMediaPlayerHandlerToStaticInner(ani_env* env, ani_class aniClass,
    ani_object output, ani_object input)
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

    NapiNativeMediaPlayerHandlerImpl *handler =
        reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(nativePtr);
    if (!AniParseUtils::Wrap(env, output, NATIVE_MEDIA_PLAYER_HANDLER_INNER_CLASS_NAME,
                             reinterpret_cast<ani_long>(handler))) {
        WVLOG_E("[TRANSFER] NapiNativeMediaPlayerHandlerImpl wrap failed");
        return ANI_FALSE;
    }

    handler->IncRefCount();
    return ANI_TRUE;
}

ani_status StsNativeMediaPlayerHandlerinnerInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }

    ani_class nativeMediaPlayerHandlerinnerCls = nullptr;
    auto status = env->FindClass(NATIVE_MEDIA_PLAYER_HANDLER_INNER_CLASS_NAME, &nativeMediaPlayerHandlerinnerCls);
    if (status != ANI_OK || !nativeMediaPlayerHandlerinnerCls) {
        WVLOG_E(
            "find %{public}s class failed, status: %{public}d", NATIVE_MEDIA_PLAYER_HANDLER_INNER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "handleStatusChanged", nullptr, reinterpret_cast<void*>(HandleStatusChanged) },
        ani_native_function { "handleVolumeChanged", nullptr, reinterpret_cast<void*>(HandleVolumeChanged) },
        ani_native_function { "handleMutedChanged", nullptr, reinterpret_cast<void*>(HandleMutedChanged) },
        ani_native_function {
            "handlePlaybackRateChanged", nullptr, reinterpret_cast<void*>(HandlePlaybackRateChanged) },
        ani_native_function { "handleDurationChanged", nullptr, reinterpret_cast<void*>(HandleDurationChanged) },
        ani_native_function { "handleTimeUpdate", nullptr, reinterpret_cast<void*>(HandleTimeUpdate) },
        ani_native_function {
            "handleBufferedEndTimeChanged", nullptr, reinterpret_cast<void*>(HandleBufferedEndTimeChanged) },
        ani_native_function { "handleEnded", nullptr, reinterpret_cast<void*>(HandleEnded) },
        ani_native_function {
            "handleNetworkStateChanged", nullptr, reinterpret_cast<void*>(HandleNetworkStateChanged) },
        ani_native_function { "handleReadyStateChanged", nullptr, reinterpret_cast<void*>(HandleReadyStateChanged) },
        ani_native_function { "handleFullscreenChanged", nullptr, reinterpret_cast<void*>(HandleFullScreenChanged) },
        ani_native_function { "handleSeeking", nullptr, reinterpret_cast<void*>(HandleSeeking) },
        ani_native_function { "handleSeekFinished", nullptr, reinterpret_cast<void*>(HandleSeekFinished) },
        ani_native_function { "handleError", nullptr, reinterpret_cast<void*>(HandleError) },
        ani_native_function { "handleVideoSizeChanged", nullptr, reinterpret_cast<void*>(HandleVideoSizeChanged) },
        ani_native_function { "transferNativeMediaPlayerHandlerToStaticInner", nullptr,
            reinterpret_cast<void*>(TransferNativeMediaPlayerHandlerToStaticInner) },
    };

    status = env->Class_BindNativeMethods(nativeMediaPlayerHandlerinnerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS

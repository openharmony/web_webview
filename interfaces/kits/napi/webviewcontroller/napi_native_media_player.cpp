/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"){}
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

#include "napi_native_media_player.h"

#include "business_error.h"
#include "napi_parse_utils.h"
#include "native_media_player_impl.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS::NWeb {

const double MAX_VOLUME = 1.0;
const double MAX_PLAYBACK_RATE = 10.0;

void NapiNativeMediaPlayerHandler::Init(napi_env env, napi_value value)
{
    WVLOG_I("begin to init native media player napi properties");

    napi_property_descriptor transferDesc[] = {
        DECLARE_NAPI_FUNCTION("__createNativeMediaPlayerHandlerTransfer__",
            MediaPlayerTransfer::CreateNativeMediaPlayerHandlerTransfer),
    };

    napi_define_properties(env, value, sizeof(transferDesc) / sizeof(transferDesc[0]), transferDesc);

    NAPI_CALL_RETURN_VOID(env, ExportEnumPreload(env, &value));

    NAPI_CALL_RETURN_VOID(env, ExportEnumMediaType(env, &value));

    NAPI_CALL_RETURN_VOID(env, ExportEnumSourceType(env, &value));

    NAPI_CALL_RETURN_VOID(env, ExportEnumMediaError(env, &value));

    NAPI_CALL_RETURN_VOID(env, ExportEnumReadyState(env, &value));

    NAPI_CALL_RETURN_VOID(env, ExportEnumSuspendType(env, &value));

    NAPI_CALL_RETURN_VOID(env, ExportEnumNetworkState(env, &value));

    NAPI_CALL_RETURN_VOID(env, ExportEnumPlaybackStatus(env, &value));
}

napi_status NapiNativeMediaPlayerHandler::DefineProperties(napi_env env, napi_value* value)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("handleStatusChanged", NapiNativeMediaPlayerHandler::HandleStatusChanged),
        DECLARE_NAPI_FUNCTION("handleVolumeChanged", NapiNativeMediaPlayerHandler::HandleVolumeChanged),
        DECLARE_NAPI_FUNCTION("handleMutedChanged", NapiNativeMediaPlayerHandler::HandleMutedChanged),
        DECLARE_NAPI_FUNCTION("handlePlaybackRateChanged", NapiNativeMediaPlayerHandler::HandlePlaybackRateChanged),
        DECLARE_NAPI_FUNCTION("handleDurationChanged", NapiNativeMediaPlayerHandler::HandleDurationChanged),
        DECLARE_NAPI_FUNCTION("handleTimeUpdate", NapiNativeMediaPlayerHandler::HandleTimeUpdate),
        DECLARE_NAPI_FUNCTION(
            "handleBufferedEndTimeChanged", NapiNativeMediaPlayerHandler::HandleBufferedEndTimeChanged),
        DECLARE_NAPI_FUNCTION("handleEnded", NapiNativeMediaPlayerHandler::HandleEnded),
        DECLARE_NAPI_FUNCTION("handleNetworkStateChanged", NapiNativeMediaPlayerHandler::HandleNetworkStateChanged),
        DECLARE_NAPI_FUNCTION("handleReadyStateChanged", NapiNativeMediaPlayerHandler::HandleReadyStateChanged),
        DECLARE_NAPI_FUNCTION("handleFullscreenChanged", NapiNativeMediaPlayerHandler::HandleFullScreenChanged),
        DECLARE_NAPI_FUNCTION("handleSeeking", NapiNativeMediaPlayerHandler::HandleSeeking),
        DECLARE_NAPI_FUNCTION("handleSeekFinished", NapiNativeMediaPlayerHandler::HandleSeekFinished),
        DECLARE_NAPI_FUNCTION("handleError", NapiNativeMediaPlayerHandler::HandleError),
        DECLARE_NAPI_FUNCTION("handleVideoSizeChanged", NapiNativeMediaPlayerHandler::HandleVideoSizeChanged),
    };

    return napi_define_properties(env, *value, sizeof(properties) / sizeof(properties[0]), properties);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumPreload(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum preload");

    const std::string NPI_PRELOAD_ENUM_NAME = "Preload";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NONE", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(Preload::NONE))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "METADATA", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(Preload::METADATA))),
        DECLARE_NAPI_STATIC_PROPERTY("AUTO", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(Preload::AUTO))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_PRELOAD_ENUM_NAME.c_str(), NPI_PRELOAD_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_PRELOAD_ENUM_NAME.c_str(), enumValue);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumMediaType(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum media type");

    const std::string NPI_MEDIA_TYPE_ENUM_NAME = "MediaType";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "VIDEO", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(MediaType::VIDEO))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "AUDIO", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(MediaType::AUDIO))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_MEDIA_TYPE_ENUM_NAME.c_str(), NPI_MEDIA_TYPE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_MEDIA_TYPE_ENUM_NAME.c_str(), enumValue);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumSourceType(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum source type");

    const std::string NPI_SOURCE_TYPE_ENUM_NAME = "SourceType";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("URL", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(SourceType::URL))),
        DECLARE_NAPI_STATIC_PROPERTY("MSE", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(SourceType::MSE))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_SOURCE_TYPE_ENUM_NAME.c_str(), NPI_SOURCE_TYPE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_SOURCE_TYPE_ENUM_NAME.c_str(), enumValue);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumMediaError(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum media error");

    const std::string NPI_MEDIA_ERROR_ENUM_NAME = "MediaError";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "NETWORK_ERROR", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(MediaError::NETWORK_ERROR))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "FORMAT_ERROR", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(MediaError::FORMAT_ERROR))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "DECODE_ERROR", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(MediaError::DECODE_ERROR))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_MEDIA_ERROR_ENUM_NAME.c_str(), NPI_MEDIA_ERROR_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_MEDIA_ERROR_ENUM_NAME.c_str(), enumValue);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumReadyState(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum ready state");

    const std::string NPI_READY_STATE_ENUM_NAME = "ReadyState";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "HAVE_NOTHING", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(ReadyState::HAVE_NOTHING))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "HAVE_METADATA", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(ReadyState::HAVE_METADATA))),
        DECLARE_NAPI_STATIC_PROPERTY("HAVE_CURRENT_DATA",
            NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(ReadyState::HAVE_CURRENT_DATA))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "HAVE_FUTURE_DATA", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(ReadyState::HAVE_FUTURE_DATA))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "HAVE_ENOUGH_DATA", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(ReadyState::HAVE_ENOUGH_DATA))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_READY_STATE_ENUM_NAME.c_str(), NPI_READY_STATE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_READY_STATE_ENUM_NAME.c_str(), enumValue);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumSuspendType(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum suspend type");

    const std::string NPI_SUSPEND_TYPE_ENUM_NAME = "SuspendType";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("ENTER_BACK_FORWARD_CACHE",
            NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(SuspendType::ENTER_BACK_FORWARD_CACHE))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "ENTER_BACKGROUND", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(SuspendType::ENTER_BACKGROUND))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "AUTO_CLEANUP", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(SuspendType::AUTO_CLEANUP))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_SUSPEND_TYPE_ENUM_NAME.c_str(), NPI_SUSPEND_TYPE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_SUSPEND_TYPE_ENUM_NAME.c_str(), enumValue);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumNetworkState(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum network state");

    const std::string NPI_NETWORK_STATE_ENUM_NAME = "NetworkState";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "EMPTY", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(NetworkState::EMPTY))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "IDLE", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(NetworkState::IDLE))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "LOADING", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(NetworkState::LOADING))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NETWORK_ERROR", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(NetworkState::NETWORK_ERROR))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_NETWORK_STATE_ENUM_NAME.c_str(), NPI_NETWORK_STATE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_NETWORK_STATE_ENUM_NAME.c_str(), enumValue);
}

napi_status NapiNativeMediaPlayerHandler::ExportEnumPlaybackStatus(napi_env env, napi_value* value)
{
    WVLOG_D("begin to export enum playback status");

    const std::string NPI_PLAYBACK_STATUS_ENUM_NAME = "PlaybackStatus";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "PAUSED", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(PlaybackStatus::PAUSED))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "PLAYING", NapiParseUtils::ToInt32Value(env, static_cast<int32_t>(PlaybackStatus::PLAYING))),
    };

    napi_value enumValue = nullptr;
    napi_define_class(env, NPI_PLAYBACK_STATUS_ENUM_NAME.c_str(), NPI_PLAYBACK_STATUS_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties,
        &enumValue);
    return napi_set_named_property(env, *value, NPI_PLAYBACK_STATUS_ENUM_NAME.c_str(), enumValue);
}

napi_value NapiNativeMediaPlayerHandler::HandleStatusChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_status_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    int status = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], status)) {
        WVLOG_E("failed to parse status");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if ((status < static_cast<int>(PlaybackStatus::PAUSED)) || (status > static_cast<int>(PlaybackStatus::PLAYING))) {
        WVLOG_E("status is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_status_changed status is %{public}d", status);
    handler->HandleStatusChanged(static_cast<PlaybackStatus>(status));
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleVolumeChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_volume_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    double volume = 0.0;
    if (!NapiParseUtils::ParseDouble(env, argv[INTEGER_ZERO], volume)) {
        WVLOG_E("failed to parse volume");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if ((volume < 0.0) || (volume > MAX_VOLUME)) {
        WVLOG_E("volume is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_volume_changed volume  is %{public}f", volume);
    handler->HandleVolumeChanged(volume);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleMutedChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_muted_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    bool flag = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[INTEGER_ZERO], flag)) {
        WVLOG_E("failed to parse flag");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_muted_changed flag  is %{public}d", flag);
    handler->HandleMutedChanged(flag);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandlePlaybackRateChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_playback_rate_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    double rate = 0.0;
    if (!NapiParseUtils::ParseDouble(env, argv[INTEGER_ZERO], rate)) {
        WVLOG_E("failed to parse rate");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if ((rate < 0.0) || (rate > MAX_PLAYBACK_RATE)) {
        WVLOG_E("rate is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_playback_rate_changed rate is %{public}f", rate);
    handler->HandlePlaybackRateChanged(rate);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleDurationChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_duration_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    double duration = 0.0;
    if (!NapiParseUtils::ParseDouble(env, argv[INTEGER_ZERO], duration)) {
        WVLOG_E("failed to parse duration");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if (duration < 0.0) {
        WVLOG_E("duration is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_duration_changed  is %{public}f", duration);
    handler->HandleDurationChanged(duration);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleTimeUpdate(napi_env env, napi_callback_info info)
{
    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    double time = 0.0;
    if (!NapiParseUtils::ParseDouble(env, argv[INTEGER_ZERO], time)) {
        WVLOG_E("failed to parse time");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if (time < 0.0) {
        WVLOG_E("time is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("HandleTimeUpdate  time is %{public}f", time);
    handler->HandleTimeUpdate(time);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleBufferedEndTimeChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_buffered_end_time_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    double time = 0.0;
    if (!NapiParseUtils::ParseDouble(env, argv[INTEGER_ZERO], time)) {
        WVLOG_E("failed to parse time");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if (time < 0.0) {
        WVLOG_E("time is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_buffered_end_time_changed time is %{public}f", time);
    handler->HandleBufferedEndTimeChanged(time);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleEnded(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_ended is called");

    napi_value value = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &value, nullptr));

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    handler->HandleEnded();
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleNetworkStateChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_network_state_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    int state = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], state)) {
        WVLOG_E("failed to parse state");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if ((state < static_cast<int>(NetworkState::EMPTY)) || (state > static_cast<int>(NetworkState::NETWORK_ERROR))) {
        WVLOG_E("state is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_network_state_changed state is %{public}d", state);
    handler->HandleNetworkStateChanged(static_cast<NetworkState>(state));
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleReadyStateChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_ready_state_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    int state = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], state)) {
        WVLOG_E("failed to parse state");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if ((state < static_cast<int>(ReadyState::HAVE_NOTHING)) ||
        (state > static_cast<int>(ReadyState::HAVE_ENOUGH_DATA))) {
        WVLOG_E("state is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_ready_state_changed state is %{public}d", state);
    handler->HandleReadyStateChanged(static_cast<ReadyState>(state));
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleFullScreenChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_full_screen_changed is called");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    bool flag = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[INTEGER_ZERO], flag)) {
        WVLOG_E("failed to parse flag");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("handle_full_screen_changed flag is %{public}d", flag);
    handler->HandleFullScreenChanged(flag);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleSeeking(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_seeking is called");

    napi_value value = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &value, nullptr));

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    handler->HandleSeeking();
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleSeekFinished(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_seek_finished is called");

    napi_value value = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &value, nullptr));

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    handler->HandleSeekFinished();
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleError(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_error is called");

    size_t argc = INTEGER_TWO;
    napi_value value = nullptr;
    napi_value argv[INTEGER_TWO];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_TWO) {
        WVLOG_E("arg count %{public}zu is not equal to 2", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    int error = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], error)) {
        WVLOG_E("failed to parse error");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if ((error < static_cast<int>(MediaError::NETWORK_ERROR)) || (error > static_cast<int>(MediaError::DECODE_ERROR))) {
        WVLOG_E("error is invalid");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    std::string message;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ONE], message)) {
        WVLOG_E("failed to parse message");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    handler->HandleError(static_cast<MediaError>(error), message);
    return nullptr;
}

napi_value NapiNativeMediaPlayerHandler::HandleVideoSizeChanged(napi_env env, napi_callback_info info)
{
    WVLOG_D("handle_video_size_changed is called");

    size_t argc = INTEGER_TWO;
    napi_value value = nullptr;
    napi_value argv[INTEGER_TWO];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_TWO) {
        WVLOG_E("arg count %{public}zu is not equal to 2", argc);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    double width = 0.0;
    if (!NapiParseUtils::ParseDouble(env, argv[INTEGER_ZERO], width)) {
        WVLOG_E("failed to parse width");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    double height = 0.0;
    if (!NapiParseUtils::ParseDouble(env, argv[INTEGER_ONE], height)) {
        WVLOG_E("failed to parse height");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* handler = nullptr;
    napi_unwrap(env, value, (void**)&handler);
    if (!handler) {
        WVLOG_E("native media player handler is null");
        return nullptr;
    }

    WVLOG_D("HandleVideoSizeChanged width is %{public}f, heigh is %{public}f",
        width, height);
    handler->HandleVideoSizeChanged(width, height);
    return nullptr;
}

static napi_value CreateNativeMediaPlayerHandler(napi_env env, NapiNativeMediaPlayerHandlerImpl* handler)
{
    WVLOG_I("CreateNativeMedisPlayerHandler enter");
    napi_value jsValue = nullptr;
    napi_create_object(env, &jsValue);

    NAPI_CALL(env, napi_wrap(env, jsValue, handler,
        [](napi_env env, void *data, void *hint) {
            NapiNativeMediaPlayerHandlerImpl *handler = static_cast<NapiNativeMediaPlayerHandlerImpl *>(data);
            delete handler;
        },
        nullptr, nullptr));
    handler->IncRefCount();

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("handleStatusChanged", NapiNativeMediaPlayerHandler::HandleStatusChanged),
        DECLARE_NAPI_FUNCTION("handleVolumeChanged", NapiNativeMediaPlayerHandler::HandleVolumeChanged),
        DECLARE_NAPI_FUNCTION("handleMutedChanged", NapiNativeMediaPlayerHandler::HandleMutedChanged),
        DECLARE_NAPI_FUNCTION("handlePlaybackRateChanged", NapiNativeMediaPlayerHandler::HandlePlaybackRateChanged),
        DECLARE_NAPI_FUNCTION("handleDurationChanged", NapiNativeMediaPlayerHandler::HandleDurationChanged),
        DECLARE_NAPI_FUNCTION("handleTimeUpdate", NapiNativeMediaPlayerHandler::HandleTimeUpdate),
        DECLARE_NAPI_FUNCTION(
            "handleBufferedEndTimeChanged", NapiNativeMediaPlayerHandler::HandleBufferedEndTimeChanged),
        DECLARE_NAPI_FUNCTION("handleEnded", NapiNativeMediaPlayerHandler::HandleEnded),
        DECLARE_NAPI_FUNCTION("handleNetworkStateChanged", NapiNativeMediaPlayerHandler::HandleNetworkStateChanged),
        DECLARE_NAPI_FUNCTION("handleReadyStateChanged", NapiNativeMediaPlayerHandler::HandleReadyStateChanged),
        DECLARE_NAPI_FUNCTION("handleFullscreenChanged", NapiNativeMediaPlayerHandler::HandleFullScreenChanged),
        DECLARE_NAPI_FUNCTION("handleSeeking", NapiNativeMediaPlayerHandler::HandleSeeking),
        DECLARE_NAPI_FUNCTION("handleSeekFinished", NapiNativeMediaPlayerHandler::HandleSeekFinished),
        DECLARE_NAPI_FUNCTION("handleError", NapiNativeMediaPlayerHandler::HandleError),
        DECLARE_NAPI_FUNCTION("handleVideoSizeChanged", NapiNativeMediaPlayerHandler::HandleVideoSizeChanged),
    };

    NAPI_CALL(env, napi_define_properties(env, jsValue, sizeof(properties) / sizeof(properties[0]), properties));
    return jsValue;
}

napi_value MediaPlayerTransfer::CreateNativeMediaPlayerHandlerTransfer(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result;
    napi_get_undefined(env, &result);
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("[CreateNativeMediaPlayerHandlerTransfer] number of params is invalid");
        return result;
    }

    int64_t addr = 0;
    if (!NapiParseUtils::ParseInt64(env, argv[INTEGER_ZERO], addr)) {
        WVLOG_E("[CreateNativeMediaPlayerHandlerTransfer] type of param is error");
        return result;
    }

    NapiNativeMediaPlayerHandlerImpl* handler =  reinterpret_cast<NapiNativeMediaPlayerHandlerImpl*>(addr);
    napi_value jsValue = CreateNativeMediaPlayerHandler(env, handler);
    if (jsValue) {
        return jsValue;
    }
    return result;
}
} // namespace OHOS::NWeb

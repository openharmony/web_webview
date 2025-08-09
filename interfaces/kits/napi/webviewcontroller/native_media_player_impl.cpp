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

#include "native_media_player_impl.h"

#include "napi_common_macros.h"
#include "napi_native_media_player.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

NWebNativeMediaPlayerBridgeImpl::NWebNativeMediaPlayerBridgeImpl(int32_t nwebId, napi_env env, napi_value value)
    : nwebId_(nwebId), env_(env), value_(value)
{}

void NWebNativeMediaPlayerBridgeImpl::UpdateRect(double x, double y, double width, double height)
{
    WVLOG_D("begin to update rect,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "updateRect", callback);

    napi_value argv[INTEGER_FOUR] = { nullptr };
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, x, &argv[INTEGER_ZERO]));
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, y, &argv[INTEGER_ONE]));
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, width, &argv[INTEGER_TWO]));
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, height, &argv[INTEGER_THREE]));

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_FOUR, argv, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::Play()
{
    WVLOG_D("begin to play,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "play", callback);

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ZERO, nullptr, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::Pause()
{
    WVLOG_D("begin to pause,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "pause", callback);

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ZERO, nullptr, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::Seek(double time)
{
    WVLOG_D("begin to seek,nweb id is %{public}d,time is %{public}f", nwebId_, time);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "seek", callback);

    napi_value argv[INTEGER_ONE] = { nullptr };
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, time, &argv[INTEGER_ZERO]));

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ONE, argv, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::SetVolume(double volume)
{
    WVLOG_D("begin to set volume,nweb id is %{public}d,volume is %{public}f", nwebId_, volume);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "setVolume", callback);

    napi_value argv[INTEGER_ONE] = { nullptr };
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, volume, &argv[INTEGER_ZERO]));

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ONE, argv, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::SetMuted(bool isMuted)
{
    WVLOG_D("begin to set muted,nweb id is %{public}d,muted flag is %{public}d", nwebId_, isMuted);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "setMuted", callback);

    napi_value argv[INTEGER_ONE] = { nullptr };
    NAPI_CALL_RETURN_VOID(env_, napi_get_boolean(env_, isMuted, &argv[INTEGER_ZERO]));

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ONE, argv, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::SetPlaybackRate(double playbackRate)
{
    WVLOG_D("begin to set playback rate,nweb id is %{public}d,playback rate is %{public}f", nwebId_, playbackRate);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "setPlaybackRate", callback);

    napi_value argv[INTEGER_ONE] = { nullptr };
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, playbackRate, &argv[INTEGER_ZERO]));

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ONE, argv, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::Release()
{
    WVLOG_D("begin to release,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "release", callback);

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ZERO, nullptr, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::EnterFullScreen()
{
    WVLOG_D("begin to enter full screen,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "enterFullscreen", callback);

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ZERO, nullptr, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::ExitFullScreen()
{
    WVLOG_D("begin to exit full screen,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "exitFullscreen", callback);

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ZERO, nullptr, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::ResumeMediaPlayer()
{
    WVLOG_D("begin to resume media player,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "resumePlayer", callback);

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ZERO, nullptr, nullptr));
}

void NWebNativeMediaPlayerBridgeImpl::SuspendMediaPlayer(SuspendType type)
{
    WVLOG_D("begin to suspend media player,nweb id is %{public}d", nwebId_);

    napi_value callback = nullptr;
    NAPI_GET_CALLBACK_RETURN_VOID(env_, value_, "suspendPlayer", callback);

    napi_value argv[INTEGER_ONE] = { nullptr };
    NAPI_CALL_RETURN_VOID(env_, napi_create_int32(env_, static_cast<int>(type), &argv[INTEGER_ZERO]));

    NAPI_CALL_RETURN_VOID(env_, napi_call_function(env_, value_, callback, INTEGER_ONE, argv, nullptr));
}

NWebCreateNativeMediaPlayerCallbackImpl::NWebCreateNativeMediaPlayerCallbackImpl(
    int32_t nwebId, napi_env env, napi_ref callback)
    : nwebId_(nwebId), env_(env), callback_(callback)
{}

NWebCreateNativeMediaPlayerCallbackImpl::~NWebCreateNativeMediaPlayerCallbackImpl()
{
    napi_delete_reference(env_, callback_);
}

std::shared_ptr<NWebNativeMediaPlayerBridge> NWebCreateNativeMediaPlayerCallbackImpl::OnCreate(
    std::shared_ptr<NWebNativeMediaPlayerHandler> handler, std::shared_ptr<NWebMediaInfo> mediaInfo)
{
    WVLOG_D("begin to create native media player,nweb id is %{public}d", nwebId_);

    if (!callback_) {
        WVLOG_E("callback is null,nweb id is %{public}d", nwebId_);
        return nullptr;
    }

    if (!handler || !mediaInfo) {
        WVLOG_E("param is null,nweb id is %{public}d", nwebId_);
        return nullptr;
    }

    napi_value callback = nullptr;
    napi_get_reference_value(env_, callback_, &callback);
    if (callback == nullptr) {
        WVLOG_E("failed to get callback func,nweb id is %{public}d", nwebId_);
        return nullptr;
    }

    napi_value argv[INTEGER_TWO] = { nullptr };
    ConstructHandler(&argv[INTEGER_ZERO], handler);
    ConstructMediaInfo(&argv[INTEGER_ONE], mediaInfo);

    napi_value result = nullptr;
    NAPI_CALL(env_, napi_call_function(env_, nullptr, callback, INTEGER_TWO, argv, &result));

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env_, result, &valueType);
    if (valueType != napi_object) {
        WVLOG_E("result type is invalid,nweb id is %{public}d", nwebId_);
        return nullptr;
    }

    return std::make_shared<NWebNativeMediaPlayerBridgeImpl>(nwebId_, env_, result);
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructRect(
    napi_value* value, std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_object(env_, value));

    napi_value x = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, surfaceInfo->GetX(), &x));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "x", x));

    napi_value y = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, surfaceInfo->GetY(), &y));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "y", y));

    napi_value width = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, surfaceInfo->GetWidth(), &width));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "width", width));

    napi_value height = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_double(env_, surfaceInfo->GetHeight(), &height));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "height", height));
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructHandler(
    napi_value* value, std::shared_ptr<NWebNativeMediaPlayerHandler> handler)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_object(env_, value));

    napi_wrap(
        env_, *value, new NapiNativeMediaPlayerHandlerImpl(nwebId_, handler),
        [](napi_env /*env*/, void* data, void* /*hint*/) {
            NapiNativeMediaPlayerHandlerImpl* handler = (NapiNativeMediaPlayerHandlerImpl*)data;
            delete handler;
        },
        nullptr, nullptr);
    NAPI_CALL_RETURN_VOID(env_, NapiNativeMediaPlayerHandler::DefineProperties(env_, value));
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructControls(
    napi_value* value, const std::vector<std::string>& controls)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_array_with_length(env_, controls.size(), value));

    for (unsigned int i = 0; i < controls.size(); i++) {
        napi_value control = nullptr;
        NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(
            env_, controls[i].c_str(), controls[i].length(), &control));
        NAPI_CALL_RETURN_VOID(env_, napi_set_element(env_, *value, i, control));
    }
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructHeaders(
    napi_value* value, const std::map<std::string, std::string>& headers)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_object(env_, value));

    for (const auto& header : headers) {
        napi_value argv = nullptr;
        NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(
            env_, header.second.c_str(), header.second.length(), &argv));
        NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, header.first.c_str(), argv));
    }
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructAttributes(
    napi_value* value, const std::map<std::string, std::string>& attributes)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_object(env_, value));

    for (const auto& attribute : attributes) {
        napi_value argv = nullptr;
        NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(
            env_, attribute.second.c_str(), attribute.second.length(), &argv));
        NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, attribute.first.c_str(), argv));
    }
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructMediaInfo(
    napi_value* value, std::shared_ptr<NWebMediaInfo> mediaInfo)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_object(env_, value));

    napi_value embedId = nullptr;
    std::string id = mediaInfo->GetEmbedId();
    NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, id.c_str(), id.length(), &embedId));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "embedID", embedId));

    napi_value mediaType = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_int32(env_, static_cast<int>(mediaInfo->GetMediaType()), &mediaType));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "mediaType", mediaType));

    napi_value mediaSrcList = nullptr;
    ConstructSourceInfos(&mediaSrcList, mediaInfo->GetSourceInfos());
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "mediaSrcList", mediaSrcList));

    napi_value surfaceInfo = nullptr;
    ConstructSurfaceInfo(&surfaceInfo, mediaInfo->GetSurfaceInfo());
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "surfaceInfo", surfaceInfo));

    napi_value isControlsShown = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_get_boolean(env_, mediaInfo->GetIsControlsShown(), &isControlsShown));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "controlsShown", isControlsShown));

    napi_value controlList = nullptr;
    ConstructControls(&controlList, mediaInfo->GetControls());
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "controlList", controlList));

    napi_value headers = nullptr;
    ConstructHeaders(&headers, mediaInfo->GetHeaders());
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "headers", headers));

    napi_value attributes = nullptr;
    ConstructAttributes(&attributes, mediaInfo->GetAttributes());
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "attributes", attributes));

    napi_value isMuted = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_get_boolean(env_, mediaInfo->GetIsMuted(), &isMuted));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "muted", isMuted));

    napi_value posterUrl = nullptr;
    std::string url = mediaInfo->GetPosterUrl();
    NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, url.c_str(), url.length(), &posterUrl));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "posterUrl", posterUrl));

    napi_value preload = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_int32(env_, static_cast<int>(mediaInfo->GetPreload()), &preload));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "preload", preload));
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructSourceInfos(
    napi_value* value, const std::vector<std::shared_ptr<NWebMediaSourceInfo>>& sourceInfos)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_array_with_length(env_, sourceInfos.size(), value));

    for (unsigned int i = 0; i < sourceInfos.size(); i++) {
        if (!sourceInfos[i]) {
            continue;
        }

        napi_value sourceInfo = nullptr;
        NAPI_CALL_RETURN_VOID(env_, napi_create_object(env_, &sourceInfo));

        napi_value type = nullptr;
        NAPI_CALL_RETURN_VOID(env_, napi_create_int32(env_, static_cast<int>(sourceInfos[i]->GetType()), &type));
        NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, sourceInfo, "type", type));

        napi_value source = nullptr;
        std::string mediaSource = sourceInfos[i]->GetSource();
        NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, mediaSource.c_str(), mediaSource.length(), &source));
        NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, sourceInfo, "source", source));

        napi_value format = nullptr;
        std::string mediaFormat = sourceInfos[i]->GetFormat();
        NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, mediaFormat.c_str(), mediaFormat.length(), &format));
        NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, sourceInfo, "format", format));

        NAPI_CALL_RETURN_VOID(env_, napi_set_element(env_, *value, i, sourceInfo));
    }
}

void NWebCreateNativeMediaPlayerCallbackImpl::ConstructSurfaceInfo(
    napi_value* value, std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo)
{
    NAPI_CALL_RETURN_VOID(env_, napi_create_object(env_, value));

    if (!surfaceInfo) {
        return;
    }

    napi_value surfaceId = nullptr;
    std::string id = surfaceInfo->GetId();
    NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, id.c_str(), id.length(), &surfaceId));
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "id", surfaceId));

    napi_value rect = nullptr;
    ConstructRect(&rect, surfaceInfo);
    NAPI_CALL_RETURN_VOID(env_, napi_set_named_property(env_, *value, "rect", rect));
}

} // namespace OHOS::NWeb

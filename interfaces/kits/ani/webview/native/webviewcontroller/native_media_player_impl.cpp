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
#include "napi_native_mediaplayer_handler_impl.h"

#include <ani.h>

#include "napi/native_api.h"
#include "napi_common_macros.h"
#include "nweb_log.h"
#include "nweb_native_media_player.h"

namespace OHOS::NWeb {

constexpr int INTEGER_ZERO = 0;
constexpr int INTEGER_ONE = 1;

namespace {
const char* NATIVE_MEDIA_PLAYER_HANDLER_INNER = "@ohos.web.webview.webview.NativeMediaPlayerHandlerinner";
const char* MEDIA_INFO_INNER = "@ohos.web.webview.webview.MediaInfoinner";
const char* RECT_EVENT_INNER = "@ohos.web.webview.webview.RectEventinner";
const char* CLASS_MEDIA_SOURCE_INFO = "@ohos.web.webview.webview.MediaSourceInfo";
const char* NATIVE_MEDIA_PLAYER_CLASS_NAME = "@ohos.web.webview.webview.NativeMediaPlayerSurfaceInfo";
const char* ANI_ENUM_MEDIA_TYPE = "@ohos.web.webview.webview.MediaType";
const char* ANI_ENUM_PRELOAD = "@ohos.web.webview.webview.Preload";
const char* ANI_ENUM_SOURCE_TYPE = "@ohos.web.webview.webview.SourceType";
const char* ANI_ENUM_SUSPEND_TYPE = "@ohos.web.webview.webview.SuspendType";
} // namespace

bool Wrap(ani_env* env, const ani_object& object, const char* className, const ani_long& thisVar)
{
    ani_status status;
    ani_class cls;
    if ((status = env->FindClass(className, &cls)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap FindClass status: %{public}d", status);
        return false;
    }
    ani_method innerWrapMethod;
    if ((status = env->Class_FindMethod(cls, "bindNativePtr", "l:", &innerWrapMethod)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Class_FindMethod status: %{public}d", status);
        return false;
    }
    if ((status = env->Object_CallMethod_Void(object, innerWrapMethod, thisVar)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Object_CallMethod_Void status: %{public}d", status);
        return false;
    }
    return true;
}

bool CreateObjectVoid(ani_env* env, const char* className, ani_object& object)
{
    ani_class cls;
    ani_status status = env->FindClass(className, &cls);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", className, status);
        return false;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) != ANI_OK) {
        WVLOG_E("get %{public}s ctor method failed, status: %{public}d", className, status);
        return false;
    }
    if ((status = env->Object_New(cls, ctor, &object)) != ANI_OK) {
        WVLOG_E("new %{public}s failed, status: %{public}d", className, status);
        return false;
    }
    return true;
}

bool GetEnumItemByIndex(ani_env* env, const char* enumName, int32_t typeIndex, ani_enum_item& eType)
{
    ani_enum enumType;
    ani_status status = env->FindEnum(enumName, &enumType);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s enum failed, status: %{public}d", enumName, status);
        return false;
    }
    if ((status = env->Enum_GetEnumItemByIndex(enumType, typeIndex, &eType)) != ANI_OK) {
        WVLOG_E("get %{public}s item by index failed, status: %{public}d", enumName, status);
        return false;
    }
    return true;
}

ani_string StringToAniStr(ani_env* env, const std::string& str)
{
    ani_string result {};
    if (ANI_OK != env->String_NewUTF8(str.c_str(), str.size(), &result)) {
        return nullptr;
    }
    return result;
}


NWebNativeMediaPlayerBridgeImpl::NWebNativeMediaPlayerBridgeImpl(int32_t nwebId, ani_vm* vm, ani_ref value)
    : nwebId_(nwebId), vm_(vm)
{
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    env->GlobalReference_Create(value, &value_);
}

NWebNativeMediaPlayerBridgeImpl::~NWebNativeMediaPlayerBridgeImpl()
{
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    env->GlobalReference_Delete(value_);
}

void NWebNativeMediaPlayerBridgeImpl::UpdateRect(double x, double y, double width, double height)
{
    WVLOG_I("begin to update rect, nweb id is %{public}d", nwebId_);

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "updateRect", nullptr, 
                                          static_cast<ani_double>(x), 
                                          static_cast<ani_double>(y), 
                                          static_cast<ani_double>(width), 
                                          static_cast<ani_double>(height)) != ANI_OK) {
        WVLOG_E("UpdateRect failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::Play()
{
    WVLOG_I("begin to play, nweb id is %{public}d", nwebId_);

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "play", nullptr) != ANI_OK) {
        WVLOG_E("Play failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::Pause()
{
    WVLOG_I("begin to pause, nweb id is %{public}d", nwebId_);

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "pause", nullptr) != ANI_OK) {
        WVLOG_E("Pause failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::Seek(double time)
{
    WVLOG_I("begin to seek, nweb id is %{public}d, time is %{public}f", nwebId_, time);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "seek", nullptr, 
                                          static_cast<ani_double>(time)) != ANI_OK) {
        WVLOG_E("Seek failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::SetVolume(double volume)
{
    WVLOG_I("begin to set volume, nweb id is %{public}d, volume is %{public}f", nwebId_, volume);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "setVolume", nullptr, 
                                          static_cast<ani_double>(volume)) != ANI_OK) {
        WVLOG_E("SetVolume failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::SetMuted(bool isMuted)
{
    WVLOG_I("begin to set muted, nweb id is %{public}d, muted flag is %{public}d", nwebId_, isMuted);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "setMuted", nullptr, 
                                          static_cast<ani_boolean>(isMuted)) != ANI_OK) {
        WVLOG_E("SetMuted failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::SetPlaybackRate(double playbackRate)
{
    WVLOG_I("begin to set playback rate, nweb id is %{public}d, playback rate is %{public}f", nwebId_, playbackRate);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "setPlaybackRate", nullptr, 
                                          static_cast<ani_double>(playbackRate)) != ANI_OK) {
        WVLOG_E("SetPlaybackRate failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::Release()
{
    WVLOG_I("begin to release, nweb id is %{public}d", nwebId_);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "release", nullptr) != ANI_OK) {
        WVLOG_E("Release failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::EnterFullScreen()
{
    WVLOG_I("begin to enter full screen, nweb id is %{public}d", nwebId_);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "enterFullscreen", nullptr) != ANI_OK) {
        WVLOG_E("EnterFullScreen failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::ExitFullScreen()
{
    WVLOG_I("begin to exit full screen, nweb id is %{public}d", nwebId_);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "exitFullscreen", nullptr) != ANI_OK) {
        WVLOG_E("ExitFullScreen failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::ResumeMediaPlayer()
{
    WVLOG_I("begin to resume media player, nweb id is %{public}d", nwebId_);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    ani_ref argv[INTEGER_ZERO];
    ani_ref resumeRef {};
    ani_ref ref;
    if (env->Object_GetPropertyByName_Ref(reinterpret_cast<ani_object>(value_), "resumePlayer", &resumeRef) != 
        ANI_OK){
        WVLOG_E("ResumeMediaPlayer failed");
        return;
    }
    if (env->FunctionalObject_Call(static_cast<ani_fn_object>(resumeRef), ani_size(INTEGER_ZERO), argv, &ref) !=
        ANI_OK) {
        WVLOG_E("ResumeMediaPlayer failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::SuspendMediaPlayer(SuspendType type)
{
    WVLOG_I("begin to suspend media player, nweb id is %{public}d %{public}d", nwebId_, static_cast<int>(type));
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    ani_enum_item SuspendTypeRef;
    if (!GetEnumItemByIndex(env, ANI_ENUM_SUSPEND_TYPE, static_cast<int32_t>(type), SuspendTypeRef)){
        WVLOG_E("SuspendMediaPlayer failed");
        return;
    }

    ani_ref argv[INTEGER_ONE];
    argv[INTEGER_ZERO] = static_cast<ani_ref>(SuspendTypeRef);
    ani_ref suspendRef {};
    ani_ref ref;
    if (env->Object_GetPropertyByName_Ref(reinterpret_cast<ani_object>(value_), "suspendPlayer", &suspendRef) != 
        ANI_OK){
        WVLOG_E("SuspendMediaPlayer failed");
        return;
    }
    if (env->FunctionalObject_Call(static_cast<ani_fn_object>(suspendRef), ani_size(INTEGER_ONE), argv, &ref) !=
        ANI_OK) {
        WVLOG_E("SuspendMediaPlayer failed");
    }
}

NWebCreateNativeMediaPlayerCallbackImpl::NWebCreateNativeMediaPlayerCallbackImpl(
    int32_t nwebId, ani_vm* vm, ani_fn_object callback)
    : nwebId_(nwebId), vm_(vm)
{
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    env->GlobalReference_Create(static_cast<ani_ref>(callback), &callback_);
}

NWebCreateNativeMediaPlayerCallbackImpl::~NWebCreateNativeMediaPlayerCallbackImpl()
{
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    env->GlobalReference_Delete(callback_);
}

std::shared_ptr<NWebNativeMediaPlayerBridge> NWebCreateNativeMediaPlayerCallbackImpl::OnCreate(
    std::shared_ptr<NWebNativeMediaPlayerHandler> handler, std::shared_ptr<NWebMediaInfo> mediaInfo)
{
    WVLOG_I("begin to create native media player, nweb id is %{public}d", nwebId_);

    if (!callback_) {
        WVLOG_E("callback is null, nweb id is %{public}d", nwebId_);
        return nullptr;
    }
    if (!handler || !mediaInfo) {
        WVLOG_E("param is null, nweb id is %{public}d", nwebId_);
        return nullptr;
    }

    ani_object argv1 = ConstructHandler(handler);
    ani_object argv2 = ConstructMediaInfo(mediaInfo);
    if (argv1 == nullptr || argv2 == nullptr) {
        WVLOG_E("Construct failed");
        return nullptr;
    }

    ani_ref result = nullptr;
    std::vector<ani_ref> vec;
    vec.push_back(argv1);
    vec.push_back(argv2);

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    if (env->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(callback_), vec.size(), vec.data(), &result) !=
        ANI_OK) {
        WVLOG_E("FunctionalObject_Call failed");
        return nullptr;
    }

    return std::make_shared<NWebNativeMediaPlayerBridgeImpl>(nwebId_, vm_, result);
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructRect(
    std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo)
{
    WVLOG_I("ConstructRect start");

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object;
    if (!CreateObjectVoid(env, RECT_EVENT_INNER, object)) {
        return nullptr;
    }
    ani_double x = static_cast<ani_double>(surfaceInfo->GetX());
    ani_double y = static_cast<ani_double>(surfaceInfo->GetY());
    ani_double width = static_cast<ani_double>(surfaceInfo->GetWidth());
    ani_double height = static_cast<ani_double>(surfaceInfo->GetHeight());

    env->Object_SetPropertyByName_Double(object, "x", x);
    env->Object_SetPropertyByName_Double(object, "y", y);
    env->Object_SetPropertyByName_Double(object, "width", width);
    env->Object_SetPropertyByName_Double(object, "height", height);

    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructHandler(
    std::shared_ptr<NWebNativeMediaPlayerHandler> handler)
{
    WVLOG_I("ConstructHandler start");

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }

    NapiNativeMediaPlayerHandlerImpl* napiNativeMediaPlayerHandlerImpl =
        new NapiNativeMediaPlayerHandlerImpl(nwebId_, handler);
    if (napiNativeMediaPlayerHandlerImpl == nullptr) {
        WVLOG_E("new napiNativeMediaPlayerHandlerImpl failed");
        return nullptr;
    }

    ani_object object = nullptr;
    if (!CreateObjectVoid(env, NATIVE_MEDIA_PLAYER_HANDLER_INNER, object)) {
        WVLOG_E("new NapiNativeMediaPlayerHandlerImpl createobject failed");
        delete napiNativeMediaPlayerHandlerImpl;
        napiNativeMediaPlayerHandlerImpl = nullptr;
        return nullptr;
    }

    if (!Wrap(env, object, NATIVE_MEDIA_PLAYER_HANDLER_INNER,
            reinterpret_cast<ani_long>(napiNativeMediaPlayerHandlerImpl))) {
        WVLOG_E("aniNativeMediaPlayerHandlerImpl wrap failed");
        delete napiNativeMediaPlayerHandlerImpl;
        napiNativeMediaPlayerHandlerImpl = nullptr;
        return nullptr;
    }
    napiNativeMediaPlayerHandlerImpl->IncRefCount();
    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructControls(const std::vector<std::string>& controls)
{
    WVLOG_I("ConstructControls start");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefined failed");
        return nullptr;
    }
    ani_array array;
    if (ANI_OK != env->Array_New(controls.size(), undefinedRef, &array)) {
        WVLOG_E("ConstructControls failed");
        return nullptr;
    }

    for (size_t i = 0; i < controls.size(); i++) {
        auto item = StringToAniStr(env, controls[i]);
        env->Array_Set(array, i, item);
    }

    return static_cast<ani_object>(array);
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructHeaders(const std::map<std::string, std::string>& headers)
{
    WVLOG_I("ConstructHeaders start");

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object = nullptr;
    if (!CreateObjectVoid(env, "escompat.Record", object) || object == nullptr) {
        return nullptr;
    }
    ani_class cls;
    ani_status status = env->FindClass("escompat.Record", &cls);
    if (status != ANI_OK) {
        WVLOG_E("find class failed, status: %{public}d", status);
        return nullptr;
    }
    ani_method set;
    if ((status = env->Class_FindMethod(cls, "$_set", nullptr, &set)) != ANI_OK) {
        WVLOG_E("get set method failed, status: %{public}d",status);
        return nullptr;
    }
    for (const auto& header : headers) {
        ani_string key;
        ani_string value;
        env->String_NewUTF8(header.first.c_str(), header.first.length(), &key);
        env->String_NewUTF8(header.second.c_str(), header.second.length(), &value);
        if (ANI_OK != env->Object_CallMethod_Void(object, set, key, value)) {
            return nullptr;
        }
    }
    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructAttributes(
    const std::map<std::string, std::string>& attributes)
{
    WVLOG_I("ConstructAttributes start");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object = nullptr;
    if (!CreateObjectVoid(env, "escompat.Record", object) || object == nullptr) {
        return nullptr;
    }
    ani_class cls;
    ani_status status = env->FindClass("escompat.Record", &cls);
    if (status != ANI_OK) {
        WVLOG_E("find class failed, status: %{public}d", status);
        return nullptr;
    }
    ani_method set;
    if ((status = env->Class_FindMethod(cls, "$_set", nullptr, &set)) != ANI_OK) {
        WVLOG_E("get set method failed, status: %{public}d",status);
        return nullptr;
    }
    for (const auto& attribute : attributes) {
        ani_string key;
        ani_string value;
        env->String_NewUTF8(attribute.first.c_str(), attribute.first.length(), &key);
        env->String_NewUTF8(attribute.second.c_str(), attribute.second.length(), &value);
        if (ANI_OK != env->Object_CallMethod_Void(object, set, key, value)) {
            return nullptr;
        }
    }
    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructMediaInfo(std::shared_ptr<NWebMediaInfo> mediaInfo)
{
    WVLOG_I("ConstructMediaInfo start");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object = nullptr;
    if (!CreateObjectVoid(env, MEDIA_INFO_INNER, object)) {
        return nullptr;
    }

    ani_string embedId;
    std::string id = mediaInfo->GetEmbedId();
    env->String_NewUTF8(id.c_str(), id.length(), &embedId);
    ani_ref embedIdref = static_cast<ani_ref>(embedId);
    if (embedIdref == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "embedID", embedIdref);

    ani_enum_item mediaTypeRef;
    if (!GetEnumItemByIndex(env, ANI_ENUM_MEDIA_TYPE, 
                            static_cast<int32_t>(mediaInfo->GetMediaType()), mediaTypeRef)) {
        return nullptr;
    }
    env->Object_SetPropertyByName_Ref(object, "mediaType", mediaTypeRef);

    ani_object mediaSrcList = ConstructSourceInfos(mediaInfo->GetSourceInfos());
    ani_ref mediaSrcListRef = static_cast<ani_ref>(mediaSrcList);
    if (mediaSrcListRef == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "mediaSrcList", mediaSrcListRef);

    ani_object surfaceInfo = ConstructSurfaceInfo(mediaInfo->GetSurfaceInfo());
    ani_ref surfaceInfoRef = static_cast<ani_ref>(surfaceInfo);
    if (surfaceInfoRef == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "surfaceInfo", surfaceInfoRef);

    ani_boolean isControlsShown = static_cast<ani_boolean>(mediaInfo->GetIsControlsShown());
    env->Object_SetPropertyByName_Boolean(object, "controlsShown", isControlsShown);

    ani_object controlList = ConstructControls(mediaInfo->GetControls());
    ani_ref controlListRef = static_cast<ani_ref>(controlList);
    if (controlListRef == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "controlList", controlListRef);

    ani_object headers = ConstructHeaders(mediaInfo->GetHeaders());
    ani_ref headersRef = static_cast<ani_ref>(headers);
    if (headersRef == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "headers", headersRef);

    ani_object attributes = ConstructAttributes(mediaInfo->GetAttributes());
    ani_ref attributesRef = static_cast<ani_ref>(attributes);
    if (attributesRef == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "attributes", attributesRef);
    
    ani_boolean isMuted = static_cast<ani_boolean>(mediaInfo->GetIsMuted());
    env->Object_SetPropertyByName_Boolean(object, "muted", isMuted);

    ani_string posterUrl {};
    std::string url = mediaInfo->GetPosterUrl();
    env->String_NewUTF8(url.c_str(), url.length(), &posterUrl);
    ani_ref posterUrlref = static_cast<ani_ref>(posterUrl);
    if (posterUrlref == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "posterUrl", posterUrlref);

    ani_enum_item preloadRef;
    if (!GetEnumItemByIndex(env, ANI_ENUM_PRELOAD, 
                            static_cast<int32_t>(mediaInfo->GetPreload()), preloadRef)) {
        return nullptr;
    }
    env->Object_SetPropertyByName_Ref(object, "preload", preloadRef);

    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructSourceInfos(
    const std::vector<std::shared_ptr<NWebMediaSourceInfo>>& sourceInfos)
{
    WVLOG_I("ConstructSourceInfos start");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefined failed");
        return nullptr;
    }
    ani_array array;
    if (ANI_OK != env->Array_New(sourceInfos.size(), undefinedRef, &array)) {
        WVLOG_E("ConstructSourceInfos failed");
        return nullptr;
    }

    for (size_t i = 0; i < sourceInfos.size(); i++) {
        ani_object sourceInfo = nullptr;
        if (!CreateObjectVoid(env, CLASS_MEDIA_SOURCE_INFO, sourceInfo)) {
            return nullptr;
        }

        ani_enum_item typeRef;
        if (!GetEnumItemByIndex(env, ANI_ENUM_SOURCE_TYPE, 
                                static_cast<int32_t>(sourceInfos[i]->GetType()), typeRef)) {
            return nullptr;
        }
        env->Object_SetPropertyByName_Ref(sourceInfo, "type", typeRef);

        auto sourceref = StringToAniStr(env, sourceInfos[i]->GetSource());
        if (sourceref == nullptr) { return nullptr; }
        env->Object_SetPropertyByName_Ref(sourceInfo, "source", sourceref);

        auto formatref = StringToAniStr(env, sourceInfos[i]->GetFormat());
        if (formatref == nullptr) { return nullptr; }
        env->Object_SetPropertyByName_Ref(sourceInfo, "format", formatref);

        env->Array_Set(array, i, static_cast<ani_ref>(sourceInfo));
    }
    return static_cast<ani_object>(array);
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructSurfaceInfo(
    std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo)
{
    WVLOG_I("ConstructSurfaceInfo start");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object;
    if (!CreateObjectVoid(env, NATIVE_MEDIA_PLAYER_CLASS_NAME, object)) {
        return nullptr;
    }

    ani_string surfaceId {};
    std::string id = surfaceInfo->GetId();
    env->String_NewUTF8(id.c_str(), id.length(), &surfaceId);
    ani_ref surfaceIdref = static_cast<ani_ref>(surfaceId);
    if (surfaceIdref == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "id", surfaceIdref);

    ani_object rect = ConstructRect(surfaceInfo);
    ani_ref rectRef = static_cast<ani_ref>(rect);
    if (rectRef == nullptr) { return nullptr; }
    env->Object_SetPropertyByName_Ref(object, "rect", rectRef);

    return object;
}
} // namespace OHOS::NWeb

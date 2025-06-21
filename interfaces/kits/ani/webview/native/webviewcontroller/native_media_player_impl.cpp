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

#include <ani.h>

#include "napi/native_api.h"
#include "napi_common_macros.h"
#include "nweb_log.h"
#include "nweb_native_media_player.h"

namespace OHOS::NWeb {

constexpr int INTEGER_ZERO = 0;
constexpr int INTEGER_ONE = 1;
constexpr int INTEGER_TWO = 2;
constexpr int INTEGER_THREE = 3;
constexpr int INTEGER_FOUR = 4;

namespace {
const char* NATIVE_MEDIA_PLAYER_HANDLER_INNER = "L@ohos/web/webview/webview/NativeMediaPlayerHandlerinner;";
const char* MWDIA_INFO_INNER = "L@ohos/web/webview/webview/MediaInfoinner;";
const char* RECT_EVENT_INNER = "L@ohos/web/webview/webview/RectEventinner;";
const char* CLASS_MEDIA_SOURCE_INFO = "L@ohos/web/webview/webview/MediaSourceInfo;";
const char* CLASS_NATIVE_MEDIA_PLAYER_SURFACE_INFO = "L@ohos/web/webview/webview/NativeMediaPlayerSurfaceInfo;";
const char* CLASS_NAME_STRING = "Lstd/core/String;";
const char* ANI_ENUM_MEDIA_TYPE = "L@ohos/web/webview/webview/MediaType;";
const char* ANI_ENUM_PRELOAD = "L@ohos/web/webview/webview/Preload;";
const char* ANI_ENUM_SOURCE_TYPE = "L@ohos/web/webview/webview/SourceType;";
const char* ANI_ENUM_SUSPEND_TYPE = "L@ohos/web/webview/webview/SuspendType;";
} // namespace

bool EnumParseInt32_t(ani_env* env, ani_enum_item enum_item, int32_t& outValue)
{
    WVLOG_D("EnumParseInt32_t");

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
    env->EnumItem_GetValue_Int(enum_item, &number);
    outValue = static_cast<int32_t>(number);
    WVLOG_I("EnumParseInt32_t%{public}d", outValue);
    return true;
}

bool Wrap(ani_env* env, const ani_object& object, const char* className, const ani_long& thisVar)
{
    ani_status status;
    ani_class cls;
    if ((status = env->FindClass(className, &cls)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap FindClass status: %{public}d", status);
        return false;
    }
    ani_method innerWrapMethod;
    if ((status = env->Class_FindMethod(cls, "bindNativePtr", "J:V", &innerWrapMethod)) != ANI_OK) {
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

AniNativeMediaPlayerHandlerImpl::AniNativeMediaPlayerHandlerImpl(
    int32_t nwebId, std::shared_ptr<NWebNativeMediaPlayerHandler> handler)
    : nwebId_(nwebId), handler_(handler)
{}

void AniNativeMediaPlayerHandlerImpl::HandleStatusChanged(PlaybackStatus status)
{
    WVLOG_D("begin to handle status changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle status changed,nweb id is %{public}d, %{public}d", nwebId_, static_cast<int>(status));
        handler_->HandleStatusChanged(status);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleVolumeChanged(double volume)
{
    WVLOG_D("begin to handle volume changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle volume changed,nweb id is %{public}d, %{public}f", nwebId_, volume);
        handler_->HandleVolumeChanged(volume);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleMutedChanged(bool isMuted)
{
    WVLOG_D("begin to handle muted changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle muted changed,nweb id is %{public}d, %{public}d", nwebId_, isMuted);
        handler_->HandleMutedChanged(isMuted);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandlePlaybackRateChanged(double playbackRate)
{
    WVLOG_D("begin to handle playback rate changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle playback rate changed,nweb id is %{public}d, %{public}f", nwebId_, playbackRate);
        handler_->HandlePlaybackRateChanged(playbackRate);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleDurationChanged(double duration)
{
    WVLOG_D("begin to handle duration changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle duration changed,nweb id is %{public}d, %{public}f", nwebId_, duration);
        handler_->HandleDurationChanged(duration);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleTimeUpdate(double playTime)
{
    WVLOG_D("begin to handle time update,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle time update,nweb id is %{public}d, %{public}f", nwebId_, playTime);
        handler_->HandleTimeUpdate(playTime);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleBufferedEndTimeChanged(double bufferedEndTime)
{
    WVLOG_D("begin to handle buffered end time changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D(
            "begin to handle buffered end time changed,nweb id is %{public}d, %{public}f", nwebId_, bufferedEndTime);
        handler_->HandleBufferedEndTimeChanged(bufferedEndTime);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleEnded()
{
    WVLOG_D("begin to handle end,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle end,nweb id is %{public}d", nwebId_);
        handler_->HandleEnded();
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleNetworkStateChanged(NetworkState state)
{
    WVLOG_D("begin to handle network state changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle network state changed,nweb id is %{public}d, %{public}d", nwebId_,
            static_cast<int>(state));
        handler_->HandleNetworkStateChanged(state);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleReadyStateChanged(ReadyState state)
{
    WVLOG_D("begin to handle ready state changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D(
            "begin to handle ready state changed,nweb id is %{public}d, %{public}d", nwebId_, static_cast<int>(state));
        handler_->HandleReadyStateChanged(state);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleFullScreenChanged(bool isFullScreen)
{
    WVLOG_D("begin to handle full screen changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle full screen changed,nweb id is %{public}d, %{public}d", nwebId_, isFullScreen);
        handler_->HandleFullScreenChanged(isFullScreen);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleSeeking()
{
    WVLOG_D("begin to handle seeking,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle seeking,nweb id is %{public}d", nwebId_);
        handler_->HandleSeeking();
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleSeekFinished()
{
    WVLOG_D("begin to handle seek finished,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle seek finished,nweb id is %{public}d", nwebId_);
        handler_->HandleSeekFinished();
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleError(MediaError error, const std::string& message)
{
    WVLOG_D("begin to handle error,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D("begin to handle error,nweb id is %{public}d, %{public}d, %{public}s", nwebId_, static_cast<int>(error),
            message.c_str());
        handler_->HandleError(error, message);
    }
}

void AniNativeMediaPlayerHandlerImpl::HandleVideoSizeChanged(double width, double height)
{
    WVLOG_D("begin to handle video size changed,nweb id is %{public}d", nwebId_);

    if (handler_) {
        WVLOG_D(
            "begin to handle video size changed,nweb id is %{public}d, %{public}f, %{public}f", nwebId_, width, height);
        handler_->HandleVideoSizeChanged(width, height);
    }
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

ani_object createObjectDouble(ani_env* env, ani_double status)
{
    static constexpr const char* className = "Lstd/core/Double;";
    ani_class doubleCls {};
    env->FindClass(className, &doubleCls);
    ani_method ctor {};
    env->Class_FindMethod(doubleCls, "<ctor>", "d:", &ctor);
    ani_object obj {};
    if (env->Object_New(doubleCls, ctor, &obj, status) != ANI_OK) {
        WVLOG_D("createObjectDouble failed");
        return nullptr;
    }
    return obj;
}

ani_object createObjectBoolean(ani_env* env, ani_boolean status)
{
    static constexpr const char* className = "Lstd/core/Boolean;";
    ani_class booleanCls {};
    env->FindClass(className, &booleanCls);
    ani_method ctor {};
    env->Class_FindMethod(booleanCls, "<ctor>", "z:", &ctor);
    ani_object obj {};
    if (env->Object_New(booleanCls, ctor, &obj, status) != ANI_OK) {
        WVLOG_D("createObjectBoolean failed");
        return nullptr;
    }
    return obj;
}

ani_object createObjectInt(ani_env* env, ani_int status)
{
    static constexpr const char* className = "Lstd/core/Int;";
    ani_class intCls {};
    env->FindClass(className, &intCls);
    ani_method ctor {};
    env->Class_FindMethod(intCls, "<ctor>", "i:", &ctor);
    ani_object obj {};
    if (env->Object_New(intCls, ctor, &obj, status) != ANI_OK) {
        WVLOG_D("createObjectInt failed");
        return nullptr;
    }
    return obj;
}

void NWebNativeMediaPlayerBridgeImpl::UpdateRect(double x, double y, double width, double height)
{
    WVLOG_D("begin to update rect,nweb id is %{public}d %{public}f %{public}f %{public}f %{public}f", nwebId_, x, y,
        width, height);

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    ani_ref argv[INTEGER_FOUR];
    argv[INTEGER_ZERO] = createObjectDouble(env, x);
    argv[INTEGER_ONE] = createObjectDouble(env, y);
    argv[INTEGER_TWO] = createObjectDouble(env, width);
    argv[INTEGER_THREE] = createObjectDouble(env, height);

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "updateRect", nullptr, argv) !=
        ANI_OK) {
        WVLOG_E("UpdateRect failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::Play()
{
    WVLOG_D("begin to play,nweb id is %{public}d", nwebId_);

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
    WVLOG_D("begin to pause,nweb id is %{public}d", nwebId_);

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
    WVLOG_D("begin to seek,nweb id is %{public}d,time is %{public}f", nwebId_, time);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    ani_ref argv[INTEGER_ONE];
    argv[INTEGER_ZERO] = createObjectDouble(env, time);

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "seek", nullptr, argv) != ANI_OK) {
        WVLOG_E("Seek failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::SetVolume(double volume)
{
    WVLOG_D("begin to set volume,nweb id is %{public}d,volume is %{public}f", nwebId_, volume);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    ani_ref argv[INTEGER_ONE];
    argv[INTEGER_ZERO] = createObjectDouble(env, volume);

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "setVolume", nullptr, argv) != ANI_OK) {
        WVLOG_E("SetVolume failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::SetMuted(bool isMuted)
{
    WVLOG_D("begin to set muted,nweb id is %{public}d,muted flag is %{public}d", nwebId_, isMuted);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    ani_boolean isMutedBool = isMuted ? true : false;
    ani_ref argv[INTEGER_ONE];
    argv[INTEGER_ZERO] = createObjectBoolean(env, isMutedBool);

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "setMuted", nullptr, argv) != ANI_OK) {
        WVLOG_E("SetMuted failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::SetPlaybackRate(double playbackRate)
{
    WVLOG_D("begin to set playback rate,nweb id is %{public}d,playback rate is %{public}f", nwebId_, playbackRate);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    ani_ref argv[INTEGER_ONE];
    argv[INTEGER_ZERO] = createObjectDouble(env, playbackRate);

    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "setPlaybackRate", nullptr, argv) !=
        ANI_OK) {
        WVLOG_E("SetPlaybackRate failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::Release()
{
    WVLOG_D("begin to release,nweb id is %{public}d", nwebId_);
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
    WVLOG_D("begin to enter full screen,nweb id is %{public}d", nwebId_);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "enterFullscreen", nullptr) != ANI_OK) {
        WVLOG_E("begin to enter full screen,nweb id is failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::ExitFullScreen()
{
    WVLOG_D("begin to exit full screen,nweb id is %{public}d", nwebId_);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    if (env->Object_CallMethodByName_Void(reinterpret_cast<ani_object>(value_), "exitFullscreen", nullptr) != ANI_OK) {
        WVLOG_E("begin to exit full screen,nweb id is failed");
    }
}

void NWebNativeMediaPlayerBridgeImpl::ResumeMediaPlayer()
{
    WVLOG_D("begin to resume media player,nweb id is %{public}d", nwebId_);
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }
    ani_class mediaCls;
    auto status = env->FindClass("L@ohos/web/webview/webview/NativeMediaPlayerBridgeinner;", &mediaCls);
    ani_ref fnRef {};
    auto status1 = env->Object_GetFieldByName_Ref(mediaCls, "resumePlayer", &fnRef);

    auto status2 = env->FunctionalObject_Call(static_cast<ani_fn_object>(fnRef), ani_size(INTEGER_ZERO), nullptr, nullptr);
    WVLOG_D("begiPlay failed%{public}d %{public}d %{public}d", status,status1,status2);
}

void NWebNativeMediaPlayerBridgeImpl::SuspendMediaPlayer(SuspendType type)
{
    WVLOG_D("begin to suspend media player,nweb id is %{public}d %{public}d", nwebId_, static_cast<int>(type));
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return;
    }

    ani_enum_item SuspendTypeRef;
    if (GetEnumItemByIndex(env, ANI_ENUM_SUSPEND_TYPE, static_cast<int32_t>(type), SuspendTypeRef)) {
        WVLOG_D("begin to suspend media player,nweb id is");
    }
    int statusStr;
    EnumParseInt32_t(env, SuspendTypeRef, statusStr);

    ani_ref argv[INTEGER_ONE];
    argv[INTEGER_ZERO] = reinterpret_cast<ani_ref>(static_cast<SuspendType>(statusStr));

    ani_class cls;
    env->FindClass("L@ohos/web/webview/webview/NativeMediaPlayerBridgeinner;", &cls);
    ani_ref fnRef {};
    env->Object_GetFieldByName_Ref(cls, "suspendPlayer", &fnRef);

    auto status = env->FunctionalObject_Call(static_cast<ani_fn_object>(fnRef), ani_size(INTEGER_ONE), argv, nullptr);
    WVLOG_D("begiPlay failed%{public}d", status);
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
    WVLOG_D("begin to create native media player,nweb id is %{public}d", nwebId_);

    if (!callback_) {
        WVLOG_E("callback is null,nweb id is %{public}d", nwebId_);
        return nullptr;
    }
    if (!handler || !mediaInfo) {
        WVLOG_E("param is null,nweb id is %{public}d", nwebId_);
        return nullptr;
    }

    ani_object argv1 = ConstructHandler(handler);
    ani_object argv2 = ConstructMediaInfo(mediaInfo);
    if (argv1 != nullptr || argv2 != nullptr) {
        WVLOG_E("Construct failed");
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
    }
    if (result == nullptr) {
        WVLOG_E("FunctionalObject_Call failed");
    }

    return std::make_shared<NWebNativeMediaPlayerBridgeImpl>(nwebId_, vm_, result);
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructRect(
    std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo)
{
    WVLOG_D("ConstructRect");

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
    ani_double heigth = static_cast<ani_double>(surfaceInfo->GetHeight());

    env->Object_SetPropertyByName_Double(object, "x", x);
    env->Object_SetPropertyByName_Double(object, "y", y);
    env->Object_SetPropertyByName_Double(object, "width", width);
    env->Object_SetPropertyByName_Double(object, "heigth", heigth);

    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructHandler(
    std::shared_ptr<NWebNativeMediaPlayerHandler> handler)
{
    WVLOG_D("ConstructHandler");

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }

    AniNativeMediaPlayerHandlerImpl* aniNativeMediaPlayerHandlerImpl =
        new AniNativeMediaPlayerHandlerImpl(nwebId_, handler);
    if (aniNativeMediaPlayerHandlerImpl == nullptr) {
        WVLOG_E("new aniNativeMediaPlayerHandlerImpl failed");
        return nullptr;
    }

    ani_object object = nullptr;
    if (!CreateObjectVoid(env, NATIVE_MEDIA_PLAYER_HANDLER_INNER, object)) {
        WVLOG_E("new aniNativeMediaPlayerHandlerImpl createobject failed");
        return nullptr;
    }

    if (!Wrap(env, object, NATIVE_MEDIA_PLAYER_HANDLER_INNER,
            reinterpret_cast<ani_long>(aniNativeMediaPlayerHandlerImpl))) {
        WVLOG_E("aniNativeMediaPlayerHandlerImpl wrap failed");
        delete aniNativeMediaPlayerHandlerImpl;
        aniNativeMediaPlayerHandlerImpl = nullptr;
        return nullptr;
    }
    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructControls(const std::vector<std::string>& controls)
{
    WVLOG_D("ConstructControls");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }

    ani_class stringCls = nullptr;
    if (ANI_OK != env->FindClass(CLASS_NAME_STRING, &stringCls)) {
        WVLOG_E("FindCl534ass failed");
    }
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefi345ned failed");
    }
    ani_array_ref array;
    if (ANI_OK != env->Array_New_Ref(stringCls, controls.size(), undefinedRef, &array)) {
        WVLOG_E("27ConstructCo455ntrolsfailed");
    }

    for (size_t i = 0; i < controls.size(); i++) {
        auto item = StringToAniStr(env, controls[i]);
        env->Array_Set_Ref(array, i, item);
    }

    return static_cast<ani_object>(array);
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructHeaders(const std::map<std::string, std::string>& headers)
{
    WVLOG_D("ConstructHeaders");

    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object = nullptr;
    if (!CreateObjectVoid(env, "Lescompat/Record;", object) || object == nullptr) {
        return nullptr;
    }
    for (const auto& header : headers) {
        ani_string key;
        ani_string value;
        env->String_NewUTF8(header.first.c_str(), header.first.length(), &key);
        env->String_NewUTF8(header.second.c_str(), header.second.length(), &value);
        if (ANI_OK !=
            env->Object_CallMethodByName_Void(object, "$_set", "Lstd/core/Object;Lstd/core/Object;:V", key, value)) {
            return nullptr;
        }
    }
    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructAttributes(
    const std::map<std::string, std::string>& attributes)
{
    WVLOG_D("ConstructAttributes");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object = nullptr;
    if (!CreateObjectVoid(env, "Lescompat/Record;", object) || object == nullptr) {
        return nullptr;
    }
    for (const auto& attribute : attributes) {
        ani_string key;
        ani_string value;
        env->String_NewUTF8(attribute.first.c_str(), attribute.first.length(), &key);
        env->String_NewUTF8(attribute.second.c_str(), attribute.second.length(), &value);
        if (ANI_OK !=
            env->Object_CallMethodByName_Void(object, "$_set", "Lstd/core/Object;Lstd/core/Object;:V", key, value)) {
            return nullptr;
        }
    }
    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructMediaInfo(std::shared_ptr<NWebMediaInfo> mediaInfo)
{
    WVLOG_D("ConstructMediaInfo");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }

    ani_object object = nullptr;
    if (!CreateObjectVoid(env, MWDIA_INFO_INNER, object)) {
        return nullptr;
    }

    ani_string embedId;
    std::string id = mediaInfo->GetEmbedId();
    env->String_NewUTF8(id.c_str(), id.length(), &embedId);
    ani_ref embedIdref = static_cast<ani_ref>(embedId);
    env->Object_SetPropertyByName_Ref(object, "embedID", embedIdref);

    ani_enum_item mediaTypeRef;
    if (GetEnumItemByIndex(env, ANI_ENUM_MEDIA_TYPE, static_cast<int32_t>(mediaInfo->GetMediaType()), mediaTypeRef)) {
        env->Object_SetPropertyByName_Ref(object, "mediaType", mediaTypeRef);
    }

    ani_object mediaSrcList = ConstructSourceInfos(mediaInfo->GetSourceInfos());
    ani_ref mediaSrcListRef = static_cast<ani_ref>(mediaSrcList);
    env->Object_SetPropertyByName_Ref(object, "mediaSrcList", mediaSrcListRef);

    ani_object surfaceInfo = ConstructSurfaceInfo(mediaInfo->GetSurfaceInfo());
    ani_ref surfaceInfoRef = static_cast<ani_ref>(surfaceInfo);
    env->Object_SetPropertyByName_Ref(object, "surfaceInfo", surfaceInfoRef);

    ani_boolean isControlsShown = static_cast<ani_boolean>(mediaInfo->GetIsControlsShown());
    env->Object_SetPropertyByName_Boolean(object, "controlsShown", isControlsShown);

    ani_object controlList = ConstructControls(mediaInfo->GetControls());
    ani_ref controlListRef = static_cast<ani_ref>(controlList);
    env->Object_SetPropertyByName_Ref(object, "controlList", controlListRef);

    ani_object headers = ConstructHeaders(mediaInfo->GetHeaders());
    ani_ref headersRef = static_cast<ani_ref>(headers);
    env->Object_SetPropertyByName_Ref(object, "headers", headersRef);

    ani_object attributes = ConstructAttributes(mediaInfo->GetAttributes());
    ani_ref attributesRef = static_cast<ani_ref>(attributes);
    env->Object_SetPropertyByName_Ref(object, "attributes", attributesRef);

    ani_boolean isMuted = static_cast<ani_boolean>(mediaInfo->GetIsMuted());
    env->Object_SetPropertyByName_Boolean(object, "muted", isMuted);

    ani_string posterUrl {};
    std::string url = mediaInfo->GetPosterUrl();
    env->String_NewUTF8(url.c_str(), url.length(), &posterUrl);
    ani_ref posterUrlref = static_cast<ani_ref>(posterUrl);
    env->Object_SetPropertyByName_Ref(object, "posterUrl", posterUrlref);

    ani_enum_item preloadRef;
    if (GetEnumItemByIndex(env, ANI_ENUM_PRELOAD, static_cast<int32_t>(mediaInfo->GetPreload()), preloadRef)) {
        env->Object_SetPropertyByName_Ref(object, "preload", preloadRef);
    }

    return object;
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructSourceInfos(
    const std::vector<std::shared_ptr<NWebMediaSourceInfo>>& sourceInfos)
{
    WVLOG_D("ConstructSourceInfos");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_class stringCls = nullptr;
    if (ANI_OK != env->FindClass(CLASS_MEDIA_SOURCE_INFO, &stringCls)) {
        WVLOG_E("FindClass failed");
    }
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefined failed");
    }
    ani_array_ref array;
    if (ANI_OK != env->Array_New_Ref(stringCls, sourceInfos.size(), undefinedRef, &array)) {
        WVLOG_E("27ConstructControlsfailed");
    }

    for (size_t i = 0; i < sourceInfos.size(); i++) {
        ani_object sourceInfo = nullptr;
        if (!CreateObjectVoid(env, CLASS_MEDIA_SOURCE_INFO, sourceInfo)) {
            return nullptr;
        }

        ani_enum_item typeRef;
        if (GetEnumItemByIndex(env, ANI_ENUM_SOURCE_TYPE, static_cast<int32_t>(sourceInfos[i]->GetType()), typeRef)) {
            env->Object_SetPropertyByName_Ref(sourceInfo, "type", typeRef);
        }

        auto sourceref = StringToAniStr(env, sourceInfos[i]->GetSource());
        env->Object_SetPropertyByName_Ref(sourceInfo, "source", sourceref);

        auto formatref = StringToAniStr(env, sourceInfos[i]->GetFormat());
        env->Object_SetPropertyByName_Ref(sourceInfo, "format", formatref);

        env->Array_Set_Ref(array, i, static_cast<ani_ref>(sourceInfo));
    }
    return static_cast<ani_object>(array);
}

ani_object NWebCreateNativeMediaPlayerCallbackImpl::ConstructSurfaceInfo(
    std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> surfaceInfo)
{
    WVLOG_D("ConstructSurfaceInfo");
    ani_env* env = nullptr;
    if (vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return nullptr;
    }
    ani_object object;
    if (!CreateObjectVoid(env, CLASS_NATIVE_MEDIA_PLAYER_SURFACE_INFO, object)) {
        return nullptr;
    }

    ani_string surfaceId {};
    std::string id = surfaceInfo->GetId();
    env->String_NewUTF8(id.c_str(), id.length(), &surfaceId);
    ani_ref surfaceIdref = static_cast<ani_ref>(surfaceId);
    env->Object_SetPropertyByName_Ref(object, "id", surfaceIdref);

    ani_object rect = ConstructRect(surfaceInfo);
    ani_ref rectRef = static_cast<ani_ref>(rect);
    env->Object_SetPropertyByName_Ref(object, "rect", rectRef);
    return object;
}
} // namespace OHOS::NWeb

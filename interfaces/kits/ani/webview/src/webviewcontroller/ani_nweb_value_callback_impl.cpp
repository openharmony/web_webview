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

#include "ani_nweb_value_callback_impl.h"

#include <securec.h>

#include "ani_parse_utils.h"
#include "ani_webview_controller.h"
#include "business_error.h"
#include "native_engine/native_engine.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "webview_controller.h"

namespace OHOS::NWeb {

namespace {
const std::string TASK_ID = "PostMessageTask";
static const char* ANI_WEB_MESSAGE_EXT_NAME = "@ohos.web.webview.webview.WebMessageExt";

bool Wrap(ani_env* env, const ani_object& object, const char* className, const ani_long& thisVar)
{
    if (env == nullptr) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return false;
    }
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
    if (env == nullptr) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return false;
    }
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
} // namespace

NWebValueCallbackImpl::NWebValueCallbackImpl(ani_vm* vm, ani_fn_object callback, bool extention)
    : vm_(vm), extention_(extention)
{
    if (vm_ == nullptr) {
        WVLOG_E("[WebMessagePort] vm_ is nullptr");
        return;
    }
    ani_env* env = nullptr;
    if (auto status = vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        WVLOG_E("[WebMessagePort] GetEnv status is : %{public}d", status);
        return;
    }
    env->GlobalReference_Create(static_cast<ani_ref>(callback), &callback_);
}

NWebValueCallbackImpl::~NWebValueCallbackImpl()
{
    if (vm_ == nullptr) {
        WVLOG_E("[WebMessagePort] vm_ is nullptr");
        return;
    }
    ani_env* env = nullptr;
    if (auto status = vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        WVLOG_E("[WebMessagePort] GetEnv status is : %{public}d", status);
        return;
    }
    env->GlobalReference_Delete(callback_);
}

void NWebValueCallbackImpl::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    WVLOG_D("[WebMessagePort] NWebValueCallbackImpl::OnReceiveValue() Start");
    ani_env* env = nullptr;
    ani_options aniArgs { 0, nullptr };
    if (auto status = vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env) != ANI_OK) {
        WVLOG_E("[WebMessagePort] GetEnv status is : %{public}d", status);
        return;
    }
    if (env == nullptr) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return;
    }
    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (!mainHandler_) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            return;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    if (mainHandler_ == nullptr) {
        WVLOG_E("[WebMessagePort] mainHandler_ is null.");
        return;
    }
    if (pthread_self() == engine->GetTid()) {
        NWebValueCallbackImpl::WebMessageCallback(env, result);
    } else {
        auto task = [this, result]() { NWebValueCallbackImpl::WebMessageOnReceiveValueCallback(result); };
        mainHandler_->PostTask(task, TASK_ID);
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this] { return ready_; });
        }
    }
    vm_->DetachCurrentThread();
}

void NWebValueCallbackImpl::WebMessageCallback(ani_env* env, std::shared_ptr<NWebMessage> result)
{
    WVLOG_D("[WebMessagePort] WebMessageCallback Start");
    if (env == nullptr) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return;
    }
    ani_object webMsgExt = nullptr;
    if (extention_) {
        g_WebMessagePort = false;
        if (!CreateObjectVoid(env, ANI_WEB_MESSAGE_EXT_NAME, webMsgExt)) {
            return;
        }
        g_WebMessagePort = true;
        if (webMsgExt == nullptr) {
            WVLOG_E("[WebMessagePort] new webMsgExt failed.");
            return;
        }
        WebMessageExt* webMessageExt = new (std::nothrow) WebMessageExt(result);
        if (webMessageExt == nullptr) {
            WVLOG_E("[WebMessagePort] new WebMessageExt failed.");
            return;
        }
        if (!Wrap(env, webMsgExt, ANI_WEB_MESSAGE_EXT_NAME, reinterpret_cast<ani_long>(webMessageExt))) {
            WVLOG_E("[WebMessagePort] webMessageExt wrap failed");
            delete webMessageExt;
            webMessageExt = nullptr;
            return;
        }
    } else {
        webMsgExt = static_cast<ani_object>(AniParseUtils::ConvertNWebToAniValue(env, result));
    }

    ani_ref placeHodler = nullptr;
    std::vector<ani_ref> vec;
    vec.push_back(static_cast<ani_ref>(webMsgExt));
    if (env->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(callback_), vec.size(), vec.data(), &placeHodler) !=
        ANI_OK) {
        WVLOG_E("[WebMessagePort] FunctionalObject_Call failed");
        return;
    }
}

void NWebValueCallbackImpl::WebMessageOnReceiveValueCallback(std::shared_ptr<NWebMessage> result)
{
    WVLOG_D("[WebMessagePort] WebMessageOnReceiveValueCallback Start");
    ani_env* env = nullptr;
    if (auto status = vm_->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        WVLOG_E("[WebMessagePort] GetEnv status is : %{public}d", status);
        return;
    }
    if (env == nullptr) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return;
    }
    ani_boolean errorExists;
    env->ExistUnhandledError(&errorExists);
    ani_object webMsgExt = nullptr;
    if (extention_) {
        g_WebMessagePort = false;
        if (!CreateObjectVoid(env, ANI_WEB_MESSAGE_EXT_NAME, webMsgExt)) {
            return;
        }
        g_WebMessagePort = true;
        if (webMsgExt == nullptr) {
            WVLOG_E("[WebMessagePort] new webMsgExt failed.");
            return;
        }
        WebMessageExt* webMessageExt = new (std::nothrow) WebMessageExt(result);
        if (webMessageExt == nullptr) {
            WVLOG_E("[WebMessagePort] new WebMessageExt failed.");
            return;
        }
        if (!Wrap(env, webMsgExt, ANI_WEB_MESSAGE_EXT_NAME, reinterpret_cast<ani_long>(webMessageExt))) {
            WVLOG_E("[WebMessagePort] webMessageExt wrap failed");
            delete webMessageExt;
            webMessageExt = nullptr;
            return;
        }
    } else {
        webMsgExt = static_cast<ani_object>(AniParseUtils::ConvertNWebToAniValue(env, result));
    }
    ani_ref placeHodler = nullptr;
    std::vector<ani_ref> vec;
    vec.push_back(webMsgExt);
    if (env->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(callback_), vec.size(), vec.data(), &placeHodler) !=
        ANI_OK) {
        WVLOG_E("[WebMessagePort] FunctionalObject_Call failed");
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    ready_ = true;
    condition_.notify_all();
}
} // namespace OHOS::NWeb
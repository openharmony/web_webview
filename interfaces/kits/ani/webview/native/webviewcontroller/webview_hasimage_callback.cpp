/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "webview_hasimage_callback.h"

#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"
#include "webview_controller.h"
#include "nweb_napi_scope.h"
#include "web_errors.h"

namespace OHOS::NWeb {
using namespace NWebError;

ani_ref createBooleanObject(ani_env* env, ani_boolean status)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    static constexpr const char* className = "std.core.Boolean";
    ani_status stat = ANI_ERROR;
    ani_class booleanCls {};
    stat = env->FindClass(className, &booleanCls);
    if (stat != ANI_OK) {
        WVLOG_E("createObjectBoolean findClass failed, status is : %{public}d", stat);
        return nullptr;
    }
    ani_method ctor {};
    stat = env->Class_FindMethod(booleanCls, "<ctor>", "z:", &ctor);
    if (stat != ANI_OK) {
        WVLOG_E("createObjectBoolean findMethod failed, status is : %{public}d", stat);
        return nullptr;
    }
    ani_object obj {};
    stat = env->Object_New(booleanCls, ctor, &obj, status);
    if (stat != ANI_OK) {
        WVLOG_E("createObjectBoolean failed, status is : %{public}d", stat);
        return nullptr;
    }
    return static_cast<ani_ref>(obj);
}

WebviewHasImageCallback::~WebviewHasImageCallback()
{
    if (vm_ == nullptr) {
        WVLOG_E("vm is nullptr");
        return;
    }
    ani_env* env = nullptr;
    auto status = vm_->GetEnv(ANI_VERSION_1, &env);
    if (status != ANI_OK) {
        WVLOG_E("GetEnv status is : %{public}d", status);
        return;
    }
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->GlobalReference_Delete(callback_);
}

void WebviewHasImageCallback::OnReceiveValue(bool result)
{
    WVLOG_D("WebviewHasImageCallback::OnReceiveValue() Start");
    if (vm_ == nullptr) {
        WVLOG_E("vm is nullptr");
        return;
    }

    WebviewHasImageCallback::AfterWorkCb(result);
    vm_->DetachCurrentThread();
}

void WebviewHasImageCallback::AfterWorkCb(bool result)
{
    WVLOG_D("AfterWorkCb Start");
    if (vm_ == nullptr) {
        WVLOG_E("vm is nullptr");
        return;
    }
    ani_env* env = nullptr;
    auto status = vm_->GetEnv(ANI_VERSION_1, &env);
    if (status != ANI_OK) {
        WVLOG_E("GetEnv status is : %{public}d", status);
        return;
    }
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    if (callback_) {
        WebviewHasImageCallback::AfterWorkCbAsync(env, result);
    } else if (deferred_) {
        WebviewHasImageCallback::AfterWorkCbPromise(env, result);
    } else {
        WVLOG_E("callback and deferred is failed");
        return;
    }
}

void WebviewHasImageCallback::AfterWorkCbAsync(ani_env* env, bool result)
{
    WVLOG_D("AfterWorkCbAsync Start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::vector<ani_ref> vec;
    ani_ref valueError = nullptr;
    ani_ref valueData = createBooleanObject(env, static_cast<ani_boolean>(result));
    auto status = env->GetNull(&valueError);
    if (status != ANI_OK) {
        WVLOG_E("get null failed, status is : %{public}d", status);
        return;
    }
    if (valueError == nullptr || valueData == nullptr) {
        WVLOG_E("valueError or valueData is nullptr");
        return;
    }
    vec.push_back(valueError);
    vec.push_back(valueData);
    ani_ref callbackResult = nullptr;
    auto stat = env->FunctionalObject_Call(
        reinterpret_cast<ani_fn_object>(callback_), vec.size(), vec.data(), &callbackResult);
    if (stat != ANI_OK) {
        WVLOG_E("FunctionalObject_Call failed, status is : %{public}d", stat);
        return;
    }
}

void WebviewHasImageCallback::AfterWorkCbPromise(ani_env* env, bool result)
{
    WVLOG_D("AfterWorkCbPromise Start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref setResult = createBooleanObject(env, static_cast<ani_boolean>(result));
    if (setResult == nullptr) {
        WVLOG_E("setResult is nullptr");
        return;
    }
    auto status = env->PromiseResolver_Resolve(deferred_, setResult);
    if (status != ANI_OK) {
        WVLOG_E("PromiseResolver_Resolve failed, status is : %{public}d", status);
        return;
    }
}
} // namespace NWeb
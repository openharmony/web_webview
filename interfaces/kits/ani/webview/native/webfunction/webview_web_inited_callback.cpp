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

#include "webview_web_inited_callback.h"

#include "uv.h"

namespace OHOS::NWeb {
constexpr ani_size REFERENCES_MAX_NUMBER = 16;
namespace {
void UvWebInitedCallbackThreadWoker(WebRunInitedCallbackImpl* obj)
{
    WVLOG_I("enter UvWebInitedCallbackThreadWoker");
    if (!obj || !(obj->param_)) {
        WVLOG_E("callback obj or param is nullptr");
        return;
    }

    ani_size nr_refs = REFERENCES_MAX_NUMBER;
    ani_env* env = obj->param_->GetEnv();
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->CreateLocalScope(nr_refs);
    ani_status status;
    if (obj->param_->webInitedCallback_) {
        ani_ref fnReturnVal;
        if ((status = env->FunctionalObject_Call(
                 static_cast<ani_fn_object>(obj->param_->webInitedCallback_), 0, {}, &fnReturnVal)) != ANI_OK) {
            WVLOG_E("UvWebInitedCallbackThreadWoker callback execute failed status : %{public}d", status);
            return;
        } else {
            WVLOG_I("UvWebInitedCallbackThreadWoker callback execute success!");
        }
    } else {
        WVLOG_E("callback is nullptr");
        env->DestroyLocalScope();
        return;
    }
    env->DestroyLocalScope();
    if (obj->param_) {
        delete obj->param_;
    }
    return;
}
} // namespace

WebInitedCallbackParam::WebInitedCallbackParam(ani_env* env, ani_ref callback)
{
    WVLOG_I("enter WebInitedCallbackParam");
    if (!env || !callback) {
        WVLOG_E("env or callback is nullptr");
        return;
    }
    env->GetVM(&vm_);
    env->GlobalReference_Create(callback, &webInitedCallback_);
}

WebInitedCallbackParam::~WebInitedCallbackParam()
{
    WVLOG_I("~WebInitedCallbackParam start");
    ani_env* env = GetEnv();
    if (env && webInitedCallback_) {
        env->GlobalReference_Delete(webInitedCallback_);
    } else {
        WVLOG_E("~WebInitedCallbackParam delete ref error");
    }
}

void WebRunInitedCallbackImpl::RunInitedCallback()
{
    WVLOG_I("enter RunInitedCallback");
    if (!param_->webInitedCallback_) {
        WVLOG_I("callback is null");
        return;
    }
    UvWebInitedCallbackThreadWoker(this);
    WVLOG_I("PostTask successful!");
}
} // namespace OHOS::NWeb
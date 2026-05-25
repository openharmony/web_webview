/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "webview_ai_page_command_callback.h"

#include <new>

#include "nweb_log.h"
#include "nweb_napi_scope.h"

namespace OHOS::NWeb {

void WebviewAIPageCommandCallback::OnReceiveValue(const std::string& result)
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        return;
    }

    auto* work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        return;
    }

    auto* param = new (std::nothrow) AIPageCommandParam();
    if (param == nullptr) {
        delete work;
        return;
    }
    param->env = env_;
    param->deferred = deferred_;
    param->result = result;
    work->data = reinterpret_cast<void*>(param);

    int ret = uv_queue_work_with_qos_internal(loop, work, [](uv_work_t*) {}, UvAfterWorkCb, uv_qos_user_initiated,
        "WebviewAIPageCommandCallback");
    if (ret != 0) {
        delete param;
        delete work;
    }
}

void WebviewAIPageCommandCallback::UvAfterWorkCb(uv_work_t* work, int status)
{
    (void)status;
    if (work == nullptr) {
        return;
    }
    auto* param = reinterpret_cast<AIPageCommandParam*>(work->data);
    if (param == nullptr) {
        delete work;
        return;
    }

    NApiScope scope(param->env);
    if (scope.IsVaild()) {
        ResolvePromise(param->env, param->deferred, param->result);
    }

    delete param;
    delete work;
}

void WebviewAIPageCommandCallback::ResolvePromise(napi_env env, napi_deferred deferred, const std::string& result)
{
    if (env == nullptr || deferred == nullptr) {
        return;
    }

    napi_value value = nullptr;
    napi_create_string_utf8(env, result.c_str(), result.length(), &value);
    napi_resolve_deferred(env, deferred, value);
}

} // namespace OHOS::NWeb

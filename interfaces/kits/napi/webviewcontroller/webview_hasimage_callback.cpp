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
#include "web_errors.h"

namespace OHOS::NWeb {
using namespace NWebError;

void WebviewHasImageCallback::OnReceiveValue(bool result)
{
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;

    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        return;
    }

    HasImageParam *param = new (std::nothrow) HasImageParam();
    if (param == nullptr) {
        delete work;
        return;
    }
    param->env_ = env_;
    param->callbackRef_ = callbackRef_;
    param->deferred_ = deferred_;
    param->result_ = result;

    work->data = reinterpret_cast<void*>(param);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvAfterWorkCb);
    if (ret != 0) {
        if (param != nullptr) {
            delete param;
            param = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}

void WebviewHasImageCallback::UvAfterWorkCb(uv_work_t* work, int status)
{
    (void)status;
    if (!work) {
        return;
    }
    HasImageParam *param = reinterpret_cast<HasImageParam*>(work->data);
    if (!param) {
        delete work;
        work = nullptr;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(param->env_, &scope);
    if (scope == nullptr) {
        return;
    }

    if (param->callbackRef_) {
        UvAfterWorkCbAsync(param->env_, param->callbackRef_, param->result_);
    } else if (param->deferred_) {
        UvAfterWorkCbPromise(param->env_, param->deferred_, param->result_);
    }

    napi_close_handle_scope(param->env_, scope);
    delete param;
    param = nullptr;
    delete work;
    work = nullptr;
}

void WebviewHasImageCallback::UvAfterWorkCbAsync(napi_env env, napi_ref callbackRef,
    bool result)
{
    napi_value setResult[INTEGER_TWO] = {0};
    napi_get_undefined(env, &setResult[INTEGER_ZERO]);
    napi_status getBooleanResult = napi_get_boolean(env, result, &setResult[INTEGER_ONE]);
    if (getBooleanResult != napi_ok) {
        napi_get_boolean(env, false, &setResult[INTEGER_ONE]);
    }
    napi_value args[INTEGER_TWO] = {setResult[INTEGER_ZERO], setResult[INTEGER_ONE]};
    napi_value callback = nullptr;
    napi_get_reference_value(env, callbackRef, &callback);
    napi_value callbackResult = nullptr;
    napi_call_function(env, nullptr, callback, INTEGER_TWO, args, &callbackResult);

    napi_delete_reference(env, callbackRef);
}

void WebviewHasImageCallback::UvAfterWorkCbPromise(napi_env env, napi_deferred deferred,
    bool result)
{
    napi_value setResult;
    napi_status getBooleanResult = napi_get_boolean(env, result, &setResult);
    if (getBooleanResult != napi_ok) {
        napi_get_boolean(env, false, &setResult);
    }
    napi_resolve_deferred(env, deferred, setResult);
}

} // namespace NWeb
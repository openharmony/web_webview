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

#include "nweb_log.h"
#include "uv.h"

namespace OHOS::NWeb {
void WebRunInitedCallbackImpl::RunInitedCallback()
{
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(param_->env_, &loop);

    if (loop == nullptr) {
        WVLOG_E("get uv event loop failed");
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("new uv work failed");
        return;
    }
    work->data = reinterpret_cast<void*>(param_);
    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int status) {
        if (work == nullptr) {
            WVLOG_E("uv work is null");
            return;
        }
        WebInitedCallbackParam *data = reinterpret_cast<WebInitedCallbackParam*>(work->data);
        if (data == nullptr) {
            delete work;
            work = nullptr;
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(data->env_, &scope);
        if (scope == nullptr) {
            return;
        }
        napi_value webInitedResult = nullptr;
        napi_value jsWebInitedCallback = nullptr;
        napi_get_reference_value(data->env_, data->webInitedCallback_, &jsWebInitedCallback);
        napi_call_function(data->env_, nullptr, jsWebInitedCallback, 0, {}, &webInitedResult);

        napi_close_handle_scope(data->env_, scope);
        delete data;
        data = nullptr;
        delete work;
        work = nullptr;
    });
    if (ret != 0) {
        if (param_ != nullptr) {
            delete param_;
            param_ = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}
}
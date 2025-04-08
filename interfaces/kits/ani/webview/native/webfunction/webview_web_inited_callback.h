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
#ifndef NWEB_WEBVIEW_WEB_INITED_CALLBACK_H
#define NWEB_WEBVIEW_WEB_INITED_CALLBACK_H

#include <condition_variable>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_parse_utils.h"
#include "nweb_value_callback.h"
#include "ohos_init_web_adapter.h"

namespace OHOS::NWeb {
class WebInitedCallbackParam {
public:
    WebInitedCallbackParam(napi_env env, napi_ref callback) : env_(env), webInitedCallback_(callback) {}

    napi_env env_;
    napi_ref webInitedCallback_;
};

class WebRunInitedCallbackImpl : public WebRunInitedCallback {
public:
    explicit WebRunInitedCallbackImpl(WebInitedCallbackParam *param) : param_(param) {}
    ~WebRunInitedCallbackImpl() override {}
    void RunInitedCallback() override;

    WebInitedCallbackParam *param_ = nullptr;
};
}
#endif

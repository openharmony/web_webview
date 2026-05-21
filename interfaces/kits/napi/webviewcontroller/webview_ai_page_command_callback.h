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

#ifndef NWEB_WEBVIEW_AI_PAGE_COMMAND_CALLBACK_H
#define NWEB_WEBVIEW_AI_PAGE_COMMAND_CALLBACK_H

#include <string>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nweb_value_callback.h"

namespace OHOS::NWeb {

class WebviewAIPageCommandCallback : public NWebStringValueCallback {
public:
    WebviewAIPageCommandCallback(napi_env env, napi_deferred deferred)
        : env_(env), deferred_(deferred)
    {}
    ~WebviewAIPageCommandCallback() = default;

    void OnReceiveValue(const std::string& result) override;

private:
    struct AIPageCommandParam {
        napi_env env = nullptr;
        napi_deferred deferred = nullptr;
        std::string result;
    };

    napi_env env_ = nullptr;
    napi_deferred deferred_ = nullptr;

    static void UvAfterWorkCb(uv_work_t* work, int status);
    static void ResolvePromise(napi_env env, napi_deferred deferred, const std::string& result);
};

} // namespace OHOS::NWeb

#endif // NWEB_WEBVIEW_AI_PAGE_COMMAND_CALLBACK_H

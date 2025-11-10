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

#ifndef OHOS_NWEB_SCHEME_HANDLER_H
#define OHOS_NWEB_SCHEME_HANDLER_H

#include <ani.h>
#include <string>
#include <uv.h>

#include "arkweb_scheme_handler.h"
#include "refbase.h"
#include "web_resource_handler.h"
#include "web_scheme_handler_request.h"
#include "web_scheme_handler_response.h"

namespace OHOS {
namespace NWeb {
constexpr ani_size REFERENCES_MAX_NUMBER = 16;

class WebSchemeHandler {
public:
    explicit WebSchemeHandler(ani_env* env);
    ~WebSchemeHandler();

    static const ArkWeb_SchemeHandler* GetArkWebSchemeHandler(WebSchemeHandler* handler);
    static WebSchemeHandler* GetWebSchemeHandler(const ArkWeb_SchemeHandler* handler);

    void RequestStart(
        ArkWeb_ResourceRequest* request, const ArkWeb_ResourceHandler* arkwebResourceHandler, bool* intercept);
    void RequestStop(const ArkWeb_ResourceRequest* resourceRequest);

    void PutRequestStart(ani_env* env, ani_vm* vm, ani_fn_object callback);
    void PutRequestStop(ani_env* env, ani_vm* vm, ani_fn_object callback);

    ani_ref delegate_ = nullptr;
    static std::unordered_map<WebSchemeHandler*, const ArkWeb_SchemeHandler*> webSchemeHandlerMap_;
    static std::unordered_map<const ArkWeb_SchemeHandler*, WebSchemeHandler*> arkWebSchemeHandlerMap_;
    ani_env* GetEnv()
    {
        ani_env* env = nullptr;
        if (vm_) {
            vm_->GetEnv(ANI_VERSION_1, &env);
        }
        return env;
    }

private:
    typedef struct RequestStopParam {
        ani_env* env_;
        ani_ref callbackRef_;
        WebSchemeHandlerRequest* request_;
        const ArkWeb_ResourceRequest* arkWebRequest_;
    } RequestStopParam;

    static void RequestStopAfterWorkCb(RequestStopParam* param);
    ani_env* env_ = nullptr;
    ani_vm* vm_ = nullptr;
    ArkWeb_OnRequestStart onRequestStart_ = nullptr;
    ArkWeb_OnRequestStop onRequestStop_ = nullptr;
    ani_ref request_start_callback_ = nullptr;
    ani_ref request_stop_callback_ = nullptr;
};

} // namespace NWeb
} // namespace OHOS
#endif // OHOS_NWEB_SCHEME_HANDLER_H
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

#ifndef OHOS_NWEB_SCHEME_HANDLER_REQUEST_H
#define OHOS_NWEB_SCHEME_HANDLER_REQUEST_H

#include <ani.h>
#include <string>
#include <uv.h>

#include "arkweb_scheme_handler.h"
#include "refbase.h"

namespace OHOS {
namespace NWeb {
constexpr ani_size REFERENCES_MAX_NUMBER = 16;

using WebHeaderList = std::vector<std::pair<std::string, std::string>>;
class WebSchemeHandlerRequest {
public:
    explicit WebSchemeHandlerRequest(ani_env* env);
    WebSchemeHandlerRequest(ani_env* env, const ArkWeb_ResourceRequest* request);
    ~WebSchemeHandlerRequest();

    char* GetRequestUrl();
    char* GetMethod();
    char* GetReferrer();
    bool IsRedirect();
    bool IsMainFrame();
    bool HasGesture();
    const WebHeaderList& GetHeader();
    ArkWeb_HttpBodyStream* GetHttpBodyStream();
    int32_t GetRequestResourceType();
    char* GetFrameUrl();

private:
    ani_vm* vm_ = nullptr;
    ani_env* env_;
    char* url_ = nullptr;
    char* method_ = nullptr;
    char* referrer_ = nullptr;
    bool isRedirect_ = false;
    bool isMainFrame_ = false;
    bool hasGesture_ = false;
    WebHeaderList headerList_;
    ArkWeb_HttpBodyStream* stream_ = nullptr;
    int32_t requestResourceType_ = -1;
    char* frameUrl_ = nullptr;
};

class WebResourceHandler : public RefBase {
public:
    explicit WebResourceHandler(ani_env* env);
    WebResourceHandler(ani_env* env, const ArkWeb_ResourceHandler* handler);
    ~WebResourceHandler();
    int32_t DidReceiveResponse(const ArkWeb_Response* response);
    int32_t DidReceiveResponseBody(const uint8_t* buffer, int64_t buflen);
    int32_t DidFinish();
    int32_t DidFailWithError(ArkWeb_NetError errorCode);
    void DestoryArkWebResourceHandler();
    void SetFinishFlag()
    {
        isFinished_ = true;
    }

private:
    ani_vm* vm_ = nullptr;
    bool isFinished_ = false;
    ArkWeb_ResourceHandler* handler_ = nullptr;
};

class WebSchemeHandler {
public:
    explicit WebSchemeHandler(ani_env* env);
    ~WebSchemeHandler();

    static const ArkWeb_SchemeHandler* GetArkWebSchemeHandler(WebSchemeHandler* handler);
    static WebSchemeHandler* GetWebSchemeHandler(const ArkWeb_SchemeHandler* handler);

    void RequestStart(
        ArkWeb_ResourceRequest* request, const ArkWeb_ResourceHandler* ArkWeb_ResourceHandler, bool* intercept);
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
#endif // OHOS_NWEB_ANI_SCHEME_HANDLER_REQUEST_H
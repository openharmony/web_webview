/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_WEB_SCHEME_HANDLER_H
#define NWEB_WEB_SCHEME_HANDLER_H

#include <string>
#include <unistd.h>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "arkweb_scheme_handler.h"
#include "refbase.h"
#include "web_resource_handler.h"
#include "web_scheme_handler_request.h"
#include "web_scheme_handler_response.h"

namespace OHOS {
namespace NWeb {

class WebSchemeHandler {
public:
    explicit WebSchemeHandler(napi_env env);
    ~WebSchemeHandler();

    static const ArkWeb_SchemeHandler* GetArkWebSchemeHandler(
        WebSchemeHandler* handler);
    static WebSchemeHandler* GetWebSchemeHandler(
        const ArkWeb_SchemeHandler* handler);

    void RequestStart(ArkWeb_ResourceRequest* request,
                      const ArkWeb_ResourceHandler* ArkWeb_ResourceHandler,
                      bool* intercept);
    void RequestStop(const ArkWeb_ResourceRequest* resourceRequest);

    void PutRequestStart(napi_env, napi_value callback);
    void PutRequestStop(napi_env, napi_value callback);
    void DeleteReference(WebSchemeHandler* schemehandler);

    napi_ref delegate_ = nullptr;
private:
    typedef struct RequestStopParam {
        napi_env env_;
        napi_ref callbackRef_;
        WebSchemeHandlerRequest* request_;
        const ArkWeb_ResourceRequest* arkWebRequest_;
    } RequestStopParam;

    static void RequestStopAfterWorkCb(uv_work_t* work, int status);

    napi_env env_;
    ArkWeb_OnRequestStart onRequestStart_ = nullptr;
    ArkWeb_OnRequestStop onRequestStop_ = nullptr;
    napi_ref request_start_callback_ = nullptr;
    napi_ref request_stop_callback_ = nullptr;
    pid_t thread_id_;
};

class WebHttpBodyStream {
public:
    explicit WebHttpBodyStream(napi_env env);
    static void HttpBodyStreamReadCallback(
        const ArkWeb_HttpBodyStream* httpBodyStream, uint8_t* buffer, int bytesRead);
    static void HttpBodyStreamInitCallback(
        const ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_NetError result);
    WebHttpBodyStream(napi_env env, ArkWeb_HttpBodyStream* stream);
    ~WebHttpBodyStream();
    void Init(napi_ref jsCallback, napi_deferred deferred);
    void Read(int bufLen, napi_ref jsCallback, napi_deferred deferred);
    uint64_t GetPostion() const;
    uint64_t GetSize() const;
    bool IsChunked() const;
    bool IsEof();
    bool IsInMemory();

    void ExecuteInit(ArkWeb_NetError result);
    void ExecuteRead(uint8_t* buffer, int bytesRead);
private:
    struct InitParam {
        napi_env env;
        napi_async_work asyncWork;
        napi_deferred deferred;
        napi_ref callbackRef;
        ArkWeb_NetError result;
    };

    struct ReadParam {
        napi_env env;
        napi_async_work asyncWork;
        napi_deferred deferred;
        napi_ref callbackRef;
        uint8_t* buffer = nullptr;
        int bytesRead = 0;
    };

    static void ExecuteReadComplete(napi_env env, napi_status status, void* data);
    static void ExecuteInitComplete(napi_env env, napi_status status, void* data);

    napi_ref initJsCallback_ = nullptr;
    napi_deferred initDeferred_ = nullptr;
    napi_ref readJsCallback_ = nullptr;
    napi_deferred readDeferred_ = nullptr;
    napi_env env_;
    ArkWeb_HttpBodyStream* stream_ = nullptr;
};
}
}
#endif

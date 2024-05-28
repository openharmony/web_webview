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

#ifndef NWEB_WEB_SCHEME_HANDLER_REQUEST_H
#define NWEB_WEB_SCHEME_HANDLER_REQUEST_H

#include <string>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "arkweb_scheme_handler.h"

namespace OHOS {
namespace NWeb {
using WebHeaderList = std::vector<std::pair<std::string, std::string>>;
class WebSchemeHandlerRequest {
public:
    explicit WebSchemeHandlerRequest(napi_env env);
    WebSchemeHandlerRequest(napi_env env, const ArkWeb_ResourceRequest* request);
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
    napi_env env_;
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

class WebSchemeHandlerResponse {
public:
    explicit WebSchemeHandlerResponse(napi_env env);
    WebSchemeHandlerResponse(napi_env env, ArkWeb_Response* response);
    ~WebSchemeHandlerResponse();

    char* GetUrl();
    int32_t SetUrl(const char* url);
    int32_t GetStatus();
    int32_t SetStatus(int32_t status);
    char* GetStatusText();
    int32_t SetStatusText(const char* statusText);
    char* GetMimeType();
    int32_t SetMimeType(const char* mimeType);
    char* GetEncoding();
    int32_t SetEncoding(const char* encoding);
    char* GetHeaderByName(const char* name);
    int32_t SetHeaderByName(const char* name, const char* value, bool overwrite);
    int32_t GetErrorCode();
    int32_t SetErrorCode(int32_t code);
    ArkWeb_Response* GetArkWebResponse()
    {
        return response_;
    }
private:
    napi_env env_;
    ArkWeb_Response* response_ = nullptr;
};

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

    napi_ref delegate_ = nullptr;
    static std::unordered_map<WebSchemeHandler*, const ArkWeb_SchemeHandler*>
        webSchemeHandlerMap_;
    static std::unordered_map<const ArkWeb_SchemeHandler*, WebSchemeHandler*>
        arkWebSchemeHandlerMap_;
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
};

class WebResourceHandler {
public:
    explicit WebResourceHandler(napi_env env);
    WebResourceHandler(napi_env env, const ArkWeb_ResourceHandler* handler);
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
    bool isFinished_ = false;
    ArkWeb_ResourceHandler* handler_ = nullptr;
    napi_env env_;
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
    uint64_t GetPostion();
    uint64_t GetSize();
    bool IsChunked();
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

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

#include "web_scheme_handler_request.h"

#include <securec.h>

#include "napi_web_scheme_handler_request.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"
#include "business_error.h"
#include "web_errors.h"

namespace OHOS::NWeb {
namespace {
void OnRequestStart(const ArkWeb_SchemeHandler* schemeHandler,
                    ArkWeb_ResourceRequest* resourceRequest,
                    const ArkWeb_ResourceHandler* resourceHandler,
                    bool* intercept)
{
    WVLOG_D("SchemeHandler OnRequestStart");
    if (!schemeHandler) {
        WVLOG_E("OnRequestStart schemeHandler is nullptr");
        return;
    }
    WebSchemeHandler* handler =
        WebSchemeHandler::GetWebSchemeHandler(schemeHandler);
    if (!handler) {
        WVLOG_E("GetWebSchemeHandler failed");
        return;
    }
    handler->RequestStart(resourceRequest, resourceHandler,
        intercept);
}

void OnRequestStop(const ArkWeb_SchemeHandler* schemeHandler,
                   const ArkWeb_ResourceRequest* resourceRequest)
{
    WVLOG_D("SchemeHandler OnRequestStop");
    if (!schemeHandler) {
        WVLOG_E("OnRequestStop schemeHandler is nullptr");
        return;
    }
    WebSchemeHandler* handler =
        WebSchemeHandler::GetWebSchemeHandler(schemeHandler);
    if (!handler) {
        WVLOG_E("GetWebSchemeHandler failed");
        return;
    }
    handler->RequestStop(resourceRequest);
}
}

WebSchemeHandlerRequest::WebSchemeHandlerRequest(napi_env env)
    : env_(env)
{
    WVLOG_D("WebSchemeHandlerRequest::WebSchemeHandlerRequest");
}

WebSchemeHandlerRequest::WebSchemeHandlerRequest(napi_env env,
    const ArkWeb_ResourceRequest* request)
{
    env_ = env;
    OH_ArkWebResourceRequest_GetUrl(request, &url_);
    OH_ArkWebResourceRequest_GetMethod(request, &method_);
    OH_ArkWebResourceRequest_GetReferrer(request, &referrer_);
    isRedirect_ = OH_ArkWebResourceRequest_IsRedirect(request);
    isMainFrame_ = OH_ArkWebResourceRequest_IsMainFrame(request);
    hasGesture_ = OH_ArkWebResourceRequest_HasGesture(request);
    OH_ArkWebResourceRequest_GetHttpBodyStream(request, &stream_);
    requestResourceType_ = OH_ArkWebResourceRequest_GetResourceType(request);
    OH_ArkWebResourceRequest_GetFrameUrl(request, &frameUrl_);

    ArkWeb_RequestHeaderList* arkWebHeaderlist = nullptr;
    OH_ArkWebResourceRequest_GetRequestHeaders(request, &arkWebHeaderlist);
    if (!arkWebHeaderlist) {
        WVLOG_E("OH_ArkWebRequestHeaderList_Create failed");
        return;
    }
    int32_t size = OH_ArkWebRequestHeaderList_GetSize(arkWebHeaderlist);
    if (size <= 0) {
        WVLOG_E("OH_ArkWebRequestHeaderList_GetSize:%{public}d", size);
        return;
    }
    for (int32_t index = 0; index < size; index++) {
        char* key;
        char* value;
        OH_ArkWebRequestHeaderList_GetHeader(arkWebHeaderlist, index, &key, &value);
        if (!key || !value) {
            continue;
        }
        std::string strKey(key);
        std::string strValue(value);
        headerList_.emplace_back(std::make_pair(strKey, strValue));
        OH_ArkWeb_ReleaseString(key);
        OH_ArkWeb_ReleaseString(value);
    }
    OH_ArkWebRequestHeaderList_Destroy(arkWebHeaderlist);
}

WebSchemeHandlerRequest::~WebSchemeHandlerRequest()
{
    WVLOG_D("WebSchemeHandlerRequest::~WebSchemeHandlerRequest");
    OH_ArkWeb_ReleaseString(url_);
    OH_ArkWeb_ReleaseString(method_);
    OH_ArkWeb_ReleaseString(referrer_);
}

char* WebSchemeHandlerRequest::GetRequestUrl()
{
    return url_;
}

char* WebSchemeHandlerRequest::GetMethod()
{
    return method_;
}

char* WebSchemeHandlerRequest::GetReferrer()
{
    return referrer_;
}

bool WebSchemeHandlerRequest::IsRedirect()
{
    return isRedirect_;
}

bool WebSchemeHandlerRequest::IsMainFrame()
{
    return isMainFrame_;
}

bool WebSchemeHandlerRequest::HasGesture()
{
    return hasGesture_;
}

const WebHeaderList& WebSchemeHandlerRequest::GetHeader()
{
    return headerList_;
}

ArkWeb_HttpBodyStream* WebSchemeHandlerRequest::GetHttpBodyStream()
{
    return stream_;
}

int32_t WebSchemeHandlerRequest::GetRequestResourceType()
{
    return requestResourceType_;
}

char* WebSchemeHandlerRequest::GetFrameUrl()
{
    return frameUrl_;
}

WebSchemeHandlerResponse::WebSchemeHandlerResponse(napi_env env)
    : env_(env)
{
    WVLOG_D("WebSchemeHandlerResponse::WebSchemeHandlerResponse");
    OH_ArkWeb_CreateResponse(&response_);
}

WebSchemeHandlerResponse::WebSchemeHandlerResponse(napi_env env,
    ArkWeb_Response* response)
    : env_(env), response_(response)
{
    WVLOG_D("WebSchemeHandlerResponse::WebSchemeHandlerResponse");
}

WebSchemeHandlerResponse::~WebSchemeHandlerResponse()
{
    WVLOG_I("WebSchemeHandlerResponse::~WebSchemeHandlerResponse");
    (void)env_;
    (void)response_;
    OH_ArkWeb_DestroyResponse(response_);
}

char* WebSchemeHandlerResponse::GetUrl()
{
    if (!response_) {
        WVLOG_E("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char* url;
    OH_ArkWebResponse_GetUrl(response_, &url);
    return url;
}

int32_t WebSchemeHandlerResponse::SetUrl(const char* url)
{
    return OH_ArkWebResponse_SetUrl(response_, url);
}

int32_t WebSchemeHandlerResponse::GetStatus()
{
    return OH_ArkWebResponse_GetStatus(response_);
}

int32_t WebSchemeHandlerResponse::SetStatus(int32_t status)
{
    return OH_ArkWebResponse_SetStatus(response_, status);
}

char* WebSchemeHandlerResponse::GetStatusText()
{
    if (!response_) {
        WVLOG_E("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char *statusText;
    OH_ArkWebResponse_GetStatusText(response_, &statusText);
    return statusText;
}

int32_t WebSchemeHandlerResponse::SetStatusText(const char* statusText)
{
    return OH_ArkWebResponse_SetStatusText(response_, statusText);
}

char* WebSchemeHandlerResponse::GetMimeType()
{
    if (!response_) {
        WVLOG_E("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char *mimeType;
    OH_ArkWebResponse_GetMimeType(response_, &mimeType);
    return mimeType;
}

int32_t WebSchemeHandlerResponse::SetMimeType(const char* mimeType)
{
    return OH_ArkWebResponse_SetMimeType(response_, mimeType);
}

char* WebSchemeHandlerResponse::GetEncoding()
{
    if (!response_) {
        WVLOG_E("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char *encoding;
    OH_ArkWebResponse_GetCharset(response_, &encoding);
    return encoding;
}

int32_t WebSchemeHandlerResponse::SetEncoding(const char* encoding)
{
    return OH_ArkWebResponse_SetCharset(response_, encoding);
}

char* WebSchemeHandlerResponse::GetHeaderByName(const char* name)
{
    if (!response_) {
        WVLOG_E("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char *value;
    OH_ArkWebResponse_GetHeaderByName(response_, name, &value);
    return value;
}

int32_t WebSchemeHandlerResponse::SetHeaderByName(
    const char* name, const char* value, bool overwrite)
{
    return OH_ArkWebResponse_SetHeaderByName(response_, name, value, overwrite);
}

int32_t WebSchemeHandlerResponse::GetErrorCode()
{
    return static_cast<int32_t>(OH_ArkWebResponse_GetError(response_));
}

int32_t WebSchemeHandlerResponse::SetErrorCode(int32_t code)
{
    return OH_ArkWebResponse_SetError(response_, static_cast<ArkWeb_NetError>(code));
}

std::unordered_map<WebSchemeHandler*, const ArkWeb_SchemeHandler*>
    WebSchemeHandler::webSchemeHandlerMap_;
std::unordered_map<const ArkWeb_SchemeHandler*, WebSchemeHandler*>
    WebSchemeHandler::arkWebSchemeHandlerMap_;

const ArkWeb_SchemeHandler* WebSchemeHandler::GetArkWebSchemeHandler(
    WebSchemeHandler* handler)
{
    return WebSchemeHandler::webSchemeHandlerMap_.find(handler) !=
        WebSchemeHandler::webSchemeHandlerMap_.end() ?
        WebSchemeHandler::webSchemeHandlerMap_[handler] : nullptr;
}

WebSchemeHandler* WebSchemeHandler::GetWebSchemeHandler(const ArkWeb_SchemeHandler* handler)
{
    return WebSchemeHandler::arkWebSchemeHandlerMap_.find(handler) !=
        WebSchemeHandler::arkWebSchemeHandlerMap_.end() ?
        WebSchemeHandler::arkWebSchemeHandlerMap_[handler] : nullptr;
}

WebSchemeHandler::WebSchemeHandler(napi_env env)
    : env_(env)
{
    ArkWeb_SchemeHandler* handler;
    OH_ArkWeb_CreateSchemeHandler(&handler);
    if (!handler) {
        WVLOG_E("create WebSchemeHandler failed");
        return;
    }
    onRequestStart_ = &OnRequestStart;
    onRequestStop_ = &OnRequestStop;
    OH_ArkWebSchemeHandler_SetOnRequestStart(handler, onRequestStart_);
    OH_ArkWebSchemeHandler_SetOnRequestStop(handler, onRequestStop_);
    OH_ArkWebSchemeHandler_SetFromEts(handler, true);
    webSchemeHandlerMap_.insert(std::make_pair(this, handler));
    arkWebSchemeHandlerMap_.insert(std::make_pair(handler, this));
}

WebSchemeHandler::~WebSchemeHandler()
{
    WVLOG_D("WebSchemeHandler::~WebSchemeHandler");
    napi_delete_reference(env_, request_start_callback_);
    napi_delete_reference(env_, request_stop_callback_);
    ArkWeb_SchemeHandler* handler =
        const_cast<ArkWeb_SchemeHandler*>(GetArkWebSchemeHandler(this));
    if (!handler) {
        WVLOG_E("~WebSchemeHandler not found ArkWeb_SchemeHandler");
        return;
    }
    webSchemeHandlerMap_.erase(this);
    arkWebSchemeHandlerMap_.erase(handler);
    OH_ArkWeb_DestroySchemeHandler(handler);
}

void WebSchemeHandler::RequestStart(ArkWeb_ResourceRequest* request,
                                    const ArkWeb_ResourceHandler* ArkWeb_ResourceHandler,
                                    bool* intercept)
{
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env_, &scope);
    if (!scope) {
        WVLOG_E("scheme handler RequestStart scope is nullptr");
        return;
    }
    
    WVLOG_D("WebSchemeHandler::RequestStart");
    size_t paramCount = 2;
    napi_value callbackFunc = nullptr;
    napi_status status;
    if (!request_start_callback_) {
        WVLOG_E("scheme handler onRequestStart nil env");
        return;
    }
    status = napi_get_reference_value(env_, request_start_callback_, &callbackFunc);
    if (status != napi_ok || callbackFunc == nullptr) {
        WVLOG_E("scheme handler get onRequestStart func failed.");
        return;
    }

    napi_value requestValue[2] = {0};
    napi_create_object(env_, &requestValue[0]);
    napi_create_object(env_, &requestValue[1]);
    WebSchemeHandlerRequest* schemeHandlerRequest = new (std::nothrow) WebSchemeHandlerRequest(env_, request);
    if (schemeHandlerRequest == nullptr) {
        WVLOG_E("RequestStart, new schemeHandlerRequest failed");
        return;
    }
    WebResourceHandler* resourceHandler = new (std::nothrow) WebResourceHandler(env_, ArkWeb_ResourceHandler);
    if (resourceHandler == nullptr) {
        WVLOG_E("RequestStart, new resourceHandler failed");
        delete schemeHandlerRequest;
        return;
    }
    if (OH_ArkWebResourceRequest_SetUserData(request, resourceHandler) != 0) {
        WVLOG_W("OH_ArkWebResourceRequest_SetUserData failed");
    }
    napi_wrap(
        env_, requestValue[0], schemeHandlerRequest,
        [](napi_env /* env */, void *data, void * /* hint */) {
            WebSchemeHandlerRequest *request = (WebSchemeHandlerRequest *)data;
            delete request;
        }, nullptr, nullptr);
    NapiWebSchemeHandlerRequest::DefineProperties(env_, &requestValue[0]);
    napi_wrap(
        env_, requestValue[1], resourceHandler,
        [](napi_env /* env */, void *data, void * /* hint */) {
            WebResourceHandler *resourceHandler = (WebResourceHandler *)data;
            delete resourceHandler;
        }, nullptr, nullptr);
    NapiWebResourceHandler::DefineProperties(env_, &requestValue[1]);
    napi_value result = nullptr;
    status = napi_call_function(
        env_, nullptr, callbackFunc, paramCount, requestValue, &result);
    if (status != napi_status::napi_ok) {
        WVLOG_W("scheme handler call onRequestStart failed.");
    }
    if (!NapiParseUtils::ParseBoolean(env_, result, *intercept)) {
        WVLOG_E("scheme handler onRequestStart intercept parse failed");
        *intercept = false;
    }
    if (!*intercept) {
        resourceHandler->SetFinishFlag();
    }
    napi_close_handle_scope(env_, scope);
}

void WebSchemeHandler::RequestStopAfterWorkCb(uv_work_t* work, int status)
{
    WVLOG_D("WebSchemeHandler::RequestStopAfterWorkCb");
    if (!work) {
        return;
    }
    RequestStopParam *param =
        reinterpret_cast<struct RequestStopParam*>(work->data);
    if (!param) {
        delete work;
        work = nullptr;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(param->env_, &scope);
    if (scope == nullptr) {
        delete param;
        delete work;
        return;
    }
    napi_value callbackFunc = nullptr;
    napi_status napiStatus;
    if (!param->callbackRef_) {
        WVLOG_E("scheme handler onRequestStop nil env");
        napi_close_handle_scope(param->env_, scope);
        delete param;
        delete work;
        return;
    }
    napiStatus = napi_get_reference_value(param->env_, param->callbackRef_, &callbackFunc);
    if (napiStatus != napi_ok || callbackFunc == nullptr) {
        WVLOG_E("scheme handler get onRequestStop func failed.");
        napi_close_handle_scope(param->env_, scope);
        delete param;
        delete work;
        return;
    }
    napi_value requestValue;
    napi_create_object(param->env_, &requestValue);
    napi_wrap(
        param->env_, requestValue, param->request_,
        [](napi_env /* env */, void *data, void * /* hint */) {
            WebSchemeHandlerRequest *request = (WebSchemeHandlerRequest *)data;
            delete request;
        },
        nullptr, nullptr);
    NapiWebSchemeHandlerRequest::DefineProperties(param->env_, &requestValue);
    napi_value result = nullptr;
    napiStatus = napi_call_function(
        param->env_, nullptr, callbackFunc, 1, &requestValue, &result);
    if (napiStatus != napi_status::napi_ok) {
        WVLOG_E("scheme handler call onRequestStop failed.");
    }
    WebResourceHandler* resourceHandler =
        reinterpret_cast<WebResourceHandler*>(
            OH_ArkWebResourceRequest_GetUserData(param->arkWebRequest_));
    if (resourceHandler) {
        resourceHandler->SetFinishFlag();
    }
    napi_close_handle_scope(param->env_, scope);
    delete param;
    param = nullptr;
    delete work;
    work = nullptr;
}

void WebSchemeHandler::RequestStop(const ArkWeb_ResourceRequest* resourceRequest)
{
    WVLOG_D("WebSchemeHandler::RequestStop");
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
    RequestStopParam *param = new (std::nothrow) RequestStopParam();
    if (param == nullptr) {
        delete work;
        return;
    }
    param->env_ = env_;
    param->callbackRef_ = request_stop_callback_;
    param->request_ = new (std::nothrow) WebSchemeHandlerRequest(param->env_, resourceRequest);
    if (param->request_ == nullptr) {
        delete work;
        delete param;
        return;
    }
    param->arkWebRequest_ = resourceRequest;
    work->data = reinterpret_cast<void*>(param);
    int ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {},
        RequestStopAfterWorkCb, uv_qos_user_initiated);
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

void WebSchemeHandler::PutRequestStart(napi_env, napi_value callback)
{
    WVLOG_D("WebSchemeHandler::PutRequestStart");
    napi_status status = napi_create_reference(env_, callback, 1, &request_start_callback_);
    if (status != napi_status::napi_ok) {
        WVLOG_E("PutRequestStart create reference failed.");
    }
}
void WebSchemeHandler::PutRequestStop(napi_env, napi_value callback)
{
    WVLOG_D("WebSchemeHandler::PutRequestStop");
    napi_status status = napi_create_reference(env_, callback, 1, &request_stop_callback_);
    if (status != napi_status::napi_ok) {
        WVLOG_E("PutRequestStop create reference failed.");
    }
}

WebResourceHandler::WebResourceHandler(napi_env env)
    : env_(env)
{
    WVLOG_D("create WebResourceHandler");
}

WebResourceHandler::WebResourceHandler(napi_env env, const ArkWeb_ResourceHandler* handler)
    : handler_(const_cast<ArkWeb_ResourceHandler*>(handler))
{
    WVLOG_D("create WebResourceHandler");
    env_ = env;
}

WebResourceHandler::~WebResourceHandler()
{
    WVLOG_D("~WebResourceHandler");
}

int32_t WebResourceHandler::DidReceiveResponse(const ArkWeb_Response* response)
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    return OH_ArkWebResourceHandler_DidReceiveResponse(handler_, response);
}

int32_t WebResourceHandler::DidReceiveResponseBody(const uint8_t* buffer, int64_t buflen)
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    return OH_ArkWebResourceHandler_DidReceiveData(handler_, buffer, buflen);
}

int32_t WebResourceHandler::DidFinish()
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    int32_t ret = OH_ArkWebResourceHandler_DidFinish(handler_);
    if (ret == 0) {
        isFinished_ = true;
    }
    return ret;
}

int32_t WebResourceHandler::DidFailWithError(ArkWeb_NetError errorCode)
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    int32_t ret = OH_ArkWebResourceHandler_DidFailWithError(handler_, errorCode);
    if (ret == 0) {
        isFinished_ = true;
    }
    return ret;
}

void WebResourceHandler::DestoryArkWebResourceHandler()
{
    if (handler_) {
        OH_ArkWebResourceHandler_Destroy(handler_);
        handler_ = nullptr;
    }
}

WebHttpBodyStream::WebHttpBodyStream(napi_env env)
{
    WVLOG_D("WebHttpBodyStream::WebHttpBodyStream");
    env_ = env;
}

WebHttpBodyStream::WebHttpBodyStream(napi_env env,
    ArkWeb_HttpBodyStream* stream)
{
    WVLOG_D("WebHttpBodyStream::WebHttpBodyStream");
    env_ = env;
    stream_ = stream;
    if (OH_ArkWebHttpBodyStream_SetUserData(stream_, this) != 0) {
        WVLOG_E("OH_ArkWebHttpBodyStream_SetUserData failed");
        return;
    }
    if (OH_ArkWebHttpBodyStream_SetReadCallback(stream_,
        &WebHttpBodyStream::HttpBodyStreamReadCallback) != 0) {
        WVLOG_E("OH_ArkWebHttpBodyStream_SetReadCallback failed");
        return;
    }
}

WebHttpBodyStream::~WebHttpBodyStream()
{
    WVLOG_D("WebHttpBodyStream::~WebHttpBodyStream");
    if (!stream_) {
        OH_ArkWebResourceRequest_DestroyHttpBodyStream(stream_);
        stream_ = nullptr;
    }
}

void WebHttpBodyStream::HttpBodyStreamReadCallback(
    const ArkWeb_HttpBodyStream* httpBodyStream, uint8_t* buffer, int bytesRead)
{
    WVLOG_D("WebHttpBodyStream::HttpBodyStreamReadCallback");
    WebHttpBodyStream* stream = reinterpret_cast<WebHttpBodyStream*>(
        OH_ArkWebHttpBodyStream_GetUserData(httpBodyStream));
    if (!stream) {
        WVLOG_E("OH_ArkWebHttpBodyStream_GetUserData is nullptr");
        return;
    }
    stream->ExecuteRead(buffer, bytesRead);
}

void WebHttpBodyStream::HttpBodyStreamInitCallback(
    const ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_NetError result)
{
    WVLOG_D("WebHttpBodyStream::HttpBodyStreamInitCallback");
    WebHttpBodyStream* stream = reinterpret_cast<WebHttpBodyStream*>(
        OH_ArkWebHttpBodyStream_GetUserData(httpBodyStream));
    if (!stream) {
        WVLOG_E("OH_ArkWebHttpBodyStream_GetUserData is nullptr");
        return;
    }
    stream->ExecuteInit(result);
}

void WebHttpBodyStream::Init(napi_ref jsCallback, napi_deferred deferred)
{
    if (!jsCallback && !deferred) {
        WVLOG_E("WebHttpBodyStream::InitCallback callback is nullptr");
        return;
    }
    if (jsCallback) {
        initJsCallback_ = std::move(jsCallback);
    }
    if (deferred) {
        initDeferred_ = std::move(deferred);
    }
    int ret = OH_ArkWebHttpBodyStream_Init(stream_,
        &WebHttpBodyStream::HttpBodyStreamInitCallback);
    if (ret != 0) {
        WVLOG_E("OH_ArkWebHttpBodyStream_Init failed");
        return;
    }
}

void WebHttpBodyStream::Read(int bufLen, napi_ref jsCallback, napi_deferred deferred)
{
    WVLOG_D("WebHttpBodyStream::Read");
    if (!jsCallback && !deferred) {
        WVLOG_E("WebHttpBodyStream::Read callback is nullptr");
        return;
    }
    if (bufLen <= 0) {
        return;
    }
    if (jsCallback) {
        readJsCallback_ = std::move(jsCallback);
    }
    if (deferred) {
        readDeferred_ = std::move(deferred);
    }
    uint8_t* buffer = new (std::nothrow) uint8_t[bufLen];
    if (buffer == nullptr) {
        return;
    }
    OH_ArkWebHttpBodyStream_Read(stream_, buffer, bufLen);
}

void WebHttpBodyStream::ExecuteInit(ArkWeb_NetError result)
{
    WVLOG_D("WebHttpBodyStream::ExecuteInit");
    if (!env_) {
        return ;
    }
    InitParam *param = new (std::nothrow) InitParam {
        .env = env_,
        .asyncWork = nullptr,
        .deferred = initDeferred_,
        .callbackRef = initJsCallback_,
        .result = result,
    };
    if (param == nullptr) {
        return;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL_RETURN_VOID(env_, napi_create_async_work(env_, nullptr, resourceName,
        [](napi_env env, void *data) {},
        ExecuteInitComplete, static_cast<void *>(param), &param->asyncWork));
    NAPI_CALL_RETURN_VOID(env_, napi_queue_async_work_with_qos(env_, param->asyncWork, napi_qos_user_initiated));
}

void WebHttpBodyStream::ExecuteInitComplete(napi_env env, napi_status status, void* data)
{
    WVLOG_D("WebHttpBodyStream::ExecuteInitComplete");
    InitParam* param = static_cast<InitParam*>(data);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (!param) {
        return;
    }
    if (!scope) {
        delete param;
        return;
    }
    napi_value result[INTEGER_ONE] = {0};
    if (param->result != 0) {
        result[INTEGER_ZERO] = NWebError::BusinessError::CreateError(
            env, NWebError::HTTP_BODY_STREAN_INIT_FAILED);
    } else {
        napi_get_null(env, &result[INTEGER_ZERO]);
    }
    if (param->callbackRef) {
        napi_value callback = nullptr;
        napi_get_reference_value(env, param->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, INTEGER_ONE, &result[INTEGER_ZERO], nullptr);
        napi_delete_reference(env, param->callbackRef);
    } else if (param->deferred) {
        if (param->result != 0) {
            napi_reject_deferred(env, param->deferred, result[INTEGER_ZERO]);
        } else {
            napi_resolve_deferred(env, param->deferred, result[INTEGER_ZERO]);
        }
    }
    napi_delete_async_work(env, param->asyncWork);
    napi_close_handle_scope(env, scope);
    delete param;
}

void WebHttpBodyStream::ExecuteReadComplete(napi_env env, napi_status status, void* data)
{
    WVLOG_D("WebHttpBodyStream::ExecuteReadComplete");
    ReadParam* param = static_cast<ReadParam*>(data);
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (!param) {
        return;
    } 
    if (!scope) {
        delete param;
        return;
    }
    napi_value result[INTEGER_ONE] = {0};
    void *bufferData = nullptr;
    napi_create_arraybuffer(env, param->bytesRead, &bufferData, &result[INTEGER_ZERO]);
    if (memcpy_s(bufferData, param->bytesRead, param->buffer, param->bytesRead) != 0 &&
        param->bytesRead > 0) {
        WVLOG_W("WebHttpBodyStream::ExecuteRead memcpy failed");
    }
    if (param->buffer) {
        delete param->buffer;
    }
    if (param->callbackRef) {
        napi_value callback = nullptr;
        napi_get_reference_value(env, param->callbackRef, &callback);
        napi_call_function(env, nullptr, callback, INTEGER_ONE, &result[INTEGER_ZERO], nullptr);
        napi_delete_reference(env, param->callbackRef);
    } else if (param->deferred) {
        napi_resolve_deferred(env, param->deferred, result[INTEGER_ZERO]);
    }
    napi_delete_async_work(env, param->asyncWork);
    napi_close_handle_scope(env, scope);
    delete param;
}

void WebHttpBodyStream::ExecuteRead(uint8_t* buffer, int bytesRead)
{
    if (!env_) {
        return;
    }
    ReadParam *param = new (std::nothrow) ReadParam {
        .env = env_,
        .asyncWork = nullptr,
        .deferred = readDeferred_,
        .callbackRef = readJsCallback_,
        .buffer = buffer,
        .bytesRead = bytesRead,
    };
    if (param == nullptr) {
        return;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL_RETURN_VOID(env_, napi_create_string_utf8(env_, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL_RETURN_VOID(env_, napi_create_async_work(env_, nullptr, resourceName,
        [](napi_env env, void *data) {},
        ExecuteReadComplete, static_cast<void *>(param), &param->asyncWork));
    NAPI_CALL_RETURN_VOID(env_, 
        napi_queue_async_work_with_qos(env_, param->asyncWork, napi_qos_user_initiated));
}

uint64_t WebHttpBodyStream::GetPostion()
{
    return OH_ArkWebHttpBodyStream_GetPosition(stream_);
}

uint64_t WebHttpBodyStream::GetSize()
{
    return OH_ArkWebHttpBodyStream_GetSize(stream_);
}

bool WebHttpBodyStream::IsChunked()
{
    return OH_ArkWebHttpBodyStream_IsChunked(stream_);
}

bool WebHttpBodyStream::IsEof()
{
    return OH_ArkWebHttpBodyStream_IsEof(stream_);
}

bool WebHttpBodyStream::IsInMemory()
{
    return OH_ArkWebHttpBodyStream_IsInMemory(stream_);
}
}

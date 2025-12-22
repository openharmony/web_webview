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

#include "web_scheme_handler.h"

#include <securec.h>
#include <mutex>

#include "napi_web_scheme_handler_request.h"
#include "napi_parse_utils.h"
#include "nweb_napi_scope.h"
#include "nweb_log.h"
#include "business_error.h"
#include "web_errors.h"

namespace OHOS::NWeb {
namespace {

std::unordered_map<WebSchemeHandler*, const ArkWeb_SchemeHandler*>
    g_web_scheme_handler_map;
std::unordered_map<const ArkWeb_SchemeHandler*, WebSchemeHandler*>
    g_ark_web_scheme_handler_map;
std::mutex g_mutex_for_handler_map;

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

const ArkWeb_SchemeHandler* WebSchemeHandler::GetArkWebSchemeHandler(
    WebSchemeHandler* handler)
{
    std::lock_guard<std::mutex> auto_lock(g_mutex_for_handler_map);
    auto iter = g_web_scheme_handler_map.find(handler);
    if (iter == g_web_scheme_handler_map.end()) {
        return nullptr;
    }
    return iter->second;
}

WebSchemeHandler* WebSchemeHandler::GetWebSchemeHandler(const ArkWeb_SchemeHandler* handler)
{
    std::lock_guard<std::mutex> auto_lock(g_mutex_for_handler_map);
    auto iter = g_ark_web_scheme_handler_map.find(handler);
    if (iter == g_ark_web_scheme_handler_map.end()) {
        return nullptr;
    }
    return iter->second;
}

WebSchemeHandler::WebSchemeHandler(napi_env env)
    : env_(env),
      thread_id_(gettid())
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

    {
        std::lock_guard<std::mutex> auto_lock(g_mutex_for_handler_map);
        g_web_scheme_handler_map.insert(std::make_pair(this, handler));
        g_ark_web_scheme_handler_map.insert(std::make_pair(handler, this));
    }
}

WebSchemeHandler::~WebSchemeHandler()
{
    WVLOG_D("WebSchemeHandler::~WebSchemeHandler");
    pid_t current_tid = gettid();
    if (current_tid != thread_id_) {
        WVLOG_E("~WebSchemeHandler is in wrong thread! %{public}d != %{public}d",
                current_tid, thread_id_);
    }
    napi_delete_reference(env_, request_start_callback_);
    napi_delete_reference(env_, request_stop_callback_);
    ArkWeb_SchemeHandler* handler =
        const_cast<ArkWeb_SchemeHandler*>(GetArkWebSchemeHandler(this));
    if (!handler) {
        WVLOG_E("~WebSchemeHandler not found ArkWeb_SchemeHandler");
        return;
    }
    {
        std::lock_guard<std::mutex> auto_lock(g_mutex_for_handler_map);
        g_web_scheme_handler_map.erase(this);
        g_ark_web_scheme_handler_map.erase(handler);
    }
    OH_ArkWeb_DestroySchemeHandler(handler);
}

void WebSchemeHandler::RequestStart(ArkWeb_ResourceRequest* request,
                                    const ArkWeb_ResourceHandler* ArkWeb_ResourceHandler,
                                    bool* intercept)
{
    NApiScope scope(env_);
    if (!scope.IsVaild()) {
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
    WebSchemeHandlerRequest* schemeHandlerRequest = new (std::nothrow) WebSchemeHandlerRequest(request);
    if (schemeHandlerRequest == nullptr) {
        WVLOG_E("RequestStart, new schemeHandlerRequest failed");
        return;
    }
    sptr<WebResourceHandler> resourceHandler = new (std::nothrow) WebResourceHandler(ArkWeb_ResourceHandler);
    if (resourceHandler == nullptr) {
        WVLOG_E("RequestStart, new resourceHandler failed");
        delete schemeHandlerRequest;
        return;
    }
    if (OH_ArkWebResourceRequest_SetUserData(request, resourceHandler.GetRefPtr()) != 0) {
        WVLOG_W("OH_ArkWebResourceRequest_SetUserData failed");
    } else {
        resourceHandler->IncStrongRef(nullptr);
    }
    napi_wrap(
        env_, requestValue[0], schemeHandlerRequest,
        [](napi_env /* env */, void *data, void * /* hint */) {
            WebSchemeHandlerRequest *request = (WebSchemeHandlerRequest *)data;
            delete request;
        }, nullptr, nullptr);
    NapiWebSchemeHandlerRequest::DefineProperties(env_, &requestValue[0]);
    napi_wrap(
        env_, requestValue[1], resourceHandler.GetRefPtr(),
        [](napi_env /* env */, void *data, void * /* hint */) {
            static_cast<WebResourceHandler*>(data)->DecStrongRef(data);
        }, nullptr, nullptr);
    NapiWebResourceHandler::DefineProperties(env_, &requestValue[1]);
    resourceHandler->IncStrongRef(nullptr);
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
        resourceHandler->DecStrongRef(resourceHandler);
    }
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
    NApiScope scope(param->env_);
    if (!scope.IsVaild()) {
        delete param;
        delete work;
        return;
    }
    napi_value callbackFunc = nullptr;
    napi_status napiStatus;
    if (!param->callbackRef_) {
        WVLOG_E("scheme handler onRequestStop nil env");
        delete param;
        delete work;
        return;
    }
    napiStatus = napi_get_reference_value(param->env_, param->callbackRef_, &callbackFunc);
    if (napiStatus != napi_ok || callbackFunc == nullptr) {
        WVLOG_E("scheme handler get onRequestStop func failed.");
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
        resourceHandler->DecStrongRef(resourceHandler);
    }
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
    param->request_ = new (std::nothrow) WebSchemeHandlerRequest(resourceRequest);
    if (param->request_ == nullptr) {
        delete work;
        delete param;
        return;
    }
    param->arkWebRequest_ = resourceRequest;
    work->data = reinterpret_cast<void*>(param);
    int ret = uv_queue_work_with_qos_internal(loop, work, [](uv_work_t *work) {},
        RequestStopAfterWorkCb, uv_qos_user_initiated, "WebviewWebSchemeHandler");
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

void WebSchemeHandler::DeleteReference(WebSchemeHandler* schemehandler)
{
    ArkWeb_SchemeHandler* handler =
        const_cast<ArkWeb_SchemeHandler*>(GetArkWebSchemeHandler(schemehandler));
    if (handler && schemehandler->delegate_) {
        napi_delete_reference(schemehandler->env_, schemehandler->delegate_);
        schemehandler->delegate_ = nullptr;
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
    if (stream_) {
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
    if (!stream_) {
        delete[] buffer;
        buffer = nullptr;
        return;
    }
    OH_ArkWebHttpBodyStream_Read(stream_, buffer, bufLen);
}

void WebHttpBodyStream::DeleteInitJsCallbackRef()
{
    if (!env_) {
        return;
    }
    if (initJsCallback_) {
        napi_delete_reference(env_, initJsCallback_);
        initJsCallback_ = nullptr;
    }
}

void WebHttpBodyStream::ExecuteInit(ArkWeb_NetError result)
{
    WVLOG_D("WebHttpBodyStream::ExecuteInit");
    if (!env_) {
        DeleteInitJsCallbackRef();
        return;
    }
    NApiScope scope(env_);
    if (!scope.IsVaild()) {
        DeleteInitJsCallbackRef();
        return;
    }
    InitParam *param = new (std::nothrow) InitParam {
        .env = env_,
        .asyncWork = nullptr,
        .deferred = initDeferred_,
        .callbackRef = initJsCallback_,
        .result = result,
    };
    if (param == nullptr) {
        DeleteInitJsCallbackRef();
        return;
    }
    napi_value resourceName = nullptr;
    if (napi_create_string_utf8(env_, __func__, NAPI_AUTO_LENGTH, &resourceName) != napi_status::napi_ok) {
        DeleteInitJsCallbackRef();
        delete param;
        return;
    }
    if (napi_create_async_work(env_, nullptr, resourceName,
        [](napi_env env, void *data) {},
        ExecuteInitComplete, static_cast<void *>(param), &param->asyncWork) != napi_status::napi_ok) {
        DeleteInitJsCallbackRef();
        delete param;
        return;
    }
    if (napi_queue_async_work_with_qos(env_, param->asyncWork, napi_qos_user_initiated) != napi_status::napi_ok) {
        napi_delete_async_work(env_, param->asyncWork);
        DeleteInitJsCallbackRef();
        delete param;
    }
}

void WebHttpBodyStream::ExecuteInitComplete(napi_env env, napi_status status, void* data)
{
    WVLOG_D("WebHttpBodyStream::ExecuteInitComplete");
    InitParam* param = static_cast<InitParam*>(data);
    if (!param) {
        return;
    }
    NApiScope scope(env);
    if (!scope.IsVaild()) {
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
    delete param;
}

void WebHttpBodyStream::ExecuteReadComplete(napi_env env, napi_status status, void* data)
{
    WVLOG_D("WebHttpBodyStream::ExecuteReadComplete");
    ReadParam* param = static_cast<ReadParam*>(data);
    if (!param) {
        return;
    }
    NApiScope scope(env);
    if (!scope.IsVaild()) {
        if (param->buffer) {
            delete param->buffer;
        }
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
    delete param;
}

void WebHttpBodyStream::DeleteReadJsCallbackRef()
{
    if (!env_) {
        return;
    }
    if (readJsCallback_) {
        napi_delete_reference(env_, readJsCallback_);
        readJsCallback_ = nullptr;
    }
}

void WebHttpBodyStream::ExecuteRead(uint8_t* buffer, int bytesRead)
{
    if (!env_) {
        DeleteReadJsCallbackRef();
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
        DeleteReadJsCallbackRef();
        return;
    }
    napi_value resourceName = nullptr;
    if (napi_create_string_utf8(env_, __func__, NAPI_AUTO_LENGTH, &resourceName) != napi_status::napi_ok) {
        DeleteReadJsCallbackRef();
        delete param;
        return;
    }
    if (napi_create_async_work(env_, nullptr, resourceName,
        [](napi_env env, void *data) {},
        ExecuteReadComplete, static_cast<void *>(param), &param->asyncWork) != napi_status::napi_ok) {
        DeleteReadJsCallbackRef();
        delete param;
        return;
    }
    if (napi_queue_async_work_with_qos(env_, param->asyncWork, napi_qos_user_initiated) != napi_status::napi_ok) {
        napi_delete_async_work(env_, param->asyncWork);
        DeleteReadJsCallbackRef();
        delete param;
    }
}


uint64_t WebHttpBodyStream::GetPostion() const
{
    return OH_ArkWebHttpBodyStream_GetPosition(stream_);
}

uint64_t WebHttpBodyStream::GetSize() const
{
    return OH_ArkWebHttpBodyStream_GetSize(stream_);
}

bool WebHttpBodyStream::IsChunked() const
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

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <sstream>
#include <string>

#include "business_error.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS::NWeb {
const char* WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME = "L@ohos/web/webview/webview/WebSchemeHandlerRequest;";
const char* WEB_RESOURCE_HANDLER_CLASS_NAME = "L@ohos/web/webview/webview/WebResourceHandler;";
namespace {
const std::string TASK_ID = "PostMessageTask";

bool Wrap(ani_env* env, const ani_object& object, const char* className, const ani_long& thisVar)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_status status;
    ani_class cls;
    if ((status = env->FindClass(className, &cls)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap FindClass status: %{public}d", status);
        return false;
    }
    ani_method innerWrapMethod;
    if ((status = env->Class_FindMethod(cls, "bindNativePtr", "J:V", &innerWrapMethod)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Class_FindMethod status: %{public}d", status);
        return false;
    }
    if ((status = env->Object_CallMethod_Void(object, innerWrapMethod, thisVar)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Object_CallMethod_Void status: %{public}d", status);
        return false;
    }
    return true;
}

bool CreateObjectVoid(ani_env *env, const char *className, ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_status status = env->FindClass(className, &cls);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", className, status);
        return false;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) != ANI_OK) {
        WVLOG_E("get %{public}s ctor method failed, status: %{public}d", className, status);
        return false;
    }
    if ((status = env->Object_New(cls, ctor, &object)) != ANI_OK) {
        WVLOG_E("new %{public}s failed, status: %{public}d", className, status);
        return false;
    }
    return true;
}

bool ParseBoolean(ani_env* env, ani_ref ref, bool& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class booleanClass;
    ani_status status = env->FindClass("Lstd/core/Boolean;", &booleanClass);
    if (status != ANI_OK) {
        WVLOG_E("ParseBoolean FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isBoolean;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), booleanClass, &isBoolean) != ANI_OK ||
        isBoolean != ANI_TRUE) {
        WVLOG_E("ParseBoolean failed - invalid boolean type");
        return false;
    }

    ani_boolean boolValue;
    env->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "unboxed", ":Z", &boolValue);
    outValue = static_cast<bool>(boolValue);
    return true;
}

ani_object WrapBusinessError(ani_env* env, const std::string& msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        WVLOG_E("null env");
        return nullptr;
    }

    ani_string aniMsg = nullptr;
    if ((status = env->String_NewUTF8(msg.c_str(), msg.size(), &aniMsg)) != ANI_OK) {
        WVLOG_E("String_NewUTF8 failed %{public}d", status);
        return nullptr;
    }

    ani_ref undefRef;
    if ((status = env->GetUndefined(&undefRef)) != ANI_OK) {
        WVLOG_E("GetUndefined failed %{public}d", status);
        return nullptr;
    }

    if ((status = env->FindClass("Lescompat/Error;", &cls)) != ANI_OK) {
        WVLOG_E("FindClass failed %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;Lescompat/ErrorOptions;:V", &method)) !=
        ANI_OK) {
        WVLOG_E("Class_FindMethod failed %{public}d", status);
        return nullptr;
    }

    if ((status = env->Object_New(cls, method, &obj, aniMsg, undefRef)) != ANI_OK) {
        WVLOG_E("Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}

ani_object CreateBusinessError(ani_env* env, ani_int code, const std::string& msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        WVLOG_E("null env");
        return nullptr;
    }
    if ((status = env->FindClass("L@ohos/base/BusinessError;", &cls)) != ANI_OK) {
        WVLOG_E("FindClass failed %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "ILescompat/Error;:V", &method)) != ANI_OK) {
        WVLOG_E("Class_FindMethod failed %{public}d", status);
        return nullptr;
    }
    ani_object error = WrapBusinessError(env, msg);
    if (error == nullptr) {
        WVLOG_E("error nulll");
        return nullptr;
    }
    if ((status = env->Object_New(cls, method, &obj, code, error)) != ANI_OK) {
        WVLOG_E("Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}

ani_ref CreateError(ani_env* env, int32_t err)
{
    std::string errMsg = NWebError::GetErrMsgByErrCode(err);
    return CreateBusinessError(env, err, errMsg);
}

void OnRequestStart(const ArkWeb_SchemeHandler* schemeHandler, ArkWeb_ResourceRequest* resourceRequest,
    const ArkWeb_ResourceHandler* resourceHandler, bool* intercept)
{
    WVLOG_D("SchemeHandler OnRequestStart");
    if (!schemeHandler) {
        WVLOG_E("OnRequestStart schemeHandler is nullptr");
        return;
    }
    WebSchemeHandler* handler = WebSchemeHandler::GetWebSchemeHandler(schemeHandler);
    if (!handler) {
        WVLOG_E("GetWebSchemeHandler failed");
        return;
    }
    handler->RequestStart(resourceRequest, resourceHandler, intercept);
}

void OnRequestStop(const ArkWeb_SchemeHandler* schemeHandler, const ArkWeb_ResourceRequest* resourceRequest)
{
    WVLOG_D("SchemeHandler OnRequestStop");
    if (!schemeHandler) {
        WVLOG_E("OnRequestStop schemeHandler is nullptr");
        return;
    }
    WebSchemeHandler* handler = WebSchemeHandler::GetWebSchemeHandler(schemeHandler);
    if (!handler) {
        WVLOG_E("GetWebSchemeHandler failed");
        return;
    }
    handler->RequestStop(resourceRequest);
}
} // namespace

WebSchemeHandlerRequest::WebSchemeHandlerRequest(ani_env* env) : env_(env)
{
    WVLOG_D("WebSchemeHandlerRequest::WebSchemeHandlerRequest");
    if (vm_ == nullptr) {
        WVLOG_E("vm_ is nullptr");
        return;
    }
    if (env_->GetVM(&vm_) != ANI_OK) {
        WVLOG_E("Failed to get VM from env");
        return;
    }
}

WebSchemeHandlerRequest::WebSchemeHandlerRequest(ani_env* env, const ArkWeb_ResourceRequest* request) : env_(env)
{
    WVLOG_D("WebSchemeHandlerRequest::WebSchemeHandlerRequest");
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

std::unordered_map<WebSchemeHandler*, const ArkWeb_SchemeHandler*> WebSchemeHandler::webSchemeHandlerMap_;
std::unordered_map<const ArkWeb_SchemeHandler*, WebSchemeHandler*> WebSchemeHandler::arkWebSchemeHandlerMap_;

const ArkWeb_SchemeHandler* WebSchemeHandler::GetArkWebSchemeHandler(WebSchemeHandler* handler)
{
    return WebSchemeHandler::webSchemeHandlerMap_.find(handler) != WebSchemeHandler::webSchemeHandlerMap_.end()
               ? WebSchemeHandler::webSchemeHandlerMap_[handler]
               : nullptr;
}

WebSchemeHandler* WebSchemeHandler::GetWebSchemeHandler(const ArkWeb_SchemeHandler* handler)
{
    return WebSchemeHandler::arkWebSchemeHandlerMap_.find(handler) != WebSchemeHandler::arkWebSchemeHandlerMap_.end()
               ? WebSchemeHandler::arkWebSchemeHandlerMap_[handler]
               : nullptr;
}

WebSchemeHandler::WebSchemeHandler(ani_env* env) : vm_(nullptr)
{
    WVLOG_D("create WebSchemeHandler");
    if (!env) {
        WVLOG_E("create WebSchemeHandler env null");
        return;
    }
    env_ = env;
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
    GetEnv()->GlobalReference_Delete(request_start_callback_);
    GetEnv()->GlobalReference_Delete(request_stop_callback_);
    ArkWeb_SchemeHandler* handler = const_cast<ArkWeb_SchemeHandler*>(GetArkWebSchemeHandler(this));
    if (!handler) {
        WVLOG_E("~WebSchemeHandler not found ArkWeb_SchemeHandler");
        return;
    }
    webSchemeHandlerMap_.erase(this);
    arkWebSchemeHandlerMap_.erase(handler);
    OH_ArkWeb_DestroySchemeHandler(handler);
}

void WebSchemeHandler::RequestStart(
    ArkWeb_ResourceRequest* request, const ArkWeb_ResourceHandler* ArkWeb_ResourceHandler, bool* intercept)
{
    if (vm_ == nullptr) {
        WVLOG_E("WebSchemeHandler::RequestStart nil vm");
        return;
    }

    ani_env* env = GetEnv();
    if (!request_start_callback_ || !request) {
        WVLOG_E("WebSchemeHandler request_start_callback_ or request is null.");
        return;
    }
    if (!env_ || !env) {
        WVLOG_E("WebSchemeHandler env is null.");
        return;
    }
    ani_size nr_refs = REFERENCES_MAX_NUMBER;
    env->CreateLocalScope(nr_refs);
    WebSchemeHandlerRequest* schemeHandlerRequest = new (std::nothrow) WebSchemeHandlerRequest(env, request);
    if (schemeHandlerRequest == nullptr) {
        WVLOG_E("RequestStart, new schemeHandlerRequest failed");
        env->DestroyLocalScope();
        return;
    }
    sptr<WebResourceHandler> resourceHandler = new (std::nothrow) WebResourceHandler(env, ArkWeb_ResourceHandler);
    if (resourceHandler == nullptr) {
        WVLOG_E("RequestStart, new resourceHandler failed");
        delete schemeHandlerRequest;
        env->DestroyLocalScope();
        return;
    }

    ani_object requestObject = {};
    if (!CreateObjectVoid(env, WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME, requestObject)) {
        WVLOG_E("[SchemeHandler] CreaterequestObject failed");
        delete schemeHandlerRequest;
        resourceHandler->DecStrongRef(resourceHandler);
        env->DestroyLocalScope();
        return;
    }

    ani_object resourceObject = {};
    if (!CreateObjectVoid(env, WEB_RESOURCE_HANDLER_CLASS_NAME, resourceObject)) {
        WVLOG_E("[SchemeHandler] CreateresourceObject Failed");
        delete schemeHandlerRequest;
        resourceHandler->DecStrongRef(resourceHandler);
        env->DestroyLocalScope();
        return;
    }
    if (OH_ArkWebResourceRequest_SetUserData(request, resourceHandler.GetRefPtr()) != 0) {
        WVLOG_E("OH_ArkWebResourceRequest_SetUserData failed");
    } else {
        resourceHandler->IncStrongRef(nullptr);
    }
    if (!Wrap(env, requestObject, WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME,
            reinterpret_cast<ani_long>(schemeHandlerRequest))) {
        WVLOG_E("[SchemeHandler] WebSchemeHandlerRequest wrap failed");
        delete schemeHandlerRequest;
        schemeHandlerRequest = nullptr;
        env->DestroyLocalScope();
        return;
    }

    if (!Wrap(env, resourceObject, WEB_RESOURCE_HANDLER_CLASS_NAME,
            reinterpret_cast<ani_long>(resourceHandler.GetRefPtr()))) {
        WVLOG_E("[SchemeHandler] WebResourceHandler wrap failed");
        resourceHandler->DecStrongRef(resourceHandler);
        resourceHandler = nullptr;
        env->DestroyLocalScope();
        return;
    }
    if (requestObject == nullptr || resourceObject == nullptr) {
        WVLOG_E("requestObject or resourceRequest is null");
        env->DestroyLocalScope();
        return;
    }
    resourceHandler->IncStrongRef(nullptr);
    std::vector<ani_ref> vec;
    vec.push_back(static_cast<ani_object>(requestObject));
    vec.push_back(static_cast<ani_object>(resourceObject));
    ani_ref fnReturnVal;
    ani_status status = env->FunctionalObject_Call(
        reinterpret_cast<ani_fn_object>(request_start_callback_), vec.size(), vec.data(), &fnReturnVal);
    if (status != ANI_OK) {
        WVLOG_E("scheme handler call onRequestStart failed.");
    }
    if (!ParseBoolean(env, fnReturnVal, *intercept)) {
        WVLOG_E("scheme handler onRequestStart intercept parse failed");
        *intercept = false;
    }
    env->DestroyLocalScope();
    if (!*intercept) {
        resourceHandler->SetFinishFlag();
        resourceHandler->DecStrongRef(resourceHandler);
    }
}

void WebSchemeHandler::RequestStopAfterWorkCb(RequestStopParam* param)
{
    if (!param) {
        WVLOG_E("RequestStopAfterWorkCb: param is null");
        return;
    }
    if (vm_ == nullptr) {
        WVLOG_E("RequestStopAfterWorkCb: nil vm");
        delete param;
        return;
    }
    if (auto status = vm_->GetEnv(ANI_VERSION_1, &param->env_) != ANI_OK) {
        WVLOG_E("RequestStopAfterWorkCb: GetEnv status is : %{public}d", status);
        delete param;
        return;
    }
    if (param->env_ == nullptr || !param->callbackRef_) {
        WVLOG_E("RequestStopAfterWorkCb: callbackRef_ or env_ is null");
        delete param;
        return;
    }
    ani_ref callbackFunc = nullptr;
    ani_status status =
        param->env_->GlobalReference_Create(reinterpret_cast<ani_ref>(param->callbackRef_), &callbackFunc);
    if (status != ANI_OK || callbackFunc == nullptr) {
        WVLOG_E("RequestStopAfterWorkCb: GlobalReference_Create failed");
        delete param;
        return;
    }
    ani_object requestValue = {};
    if (!CreateObjectVoid(param->env_, WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME, requestValue)) {
        WVLOG_E("RequestStopAfterWorkCb: create requestValue failed");
        delete param;
        return;
    }
    if (!Wrap(param->env_, requestValue, WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME,
            reinterpret_cast<ani_long>(param->request_))) {
        WVLOG_E("RequestStopAfterWorkCb: WebSchemeHandlerRequest wrap failed");
        delete param;
        return;
    }
    std::vector<ani_ref> vec;
    vec.push_back(static_cast<ani_object>(requestValue));
    ani_ref fnReturnVal;
    status = param->env_->FunctionalObject_Call(
        reinterpret_cast<ani_fn_object>(callbackFunc), vec.size(), vec.data(), &fnReturnVal);
    if (status != ANI_OK) {
        WVLOG_E("RequestStopAfterWorkCb:FunctionalObject_Call failed.");
        delete param;
        return;
    }
    if (callbackFunc != nullptr) {
        env_->GlobalReference_Delete(callbackFunc);
    }
    WebResourceHandler* resourceHandler =
        reinterpret_cast<WebResourceHandler*>(OH_ArkWebResourceRequest_GetUserData(param->arkWebRequest_));
    if (resourceHandler) {
        resourceHandler->SetFinishFlag();
        resourceHandler->DecStrongRef(resourceHandler);
    }
    delete param;
    param = nullptr;
}

void WebSchemeHandler::RequestStop(const ArkWeb_ResourceRequest* resourceRequest)
{
    if (vm_ == nullptr) {
        WVLOG_E("RequestStop: RequestStop nil vm");
        return;
    }
    ani_env* env = nullptr;
    ani_options aniArgs { 0, nullptr };
    if (auto status = vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env) != ANI_OK) {
        WVLOG_E("RequestStop: AttachCurrentThread status is : %{public}d", status);
        return;
    }
    if (env == nullptr) {
        WVLOG_E("RequestStop: env is nullptr");
        return;
    }
     if (!mainHandler_) {
        std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            WVLOG_E("RequestStop: GetMainEventRunner failed");
            return;
        }
        mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    }
    if (mainHandler_ == nullptr) {
        WVLOG_E("RequestStop: mainHandler_ is null.");
        return;
    }
    if (!request_stop_callback_) {
        WVLOG_E("RequestStop: request_stop_callback is null");
        return;
    }
    WebSchemeHandlerRequest* request = new (std::nothrow) WebSchemeHandlerRequest(env_, resourceRequest);
    if (request == nullptr) {
        WVLOG_E("RequestStop: failed to create WebSchemeHandlerRequest");
        return;
    }
    RequestStopParam* param = new (std::nothrow) RequestStopParam();
    if (param == nullptr) {
        WVLOG_E("RequestStop: RequestStop failed to create RequestStopParam");
        delete request;
        return;
    }
    param->env_ = env;
    param->callbackRef_ = request_stop_callback_;
    param->request_ = request;
    param->arkWebRequest_ = resourceRequest;
    auto task = [this, param]() { WebSchemeHandler::RequestStopAfterWorkCb(param); };
        mainHandler_->PostTask(task, TASK_ID);
}

void WebSchemeHandler::PutRequestStart(ani_env* env, ani_vm* vm, ani_fn_object callback)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_D("WebSchemeHandler::PutRequestStart");
    if (!vm) {
        WVLOG_E("PutRequestStart vm null");
        return;
    }
    vm_ = vm;
    ani_status status = env->GlobalReference_Create(reinterpret_cast<ani_ref>(callback), &request_start_callback_);
    if (status != ANI_OK) {
        WVLOG_E("PutRequestStart create reference failed.");
    }
}

void WebSchemeHandler::PutRequestStop(ani_env* env, ani_vm* vm, ani_fn_object callback)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_D("WebSchemeHandler::PutRequestStop");
    if (!vm) {
        WVLOG_E("PutRequestStop vm null");
        return;
    }
    vm_ = vm;
    ani_status status = env->GlobalReference_Create(reinterpret_cast<ani_ref>(callback), &request_stop_callback_);
    if (status != ANI_OK) {
        WVLOG_E("PutRequestStop create reference failed.");
    }
}

WebResourceHandler::WebResourceHandler(ani_env* env)
{
    WVLOG_D("create WebResourceHandler");
    if (vm_ == nullptr) {
        WVLOG_E("vm_ is nullptr");
        return;
    }
    if (env->GetVM(&vm_) != ANI_OK) {
        WVLOG_E("Failed to get VM from env");
        return;
    }
}

WebResourceHandler::WebResourceHandler(ani_env* env, const ArkWeb_ResourceHandler* handler)
    : handler_(const_cast<ArkWeb_ResourceHandler*>(handler))
{
    WVLOG_D("create WebResourceHandler");
    if (vm_ == nullptr) {
        WVLOG_E("vm_ is nullptr");
        return;
    }
    if (env->GetVM(&vm_) != ANI_OK) {
        WVLOG_E("Failed to get VM from env");
        return;
    }
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

WebHttpBodyStream::WebHttpBodyStream(ani_env* env)
{
    WVLOG_D("WebHttpBodyStream::WebHttpBodyStream");
    env_ = env;
}

WebHttpBodyStream::WebHttpBodyStream(ani_env* env, ArkWeb_HttpBodyStream* stream)
{
    WVLOG_D("WebHttpBodyStream::WebHttpBodyStream");
    env_ = env;
    stream_ = stream;
    if (OH_ArkWebHttpBodyStream_SetUserData(stream_, this) != 0) {
        WVLOG_E("OH_ArkWebHttpBodyStream_SetUserData failed");
        return;
    }
    if (OH_ArkWebHttpBodyStream_SetReadCallback(stream_, &WebHttpBodyStream::HttpBodyStreamReadCallback) != 0) {
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
    WebHttpBodyStream* stream =
        reinterpret_cast<WebHttpBodyStream*>(OH_ArkWebHttpBodyStream_GetUserData(httpBodyStream));
    if (!stream) {
        WVLOG_E("OH_ArkWebHttpBodyStream_GetUserData is nullptr");
        return;
    }
    stream->ExecuteRead(buffer, bytesRead);
}

void WebHttpBodyStream::HttpBodyStreamInitCallback(const ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_NetError result)
{
    WVLOG_D("WebHttpBodyStream::HttpBodyStreamInitCallback");
    WebHttpBodyStream* stream =
        reinterpret_cast<WebHttpBodyStream*>(OH_ArkWebHttpBodyStream_GetUserData(httpBodyStream));
    if (!stream) {
        WVLOG_E("OH_ArkWebHttpBodyStream_GetUserData is nullptr");
        return;
    }
    stream->ExecuteInit(result);
}

void WebHttpBodyStream::Init(ani_ref jsCallback, ani_resolver initResolver)
{
    WVLOG_D("WebHttpBodyStream::Init");
    if (!jsCallback && !initResolver) {
        WVLOG_E("WebHttpBodyStream::InitCallback callback is nullptr");
        return;
    }
    if (jsCallback) {
        initJsCallback_ = std::move(jsCallback);
    }
    if (initResolver) {
        initResolver_ = std::move(initResolver);
    }
    int ret = OH_ArkWebHttpBodyStream_Init(stream_, &WebHttpBodyStream::HttpBodyStreamInitCallback);
    if (ret != 0) {
        WVLOG_E("OH_ArkWebHttpBodyStream_Init failed");
        return;
    }
}

void WebHttpBodyStream::Read(int bufLen, ani_ref jsCallback, ani_resolver readResolver)
{
    WVLOG_D("WebHttpBodyStream::Read");
    if (!jsCallback && !readResolver) {
        WVLOG_E("WebHttpBodyStream::Read callback is nullptr");
        return;
    }
    if (bufLen <= 0) {
        return;
    }
    if (jsCallback) {
        readJsCallback_ = std::move(jsCallback);
    }
    if (readResolver) {
        readResolver_ = std::move(readResolver);
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

void WebHttpBodyStream::ExecuteInit(ArkWeb_NetError result)
{
    WVLOG_D("WebHttpBodyStream::ExecuteInit");
    if (!env_) {
        WVLOG_E("WebHttpBodyStream::ExecuteInit env_ is nullptr");
        return;
    }

    auto* asyncCtx = new InitAsyncCtx {
        .env = env_,
        .deferred = initResolver_,
        .errCode = result,
    };
    if (asyncCtx == nullptr) {
        WVLOG_E("WebHttpBodyStream::ExecuteInit");
        return;
    }
    WVLOG_D("WebHttpBodyStream::ExecuteInit task started");

    if (!asyncCtx->env) {
        WVLOG_E("WebHttpBodyStream::ExecuteInit asyncCtx or env is nullptr");
        delete asyncCtx;
        return;
    }

    ani_ref resultRef;
    asyncCtx->env->GetUndefined(&resultRef);

    if (asyncCtx->errCode != 0) {
        resultRef = CreateError(asyncCtx->env, NWebError::HTTP_BODY_STREAN_INIT_FAILED);
    } else {
        asyncCtx->env->GetUndefined(&resultRef);
    }
    std::vector<ani_ref> vec;
    vec.push_back(resultRef);
    if (asyncCtx->deferred) {
        if (asyncCtx->errCode == 0) {
            if (asyncCtx->env->PromiseResolver_Resolve(asyncCtx->deferred, resultRef) != ANI_OK) {
                WVLOG_E("WebHttpBodyStream::ExecuteInit PromiseResolver_Resolve failed");
            }
        } else {
            if (asyncCtx->env->PromiseResolver_Reject(asyncCtx->deferred, static_cast<ani_error>(resultRef)) !=
                ANI_OK) {
                WVLOG_E("WebHttpBodyStream::ExecuteInit PromiseResolver_Reject failed");
            }
        }
    } else {
        WVLOG_E("WebHttpBodyStream::ExecuteInit no deferred provided");
    }
    WVLOG_D("WebHttpBodyStream::ExecuteInit");
    delete asyncCtx;
}

void WebHttpBodyStream::ExecuteRead(uint8_t* buffer, int bytesRead)
{
    WVLOG_D("WebHttpBodyStream::ExecuteRead");
    if (!env_) {
        return;
    }
    auto* asyncCtx = new ReadAsyncCtx {
        .env = env_,
        .deferred = initResolver_,
        .buffer = buffer,
        .bytesRead = bytesRead,
    };
    if (asyncCtx == nullptr) {
        WVLOG_E("WebHttpBodyStream::ExecuteRead asyncCtx is nullptr");
        return;
    }
    WVLOG_D("WebHttpBodyStream::ExecuteRead task started");
    if (!asyncCtx->env) {
        WVLOG_E("WebHttpBodyStream::ExecuteRead asyncCtx or env is nullptr");
        delete asyncCtx;
        return;
    }
    ani_arraybuffer arraybuffer;
    void* bufferData = nullptr;
    env_->CreateArrayBuffer(asyncCtx->bytesRead, &bufferData, &arraybuffer);
    if (memcpy_s(bufferData, asyncCtx->bytesRead, asyncCtx->buffer, asyncCtx->bytesRead) != 0 &&
        asyncCtx->bytesRead > 0) {
        WVLOG_E("WebHttpBodyStream::ExecuteRead memcpy failed");
    }
    if (asyncCtx->buffer) {
        delete asyncCtx->buffer;
    }
    if (asyncCtx->deferred) {
        if (asyncCtx->env->PromiseResolver_Resolve(asyncCtx->deferred, arraybuffer) != ANI_OK) {
            WVLOG_E("WebHttpBodyStream::ExecuteInit PromiseResolver_Resolve failed");
        }
    }
    delete asyncCtx;
}

uint64_t WebHttpBodyStream::GetPosition() const
{
    return OH_ArkWebHttpBodyStream_GetPosition(stream_);
}

uint64_t WebHttpBodyStream::GetSize() const
{
    WVLOG_D("WebHttpBodyStream::GetSize");
    return OH_ArkWebHttpBodyStream_GetSize(stream_);
}

bool WebHttpBodyStream::IsChunked() const
{
    WVLOG_D("WebHttpBodyStream::IsChunked");
    return OH_ArkWebHttpBodyStream_IsChunked(stream_);
}

bool WebHttpBodyStream::IsEof()
{
    WVLOG_D("WebHttpBodyStream::IsEof");
    return OH_ArkWebHttpBodyStream_IsEof(stream_);
}

bool WebHttpBodyStream::IsInMemory()
{
    WVLOG_D("WebHttpBodyStream::IsInMemory");
    return OH_ArkWebHttpBodyStream_IsInMemory(stream_);
}

} // namespace OHOS::NWeb
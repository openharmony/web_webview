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

#include "web_scheme_handler.h"

#include <securec.h>
#include <sstream>
#include <string>

#include "business_error.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "ani_parse_utils.h"
#include "ani_business_error.h"
#include "ani_class_name.h"

namespace OHOS::NWeb {

namespace {

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
    WVLOG_E("create WebSchemeHandler");
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
    ArkWeb_ResourceRequest* request, const ArkWeb_ResourceHandler* arkwebResourceHandler, bool* intercept)
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
    WebSchemeHandlerRequest* schemeHandlerRequest = new (std::nothrow) WebSchemeHandlerRequest(request);
    if (schemeHandlerRequest == nullptr) {
        WVLOG_E("RequestStart, new schemeHandlerRequest failed");
        env->DestroyLocalScope();
        return;
    }
    sptr<WebResourceHandler> resourceHandler = new (std::nothrow) WebResourceHandler(arkwebResourceHandler);
    if (resourceHandler == nullptr) {
        WVLOG_E("RequestStart, new resourceHandler failed");
        schemeHandlerRequest->DecStrongRef(schemeHandlerRequest);
        resourceHandler->DecStrongRef(resourceHandler);
        env->DestroyLocalScope();
        return;
    }

    ani_object requestObject = {};
    if (!AniParseUtils::CreateObjectVoid(env, ANI_WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME, requestObject)) {
        WVLOG_E("[SchemeHandler] CreaterequestObject failed");
        schemeHandlerRequest->DecStrongRef(schemeHandlerRequest);
        resourceHandler->DecStrongRef(resourceHandler);
        env->DestroyLocalScope();
        return;
    }

    ani_object resourceObject = {};
    if (!AniParseUtils::CreateObjectVoid(env, ANI_WEB_RESOURCE_HANDLER_CLASS_NAME, resourceObject)) {
        WVLOG_E("[SchemeHandler] CreateresourceObject Failed");
        schemeHandlerRequest->DecStrongRef(schemeHandlerRequest);
        resourceHandler->DecStrongRef(resourceHandler);
        env->DestroyLocalScope();
        return;
    }
    if (OH_ArkWebResourceRequest_SetUserData(request, resourceHandler.GetRefPtr()) != 0) {
        WVLOG_E("OH_ArkWebResourceRequest_SetUserData failed");
    } else {
        resourceHandler->IncStrongRef(nullptr);
    }
    if (!AniParseUtils::Wrap(env, requestObject, ANI_WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME,
        reinterpret_cast<ani_long>(schemeHandlerRequest))) {
        WVLOG_E("[SchemeHandler] WebSchemeHandlerRequest wrap failed");
        schemeHandlerRequest->DecStrongRef(schemeHandlerRequest);
        schemeHandlerRequest = nullptr;
        env->DestroyLocalScope();
        return;
    }

    if (!AniParseUtils::Wrap(env, resourceObject, ANI_WEB_RESOURCE_HANDLER_CLASS_NAME,
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
    if (!AniParseUtils::ParseBoolean(env, fnReturnVal, *intercept)) {
        WVLOG_E("scheme handler onRequestStart intercept parse failed");
        *intercept = false;
    }
    env->DestroyLocalScope();
    if (!*intercept) {
        resourceHandler->SetFinishFlag();
        resourceHandler->DecStrongRef(resourceHandler);
    }
}

void WebSchemeHandler::RequestStop(const ArkWeb_ResourceRequest* resourceRequest)
{
    WVLOG_D("WebSchemeHandler::RequestStop");
    return;
}

void WebSchemeHandler::PutRequestStart(ani_env* env, ani_vm* vm, ani_fn_object callback)
{
    if (!env) {
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
    if (!env) {
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

} // namespace OHOS::NWeb
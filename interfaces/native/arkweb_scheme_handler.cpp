/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "arkweb_scheme_handler.h"

#include <dlfcn.h>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "nweb.h"
#include "nweb_log.h"
#include "nweb_helper.h"

namespace {

// Run DO macro for every function defined in the API.
#define FOR_EACH_API_FN(DO)                         \
    DO(OH_ArkWebRequestHeaderList_Create)           \
    DO(OH_ArkWebRequestHeaderList_Destroy)          \
    DO(OH_ArkWebRequestHeaderList_GetSize)          \
    DO(OH_ArkWebRequestHeaderList_GetHeader)        \
    DO(OH_ArkWebResourceRequest_GetMethod)          \
    DO(OH_ArkWebResourceRequest_GetUrl)             \
    DO(OH_ArkWebResourceRequest_GetPostData)        \
    DO(OH_ArkWebPostDataStream_SetReadCallback)     \
    DO(OH_ArkWebPostDataStream_SetUserData)         \
    DO(OH_ArkWebPostDataStream_GetUserData)         \
    DO(OH_ArkWebPostDataStream_Init)                \
    DO(OH_ArkWebPostDataStream_Read)                \
    DO(OH_ArkWebPostDataStream_GetSize)             \
    DO(OH_ArkWebPostDataStream_GetPosition)         \
    DO(OH_ArkWebPostDataStream_IsChunked)           \
    DO(OH_ArkWebPostDataStream_IsEof)               \
    DO(OH_ArkWebPostDataStream_IsInMemory)          \
    DO(OH_ArkWebResourceRequest_GetReferrer)        \
    DO(OH_ArkWebResourceRequest_GetRequestHeaders)  \
    DO(OH_ArkWebResourceRequest_IsRedirect)         \
    DO(OH_ArkWebResourceRequest_IsMainFrame)        \
    DO(OH_ArkWebResourceRequest_HasGesture)         \
    DO(OH_ArkWeb_RegisterCustomSchemes)             \
    DO(OH_ArkWeb_SetSchemeHandler)                  \
    DO(OH_ArkWebServiceWorker_SetSchemeHandler)     \
    DO(OH_ArkWebServiceWorker_ClearSchemeHandlers)  \
    DO(OH_ArkWeb_ClearSchemeHandlers)               \
    DO(OH_ArkWeb_CreateSchemeHandler)               \
    DO(OH_ArkWeb_DestroySchemeHandler)              \
    DO(OH_ArkWebSchemeHandler_SetOnRequestStart)    \
    DO(OH_ArkWebSchemeHandler_SetOnRequestStop)     \
    DO(OH_ArkWebSchemeHandler_SetUserData)          \
    DO(OH_ArkWebSchemeHandler_GetUserData)          \
    DO(OH_ArkWeb_CreateResponse)                    \
    DO(OH_ArkWeb_DestroyResponse)                   \
    DO(OH_ArkWebResponse_GetUrl)                    \
    DO(OH_ArkWebResponse_SetUrl)                    \
    DO(OH_ArkWebResponse_SetError)                  \
    DO(OH_ArkWebResponse_GetError)                  \
    DO(OH_ArkWebResponse_SetStatus)                 \
    DO(OH_ArkWebResponse_GetStatus)                 \
    DO(OH_ArkWebResponse_SetStatusText)             \
    DO(OH_ArkWebResponse_GetStatusText)             \
    DO(OH_ArkWebResponse_SetMimeType)               \
    DO(OH_ArkWebResponse_GetMimeType)               \
    DO(OH_ArkWebResponse_SetCharset)                \
    DO(OH_ArkWebResponse_GetCharset)                \
    DO(OH_ArkWebResponse_SetHeaderByName)           \
    DO(OH_ArkWebResponse_GetHeaderByName)           \
    DO(OH_ArkWebResourceHandler_DidReceiveResponse) \
    DO(OH_ArkWebResourceHandler_DidReceiveData)     \
    DO(OH_ArkWebResourceHandler_DidFinish)          \
    DO(OH_ArkWebResourceHandler_DidFailWithError)   \
    DO(OH_ArkWeb_ReleaseString)                     \
    DO(OH_ArkWeb_ReleaseByteArray)

struct SchemeHandlerApi {
#define GEN_FN_PTR(fn) decltype(&fn) impl_##fn = nullptr;
    FOR_EACH_API_FN(GEN_FN_PTR)
#undef GEN_FN_PTR
};

template <typename Fn> void LoadFunction(void *handle, const char *functionName, Fn *fnOut)
{
    void *fn = dlsym(handle, functionName);
    if (!fn) {
        OHOS::WVLOG_E("%{public}s not found.", functionName);
        return;
    }
    *fnOut = reinterpret_cast<Fn>(fn);
}

SchemeHandlerApi *g_SchemeHandlerApi = nullptr;

void LoadSchemeHandlerApi(void *handle, SchemeHandlerApi *api)
{
    // Initialize each function pointer field from the so.
#define LOAD_FN_PTR(fn) LoadFunction(handle, #fn, &api->impl_##fn);
    FOR_EACH_API_FN(LOAD_FN_PTR)
#undef LOAD_FN_PTR
}

bool EnsureSdkLoaded(void *handle)
{
    if (g_SchemeHandlerApi) {
        OHOS::WVLOG_I("SchemeHandlerApi had loaded.");
        return true;
    }

    if (handle == nullptr) {
        OHOS::WVLOG_I("EnsureSdkLoaded handle is nullptr.");
        return false;
    }

    auto *schemeHandlerApi = new SchemeHandlerApi();
    if (schemeHandlerApi == nullptr) {
        OHOS::WVLOG_I("schemeHandlerApi is nullptr.");
        return false;
    }
    LoadSchemeHandlerApi(handle, schemeHandlerApi);
    g_SchemeHandlerApi = schemeHandlerApi;
    return true;
}
#undef FOR_EACH_API_FN
} // namespace

using namespace OHOS;
void OH_ArkWebRequestHeaderList_Create(const ArkWeb_ResourceRequest* resourceRequest,
                                       ArkWeb_RequestHeaderList** requestHeaderList)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_Create) {
        WVLOG_E("OH_ArkWebRequestHeaderList_Create not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_Create(resourceRequest, requestHeaderList);
}

void OH_ArkWebRequestHeaderList_Destroy(ArkWeb_RequestHeaderList* requestHeaderList)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_Destroy) {
        WVLOG_E("OH_ArkWebRequestHeaderList_Destroy not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_Destroy(requestHeaderList);
}

int32_t OH_ArkWebRequestHeaderList_GetSize(const ArkWeb_RequestHeaderList* requestHeaderList)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_GetSize) {
        WVLOG_E("OH_ArkWebRequestHeaderList_GetSize not found.");
        return -1;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_GetSize(requestHeaderList);
}

void OH_ArkWebRequestHeaderList_GetHeader(const ArkWeb_RequestHeaderList* requestHeaderList,
                                          int32_t index, char** key,
                                          char** value)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_GetHeader) {
        WVLOG_E("OH_ArkWebRequestHeaderList_GetHeader not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebRequestHeaderList_GetHeader(requestHeaderList, index, key, value);
}

void OH_ArkWebResourceRequest_GetMethod(const ArkWeb_ResourceRequest* resourceRequest, char** method)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetMethod) {
        WVLOG_E("OH_ArkWebResourceRequest_GetMethod not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetMethod(resourceRequest, method);
}

void OH_ArkWebResourceRequest_GetUrl(const ArkWeb_ResourceRequest* resourceRequest, char** url)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetUrl) {
        WVLOG_E("OH_ArkWebResourceRequest_GetUrl not found.");
        return;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetUrl(resourceRequest, url);
}

void OH_ArkWebResourceRequest_GetPostData(const ArkWeb_ResourceRequest* resourceRequest,
                                          ArkWeb_PostDataStream** postDataStream)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetPostData) {
        WVLOG_E("OH_ArkWebResourceRequest_GetPostData not found.");
        return;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetPostData(resourceRequest, postDataStream);
}

void OH_ArkWebPostDataStream_SetUserData(ArkWeb_PostDataStream* postDataStream, void* userData)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_SetUserData) {
        WVLOG_E("OH_ArkWebPostDataStream_SetUserData not found.");
        return;
    }
    g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_SetUserData(postDataStream, userData);
}

void* OH_ArkWebPostDataStream_GetUserData(const ArkWeb_PostDataStream* postDataStream)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_GetUserData) {
        WVLOG_E("OH_ArkWebPostDataStream_GetUserData not found.");
        return nullptr;
    }
    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_GetUserData(postDataStream);
}

void OH_ArkWebPostDataStream_SetReadCallback(ArkWeb_PostDataStream* postDataStream,
                                             ArkWeb_PostDataReadCallback readCallback)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_SetReadCallback) {
        WVLOG_E("OH_ArkWebPostDataStream_SetReadCallback not found.");
        return;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_SetReadCallback(postDataStream, readCallback);
}

void OH_ArkWebPostDataStream_Init(ArkWeb_PostDataStream* postDataStream,
                                  ArkWeb_PostDataStreamInitCallback readyCallback)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_Init) {
        WVLOG_E("OH_ArkWebPostDataStream_Init not found.");
        return;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_Init(postDataStream, readyCallback);
}

void OH_ArkWebPostDataStream_Read(const ArkWeb_PostDataStream* postDataStream, uint8_t* buffer, int bufLen)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_Read) {
        WVLOG_E("OH_ArkWebPostDataStream_Read not found.");
        return;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_Read(postDataStream, buffer, bufLen);
}

uint64_t OH_ArkWebPostDataStream_GetSize(const ArkWeb_PostDataStream* postDataStream)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_GetSize) {
        WVLOG_E("OH_ArkWebPostDataStream_GetSize not found.");
        return -1;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_GetSize(postDataStream);
}

uint64_t OH_ArkWebPostDataStream_GetPosition(const ArkWeb_PostDataStream* postDataStream)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_GetPosition) {
        WVLOG_E("OH_ArkWebPostDataStream_GetPosition not found.");
        return -1;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_GetPosition(postDataStream);
}

bool OH_ArkWebPostDataStream_IsChunked(const ArkWeb_PostDataStream* postDataStream)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_IsChunked) {
        WVLOG_E("OH_ArkWebPostDataStream_IsTrunked not found.");
        return false;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_IsChunked(postDataStream);
}

bool OH_ArkWebPostDataStream_IsEof(const ArkWeb_PostDataStream* postDataStream)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_IsEof) {
        WVLOG_E("OH_ArkWebPostDataStream_HasNullSource not found.");
        return false;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_IsEof(postDataStream);
}

bool OH_ArkWebPostDataStream_IsInMemory(const ArkWeb_PostDataStream* postDataStream)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_IsInMemory) {
        WVLOG_E("OH_ArkWebPostDataStream_IsInMemory not found.");
        return false;
    }

    return g_SchemeHandlerApi->impl_OH_ArkWebPostDataStream_IsInMemory(postDataStream);
}

void OH_ArkWebResourceRequest_GetReferrer(const ArkWeb_ResourceRequest* resourceRequest, char** referrer)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetReferrer) {
        WVLOG_E("OH_ArkWebResourceRequest_GetReferrer not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetReferrer(resourceRequest, referrer);
}

void OH_ArkWebResourceRequest_GetRequestHeaders(const ArkWeb_ResourceRequest* resourceRequest,
                                                ArkWeb_RequestHeaderList** requestHeaderList)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetRequestHeaders) {
        WVLOG_E("OH_ArkWebResourceRequest_GetRequestHeaders not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_GetRequestHeaders(resourceRequest, requestHeaderList);
}

bool OH_ArkWebResourceRequest_IsRedirect(const ArkWeb_ResourceRequest* resourceRequest)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_IsRedirect) {
        WVLOG_E("OH_ArkWebResourceRequest_IsRedirect not found.");
        return false;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_IsRedirect(resourceRequest);
}

bool OH_ArkWebResourceRequest_IsMainFrame(const ArkWeb_ResourceRequest* resourceRequest)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_IsMainFrame) {
        WVLOG_E("OH_ArkWebResourceRequest_IsMainFrame not found.");
        return false;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_IsMainFrame(resourceRequest);
}

bool OH_ArkWebResourceRequest_HasGesture(const ArkWeb_ResourceRequest* resourceRequest)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_HasGesture) {
        WVLOG_E("OH_ArkWebResourceRequest_HasGesture not found.");
        return false;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceRequest_HasGesture(resourceRequest);
}

void OH_ArkWeb_RegisterCustomSchemes(const char* scheme, int32_t option)
{
    void* webEngineHandle = OHOS::NWeb::NWebHelper::Instance().GetWebEngineHandler();
    if (webEngineHandle) {
        if (!EnsureSdkLoaded(webEngineHandle)) {
            WVLOG_E("OH_ArkWeb_RegisterCustomSchemes sdk not loaded.");
        }
    } else {
        WVLOG_E("OH_ArkWeb_RegisterCustomSchemes webEngineHandle is nullptr.");
    }
    
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_RegisterCustomSchemes) {
        WVLOG_E("OH_ArkWeb_RegisterCustomSchemes not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_RegisterCustomSchemes(scheme, option);
}

bool OH_ArkWeb_SetSchemeHandler(const char* scheme, const char* webTag, ArkWeb_SchemeHandler* schemeHandler)
{
    void* webEngineHandle = OHOS::NWeb::NWebHelper::Instance().GetWebEngineHandler();
    if (webEngineHandle) {
        if (!EnsureSdkLoaded(webEngineHandle)) {
            WVLOG_E("scheme_handler sdk not loaded.");
        }
    } else {
        WVLOG_E("scheme_handler webEngineHandle is nullptr.");
    }
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_SetSchemeHandler) {
        WVLOG_E("OH_ArkWeb_SetSchemeHandler not found.");
        return false;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_SetSchemeHandler(scheme, webTag, schemeHandler);
}

bool OH_ArkWebServiceWorker_SetSchemeHandler(const char* scheme, ArkWeb_SchemeHandler* schemeHandler)
{
    void* webEngineHandle = OHOS::NWeb::NWebHelper::Instance().GetWebEngineHandler();
    if (webEngineHandle) {
        if (!EnsureSdkLoaded(webEngineHandle)) {
            WVLOG_E("scheme_handler sdk not loaded.");
        }
    } else {
        WVLOG_E("scheme_handler webEngineHandle is nullptr.");
    }
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebServiceWorker_SetSchemeHandler) {
        WVLOG_E("OH_ArkWebServiceWorker_SetSchemeHandler not found.");
        return false;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebServiceWorker_SetSchemeHandler(scheme, schemeHandler);
}

void OH_ArkWeb_ClearSchemeHandlers(const char* webTag)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_ClearSchemeHandlers) {
        WVLOG_E("OH_ArkWeb_ClearSchemeHandlers not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_ClearSchemeHandlers(webTag);
}

void OH_ArkWebServiceWorker_ClearSchemeHandlers()
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebServiceWorker_ClearSchemeHandlers) {
        WVLOG_E("OH_ArkWebServiceWorker_ClearSchemeHandlers not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebServiceWorker_ClearSchemeHandlers();
}

void OH_ArkWeb_CreateSchemeHandler(ArkWeb_SchemeHandler** schemeHandler)
{
    void* webEngineHandle = OHOS::NWeb::NWebHelper::Instance().GetWebEngineHandler();
    if (webEngineHandle) {
        if (!EnsureSdkLoaded(webEngineHandle)) {
            WVLOG_E("scheme_handler sdk not loaded.");
        }
    } else {
        WVLOG_E("scheme_handler webEngineHandle is nullptr.");
    }
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_CreateSchemeHandler) {
        WVLOG_E("OH_ArkWeb_CreateSchemeHandler not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_CreateSchemeHandler(schemeHandler);
}

void OH_ArkWeb_DestroySchemeHandler(ArkWeb_SchemeHandler* schemeHandler)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_DestroySchemeHandler) {
        WVLOG_E("OH_ArkWeb_DestroySchemeHandler not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_DestroySchemeHandler(schemeHandler);
}

void OH_ArkWebSchemeHandler_SetUserData(ArkWeb_SchemeHandler* schemeHandler, void* userData)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_SetUserData) {
        WVLOG_E("OH_ArkWebSchemeHandler_SetUserData not found.");
        return;
    }
    g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_SetUserData(schemeHandler, userData);
}

void* OH_ArkWebSchemeHandler_GetUserData(const ArkWeb_SchemeHandler* schemeHandler)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_GetUserData) {
        WVLOG_E("OH_ArkWebSchemeHandler_GetUserData not found.");
        return nullptr;
    }
    return g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_GetUserData(schemeHandler);
}

void OH_ArkWebSchemeHandler_SetOnRequestStart(ArkWeb_SchemeHandler* schemeHandler,
                                              ArkWeb_OnRequestStart onRequestStart)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_SetOnRequestStart) {
        WVLOG_E("OH_ArkWebSchemeHandler_SetOnRequestStart not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_SetOnRequestStart(schemeHandler, onRequestStart);
}

void OH_ArkWebSchemeHandler_SetOnRequestStop(ArkWeb_SchemeHandler* schemeHandler, ArkWeb_OnRequestStop onRequestStop)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_SetOnRequestStop) {
        WVLOG_E("OH_ArkWebSchemeHandler_SetOnRequestStop not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebSchemeHandler_SetOnRequestStop(schemeHandler, onRequestStop);
}


void OH_ArkWeb_CreateResponse(ArkWeb_Response** response)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_CreateResponse) {
        WVLOG_E("OH_ArkWeb_CreateResponse not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_CreateResponse(response);
}

void OH_ArkWeb_DestroyResponse(ArkWeb_Response* response)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_DestroyResponse) {
        WVLOG_E("OH_ArkWeb_DestroyResponse not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_DestroyResponse(response);
}

void OH_ArkWebResponse_SetUrl(ArkWeb_Response* response, const char* url)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetUrl) {
        WVLOG_E("OH_ArkWebResponse_SetUrl not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetUrl(response, url);
}

void OH_ArkWebResponse_GetUrl(const ArkWeb_Response* response, char** url)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetUrl) {
        WVLOG_E("OH_ArkWebResponse_GetUrl not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetUrl(response, url);
}

void OH_ArkWebResponse_SetError(ArkWeb_Response* response, ArkWeb_NetError errorCode)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetError) {
        WVLOG_E("OH_ArkWebResponse_SetError not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetError(response, errorCode);
}

ArkWeb_NetError OH_ArkWebResponse_GetError(const ArkWeb_Response* response)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetError) {
        WVLOG_E("OH_ArkWebResponse_GetError not found.");
        return ARKWEB_NET_UNKNOWN;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetError(response);
}

void OH_ArkWebResponse_SetStatus(ArkWeb_Response* response, int status)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetStatus) {
        WVLOG_E("OH_ArkWebResponse_SetStatus not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetStatus(response, status);
}

int OH_ArkWebResponse_GetStatus(const ArkWeb_Response* response)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetStatus) {
        WVLOG_E("OH_ArkWebResponse_GetStatus not found.");
        return -1;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetStatus(response);
}

void OH_ArkWebResponse_SetStatusText(ArkWeb_Response* response, const char* statusText)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetStatusText) {
        WVLOG_E("OH_ArkWebResponse_SetStatusText not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetStatusText(response, statusText);
}

void OH_ArkWebResponse_GetStatusText(const ArkWeb_Response* response, char** statusText)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetStatusText) {
        WVLOG_E("OH_ArkWebResponse_GetStatusText not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetStatusText(response, statusText);
}

void OH_ArkWebResponse_SetMimeType(ArkWeb_Response* response, const char* mimeType)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetMimeType) {
        WVLOG_E("OH_ArkWebResponse_SetMimeType not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetMimeType(response, mimeType);
}

void OH_ArkWebResponse_GetMimeType(const ArkWeb_Response* response, char** mimeType)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetMimeType) {
        WVLOG_E("OH_ArkWebResponse_GetMimeType not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetMimeType(response, mimeType);
}

void OH_ArkWebResponse_SetCharset(ArkWeb_Response* response, const char* charset)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetCharset) {
        WVLOG_E("OH_ArkWebResponse_SetCharset not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetCharset(response, charset);
}

void OH_ArkWebResponse_GetCharset(const ArkWeb_Response* response, char** charset)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetCharset) {
        WVLOG_E("OH_ArkWebResponse_GetCharset not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetCharset(response, charset);
}

void OH_ArkWebResponse_SetHeaderByName(ArkWeb_Response* response, const char* name, const char* value, bool overwrite)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetHeaderByName) {
        WVLOG_E("OH_ArkWebResponse_SetHeaderByName not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_SetHeaderByName(response, name, value, overwrite);
}

void OH_ArkWebResponse_GetHeaderByName(const ArkWeb_Response* response, const char* name, char** value)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetHeaderByName) {
        WVLOG_E("OH_ArkWebResponse_GetHeaderByName not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResponse_GetHeaderByName(response, name, value);
}

void OH_ArkWebResourceHandler_DidReceiveResponse(const ArkWeb_ResourceHandler* resourceHandler,
                                                 const ArkWeb_Response* response)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidReceiveResponse) {
        WVLOG_E("OH_ArkWebResourceHandler_DidReceiveResponse not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidReceiveResponse(resourceHandler, response);
}

void OH_ArkWebResourceHandler_DidReceiveData(const ArkWeb_ResourceHandler* resourceHandler,
                                             const uint8_t* buffer,
                                             int64_t bufLen)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidReceiveData) {
        WVLOG_E("OH_ArkWebResourceHandler_DidReceiveData not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidReceiveData(resourceHandler, buffer, bufLen);
}

void OH_ArkWebResourceHandler_DidFinish(const ArkWeb_ResourceHandler* resourceHandler)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidFinish) {
        WVLOG_E("OH_ArkWeb_ResourceHandler_DidFinish not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidFinish(resourceHandler);
}

void OH_ArkWebResourceHandler_DidFailWithError(const ArkWeb_ResourceHandler* resourceHandler,
                                               ArkWeb_NetError errorCode)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidFailWithError) {
        WVLOG_E("OH_ArkWeb_ResourceHandler_DidFailWithError not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWebResourceHandler_DidFailWithError(resourceHandler, errorCode);
}

void OH_ArkWeb_ReleaseString(char* string)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_ReleaseString) {
        WVLOG_E("OH_ArkWeb_ReleaseString not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_ReleaseString(string);
}

void OH_ArkWeb_ReleaseByteArray(uint8_t* byteArray)
{
    if (!g_SchemeHandlerApi || !g_SchemeHandlerApi->impl_OH_ArkWeb_ReleaseByteArray) {
        WVLOG_E("OH_ArkWeb_ReleaseByteArray not found.");
        return;
    }
 
    return g_SchemeHandlerApi->impl_OH_ArkWeb_ReleaseByteArray(byteArray);
}
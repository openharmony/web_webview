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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <securec.h>

#define private public
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_config_helper.h"
#include "nweb_adapter_helper.h"
#include "base/web/webview/interfaces/native/arkweb_scheme_handler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {

typedef void (*TYPE_OH_ArkWebRequestHeaderList_Destroy)(ArkWeb_RequestHeaderList* requestHeaderList);
typedef int32_t (*TYPE_OH_ArkWebRequestHeaderList_GetSize)(const ArkWeb_RequestHeaderList* requestHeaderList);
typedef void (*TYPE_OH_ArkWebRequestHeaderList_GetHeader)(
    const ArkWeb_RequestHeaderList* requestHeaderList, int32_t index, char** key, char** value);
typedef void (*TYPE_OH_ArkWebResourceRequest_GetMethod)(const ArkWeb_ResourceRequest* resourceRequest, char** method);
typedef void (*TYPE_OH_ArkWebResourceRequest_GetUrl)(const ArkWeb_ResourceRequest* resourceRequest, char** url);
typedef void (*TYPE_OH_ArkWebResourceRequest_GetHttpBodyStream)(
    const ArkWeb_ResourceRequest* resourceRequest, ArkWeb_HttpBodyStream** httpBodyStream);
typedef void (*TYPE_OH_ArkWebResourceRequest_DestroyHttpBodyStream)(ArkWeb_HttpBodyStream* httpBodyStream);
typedef int32_t (*TYPE_OH_ArkWebResourceRequest_GetResourceType)(const ArkWeb_ResourceRequest* resourceRequest);
typedef void (*TYPE_OH_ArkWebResourceRequest_GetFrameUrl)(
    const ArkWeb_ResourceRequest* resourceRequest, char** frameUrl);
typedef int32_t (*TYPE_OH_ArkWebHttpBodyStream_SetUserData)(ArkWeb_HttpBodyStream* httpBodyStream, void* userData);
typedef void* (*TYPE_OH_ArkWebHttpBodyStream_GetUserData)(const ArkWeb_HttpBodyStream* httpBodyStream);
typedef int32_t (*TYPE_OH_ArkWebHttpBodyStream_SetReadCallback)(
    ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_HttpBodyStreamReadCallback readCallback);
typedef int32_t (*TYPE_OH_ArkWebHttpBodyStream_Init)(
    ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_HttpBodyStreamInitCallback initCallback);
typedef void (*TYPE_OH_ArkWebHttpBodyStream_Read)(
    const ArkWeb_HttpBodyStream* httpBodyStream, uint8_t* buffer, int bufLen);
typedef uint64_t (*TYPE_OH_ArkWebHttpBodyStream_GetSize)(const ArkWeb_HttpBodyStream* httpBodyStream);
typedef uint64_t (*TYPE_OH_ArkWebHttpBodyStream_GetPosition)(const ArkWeb_HttpBodyStream* httpBodyStream);
typedef bool (*TYPE_OH_ArkWebHttpBodyStream_IsChunked)(const ArkWeb_HttpBodyStream* httpBodyStream);
typedef bool (*TYPE_OH_ArkWebHttpBodyStream_IsEof)(const ArkWeb_HttpBodyStream* httpBodyStream);
typedef bool (*TYPE_OH_ArkWebHttpBodyStream_IsInMemory)(const ArkWeb_HttpBodyStream* httpBodyStream);
typedef int32_t (*TYPE_OH_ArkWebResourceRequest_Destroy)(const ArkWeb_ResourceRequest* resourceRequest);
typedef void (*TYPE_OH_ArkWebResourceRequest_GetReferrer)(
    const ArkWeb_ResourceRequest* resourceRequest, char** referrer);
typedef void (*TYPE_OH_ArkWebResourceRequest_GetRequestHeaders)(
    const ArkWeb_ResourceRequest* resourceRequest, ArkWeb_RequestHeaderList** requestHeaderList);
typedef int32_t (*TYPE_OH_ArkWebResourceRequest_SetUserData)(ArkWeb_ResourceRequest* resourceRequest, void* userData);
typedef void* (*TYPE_OH_ArkWebResourceRequest_GetUserData)(const ArkWeb_ResourceRequest* resourceRequest);
typedef bool (*TYPE_OH_ArkWebResourceRequest_IsRedirect)(const ArkWeb_ResourceRequest* resourceRequest);
typedef bool (*TYPE_OH_ArkWebResourceRequest_IsMainFrame)(const ArkWeb_ResourceRequest* resourceRequest);
typedef bool (*TYPE_OH_ArkWebResourceRequest_HasGesture)(const ArkWeb_ResourceRequest* resourceRequest);
typedef int32_t (*TYPE_OH_ArkWeb_RegisterCustomSchemes)(const char* scheme, int32_t option);
typedef bool (*TYPE_OH_ArkWeb_SetSchemeHandler)(
    const char* scheme, const char* webTag, ArkWeb_SchemeHandler* schemeHandler);
typedef bool (*TYPE_OH_ArkWebServiceWorker_SetSchemeHandler)(const char* scheme, ArkWeb_SchemeHandler* schemeHandler);
typedef int32_t (*TYPE_OH_ArkWeb_ClearSchemeHandlers)(const char* webTag);
typedef int32_t (*TYPE_OH_ArkWebServiceWorker_ClearSchemeHandlers)();
typedef void (*TYPE_OH_ArkWeb_CreateSchemeHandler)(ArkWeb_SchemeHandler** schemeHandler);
typedef void (*TYPE_OH_ArkWeb_DestroySchemeHandler)(ArkWeb_SchemeHandler* schemeHandler);
typedef int32_t (*TYPE_OH_ArkWebSchemeHandler_SetUserData)(ArkWeb_SchemeHandler* schemeHandler, void* userData);
typedef void* (*TYPE_OH_ArkWebSchemeHandler_GetUserData)(const ArkWeb_SchemeHandler* schemeHandler);
typedef int32_t (*TYPE_OH_ArkWebSchemeHandler_SetOnRequestStart)(
    ArkWeb_SchemeHandler* schemeHandler, ArkWeb_OnRequestStart onRequestStart);
typedef int32_t (*TYPE_OH_ArkWebSchemeHandler_SetOnRequestStop)(
    ArkWeb_SchemeHandler* schemeHandler, ArkWeb_OnRequestStop onRequestStop);
typedef void (*TYPE_OH_ArkWeb_CreateResponse)(ArkWeb_Response** response);
typedef void (*TYPE_OH_ArkWeb_DestroyResponse)(ArkWeb_Response* response);
typedef int32_t (*TYPE_OH_ArkWebResponse_SetUrl)(ArkWeb_Response* response, const char* url);
typedef void (*TYPE_OH_ArkWebResponse_GetUrl)(const ArkWeb_Response* response, char** url);
typedef int32_t (*TYPE_OH_ArkWebResponse_SetError)(ArkWeb_Response* response, ArkWeb_NetError errorCode);
typedef ArkWeb_NetError (*TYPE_OH_ArkWebResponse_GetError)(const ArkWeb_Response* response);
typedef int32_t (*TYPE_OH_ArkWebResponse_SetStatus)(ArkWeb_Response* response, int status);
typedef int (*TYPE_OH_ArkWebResponse_GetStatus)(const ArkWeb_Response* response);
typedef int32_t (*TYPE_OH_ArkWebResponse_SetStatusText)(ArkWeb_Response* response, const char* statusText);
typedef void (*TYPE_OH_ArkWebResponse_GetStatusText)(const ArkWeb_Response* response, char** statusText);
typedef int32_t (*TYPE_OH_ArkWebResponse_SetMimeType)(ArkWeb_Response* response, const char* mimeType);
typedef void (*TYPE_OH_ArkWebResponse_GetMimeType)(const ArkWeb_Response* response, char** mimeType);
typedef int32_t (*TYPE_OH_ArkWebResponse_SetCharset)(ArkWeb_Response* response, const char* charset);
typedef void (*TYPE_OH_ArkWebResponse_GetCharset)(const ArkWeb_Response* response, char** charset);
typedef int32_t (*TYPE_OH_ArkWebResponse_SetHeaderByName)(
    ArkWeb_Response* response, const char* name, const char* value, bool ow);
typedef void (*TYPE_OH_ArkWebResponse_GetHeaderByName)(const ArkWeb_Response* response, const char* name, char** value);
typedef int32_t (*TYPE_OH_ArkWebResourceHandler_Destroy)(const ArkWeb_ResourceHandler* resourceHandler);
typedef int32_t (*TYPE_OH_ArkWebResourceHandler_DidReceiveResponse)(
    const ArkWeb_ResourceHandler* resourceHandler, const ArkWeb_Response* response);
typedef int32_t (*TYPE_OH_ArkWebResourceHandler_DidReceiveData)(
    const ArkWeb_ResourceHandler* resourceHandler, const uint8_t* buffer, int64_t bufLen);
typedef int32_t (*TYPE_OH_ArkWebResourceHandler_DidFinish)(const ArkWeb_ResourceHandler* resourceHandler);
typedef int32_t (*TYPE_OH_ArkWebResourceHandler_DidFailWithError)(
    const ArkWeb_ResourceHandler* resourceHandler, ArkWeb_NetError errorCode);
typedef void (*TYPE_OH_ArkWeb_ReleaseString)(char* string);
typedef void (*TYPE_OH_ArkWeb_ReleaseByteArray)(uint8_t* byteArray);
typedef int32_t (*TYPE_OH_ArkWebSchemeHandler_SetFromEts)(ArkWeb_SchemeHandler* schemeHandler, bool fromEts);

struct SchemeHandlerApi {
    TYPE_OH_ArkWebRequestHeaderList_Destroy impl_OH_ArkWebRequestHeaderList_Destroy;
    TYPE_OH_ArkWebRequestHeaderList_GetSize impl_OH_ArkWebRequestHeaderList_GetSize;
    TYPE_OH_ArkWebRequestHeaderList_GetHeader impl_OH_ArkWebRequestHeaderList_GetHeader;
    TYPE_OH_ArkWebResourceRequest_GetMethod impl_OH_ArkWebResourceRequest_GetMethod;
    TYPE_OH_ArkWebResourceRequest_GetUrl impl_OH_ArkWebResourceRequest_GetUrl;
    TYPE_OH_ArkWebResourceRequest_GetHttpBodyStream impl_OH_ArkWebResourceRequest_GetHttpBodyStream;
    TYPE_OH_ArkWebResourceRequest_DestroyHttpBodyStream impl_OH_ArkWebResourceRequest_DestroyHttpBodyStream;
    TYPE_OH_ArkWebResourceRequest_GetResourceType impl_OH_ArkWebResourceRequest_GetResourceType;
    TYPE_OH_ArkWebResourceRequest_GetFrameUrl impl_OH_ArkWebResourceRequest_GetFrameUrl;
    TYPE_OH_ArkWebHttpBodyStream_SetReadCallback impl_OH_ArkWebHttpBodyStream_SetReadCallback;
    TYPE_OH_ArkWebHttpBodyStream_SetUserData impl_OH_ArkWebHttpBodyStream_SetUserData;
    TYPE_OH_ArkWebHttpBodyStream_GetUserData impl_OH_ArkWebHttpBodyStream_GetUserData;
    TYPE_OH_ArkWebHttpBodyStream_Init impl_OH_ArkWebHttpBodyStream_Init;
    TYPE_OH_ArkWebHttpBodyStream_Read impl_OH_ArkWebHttpBodyStream_Read;
    TYPE_OH_ArkWebHttpBodyStream_GetSize impl_OH_ArkWebHttpBodyStream_GetSize;
    TYPE_OH_ArkWebHttpBodyStream_GetPosition impl_OH_ArkWebHttpBodyStream_GetPosition;
    TYPE_OH_ArkWebHttpBodyStream_IsChunked impl_OH_ArkWebHttpBodyStream_IsChunked;
    TYPE_OH_ArkWebHttpBodyStream_IsEof impl_OH_ArkWebHttpBodyStream_IsEof;
    TYPE_OH_ArkWebHttpBodyStream_IsInMemory impl_OH_ArkWebHttpBodyStream_IsInMemory;
    TYPE_OH_ArkWebResourceRequest_Destroy impl_OH_ArkWebResourceRequest_Destroy;
    TYPE_OH_ArkWebResourceRequest_SetUserData impl_OH_ArkWebResourceRequest_SetUserData;
    TYPE_OH_ArkWebResourceRequest_GetUserData impl_OH_ArkWebResourceRequest_GetUserData;
    TYPE_OH_ArkWebResourceRequest_GetReferrer impl_OH_ArkWebResourceRequest_GetReferrer;
    TYPE_OH_ArkWebResourceRequest_GetRequestHeaders impl_OH_ArkWebResourceRequest_GetRequestHeaders;
    TYPE_OH_ArkWebResourceRequest_IsRedirect impl_OH_ArkWebResourceRequest_IsRedirect;
    TYPE_OH_ArkWebResourceRequest_IsMainFrame impl_OH_ArkWebResourceRequest_IsMainFrame;
    TYPE_OH_ArkWebResourceRequest_HasGesture impl_OH_ArkWebResourceRequest_HasGesture;
    TYPE_OH_ArkWeb_RegisterCustomSchemes impl_OH_ArkWeb_RegisterCustomSchemes;
    TYPE_OH_ArkWeb_SetSchemeHandler impl_OH_ArkWeb_SetSchemeHandler;
    TYPE_OH_ArkWebServiceWorker_SetSchemeHandler impl_OH_ArkWebServiceWorker_SetSchemeHandler;
    TYPE_OH_ArkWebServiceWorker_ClearSchemeHandlers impl_OH_ArkWebServiceWorker_ClearSchemeHandlers;
    TYPE_OH_ArkWeb_ClearSchemeHandlers impl_OH_ArkWeb_ClearSchemeHandlers;
    TYPE_OH_ArkWeb_CreateSchemeHandler impl_OH_ArkWeb_CreateSchemeHandler;
    TYPE_OH_ArkWeb_DestroySchemeHandler impl_OH_ArkWeb_DestroySchemeHandler;
    TYPE_OH_ArkWebSchemeHandler_SetOnRequestStart impl_OH_ArkWebSchemeHandler_SetOnRequestStart;
    TYPE_OH_ArkWebSchemeHandler_SetOnRequestStop impl_OH_ArkWebSchemeHandler_SetOnRequestStop;
    TYPE_OH_ArkWebSchemeHandler_SetUserData impl_OH_ArkWebSchemeHandler_SetUserData;
    TYPE_OH_ArkWebSchemeHandler_GetUserData impl_OH_ArkWebSchemeHandler_GetUserData;
    TYPE_OH_ArkWeb_CreateResponse impl_OH_ArkWeb_CreateResponse;
    TYPE_OH_ArkWeb_DestroyResponse impl_OH_ArkWeb_DestroyResponse;
    TYPE_OH_ArkWebResponse_GetUrl impl_OH_ArkWebResponse_GetUrl;
    TYPE_OH_ArkWebResponse_SetUrl impl_OH_ArkWebResponse_SetUrl;
    TYPE_OH_ArkWebResponse_SetError impl_OH_ArkWebResponse_SetError;
    TYPE_OH_ArkWebResponse_GetError impl_OH_ArkWebResponse_GetError;
    TYPE_OH_ArkWebResponse_SetStatus impl_OH_ArkWebResponse_SetStatus;
    TYPE_OH_ArkWebResponse_GetStatus impl_OH_ArkWebResponse_GetStatus;
    TYPE_OH_ArkWebResponse_SetStatusText impl_OH_ArkWebResponse_SetStatusText;
    TYPE_OH_ArkWebResponse_GetStatusText impl_OH_ArkWebResponse_GetStatusText;
    TYPE_OH_ArkWebResponse_SetMimeType impl_OH_ArkWebResponse_SetMimeType;
    TYPE_OH_ArkWebResponse_GetMimeType impl_OH_ArkWebResponse_GetMimeType;
    TYPE_OH_ArkWebResponse_SetCharset impl_OH_ArkWebResponse_SetCharset;
    TYPE_OH_ArkWebResponse_GetCharset impl_OH_ArkWebResponse_GetCharset;
    TYPE_OH_ArkWebResponse_SetHeaderByName impl_OH_ArkWebResponse_SetHeaderByName;
    TYPE_OH_ArkWebResponse_GetHeaderByName impl_OH_ArkWebResponse_GetHeaderByName;
    TYPE_OH_ArkWebResourceHandler_Destroy impl_OH_ArkWebResourceHandler_Destroy;
    TYPE_OH_ArkWebResourceHandler_DidReceiveResponse impl_OH_ArkWebResourceHandler_DidReceiveResponse;
    TYPE_OH_ArkWebResourceHandler_DidReceiveData impl_OH_ArkWebResourceHandler_DidReceiveData;
    TYPE_OH_ArkWebResourceHandler_DidFinish impl_OH_ArkWebResourceHandler_DidFinish;
    TYPE_OH_ArkWebResourceHandler_DidFailWithError impl_OH_ArkWebResourceHandler_DidFailWithError;
    TYPE_OH_ArkWeb_ReleaseString impl_OH_ArkWeb_ReleaseString;
    TYPE_OH_ArkWeb_ReleaseByteArray impl_OH_ArkWeb_ReleaseByteArray;
    TYPE_OH_ArkWebSchemeHandler_SetFromEts impl_OH_ArkWebSchemeHandler_SetFromEts;
};

} // namespace

void TEST_OH_ArkWebRequestHeaderList_Destroy(ArkWeb_RequestHeaderList* requestHeaderList) {
    return;
}

int32_t TEST_OH_ArkWebRequestHeaderList_GetSize(const ArkWeb_RequestHeaderList* requestHeaderList) {
    return 0;
}

void TEST_OH_ArkWebRequestHeaderList_GetHeader(
    const ArkWeb_RequestHeaderList* requestHeaderList, int32_t index, char** key, char** value) {
    return;
}

void TEST_OH_ArkWebResourceRequest_GetMethod(const ArkWeb_ResourceRequest* resourceRequest, char** method) {
    return;
}

void TEST_OH_ArkWebResourceRequest_GetUrl(const ArkWeb_ResourceRequest* resourceRequest, char** url) {
    return;
}

void TEST_OH_ArkWebResourceRequest_GetHttpBodyStream(
    const ArkWeb_ResourceRequest* resourceRequest, ArkWeb_HttpBodyStream** httpBodyStream) {
    return;
}

void TEST_OH_ArkWebResourceRequest_DestroyHttpBodyStream(ArkWeb_HttpBodyStream* httpBodyStream) {
    return;
}

int32_t TEST_OH_ArkWebResourceRequest_GetResourceType(const ArkWeb_ResourceRequest* resourceRequest) {
    return 0;
}

void TEST_OH_ArkWebResourceRequest_GetFrameUrl(const ArkWeb_ResourceRequest* resourceRequest, char** frameUrl) {
    return;
}

int32_t TEST_OH_ArkWebHttpBodyStream_SetUserData(ArkWeb_HttpBodyStream* httpBodyStream, void* userData) {
    return 0;
}

void* TEST_OH_ArkWebHttpBodyStream_GetUserData(const ArkWeb_HttpBodyStream* httpBodyStream) {
    return nullptr;
}

int32_t TEST_OH_ArkWebHttpBodyStream_SetReadCallback(
    ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_HttpBodyStreamReadCallback readCallback) {
    return 0;
}

int32_t TEST_OH_ArkWebHttpBodyStream_Init(
    ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_HttpBodyStreamInitCallback initCallback) {
    return 0;
}

void TEST_OH_ArkWebHttpBodyStream_Read(
    const ArkWeb_HttpBodyStream* httpBodyStream, uint8_t* buffer, int bufLen) {
    return;
}

uint64_t TEST_OH_ArkWebHttpBodyStream_GetSize(const ArkWeb_HttpBodyStream* httpBodyStream) {
    return 1;
}

uint64_t TEST_OH_ArkWebHttpBodyStream_GetPosition(const ArkWeb_HttpBodyStream* httpBodyStream) {
    return 1;
}

bool TEST_OH_ArkWebHttpBodyStream_IsChunked(const ArkWeb_HttpBodyStream* httpBodyStream) {
    return true;
}

bool TEST_OH_ArkWebHttpBodyStream_IsEof(const ArkWeb_HttpBodyStream* httpBodyStream) {
    return true;
}

bool TEST_OH_ArkWebHttpBodyStream_IsInMemory(const ArkWeb_HttpBodyStream* httpBodyStream) {
    return true;
}

int32_t TEST_OH_ArkWebResourceRequest_Destroy(const ArkWeb_ResourceRequest* resourceRequest) {
    return 0;
}

void TEST_OH_ArkWebResourceRequest_GetReferrer(const ArkWeb_ResourceRequest* resourceRequest, char** referrer) {
    return;
}

int32_t TEST_OH_ArkWebResourceRequest_SetUserData(ArkWeb_ResourceRequest* resourceRequest, void* userData) {
    return 0;
}

void* TEST_OH_ArkWebResourceRequest_GetUserData(const ArkWeb_ResourceRequest* resourceRequest) {
    return NULL;
}

bool TEST_OH_ArkWebResourceRequest_IsRedirect(const ArkWeb_ResourceRequest* resourceRequest) {
    return true;
}

bool TEST_OH_ArkWebResourceRequest_IsMainFrame(const ArkWeb_ResourceRequest* resourceRequest) {
    return true;
}

bool TEST_OH_ArkWebResourceRequest_HasGesture(const ArkWeb_ResourceRequest* resourceRequest) {
    return true;
}

int32_t TEST_OH_ArkWeb_RegisterCustomSchemes(const char* scheme, int32_t option) {
    return 0;
}

bool TEST_OH_ArkWeb_SetSchemeHandler(
    const char* scheme, const char* webTag, ArkWeb_SchemeHandler* schemeHandler) {
    return true;
}

bool TEST_OH_ArkWebServiceWorker_SetSchemeHandler(
    const char* scheme, ArkWeb_SchemeHandler* schemeHandler) {
    return true;
}

int32_t TEST_OH_ArkWeb_ClearSchemeHandlers(const char* webTag) {
    return 0;
}

int32_t TEST_OH_ArkWebServiceWorker_ClearSchemeHandlers() {
    return 0;
}

void TEST_OH_ArkWeb_CreateSchemeHandler(ArkWeb_SchemeHandler** schemeHandler) {
    return;
}

void TEST_OH_ArkWeb_DestroySchemeHandler(ArkWeb_SchemeHandler* schemeHandler) {
    return;
}

int32_t TEST_OH_ArkWebSchemeHandler_SetUserData(ArkWeb_SchemeHandler* schemeHandler, void* userData) {
    return 0;
}

void* TEST_OH_ArkWebSchemeHandler_GetUserData(const ArkWeb_SchemeHandler* schemeHandler) {
    return nullptr;
}

int32_t TEST_OH_ArkWebSchemeHandler_SetOnRequestStart(
    ArkWeb_SchemeHandler* schemeHandler, ArkWeb_OnRequestStart onRequestStart) {
    return 0;
}

int32_t TEST_OH_ArkWebSchemeHandler_SetOnRequestStop(
    ArkWeb_SchemeHandler* schemeHandler, ArkWeb_OnRequestStop onRequestStop) {
    return 0;
}

void TEST_OH_ArkWeb_CreateResponse(ArkWeb_Response** response) {
    return;
}

void TEST_OH_ArkWeb_DestroyResponse(ArkWeb_Response* response) {
    return;
}

int32_t TEST_OH_ArkWebResponse_SetUrl(ArkWeb_Response* response, const char* url) {
    return 0;
}

void TEST_OH_ArkWebResponse_GetUrl(const ArkWeb_Response* response, char** url) {
    return;
}

int32_t TEST_OH_ArkWebResponse_SetError(ArkWeb_Response* response, ArkWeb_NetError errorCode) {
    return 0;
}

ArkWeb_NetError TEST_OH_ArkWebResponse_GetError(const ArkWeb_Response* response) {
    return ARKWEB_NET_OK;
}

int32_t TEST_OH_ArkWebResponse_SetStatus(ArkWeb_Response* response, int status) {
    return 0;
}

int TEST_OH_ArkWebResponse_GetStatus(const ArkWeb_Response* response) {
    return 0;
}

int32_t TEST_OH_ArkWebResponse_SetStatusText(ArkWeb_Response* response, const char* statusText) {
    return 0;
}

void TEST_OH_ArkWebResponse_GetStatusText(const ArkWeb_Response* response, char** statusText) {
    return;
}

int32_t TEST_OH_ArkWebResponse_SetMimeType(ArkWeb_Response* response, const char* mimeType) {
    return 0;
}

void TEST_OH_ArkWebResponse_GetMimeType(const ArkWeb_Response* response, char** mimeType) {
    return;
}

int32_t TEST_OH_ArkWebResponse_SetCharset(ArkWeb_Response* response, const char* charset) {
    return 0;
}

void TEST_OH_ArkWebResponse_GetCharset(const ArkWeb_Response* response, char** charset) {
    return;
}

int32_t TEST_OH_ArkWebResponse_SetHeaderByName(
    ArkWeb_Response* response, const char* name, const char* value, bool ow) {
    return 0;
}

void TEST_OH_ArkWebResponse_GetHeaderByName(const ArkWeb_Response* response, const char* name, char** value) {
    return;
}

int32_t TEST_OH_ArkWebResourceHandler_Destroy(const ArkWeb_ResourceHandler* resourceHandler) {
    return 0;
}

int32_t TEST_OH_ArkWebResourceHandler_DidReceiveResponse(
    const ArkWeb_ResourceHandler* resourceHandler, const ArkWeb_Response* response) {
    return 0;
}

int32_t TEST_OH_ArkWebResourceHandler_DidReceiveData(
    const ArkWeb_ResourceHandler* resourceHandler, const uint8_t* buffer, int64_t bufLen) {
    return 0;
}

int32_t TEST_OH_ArkWebResourceHandler_DidFinish(const ArkWeb_ResourceHandler* resourceHandler) {
    return 0;
}

int32_t TEST_OH_ArkWebResourceHandler_DidFailWithError(
    const ArkWeb_ResourceHandler* resourceHandler, ArkWeb_NetError errorCode) {
    return 0;
}

void TEST_OH_ArkWeb_ReleaseString(char* string) {
    return;
}

void TEST_OH_ArkWeb_ReleaseByteArray(uint8_t* byteArray) {
    return;
}

int32_t TEST_OH_ArkWebSchemeHandler_SetFromEts(ArkWeb_SchemeHandler* schemeHandler, bool fromEts) {
    return 0;
}

void TEST_OH_ArkWebResourceRequest_GetRequestHeaders(
    const ArkWeb_ResourceRequest* resourceRequest, ArkWeb_RequestHeaderList** requestHeaderList) {
    return;
}

SchemeHandlerApi g_testSchemeHandlerApi = {
    .impl_OH_ArkWebRequestHeaderList_Destroy = TEST_OH_ArkWebRequestHeaderList_Destroy,
    .impl_OH_ArkWebRequestHeaderList_GetSize = TEST_OH_ArkWebRequestHeaderList_GetSize,
    .impl_OH_ArkWebRequestHeaderList_GetHeader = TEST_OH_ArkWebRequestHeaderList_GetHeader,
    .impl_OH_ArkWebResourceRequest_GetMethod = TEST_OH_ArkWebResourceRequest_GetMethod,
    .impl_OH_ArkWebResourceRequest_GetUrl = TEST_OH_ArkWebResourceRequest_GetUrl,
    .impl_OH_ArkWebResourceRequest_GetHttpBodyStream = TEST_OH_ArkWebResourceRequest_GetHttpBodyStream,
    .impl_OH_ArkWebResourceRequest_DestroyHttpBodyStream = TEST_OH_ArkWebResourceRequest_DestroyHttpBodyStream,
    .impl_OH_ArkWebResourceRequest_GetResourceType = TEST_OH_ArkWebResourceRequest_GetResourceType,
    .impl_OH_ArkWebResourceRequest_GetFrameUrl = TEST_OH_ArkWebResourceRequest_GetFrameUrl,
    .impl_OH_ArkWebHttpBodyStream_SetUserData = TEST_OH_ArkWebHttpBodyStream_SetUserData,
    .impl_OH_ArkWebHttpBodyStream_GetUserData = TEST_OH_ArkWebHttpBodyStream_GetUserData,
    .impl_OH_ArkWebHttpBodyStream_SetReadCallback = TEST_OH_ArkWebHttpBodyStream_SetReadCallback,
    .impl_OH_ArkWebHttpBodyStream_Init = TEST_OH_ArkWebHttpBodyStream_Init,
    .impl_OH_ArkWebHttpBodyStream_Read = TEST_OH_ArkWebHttpBodyStream_Read,
    .impl_OH_ArkWebHttpBodyStream_GetSize = TEST_OH_ArkWebHttpBodyStream_GetSize,
    .impl_OH_ArkWebHttpBodyStream_GetPosition = TEST_OH_ArkWebHttpBodyStream_GetPosition,
    .impl_OH_ArkWebHttpBodyStream_IsChunked = TEST_OH_ArkWebHttpBodyStream_IsChunked,
    .impl_OH_ArkWebHttpBodyStream_IsEof = TEST_OH_ArkWebHttpBodyStream_IsEof,
    .impl_OH_ArkWebHttpBodyStream_IsInMemory = TEST_OH_ArkWebHttpBodyStream_IsInMemory,
    .impl_OH_ArkWebResourceRequest_Destroy = TEST_OH_ArkWebResourceRequest_Destroy,
    .impl_OH_ArkWebResourceRequest_SetUserData = TEST_OH_ArkWebResourceRequest_SetUserData,
    .impl_OH_ArkWebResourceRequest_GetUserData = TEST_OH_ArkWebResourceRequest_GetUserData,
    .impl_OH_ArkWebResourceRequest_GetReferrer = TEST_OH_ArkWebResourceRequest_GetReferrer,
    .impl_OH_ArkWebResourceRequest_GetRequestHeaders = TEST_OH_ArkWebResourceRequest_GetRequestHeaders,
    .impl_OH_ArkWebResourceRequest_IsRedirect = TEST_OH_ArkWebResourceRequest_IsRedirect,
    .impl_OH_ArkWebResourceRequest_IsMainFrame = TEST_OH_ArkWebResourceRequest_IsMainFrame,
    .impl_OH_ArkWebResourceRequest_HasGesture = TEST_OH_ArkWebResourceRequest_HasGesture,
    .impl_OH_ArkWeb_RegisterCustomSchemes = TEST_OH_ArkWeb_RegisterCustomSchemes,
    .impl_OH_ArkWeb_SetSchemeHandler = TEST_OH_ArkWeb_SetSchemeHandler,
    .impl_OH_ArkWebServiceWorker_SetSchemeHandler = TEST_OH_ArkWebServiceWorker_SetSchemeHandler,
    .impl_OH_ArkWeb_ClearSchemeHandlers = TEST_OH_ArkWeb_ClearSchemeHandlers,
    .impl_OH_ArkWebServiceWorker_ClearSchemeHandlers = TEST_OH_ArkWebServiceWorker_ClearSchemeHandlers,
    .impl_OH_ArkWeb_CreateSchemeHandler = TEST_OH_ArkWeb_CreateSchemeHandler,
    .impl_OH_ArkWeb_DestroySchemeHandler = TEST_OH_ArkWeb_DestroySchemeHandler,
    .impl_OH_ArkWebSchemeHandler_SetUserData = TEST_OH_ArkWebSchemeHandler_SetUserData,
    .impl_OH_ArkWebSchemeHandler_GetUserData = TEST_OH_ArkWebSchemeHandler_GetUserData,
    .impl_OH_ArkWebSchemeHandler_SetOnRequestStart = TEST_OH_ArkWebSchemeHandler_SetOnRequestStart,
    .impl_OH_ArkWebSchemeHandler_SetOnRequestStop = TEST_OH_ArkWebSchemeHandler_SetOnRequestStop,
    .impl_OH_ArkWeb_CreateResponse = TEST_OH_ArkWeb_CreateResponse,
    .impl_OH_ArkWeb_DestroyResponse = TEST_OH_ArkWeb_DestroyResponse,
    .impl_OH_ArkWebResponse_SetUrl = TEST_OH_ArkWebResponse_SetUrl,
    .impl_OH_ArkWebResponse_GetUrl = TEST_OH_ArkWebResponse_GetUrl,
    .impl_OH_ArkWebResponse_SetError = TEST_OH_ArkWebResponse_SetError,
    .impl_OH_ArkWebResponse_GetError = TEST_OH_ArkWebResponse_GetError,
    .impl_OH_ArkWebResponse_SetStatus = TEST_OH_ArkWebResponse_SetStatus,
    .impl_OH_ArkWebResponse_GetStatus = TEST_OH_ArkWebResponse_GetStatus,
    .impl_OH_ArkWebResponse_SetStatusText = TEST_OH_ArkWebResponse_SetStatusText,
    .impl_OH_ArkWebResponse_GetStatusText = TEST_OH_ArkWebResponse_GetStatusText,
    .impl_OH_ArkWebResponse_SetMimeType = TEST_OH_ArkWebResponse_SetMimeType,
    .impl_OH_ArkWebResponse_GetMimeType = TEST_OH_ArkWebResponse_GetMimeType,
    .impl_OH_ArkWebResponse_SetCharset = TEST_OH_ArkWebResponse_SetCharset,
    .impl_OH_ArkWebResponse_GetCharset = TEST_OH_ArkWebResponse_GetCharset,
    .impl_OH_ArkWebResponse_SetHeaderByName = TEST_OH_ArkWebResponse_SetHeaderByName,
    .impl_OH_ArkWebResponse_GetHeaderByName = TEST_OH_ArkWebResponse_GetHeaderByName,
    .impl_OH_ArkWebResourceHandler_Destroy = TEST_OH_ArkWebResourceHandler_Destroy,
    .impl_OH_ArkWebResourceHandler_DidReceiveResponse = TEST_OH_ArkWebResourceHandler_DidReceiveResponse,
    .impl_OH_ArkWebResourceHandler_DidReceiveData = TEST_OH_ArkWebResourceHandler_DidReceiveData,
    .impl_OH_ArkWebResourceHandler_DidFinish = TEST_OH_ArkWebResourceHandler_DidFinish,
    .impl_OH_ArkWebResourceHandler_DidFailWithError = TEST_OH_ArkWebResourceHandler_DidFailWithError,
    .impl_OH_ArkWeb_ReleaseString = TEST_OH_ArkWeb_ReleaseString,
    .impl_OH_ArkWeb_ReleaseByteArray = TEST_OH_ArkWeb_ReleaseByteArray,
    .impl_OH_ArkWebSchemeHandler_SetFromEts = TEST_OH_ArkWebSchemeHandler_SetFromEts
};


namespace NWeb {
class MockNWebEngine : public OHOS::NWeb::NWebEngine {
public:
    std::shared_ptr<NWeb> CreateNWeb(std::shared_ptr<NWebCreateInfo> create_info)
    {
        return nullptr;
    }

    std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id)
    {
        return nullptr;
    }

    std::shared_ptr<NWebDataBase> GetDataBase()
    {
        return nullptr;
    }

    std::shared_ptr<NWebWebStorage> GetWebStorage()
    {
        return nullptr;
    }

    std::shared_ptr<NWebCookieManager> GetCookieManager()
    {
        return nullptr;
    }

    std::shared_ptr<NWebDownloadManager> GetDownloadManager()
    {
        return nullptr;
    }

    void SetWebTag(int32_t nweb_id, const char* web_tag) {}

    void InitializeWebEngine(std::shared_ptr<NWebEngineInitArgs> init_args) {}

    void PrepareForPageLoad(const std::string& url, bool preconnectable, int32_t num_sockets) {}

    void SetWebDebuggingAccess(bool isEnableDebug) {}

    void AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) {}

    void RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) {}
    void ClearIntelligentTrackingPreventionBypassingList() {}

    std::string GetDefaultUserAgent()
    {
        return "";
    }

    void PauseAllTimers() {}

    void ResumeAllTimers() {}

    void PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
        const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
        const uint32_t& cache_valid_time)
    {}

    void SetRenderProcessMode(RenderProcessMode mode) {}

    RenderProcessMode GetRenderProcessMode()
    {
        return RenderProcessMode::SINGLE_MODE;
    }

    void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list) {}

    void WarmupServiceWorker(const std::string& url) {}

    void SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime) {}

    void ClearHostIP(const std::string& hostName) {}

    void SetWholeWebDrawing() {}

    std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager()
    {
        return nullptr;
    }

    void EnableBackForwardCache(bool nativeEmbed, bool mediaTakeOver) {}

    void RemoveAllCache(bool include_disk_files) {}
};


class OHArkwebSchemeHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void OHArkwebSchemeHandlerTest::SetUpTestCase(void)
{}

void OHArkwebSchemeHandlerTest::TearDownTestCase(void)
{}

void OHArkwebSchemeHandlerTest::SetUp(void)
{}

void OHArkwebSchemeHandlerTest::TearDown(void)
{}


/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_001
 * @tc.desc  : g_SchemeHandlerApi is nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_001, TestSize.Level1) {
    ArkWeb_RequestHeaderList* headerList = nullptr;
    OH_ArkWebRequestHeaderList_Destroy(headerList);
    int32_t ret = OH_ArkWebRequestHeaderList_GetSize(headerList);
    EXPECT_EQ(ret, -1);

    int32_t index = 0;
    char* key = nullptr;
    char* value = nullptr;
    OH_ArkWebRequestHeaderList_GetHeader(headerList, index, &key, &value);

    ArkWeb_ResourceRequest* resourceRequest = nullptr;
    char* method = nullptr;
    OH_ArkWebResourceRequest_GetMethod(resourceRequest, &method);

    char* url = nullptr;
    OH_ArkWebResourceRequest_GetUrl(resourceRequest, &url);

    ArkWeb_HttpBodyStream* httpBodyStream = nullptr;
    OH_ArkWebResourceRequest_GetHttpBodyStream(resourceRequest, &httpBodyStream);
    OH_ArkWebResourceRequest_DestroyHttpBodyStream(httpBodyStream);

    ret = OH_ArkWebResourceRequest_GetResourceType(resourceRequest);
    EXPECT_EQ(ret, -1);

    char* frameUrl = nullptr;
    OH_ArkWebResourceRequest_GetFrameUrl(resourceRequest, &frameUrl);

    void* userData = nullptr;
    int32_t result = OH_ArkWebHttpBodyStream_SetUserData(httpBodyStream, userData);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    void* userDataGet = OH_ArkWebHttpBodyStream_GetUserData(httpBodyStream);
    EXPECT_EQ(userDataGet, nullptr);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_002
 * @tc.desc  : g_SchemeHandlerApi is nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_002, TestSize.Level1) {
    ArkWeb_HttpBodyStream* httpBodyStream = nullptr;
    ArkWeb_HttpBodyStreamReadCallback readCallback = nullptr;
    int32_t result = OH_ArkWebHttpBodyStream_SetReadCallback(httpBodyStream, readCallback);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    ArkWeb_HttpBodyStreamInitCallback initCallback = nullptr;
    result = OH_ArkWebHttpBodyStream_Init(httpBodyStream, initCallback);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    const ArkWeb_HttpBodyStream* httpBodyStream1 = nullptr;
    uint8_t buffer[256] = {0};
    int bufLen = sizeof(buffer);
    OH_ArkWebHttpBodyStream_Read(httpBodyStream1, buffer, bufLen);

    uint64_t size = OH_ArkWebHttpBodyStream_GetSize(httpBodyStream1);
    EXPECT_EQ(size, 0);
    uint64_t position = OH_ArkWebHttpBodyStream_GetPosition(httpBodyStream1);
    EXPECT_EQ(position, 0);
    bool boolResult = OH_ArkWebHttpBodyStream_IsChunked(httpBodyStream);
    EXPECT_FALSE(boolResult);
    boolResult = OH_ArkWebHttpBodyStream_IsEof(httpBodyStream);
    EXPECT_FALSE(boolResult);
    boolResult = OH_ArkWebHttpBodyStream_IsInMemory(httpBodyStream);
    EXPECT_FALSE(boolResult);

    const ArkWeb_ResourceRequest* resourceRequest = nullptr;
    result = OH_ArkWebResourceRequest_Destroy(resourceRequest);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* referrer = nullptr;
    OH_ArkWebResourceRequest_GetReferrer(resourceRequest, &referrer);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_003
 * @tc.desc  : g_SchemeHandlerApi is nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_003, TestSize.Level1) {
    ArkWeb_ResourceRequest* resourceRequest = nullptr;
    void* userData = nullptr;
    int32_t result = OH_ArkWebResourceRequest_SetUserData(resourceRequest, userData);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    const ArkWeb_ResourceRequest* resourceRequest1 = nullptr;
    void* userData1 = OH_ArkWebResourceRequest_GetUserData(resourceRequest1);
    EXPECT_EQ(userData1, nullptr);

    ArkWeb_RequestHeaderList* requestHeaderList = nullptr;
    OH_ArkWebResourceRequest_GetRequestHeaders(resourceRequest1, &requestHeaderList);
    EXPECT_EQ(requestHeaderList, nullptr);

    bool boolResult = OH_ArkWebResourceRequest_IsRedirect(resourceRequest1);
    EXPECT_EQ(boolResult, false);
    boolResult = OH_ArkWebResourceRequest_IsMainFrame(resourceRequest1);
    EXPECT_EQ(boolResult, false);
    boolResult = OH_ArkWebResourceRequest_HasGesture(resourceRequest);
    EXPECT_EQ(boolResult, false);

    const char* webTag = nullptr;
    result = OH_ArkWeb_ClearSchemeHandlers(webTag);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    result = OH_ArkWebServiceWorker_ClearSchemeHandlers();
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    OH_ArkWeb_DestroySchemeHandler(schemeHandler);

    result = OH_ArkWebSchemeHandler_SetUserData(schemeHandler, userData);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    const ArkWeb_SchemeHandler* schemeHandler1 = nullptr;
    void* userData2 = OH_ArkWebSchemeHandler_GetUserData(schemeHandler1);
    EXPECT_EQ(userData2, nullptr);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_004
 * @tc.desc  : g_SchemeHandlerApi is nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_004, TestSize.Level1) {
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    ArkWeb_OnRequestStart onRequestStart = nullptr;
    int32_t result = OH_ArkWebSchemeHandler_SetOnRequestStart(schemeHandler, onRequestStart);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    ArkWeb_OnRequestStop onRequestStop = nullptr;
    result = OH_ArkWebSchemeHandler_SetOnRequestStop(schemeHandler, onRequestStop);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    ArkWeb_Response* response = nullptr;
    OH_ArkWeb_DestroyResponse(response);
    const char* url = "www.example.com";
    result = OH_ArkWebResponse_SetUrl(response, url);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* url1 = nullptr;
    OH_ArkWebResponse_GetUrl(response, &url1);

    ArkWeb_NetError errorCode = ARKWEB_NET_OK;
    result = OH_ArkWebResponse_SetError(response, errorCode);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    ArkWeb_NetError errorCodeRes = OH_ArkWebResponse_GetError(response);
    EXPECT_EQ(errorCodeRes, ARKWEB_ERR_FAILED);
    int status = -1;
    result = OH_ArkWebResponse_SetStatus(response, status);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    int statusRes = OH_ArkWebResponse_GetStatus(response);
    EXPECT_EQ(statusRes, -1);

    const char* statusText = "Test Status Text";
    result = OH_ArkWebResponse_SetStatusText(response, statusText);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* statusTextGet = nullptr;
    OH_ArkWebResponse_GetStatusText(response, &statusTextGet);

    const char* mimeType = "text/plain";
    result = OH_ArkWebResponse_SetMimeType(response, mimeType);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* mimeTypeRes = nullptr;
    OH_ArkWebResponse_GetMimeType(response, &mimeTypeRes);
    const char* charSet = "";
    result = OH_ArkWebResponse_SetCharset(response, charSet);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* charSetRes = nullptr;
    OH_ArkWebResponse_GetCharset(response, &charSetRes);
    const char* name = "TestHeaderName";
    const char* value = "TestHeaderValue";
    result = OH_ArkWebResponse_SetHeaderByName(response, name, value, true);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* valueRes = nullptr;
    OH_ArkWebResponse_GetHeaderByName(response, name, &valueRes);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_005
 * @tc.desc  : g_SchemeHandlerApi is nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNull_005, TestSize.Level1) {
    ArkWeb_ResourceHandler* resourceHandler = nullptr;
    int32_t res = OH_ArkWebResourceHandler_Destroy(resourceHandler);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);

    ArkWeb_Response* response;
    res = OH_ArkWebResourceHandler_DidReceiveResponse(resourceHandler, response);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);

    const uint8_t* buffer = reinterpret_cast<const uint8_t*>("TestBuffer");
    int64_t bufLen = strlen("TestBuffer");
    res = OH_ArkWebResourceHandler_DidReceiveData(nullptr, buffer, bufLen);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);
    res = OH_ArkWebResourceHandler_DidFinish(nullptr);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);

    ArkWeb_NetError errorCode = ARKWEB_NET_OK;
    res = OH_ArkWebResourceHandler_DidFailWithError(resourceHandler, errorCode);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);
    char* str = nullptr;
    OH_ArkWeb_ReleaseString(str);
    uint8_t byteArray[10] = {0};
    OH_ArkWeb_ReleaseByteArray(byteArray);
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    res = OH_ArkWebSchemeHandler_SetFromEts(schemeHandler, true);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_WebEngineHandleIsNull_004
 * @tc.desc  : webEngineHandle is nullptr and g_SchemeHandlerApi is nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_WebEngineHandleIsNull_004, TestSize.Level1) {
    NWebHelper::Instance().bundlePath_ = "";
    const char* scheme = "scheme";
    int32_t option = 0;
    const char* webTag = "webTag";
    ArkWeb_SchemeHandler* schemeHandler = nullptr;

    EXPECT_EQ(OH_ArkWeb_RegisterCustomSchemes(scheme, option), ARKWEB_ERROR_UNKNOWN);
    EXPECT_FALSE(OH_ArkWeb_SetSchemeHandler(scheme, webTag, schemeHandler));
    EXPECT_FALSE(OH_ArkWebServiceWorker_SetSchemeHandler(scheme, schemeHandler));
    OH_ArkWeb_CreateSchemeHandler(&schemeHandler);
    ArkWeb_Response* response = nullptr;
    OH_ArkWeb_CreateResponse(&response);

    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    EXPECT_EQ(OH_ArkWeb_RegisterCustomSchemes(scheme, option), ARKWEB_ERROR_UNKNOWN);
    DestroySchemeHandlerApiForTest();
    EXPECT_FALSE(OH_ArkWeb_SetSchemeHandler(scheme, webTag, schemeHandler));
    DestroySchemeHandlerApiForTest();
    EXPECT_FALSE(OH_ArkWebServiceWorker_SetSchemeHandler(scheme, schemeHandler));
    DestroySchemeHandlerApiForTest();
    OH_ArkWeb_CreateSchemeHandler(&schemeHandler);
    DestroySchemeHandlerApiForTest();
    OH_ArkWeb_CreateResponse(&response);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_001
 * @tc.desc  : g_SchemeHandlerApi is not null but g_SchemeHandlerApi->xxx is null
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_001, TestSize.Level1) {
    ArkWeb_RequestHeaderList* headerList = nullptr;
    OH_ArkWebRequestHeaderList_Destroy(headerList);
    EXPECT_EQ(OH_ArkWebRequestHeaderList_GetSize(headerList), -1);

    int32_t index = 0;
    char* key = nullptr;
    char* value = nullptr;
    OH_ArkWebRequestHeaderList_GetHeader(headerList, index, &key, &value);

    ArkWeb_ResourceRequest* resourceRequest = nullptr;
    char* method = nullptr;
    OH_ArkWebResourceRequest_GetMethod(resourceRequest, &method);

    char* url = nullptr;
    OH_ArkWebResourceRequest_GetUrl(resourceRequest, &url);

    ArkWeb_HttpBodyStream* httpBodyStream = nullptr;
    OH_ArkWebResourceRequest_GetHttpBodyStream(resourceRequest, &httpBodyStream);
    OH_ArkWebResourceRequest_DestroyHttpBodyStream(httpBodyStream);

    EXPECT_EQ(OH_ArkWebResourceRequest_GetResourceType(resourceRequest), -1);

    char* frameUrl = nullptr;
    OH_ArkWebResourceRequest_GetFrameUrl(resourceRequest, &frameUrl);

    void* userData = nullptr;
    int32_t result = OH_ArkWebHttpBodyStream_SetUserData(httpBodyStream, userData);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    EXPECT_EQ(OH_ArkWebHttpBodyStream_GetUserData(httpBodyStream), nullptr);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_002
 * @tc.desc  : g_SchemeHandlerApi is not null but g_SchemeHandlerApi->xxx is null
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_002, TestSize.Level1) {
    ArkWeb_HttpBodyStream* httpBodyStream = nullptr;
    ArkWeb_HttpBodyStreamReadCallback readCallback = nullptr;
    int32_t result = OH_ArkWebHttpBodyStream_SetReadCallback(httpBodyStream, readCallback);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    ArkWeb_HttpBodyStreamInitCallback initCallback = nullptr;
    result = OH_ArkWebHttpBodyStream_Init(httpBodyStream, initCallback);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    const ArkWeb_HttpBodyStream* httpBodyStream1 = nullptr;
    uint8_t buffer[256] = {0};
    int bufLen = sizeof(buffer);
    OH_ArkWebHttpBodyStream_Read(httpBodyStream1, buffer, bufLen);

    uint64_t size = OH_ArkWebHttpBodyStream_GetSize(httpBodyStream1);
    EXPECT_EQ(size, 0);
    uint64_t position = OH_ArkWebHttpBodyStream_GetPosition(httpBodyStream1);
    EXPECT_EQ(position, 0);
    bool boolResult = OH_ArkWebHttpBodyStream_IsChunked(httpBodyStream);
    EXPECT_FALSE(boolResult);
    boolResult = OH_ArkWebHttpBodyStream_IsEof(httpBodyStream);
    EXPECT_FALSE(boolResult);
    boolResult = OH_ArkWebHttpBodyStream_IsInMemory(httpBodyStream);
    EXPECT_FALSE(boolResult);

    const ArkWeb_ResourceRequest* resourceRequest = nullptr;
    result = OH_ArkWebResourceRequest_Destroy(resourceRequest);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* referrer = nullptr;
    OH_ArkWebResourceRequest_GetReferrer(resourceRequest, &referrer);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_003
 * @tc.desc  : g_SchemeHandlerApi is not null but g_SchemeHandlerApi->xxx is null
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_003, TestSize.Level1) {
    ArkWeb_ResourceRequest* resourceRequest = nullptr;
    void* userData = nullptr;
    int32_t result = OH_ArkWebResourceRequest_SetUserData(resourceRequest, userData);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    const ArkWeb_ResourceRequest* resourceRequest1 = nullptr;
    void* userData1 = OH_ArkWebResourceRequest_GetUserData(resourceRequest1);
    EXPECT_EQ(userData1, nullptr);

    ArkWeb_RequestHeaderList* requestHeaderList = nullptr;
    OH_ArkWebResourceRequest_GetRequestHeaders(resourceRequest1, &requestHeaderList);
    EXPECT_EQ(requestHeaderList, nullptr);

    bool boolResult = OH_ArkWebResourceRequest_IsRedirect(resourceRequest1);
    EXPECT_EQ(boolResult, false);
    boolResult = OH_ArkWebResourceRequest_IsMainFrame(resourceRequest1);
    EXPECT_EQ(boolResult, false);
    boolResult = OH_ArkWebResourceRequest_HasGesture(resourceRequest);
    EXPECT_EQ(boolResult, false);

    const char* webTag = nullptr;
    result = OH_ArkWeb_ClearSchemeHandlers(webTag);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    result = OH_ArkWebServiceWorker_ClearSchemeHandlers();
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    OH_ArkWeb_DestroySchemeHandler(schemeHandler);

    result = OH_ArkWebSchemeHandler_SetUserData(schemeHandler, userData);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);

    const ArkWeb_SchemeHandler* schemeHandler1 = nullptr;
    void* userData2 = OH_ArkWebSchemeHandler_GetUserData(schemeHandler1);
    EXPECT_EQ(userData2, nullptr);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_004
 * @tc.desc  : g_SchemeHandlerApi is not null but g_SchemeHandlerApi->xxx is null
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_004, TestSize.Level1) {
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    ArkWeb_OnRequestStart onRequestStart = nullptr;
    int32_t result = OH_ArkWebSchemeHandler_SetOnRequestStart(schemeHandler, onRequestStart);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    ArkWeb_OnRequestStop onRequestStop = nullptr;
    result = OH_ArkWebSchemeHandler_SetOnRequestStop(schemeHandler, onRequestStop);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    ArkWeb_Response* response = nullptr;
    OH_ArkWeb_DestroyResponse(response);
    const char* url = "www.example.com";
    EXPECT_EQ(OH_ArkWebResponse_SetUrl(response, url), ARKWEB_ERROR_UNKNOWN);
    char* url1 = nullptr;
    OH_ArkWebResponse_GetUrl(response, &url1);

    ArkWeb_NetError errorCode = ARKWEB_NET_OK;
    EXPECT_EQ(OH_ArkWebResponse_SetError(response, errorCode), ARKWEB_ERROR_UNKNOWN);
    ArkWeb_NetError errorCodeRes = OH_ArkWebResponse_GetError(response);
    EXPECT_EQ(errorCodeRes, ARKWEB_ERR_FAILED);
    int status = -1;
    EXPECT_EQ(OH_ArkWebResponse_SetStatus(response, status), ARKWEB_ERROR_UNKNOWN);
    EXPECT_EQ(OH_ArkWebResponse_GetStatus(response), -1);

    const char* statusText = "Test Status Text";
    EXPECT_EQ(OH_ArkWebResponse_SetStatusText(response, statusText), ARKWEB_ERROR_UNKNOWN);
    char* statusTextGet = nullptr;
    OH_ArkWebResponse_GetStatusText(response, &statusTextGet);

    const char* mimeType = "text/plain";
    EXPECT_EQ(OH_ArkWebResponse_SetMimeType(response, mimeType), ARKWEB_ERROR_UNKNOWN);
    char* mimeTypeRes = nullptr;
    OH_ArkWebResponse_GetMimeType(response, &mimeTypeRes);
    const char* charSet = "";
    EXPECT_EQ(OH_ArkWebResponse_SetCharset(response, charSet), ARKWEB_ERROR_UNKNOWN);
    char* charSetRes = nullptr;
    OH_ArkWebResponse_GetCharset(response, &charSetRes);
    const char* name = "TestHeaderName";
    const char* value = "TestHeaderValue";
    result = OH_ArkWebResponse_SetHeaderByName(response, name, value, true);
    EXPECT_EQ(result, ARKWEB_ERROR_UNKNOWN);
    char* valueRes = nullptr;
    OH_ArkWebResponse_GetHeaderByName(response, name, &valueRes);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_005
 * @tc.desc  : g_SchemeHandlerApi is not null but g_SchemeHandlerApi->xxx is null
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiXXXIsNull_005, TestSize.Level1) {
    ArkWeb_ResourceHandler* resourceHandler = nullptr;
    int32_t res = OH_ArkWebResourceHandler_Destroy(resourceHandler);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);

    ArkWeb_Response* response;
    res = OH_ArkWebResourceHandler_DidReceiveResponse(resourceHandler, response);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);

    const uint8_t* buffer = reinterpret_cast<const uint8_t*>("TestBuffer");
    int64_t bufLen = strlen("TestBuffer");
    res = OH_ArkWebResourceHandler_DidReceiveData(nullptr, buffer, bufLen);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);
    res = OH_ArkWebResourceHandler_DidFinish(nullptr);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);

    ArkWeb_NetError errorCode = ARKWEB_NET_OK;
    res = OH_ArkWebResourceHandler_DidFailWithError(resourceHandler, errorCode);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);
    char* str = nullptr;
    OH_ArkWeb_ReleaseString(str);
    uint8_t byteArray[10] = {0};
    OH_ArkWeb_ReleaseByteArray(byteArray);
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    res = OH_ArkWebSchemeHandler_SetFromEts(schemeHandler, true);
    EXPECT_EQ(res, ARKWEB_ERROR_UNKNOWN);

    DestroySchemeHandlerApiForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNotNull_001
 * @tc.desc  : g_SchemeHandlerApi is not nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNotNull_001, TestSize.Level1) {
    SetSchemeHandlerApiForTest(&g_testSchemeHandlerApi);
    ArkWeb_RequestHeaderList* headerList = nullptr;
    OH_ArkWebRequestHeaderList_Destroy(headerList);
    int32_t ret = OH_ArkWebRequestHeaderList_GetSize(headerList);
    EXPECT_EQ(ret, 0);

    int32_t index = 0;
    char* key = nullptr;
    char* value = nullptr;
    OH_ArkWebRequestHeaderList_GetHeader(headerList, index, &key, &value);

    ArkWeb_ResourceRequest* resourceRequest = nullptr;
    char* method = nullptr;
    OH_ArkWebResourceRequest_GetMethod(resourceRequest, &method);

    char* url = nullptr;
    OH_ArkWebResourceRequest_GetUrl(resourceRequest, &url);

    ArkWeb_HttpBodyStream* httpBodyStream = nullptr;
    OH_ArkWebResourceRequest_GetHttpBodyStream(resourceRequest, &httpBodyStream);
    OH_ArkWebResourceRequest_DestroyHttpBodyStream(httpBodyStream);
    EXPECT_EQ(OH_ArkWebResourceRequest_GetResourceType(resourceRequest), 0);
    OH_ArkWebResourceRequest_GetFrameUrl(resourceRequest, &url);

    void* userData = nullptr;
    ret = OH_ArkWebHttpBodyStream_SetUserData(httpBodyStream, userData);
    EXPECT_EQ(ret, 0);

    std::ignore = OH_ArkWebHttpBodyStream_GetUserData(httpBodyStream);
    ArkWeb_HttpBodyStreamReadCallback readCallback = nullptr;
    ret = OH_ArkWebHttpBodyStream_SetReadCallback(httpBodyStream, readCallback);
    EXPECT_EQ(ret, 0);

    ArkWeb_HttpBodyStreamInitCallback initCallback = nullptr;
    EXPECT_EQ(OH_ArkWebHttpBodyStream_Init(httpBodyStream, initCallback), 0);

    const ArkWeb_HttpBodyStream* httpBodyStream1 = nullptr;
    uint8_t buffer[256] = {0};
    int bufLen = sizeof(buffer);
    OH_ArkWebHttpBodyStream_Read(httpBodyStream1, buffer, bufLen);
    EXPECT_EQ(OH_ArkWebHttpBodyStream_GetSize(httpBodyStream1), 1);
    EXPECT_EQ(OH_ArkWebHttpBodyStream_GetPosition(httpBodyStream1), 1);
    EXPECT_TRUE(OH_ArkWebHttpBodyStream_IsChunked(httpBodyStream));
    EXPECT_TRUE(OH_ArkWebHttpBodyStream_IsEof(httpBodyStream));
    EXPECT_TRUE(OH_ArkWebHttpBodyStream_IsInMemory(httpBodyStream));
    EXPECT_EQ(OH_ArkWebResourceRequest_Destroy(resourceRequest), 0);
    char* referrer = nullptr;
    OH_ArkWebResourceRequest_GetReferrer(resourceRequest, &referrer);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNotNull_002
 * @tc.desc  : g_SchemeHandlerApi is not nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNotNull_002, TestSize.Level1) {
    ArkWeb_ResourceRequest* resourceRequest = nullptr;
    void* userData = nullptr;
    EXPECT_EQ(OH_ArkWebResourceRequest_SetUserData(resourceRequest, userData), 0);

    const ArkWeb_ResourceRequest* resourceRequest1 = nullptr;
    std::ignore = OH_ArkWebResourceRequest_GetUserData(resourceRequest1);
    ArkWeb_RequestHeaderList* requestHeaderList = nullptr;
    OH_ArkWebResourceRequest_GetRequestHeaders(resourceRequest1, &requestHeaderList);

    EXPECT_TRUE(OH_ArkWebResourceRequest_IsRedirect(resourceRequest1));
    EXPECT_TRUE(OH_ArkWebResourceRequest_IsMainFrame(resourceRequest1));
    EXPECT_TRUE(OH_ArkWebResourceRequest_HasGesture(resourceRequest1));

    const char* webTag = nullptr;
    EXPECT_EQ(OH_ArkWeb_ClearSchemeHandlers(webTag), 0);
    EXPECT_EQ(OH_ArkWebServiceWorker_ClearSchemeHandlers(), 0);
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    OH_ArkWeb_DestroySchemeHandler(schemeHandler);
    EXPECT_EQ(OH_ArkWebSchemeHandler_SetUserData(schemeHandler, userData), 0);

    const ArkWeb_SchemeHandler* schemeHandler1 = nullptr;
    void* userData2 = OH_ArkWebSchemeHandler_GetUserData(schemeHandler1);
    EXPECT_EQ(userData2, nullptr);

    ArkWeb_OnRequestStart onRequestStart = nullptr;
    EXPECT_EQ(OH_ArkWebSchemeHandler_SetOnRequestStart(schemeHandler, onRequestStart), 0);
    ArkWeb_OnRequestStop onRequestStop = nullptr;
    EXPECT_EQ(OH_ArkWebSchemeHandler_SetOnRequestStop(schemeHandler, onRequestStop), 0);
    ArkWeb_Response* response = nullptr;
    OH_ArkWeb_DestroyResponse(response);
    const char* url = "www.example.com";
    EXPECT_EQ(OH_ArkWebResponse_SetUrl(response, url), 0);
    char* url1 = nullptr;
    OH_ArkWebResponse_GetUrl(response, &url1);

    const char* statusText = "Test Status Text";
    EXPECT_EQ(OH_ArkWebResponse_SetStatusText(response, statusText), 0);
    char* statusTextGet = nullptr;
    OH_ArkWebResponse_GetStatusText(response, &statusTextGet);
    const char* mimeType = "text/plain";
    EXPECT_EQ(OH_ArkWebResponse_SetMimeType(response, mimeType), 0);
    char* mimeTypeRes = nullptr;
    OH_ArkWebResponse_GetMimeType(response, &mimeTypeRes);
    const char* charSet = "";
    EXPECT_EQ(OH_ArkWebResponse_SetCharset(response, charSet), 0);
    char* charSetRes = nullptr;
    OH_ArkWebResponse_GetCharset(response, &charSetRes);
    const char* name = "TestHeaderName";
    const char* value = "TestHeaderValue";
    EXPECT_EQ(OH_ArkWebResponse_SetHeaderByName(response, name, value, true), 0);
    char* valueRes = nullptr;
    OH_ArkWebResponse_GetHeaderByName(response, name, &valueRes);
}

/**
 * @tc.name  : OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNotNull_003
 * @tc.desc  : g_SchemeHandlerApi is not nullptr
 */
HWTEST_F(OHArkwebSchemeHandlerTest, OHArkwebSchemeHandlerTest_SchemeHandlerApiIsNotNull_003, TestSize.Level1) {
    ArkWeb_ResourceHandler* resourceHandler = nullptr;
    EXPECT_EQ(OH_ArkWebResourceHandler_Destroy(resourceHandler), 0);

    ArkWeb_Response* response;
    EXPECT_EQ(OH_ArkWebResourceHandler_DidReceiveResponse(resourceHandler, response), 0);

    const uint8_t* buffer = reinterpret_cast<const uint8_t*>("TestBuffer");
    int64_t bufLen = strlen("TestBuffer");
    EXPECT_EQ(OH_ArkWebResourceHandler_DidReceiveData(nullptr, buffer, bufLen), 0);
    EXPECT_EQ(OH_ArkWebResourceHandler_DidFinish(nullptr), 0);

    ArkWeb_NetError errorCode = ARKWEB_NET_OK;
    EXPECT_EQ(OH_ArkWebResourceHandler_DidFailWithError(resourceHandler, errorCode), 0);
    char* str = nullptr;
    OH_ArkWeb_ReleaseString(str);
    uint8_t byteArray[10] = {0};
    OH_ArkWeb_ReleaseByteArray(byteArray);
    ArkWeb_SchemeHandler* schemeHandler = nullptr;
    EXPECT_EQ(OH_ArkWebSchemeHandler_SetFromEts(schemeHandler, true), 0);

    NWebHelper::Instance().bundlePath_ = "";
    const char* scheme = "scheme";
    int32_t option = 0;
    const char* webTag = "webTag";
    EXPECT_EQ(OH_ArkWeb_RegisterCustomSchemes(scheme, option), 0);
    EXPECT_TRUE(OH_ArkWeb_SetSchemeHandler(scheme, webTag, schemeHandler));
    EXPECT_TRUE(OH_ArkWebServiceWorker_SetSchemeHandler(scheme, schemeHandler));
    OH_ArkWeb_CreateSchemeHandler(&schemeHandler);
    ArkWeb_Response* response1 = nullptr;
    OH_ArkWeb_CreateResponse(&response1);

    EXPECT_EQ(OH_ArkWebResponse_SetError(response1, errorCode), 0);
    EXPECT_EQ(OH_ArkWebResponse_GetError(response1), ARKWEB_NET_OK);
    EXPECT_EQ(OH_ArkWebResponse_SetStatus(response1, -1), 0);
    EXPECT_EQ(OH_ArkWebResponse_GetStatus(response1), 0);

    SetSchemeHandlerApiForTest(nullptr);
}

}
}

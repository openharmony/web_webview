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

#ifndef WEB_SCHEME_HANDLER_REQUEST_H
#define WEB_SCHEME_HANDLER_REQUEST_H

#include <cstdint>
#include <map>
#include "ffi_remote_data.h"
#include "web_errors.h"
#include "webview_javascript_result_callback.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "arkweb_scheme_handler.h"
#include "nweb_log.h"

namespace OHOS::Webview {
    enum class WebResourceType : int {
        MAIN_FRAME = 0,
        SUB_FRAME = 1,
        STYLE_SHEET = 2,
        SCRIPT = 3,
        IMAGE = 4,
        FONT_RESOURCE = 5,
        SUB_RESOURCE = 6,
        OBJECT = 7,
        MEDIA = 8,
        WORKED = 9,
        SHARED_WORKED = 10,
        PREFETCH = 11,
        FAVICON = 12,
        XHR = 13,
        PING = 14,
        SERVICE_WORKED = 15,
        CSP_REPORT = 16,
        PLUNGIN_RESOURCE = 17,
        NAVIGATION_PRELOAD_MAIN_FRAME = 19,
        NAVIGATION_PRELOAD_SUB_FRAME = 20
    };

    //WebHttpBodyStream
    class WebHttpBodyStreamImpl : public OHOS::FFI::FFIData {
        DECL_TYPE(WebHttpBodyStreamImpl, OHOS::FFI::FFIData)
    public:
        explicit WebHttpBodyStreamImpl() = default;
        WebHttpBodyStreamImpl(ArkWeb_HttpBodyStream* stream)
        {
            stream_ = stream;
        };
        ~WebHttpBodyStreamImpl()
        {
            if (stream_) {
                OH_ArkWebResourceRequest_DestroyHttpBodyStream(stream_);
                stream_ = nullptr;
            }
        };
        static void HttpBodyStreamInitCallback(const ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_NetError result);
        int32_t initialize();
        int32_t Read(int32_t bufLen);
        uint64_t GetPostion();
        uint64_t GetSize();
        bool IsChunked();
        bool IsEof();
        bool IsInMemory();
    private:
        ArkWeb_HttpBodyStream* stream_ = nullptr;
    };

    //WebSchemeHandlerRequest
    using WebHeaderList = std::vector<std::pair<std::string, std::string>>;
    class WebSchemeHandlerRequestImpl : public OHOS::FFI::FFIData {
        DECL_TYPE(WebSchemeHandlerRequestImpl, OHOS::FFI::FFIData)
    public:
        explicit WebSchemeHandlerRequestImpl() = default;
        WebSchemeHandlerRequestImpl(const ArkWeb_ResourceRequest* request)
        {
            OH_ArkWebResourceRequest_GetUrl(request, &url_);
            OH_ArkWebResourceRequest_GetMethod(request, &method_);
            OH_ArkWebResourceRequest_GetReferrer(request, &referrer_);
            isRedirect_ = OH_ArkWebResourceRequest_IsRedirect(request);
            isMainFrame_ = OH_ArkWebResourceRequest_IsMainFrame(request);
            hasGesture_ = OH_ArkWebResourceRequest_HasGesture(request);
            OH_ArkWebResourceRequest_GetFrameUrl(request, &frameUrl_);
            OH_ArkWebResourceRequest_GetHttpBodyStream(request, &stream_);
            requestResourceType_ = OH_ArkWebResourceRequest_GetResourceType(request);

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
        };
        std::string GetRequestUrl();
        std::string GetMethod();
        std::string GetReferrer();
        std::string GetFrameUrl();
        int32_t GetRequestResourceType();
        ArkWeb_HttpBodyStream* GetHttpBodyStream();
        const WebHeaderList& GetHeader();
        bool HasGesture();
        bool IsMainFrame();
        bool IsRedirect();
    private:
        char* url_ = nullptr;
        char* method_ = nullptr;
        char* referrer_ = nullptr;
        char* frameUrl_ = nullptr;
        int32_t requestResourceType_ = -1;
        bool isRedirect_ = false;
        bool isMainFrame_ = false;
        bool hasGesture_ = false;
        WebHeaderList headerList_;
        ArkWeb_HttpBodyStream* stream_ = nullptr;
    };

    // WebSchemeHandler
    class WebSchemeHandlerImpl : public OHOS::FFI::FFIData {
        DECL_TYPE(WebSchemeHandlerImpl, OHOS::FFI::FFIData)
    public:
        explicit WebSchemeHandlerImpl();
        ~WebSchemeHandlerImpl();

        static const ArkWeb_SchemeHandler* GetArkWebSchemeHandler(
            WebSchemeHandlerImpl* handler);
        static WebSchemeHandlerImpl* GetWebSchemeHandler(
            const ArkWeb_SchemeHandler* handler);

        void RequestStart(ArkWeb_ResourceRequest* request,
                        const ArkWeb_ResourceHandler* ArkWeb_ResourceHandler,
                        bool* intercept);
        void RequestStop(const ArkWeb_ResourceRequest* resourceRequest);

        void PutRequestStart(std::function<bool(int64_t, int64_t)> callback);
        void PutRequestStop(std::function<void(int64_t)> callback);

        static std::unordered_map<WebSchemeHandlerImpl*, const ArkWeb_SchemeHandler*>
            webSchemeHandlerMap_;
        static std::unordered_map<const ArkWeb_SchemeHandler*, WebSchemeHandlerImpl*>
            arkWebSchemeHandlerMap_;
    private:
        typedef struct RequestStopParam {
            std::function<void(int64_t)> callbackRef_;
            WebSchemeHandlerRequestImpl* request_;
            const ArkWeb_ResourceRequest* arkWebRequest_;
        } RequestStopParam;

        static void RequestStopAfterWorkCb(RequestStopParam* param);

        ArkWeb_OnRequestStart onRequestStart_ = nullptr;
        ArkWeb_OnRequestStop onRequestStop_ = nullptr;

        std::function<bool(int64_t, int64_t)> request_start_callback_ = nullptr;
        std::function<void(int64_t)> request_stop_callback_ = nullptr;
    };

    // WebResourceHandler
    class WebResourceHandlerImpl : public OHOS::FFI::FFIData {
        DECL_TYPE(WebResourceHandlerImpl, OHOS::FFI::FFIData)
    public:
        explicit WebResourceHandlerImpl() = default;
        WebResourceHandlerImpl(const ArkWeb_ResourceHandler* handler)
        {
            handler_ = const_cast<ArkWeb_ResourceHandler*>(handler);
        };
        ~WebResourceHandlerImpl() = default;
        int32_t DidReceiveResponse(const ArkWeb_Response* response);
        int32_t DidReceiveResponseBody(const uint8_t* buffer, int64_t buflen);
        int32_t DidFinish();
        int32_t DidFailWithError(ArkWeb_NetError errorCode);
        void DestroyArkWebResourceHandler();
        void SetFinishFlag()
        {
            isFinished_ = true;
        }
    private:
        bool isFinished_ = false;
        ArkWeb_ResourceHandler* handler_ = nullptr;
    };
}
#endif // WEB_SCHEME_HANDLER_REQUEST_H

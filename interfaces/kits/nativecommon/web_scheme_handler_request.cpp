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

#include "web_scheme_handler_request.h"

#include "nweb.h"
#include "nweb_log.h"

namespace OHOS {
namespace NWeb {

WebSchemeHandlerRequest::WebSchemeHandlerRequest()
{
    WVLOG_D("WebSchemeHandlerRequest::WebSchemeHandlerRequest");
}

WebSchemeHandlerRequest::WebSchemeHandlerRequest(const ArkWeb_ResourceRequest* request)
{
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
    OH_ArkWeb_ReleaseString(frameUrl_);
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

} // namespace NWeb
} // namespace OHOS
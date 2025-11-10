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

#include "web_scheme_handler_response.h"

#include "nweb.h"
#include "nweb_log.h"

namespace OHOS {
namespace NWeb {

WebSchemeHandlerResponse::WebSchemeHandlerResponse()
{
    WVLOG_D("WebSchemeHandlerResponse::WebSchemeHandlerResponse");
    OH_ArkWeb_CreateResponse(&response_);
}

WebSchemeHandlerResponse::WebSchemeHandlerResponse(ArkWeb_Response* response)
    : response_(response)
{
    WVLOG_D("WebSchemeHandlerResponse::WebSchemeHandlerResponse");
}

WebSchemeHandlerResponse::~WebSchemeHandlerResponse()
{
    WVLOG_I("WebSchemeHandlerResponse::~WebSchemeHandlerResponse");
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

int32_t WebSchemeHandlerResponse::GetStatus() const
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

char* WebSchemeHandlerResponse::GetEncoding() const
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

} // namespace NWeb
} // namespace OHOS
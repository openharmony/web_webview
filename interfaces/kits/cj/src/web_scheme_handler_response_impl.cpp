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

#include "web_scheme_handler_response_impl.h"

#include <cstdint>

#include "application_context.h"
#include "cj_common_ffi.h"
#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"
#include "nweb_store_web_archive_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_log.h"
#include "webview_utils.h"

namespace OHOS::Webview {
WebSchemeHandlerResponseImpl::~WebSchemeHandlerResponseImpl()
{
    (void)response_;
    OH_ArkWeb_DestroyResponse(response_);
}

char* WebSchemeHandlerResponseImpl::GetUrl()
{
    if (!response_) {
        WEBVIEWLOGE("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char* url;
    OH_ArkWebResponse_GetUrl(response_, &url);
    return url;
}

int32_t WebSchemeHandlerResponseImpl::SetUrl(const char* url)
{
    return OH_ArkWebResponse_SetUrl(response_, url);
}

int32_t WebSchemeHandlerResponseImpl::GetStatus()
{
    return OH_ArkWebResponse_GetStatus(response_);
}

int32_t WebSchemeHandlerResponseImpl::SetStatus(int32_t status)
{
    return OH_ArkWebResponse_SetStatus(response_, status);
}

char* WebSchemeHandlerResponseImpl::GetStatusText()
{
    if (!response_) {
        WEBVIEWLOGE("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char* statusText = nullptr;
    OH_ArkWebResponse_GetStatusText(response_, &statusText);
    return statusText;
}

int32_t WebSchemeHandlerResponseImpl::SetStatusText(const char* statusText)
{
    return OH_ArkWebResponse_SetStatusText(response_, statusText);
}

char* WebSchemeHandlerResponseImpl::GetMimeType()
{
    if (!response_) {
        WEBVIEWLOGE("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char* mimeType = nullptr;
    OH_ArkWebResponse_GetMimeType(response_, &mimeType);
    return mimeType;
}

int32_t WebSchemeHandlerResponseImpl::SetMimeType(const char* mimeType)
{
    return OH_ArkWebResponse_SetMimeType(response_, mimeType);
}

char* WebSchemeHandlerResponseImpl::GetEncoding()
{
    if (!response_) {
        WEBVIEWLOGE("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char* encoding;
    OH_ArkWebResponse_GetCharset(response_, &encoding);
    return encoding;
}

int32_t WebSchemeHandlerResponseImpl::SetEncoding(const char* encoding)
{
    return OH_ArkWebResponse_SetCharset(response_, encoding);
}

char* WebSchemeHandlerResponseImpl::GetHeaderByName(const char* name)
{
    if (!response_) {
        WEBVIEWLOGE("WebSchemeHandlerResponse is nullptr");
        return nullptr;
    }
    char* value;
    OH_ArkWebResponse_GetHeaderByName(response_, name, &value);
    return value;
}

int32_t WebSchemeHandlerResponseImpl::SetHeaderByName(const char* name, const char* value, bool overwrite)
{
    return OH_ArkWebResponse_SetHeaderByName(response_, name, value, overwrite);
}

int32_t WebSchemeHandlerResponseImpl::GetErrorCode()
{
    return static_cast<int32_t>(OH_ArkWebResponse_GetError(response_));
}

int32_t WebSchemeHandlerResponseImpl::SetErrorCode(int32_t code)
{
    return OH_ArkWebResponse_SetError(response_, static_cast<ArkWeb_NetError>(code));
}
} // namespace OHOS::Webview
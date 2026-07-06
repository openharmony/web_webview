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

#ifndef WEB_SCHEME_HANDLER_RESPONSE_H
#define WEB_SCHEME_HANDLER_RESPONSE_H

#include <cstdint>
#include <map>

#include "arkweb_scheme_handler.h"
#include "ffi_remote_data.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "webview_javascript_result_callback.h"

namespace OHOS::Webview {
// WebSchemeHandlerResponse
class WebSchemeHandlerResponseImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(WebSchemeHandlerResponseImpl, OHOS::FFI::FFIData)
public:
    explicit WebSchemeHandlerResponseImpl()
    {
        OH_ArkWeb_CreateResponse(&response_);
    };
    WebSchemeHandlerResponseImpl(ArkWeb_Response* response)
    {
        response_ = response;
    };
    ~WebSchemeHandlerResponseImpl();

    char* GetUrl();
    int32_t SetUrl(const char* url);
    int32_t GetStatus();
    int32_t SetStatus(int32_t status);
    char* GetStatusText();
    int32_t SetStatusText(const char* statusText);
    char* GetMimeType();
    int32_t SetMimeType(const char* mimeType);
    char* GetEncoding();
    int32_t SetEncoding(const char* encoding);
    char* GetHeaderByName(const char* name);
    int32_t SetHeaderByName(const char* name, const char* value, bool overwrite);
    int32_t GetErrorCode();
    int32_t SetErrorCode(int32_t code);
    ArkWeb_Response* GetArkWebResponse()
    {
        return response_;
    }

private:
    ArkWeb_Response* response_ = nullptr;
};
} // namespace OHOS::Webview
#endif // WEB_SCHEME_HANDLER_RESPONSE_H
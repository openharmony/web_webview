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

#ifndef OHOS_NWEB_NATIVE_COMMON_WEB_SCHEME_HANDLER_RESPONSE_H
#define OHOS_NWEB_NATIVE_COMMON_WEB_SCHEME_HANDLER_RESPONSE_H
#include <string>

#include "arkweb_scheme_handler.h"

namespace OHOS {
namespace NWeb {

class WebSchemeHandlerResponse {
public:
    explicit WebSchemeHandlerResponse();
    explicit WebSchemeHandlerResponse(ArkWeb_Response* response);
    ~WebSchemeHandlerResponse();

    char* GetUrl();
    int32_t SetUrl(const char* url);
    int32_t GetStatus() const;
    int32_t SetStatus(int32_t status);
    char* GetStatusText();
    int32_t SetStatusText(const char* statusText);
    char* GetMimeType();
    int32_t SetMimeType(const char* mimeType);
    char* GetEncoding() const;
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

} // namespace NWeb
} // namespace OHOS
#endif // OHOS_NWEB_NATIVE_COMMON_WEB_SCHEME_HANDLER_RESPONSE_H
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

#ifndef OHOS_NWEB_NATIVE_COMMON_WEB_SCHEME_HANDLER_REQUEST_H
#define OHOS_NWEB_NATIVE_COMMON_WEB_SCHEME_HANDLER_REQUEST_H
#include <string>

#include "arkweb_scheme_handler.h"
#include "refbase.h"

namespace OHOS {
namespace NWeb {

using WebHeaderList = std::vector<std::pair<std::string, std::string>>;
class WebSchemeHandlerRequest : public RefBase {
public:
    explicit WebSchemeHandlerRequest();
    WebSchemeHandlerRequest(const ArkWeb_ResourceRequest* request);
    ~WebSchemeHandlerRequest();

    char* GetRequestUrl();
    char* GetMethod();
    char* GetReferrer();
    bool IsRedirect();
    bool IsMainFrame();
    bool HasGesture();
    const WebHeaderList& GetHeader();
    ArkWeb_HttpBodyStream* GetHttpBodyStream();
    int32_t GetRequestResourceType();
    char* GetFrameUrl();
private:
    char* url_ = nullptr;
    char* method_ = nullptr;
    char* referrer_ = nullptr;
    bool isRedirect_ = false;
    bool isMainFrame_ = false;
    bool hasGesture_ = false;
    WebHeaderList headerList_;
    ArkWeb_HttpBodyStream* stream_ = nullptr;
    int32_t requestResourceType_ = -1;
    char* frameUrl_ = nullptr;
};

} // namespace NWeb
} // namespace OHOS
#endif // OHOS_NWEB_NATIVE_COMMON_WEB_RESOURCE_HANDLER_H
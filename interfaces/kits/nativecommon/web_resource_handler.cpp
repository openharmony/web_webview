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

#include "nweb.h"
#include "nweb_log.h"
#include "web_resource_handler.h"

namespace OHOS {
namespace NWeb {

WebResourceHandler::WebResourceHandler()
{
    WVLOG_D("create WebResourceHandler");
}

WebResourceHandler::WebResourceHandler(const ArkWeb_ResourceHandler* handler)
    : handler_(const_cast<ArkWeb_ResourceHandler*>(handler))
{
    WVLOG_D("create WebResourceHandler");
}

WebResourceHandler::~WebResourceHandler()
{
    WVLOG_D("~WebResourceHandler");
}

int32_t WebResourceHandler::DidReceiveResponse(const ArkWeb_Response* response)
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    return OH_ArkWebResourceHandler_DidReceiveResponse(handler_, response);
}

int32_t WebResourceHandler::DidReceiveResponseBody(const uint8_t* buffer, int64_t buflen)
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    return OH_ArkWebResourceHandler_DidReceiveData(handler_, buffer, buflen);
}

int32_t WebResourceHandler::DidFinish()
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    int32_t ret = OH_ArkWebResourceHandler_DidFinish(handler_);
    if (ret == 0) {
        isFinished_ = true;
    }
    return ret;
}

int32_t WebResourceHandler::DidFailWithError(ArkWeb_NetError errorCode, bool completeIfNoResponse)
{
    if (isFinished_) {
        return ArkWeb_ErrorCode::ARKWEB_ERROR_UNKNOWN;
    }
    int32_t ret = OH_ArkWebResourceHandler_DidFailWithErrorV2(handler_, errorCode, completeIfNoResponse);
    if (ret == 0) {
        isFinished_ = true;
    }
    return ret;
}

void WebResourceHandler::DestoryArkWebResourceHandler()
{
    if (handler_) {
        OH_ArkWebResourceHandler_Destroy(handler_);
        handler_ = nullptr;
    }
}

} // namespace NWeb
} // namespace OHOS
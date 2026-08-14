/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_url_resource_error_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebUrlResourceErrorWrapper::ArkWebUrlResourceErrorWrapper(
    ArkWebRefPtr<ArkWebUrlResourceError> ark_web_url_resource_error)
    : ark_web_url_resource_error_(ark_web_url_resource_error)
{}

int ArkWebUrlResourceErrorWrapper::ErrorCode()
{
    return ark_web_url_resource_error_->ErrorCode();
}

std::string ArkWebUrlResourceErrorWrapper::ErrorInfo()
{
    ArkWebString stErrorInfo = ark_web_url_resource_error_->ErrorInfo();

    std::string objErrorInfo = ArkWebStringStructToClass(stErrorInfo);
    ArkWebStringStructRelease(stErrorInfo);
    return objErrorInfo;
}

int ArkWebUrlResourceErrorWrapper::CustomErrorCode()
{
    if (!ark_web_url_resource_error_) {
        return 0;
    }
    return ark_web_url_resource_error_->CustomErrorCode();
}

} // namespace OHOS::ArkWeb

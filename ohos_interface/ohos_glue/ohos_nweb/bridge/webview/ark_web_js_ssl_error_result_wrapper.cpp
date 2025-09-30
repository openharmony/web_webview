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

#include "ohos_nweb/bridge/ark_web_js_ssl_error_result_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "base/include/ark_web_errno.h"

namespace OHOS::ArkWeb {

ArkWebJsSslErrorResultWrapper::ArkWebJsSslErrorResultWrapper(
    ArkWebRefPtr<ArkWebJsSslErrorResult> ark_web_js_ssl_error_result)
    : ark_web_js_ssl_error_result_(ark_web_js_ssl_error_result)
{}

void ArkWebJsSslErrorResultWrapper::HandleCancel()
{
    ark_web_js_ssl_error_result_->HandleCancel();
}

void ArkWebJsSslErrorResultWrapper::HandleConfirm()
{
    ark_web_js_ssl_error_result_->HandleConfirm();
}

void ArkWebJsSslErrorResultWrapper::HandleCancelV2(bool abortLoading)
{
    ark_web_js_ssl_error_result_->HandleCancelV2(abortLoading);
    if (ArkWebGetErrno() != RESULT_OK) {
        ark_web_js_ssl_error_result_->HandleCancel();
    }
}

} // namespace OHOS::ArkWeb

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

#include "ohos_nweb/bridge/ark_web_js_dialog_result_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebJsDialogResultWrapper::ArkWebJsDialogResultWrapper(ArkWebRefPtr<ArkWebJsDialogResult> ark_web_js_dialog_result)
    : ark_web_js_dialog_result_(ark_web_js_dialog_result)
{}

void ArkWebJsDialogResultWrapper::Cancel()
{
    ark_web_js_dialog_result_->Cancel();
}

void ArkWebJsDialogResultWrapper::Confirm()
{
    ark_web_js_dialog_result_->Confirm();
}

void ArkWebJsDialogResultWrapper::Confirm(const std::string& message)
{
    ArkWebString stMessage = ArkWebStringClassToStruct(message);

    ark_web_js_dialog_result_->Confirm(stMessage);

    ArkWebStringStructRelease(stMessage);
}

} // namespace OHOS::ArkWeb
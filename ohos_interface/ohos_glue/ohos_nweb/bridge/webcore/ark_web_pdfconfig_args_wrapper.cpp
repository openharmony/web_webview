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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_pdfconfig_args_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebPDFConfigArgsWrapper::ArkWebPDFConfigArgsWrapper(
    ArkWebRefPtr<ArkWebPDFConfigArgs> ark_web_pdfconfig_args)
    : ark_web_pdfconfig_args_(ark_web_pdfconfig_args)
{}


double ArkWebPDFConfigArgsWrapper::GetWidth()
{
    return ark_web_pdfconfig_args_->GetWidth();
}
double ArkWebPDFConfigArgsWrapper::GetHeight()
{
    return ark_web_pdfconfig_args_->GetHeight();
}
double ArkWebPDFConfigArgsWrapper::GetScale()
{
    return ark_web_pdfconfig_args_->GetScale();
}
double ArkWebPDFConfigArgsWrapper::GetMarginTop()
{
    return ark_web_pdfconfig_args_->GetMarginTop();
}
double ArkWebPDFConfigArgsWrapper::GetMarginBottom()
{
    return ark_web_pdfconfig_args_->GetMarginBottom();
}
double ArkWebPDFConfigArgsWrapper::GetMarginRight()
{
    return ark_web_pdfconfig_args_->GetMarginRight();
}
double ArkWebPDFConfigArgsWrapper::GetMarginLeft()
{
    return ark_web_pdfconfig_args_->GetMarginLeft();
}
bool ArkWebPDFConfigArgsWrapper::GetShouldPrintBackground()
{
    return ark_web_pdfconfig_args_->GetShouldPrintBackground();
}

} // namespace OHOS::ArkWeb

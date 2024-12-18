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

#include "ohos_nweb/bridge/ark_web_pdfconfig_args_impl.h"

namespace OHOS::ArkWeb {

ArkWebPDFConfigArgsImpl::ArkWebPDFConfigArgsImpl(std::shared_ptr<OHOS::NWeb::NWebPDFConfigArgs> pdfConfig)
    : nweb_pdfconfig_(pdfConfig)
{}

double ArkWebPDFConfigArgsImpl::GetWidth()
{
    return nweb_pdfconfig_->GetWidth();
}
double ArkWebPDFConfigArgsImpl::GetHeight()
{
    return nweb_pdfconfig_->GetHeight();
}
double ArkWebPDFConfigArgsImpl::GetScale()
{
    return nweb_pdfconfig_->GetScale();
}
double ArkWebPDFConfigArgsImpl::GetMarginTop()
{
    return nweb_pdfconfig_->GetMarginTop();
}
double ArkWebPDFConfigArgsImpl::GetMarginBottom()
{
    return nweb_pdfconfig_->GetMarginBottom();
}
double ArkWebPDFConfigArgsImpl::GetMarginRight()
{
    return nweb_pdfconfig_->GetMarginRight();
}
double ArkWebPDFConfigArgsImpl::GetMarginLeft()
{
    return nweb_pdfconfig_->GetMarginLeft();
}
bool ArkWebPDFConfigArgsImpl::GetShouldPrintBackground()
{
    return nweb_pdfconfig_->GetShouldPrintBackground();
}

} // namespace OHOS::ArkWeb

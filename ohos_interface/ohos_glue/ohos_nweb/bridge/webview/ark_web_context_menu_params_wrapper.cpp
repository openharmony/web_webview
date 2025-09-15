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

#include "ohos_nweb/bridge/ark_web_context_menu_params_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebContextMenuParamsWrapper::ArkWebContextMenuParamsWrapper(
    ArkWebRefPtr<ArkWebContextMenuParams> ark_web_context_menu_params)
    : ark_web_context_menu_params_(ark_web_context_menu_params)
{}

int32_t ArkWebContextMenuParamsWrapper::GetXCoord()
{
    return ark_web_context_menu_params_->GetXCoord();
}

int32_t ArkWebContextMenuParamsWrapper::GetYCoord()
{
    return ark_web_context_menu_params_->GetYCoord();
}

bool ArkWebContextMenuParamsWrapper::IsEditable()
{
    return ark_web_context_menu_params_->IsEditable();
}

std::string ArkWebContextMenuParamsWrapper::GetLinkUrl()
{
    ArkWebString stLinkUrl = ark_web_context_menu_params_->GetLinkUrl();

    std::string objLinkUrl = ArkWebStringStructToClass(stLinkUrl);
    ArkWebStringStructRelease(stLinkUrl);
    return objLinkUrl;
}

std::string ArkWebContextMenuParamsWrapper::GetPageUrl()
{
    ArkWebString stPageUrl = ark_web_context_menu_params_->GetPageUrl();

    std::string objPageUrl = ArkWebStringStructToClass(stPageUrl);
    ArkWebStringStructRelease(stPageUrl);
    return objPageUrl;
}

std::string ArkWebContextMenuParamsWrapper::GetSourceUrl()
{
    ArkWebString stSourceUrl = ark_web_context_menu_params_->GetSourceUrl();

    std::string objSourceUrl = ArkWebStringStructToClass(stSourceUrl);
    ArkWebStringStructRelease(stSourceUrl);
    return objSourceUrl;
}

std::string ArkWebContextMenuParamsWrapper::GetTitleText()
{
    ArkWebString stTitleText = ark_web_context_menu_params_->GetTitleText();

    std::string objTitleText = ArkWebStringStructToClass(stTitleText);
    ArkWebStringStructRelease(stTitleText);
    return objTitleText;
}

std::string ArkWebContextMenuParamsWrapper::GetSelectionText()
{
    ArkWebString stSelectionText = ark_web_context_menu_params_->GetSelectionText();

    std::string objSelectionText = ArkWebStringStructToClass(stSelectionText);
    ArkWebStringStructRelease(stSelectionText);
    return objSelectionText;
}

ArkWebMediaType ArkWebContextMenuParamsWrapper::GetMediaType()
{
    return static_cast<ArkWebMediaType>(ark_web_context_menu_params_->GetMediaType());
}

ArkWebSourceType ArkWebContextMenuParamsWrapper::GetSourceType()
{
    return static_cast<ArkWebSourceType>(ark_web_context_menu_params_->GetSourceType());
}

ArkWebInputFieldType ArkWebContextMenuParamsWrapper::GetInputFieldType()
{
    return static_cast<ArkWebInputFieldType>(ark_web_context_menu_params_->GetInputFieldType());
}

bool ArkWebContextMenuParamsWrapper::HasImageContents()
{
    return ark_web_context_menu_params_->HasImageContents();
}

int32_t ArkWebContextMenuParamsWrapper::GetEditStateFlags()
{
    return ark_web_context_menu_params_->GetEditStateFlags();
}

std::string ArkWebContextMenuParamsWrapper::GetUnfilteredLinkUrl()
{
    ArkWebString stUnfilteredLinkUrl = ark_web_context_menu_params_->GetUnfilteredLinkUrl();

    std::string objUnfilteredLinkUrl = ArkWebStringStructToClass(stUnfilteredLinkUrl);
    ArkWebStringStructRelease(stUnfilteredLinkUrl);
    return objUnfilteredLinkUrl;
}

int32_t ArkWebContextMenuParamsWrapper::GetContextMenuTypeFlags()
{
    return ark_web_context_menu_params_->GetContextMenuTypeFlags();
}

void ArkWebContextMenuParamsWrapper::GetImageRect(int& x, int& y, int& w, int& h)
{
    ark_web_context_menu_params_->GetImageRect(x, y, w, h);
}

bool ArkWebContextMenuParamsWrapper::IsAILink()
{
    return ark_web_context_menu_params_->IsAILink();
}

ArkWebDataMediaType ArkWebContextMenuParamsWrapper::GetContextMenuMediaType()
{
    return static_cast<ArkWebDataMediaType>(ark_web_context_menu_params_->GetContextMenuMediaType());
}

} // namespace OHOS::ArkWeb

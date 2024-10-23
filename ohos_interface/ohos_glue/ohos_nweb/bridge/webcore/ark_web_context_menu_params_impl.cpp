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

#include "ohos_nweb/bridge/ark_web_context_menu_params_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebContextMenuParamsImpl::ArkWebContextMenuParamsImpl(
    std::shared_ptr<OHOS::NWeb::NWebContextMenuParams> nweb_context_menu_params)
    : nweb_context_menu_params_(nweb_context_menu_params)
{}

int32_t ArkWebContextMenuParamsImpl::GetXCoord()
{
    return nweb_context_menu_params_->GetXCoord();
}

int32_t ArkWebContextMenuParamsImpl::GetYCoord()
{
    return nweb_context_menu_params_->GetYCoord();
}

bool ArkWebContextMenuParamsImpl::IsEditable()
{
    return nweb_context_menu_params_->IsEditable();
}

ArkWebString ArkWebContextMenuParamsImpl::GetLinkUrl()
{
    return ArkWebStringClassToStruct(nweb_context_menu_params_->GetLinkUrl());
}

ArkWebString ArkWebContextMenuParamsImpl::GetPageUrl()
{
    return ArkWebStringClassToStruct(nweb_context_menu_params_->GetPageUrl());
}

ArkWebString ArkWebContextMenuParamsImpl::GetSourceUrl()
{
    return ArkWebStringClassToStruct(nweb_context_menu_params_->GetSourceUrl());
}

ArkWebString ArkWebContextMenuParamsImpl::GetTitleText()
{
    return ArkWebStringClassToStruct(nweb_context_menu_params_->GetTitleText());
}

ArkWebString ArkWebContextMenuParamsImpl::GetSelectionText()
{
    return ArkWebStringClassToStruct(nweb_context_menu_params_->GetSelectionText());
}

int ArkWebContextMenuParamsImpl::GetMediaType()
{
    return static_cast<int>(nweb_context_menu_params_->GetMediaType());
}

int ArkWebContextMenuParamsImpl::GetSourceType()
{
    return static_cast<int>(nweb_context_menu_params_->GetSourceType());
}

int ArkWebContextMenuParamsImpl::GetInputFieldType()
{
    return static_cast<int>(nweb_context_menu_params_->GetInputFieldType());
}

bool ArkWebContextMenuParamsImpl::HasImageContents()
{
    return nweb_context_menu_params_->HasImageContents();
}

int32_t ArkWebContextMenuParamsImpl::GetEditStateFlags()
{
    return nweb_context_menu_params_->GetEditStateFlags();
}

ArkWebString ArkWebContextMenuParamsImpl::GetUnfilteredLinkUrl()
{
    return ArkWebStringClassToStruct(nweb_context_menu_params_->GetUnfilteredLinkUrl());
}

int32_t ArkWebContextMenuParamsImpl::GetContextMenuTypeFlags()
{
    return nweb_context_menu_params_->GetContextMenuTypeFlags();
}

void ArkWebContextMenuParamsImpl::GetImageRect(int& x, int& y, int& w, int& h)
{
    nweb_context_menu_params_->GetImageRect(x, y, w, h);
}

} // namespace OHOS::ArkWeb

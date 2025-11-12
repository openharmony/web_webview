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

#include "ohos_nweb/bridge/ark_web_file_selector_params_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebFileSelectorParamsImpl::ArkWebFileSelectorParamsImpl(
    std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> nweb_file_selector_params)
    : nweb_file_selector_params_(nweb_file_selector_params)
{}

int ArkWebFileSelectorParamsImpl::Mode()
{
    return static_cast<int>(nweb_file_selector_params_->Mode());
}

const ArkWebString ArkWebFileSelectorParamsImpl::Title()
{
    return ArkWebStringClassToStruct(nweb_file_selector_params_->Title());
}

bool ArkWebFileSelectorParamsImpl::IsCapture()
{
    return nweb_file_selector_params_->IsCapture();
}

const ArkWebStringVector ArkWebFileSelectorParamsImpl::AcceptType()
{
    return ArkWebStringVectorClassToStruct(nweb_file_selector_params_->AcceptType());
}

const ArkWebString ArkWebFileSelectorParamsImpl::DefaultFilename()
{
    return ArkWebStringClassToStruct(nweb_file_selector_params_->DefaultFilename());
}

const ArkWebStringVector ArkWebFileSelectorParamsImpl::MimeType()
{
    return ArkWebStringVectorClassToStruct(nweb_file_selector_params_->MimeType());
}

const ArkWebString ArkWebFileSelectorParamsImpl::DefaultPath()
{
    return ArkWebStringClassToStruct(nweb_file_selector_params_->DefaultPath());
}

const ArkWebStringVector ArkWebFileSelectorParamsImpl::Descriptions()
{
    return ArkWebStringVectorClassToStruct(nweb_file_selector_params_->Descriptions());
}

bool ArkWebFileSelectorParamsImpl::IsAcceptAllOptionExcluded()
{
    return nweb_file_selector_params_->IsAcceptAllOptionExcluded();
}

const ArkWebString ArkWebFileSelectorParamsImpl::Accepts()
{
    return ArkWebStringClassToStruct(nweb_file_selector_params_->Accepts());
}
}
} // namespace OHOS::ArkWeb

/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "ohos_nweb/bridge/ark_web_auto_fill_item_impl.h"
#include "ohos_nweb/cpptoc/ark_web_auto_fill_item_vector_cpptoc.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebAutoFillItemImpl::ArkWebAutoFillItemImpl(
    std::shared_ptr<OHOS::NWeb::AutoFillItem> nweb_item)
    : nweb_item_(nweb_item) {}
 
ArkWebString ArkWebAutoFillItemImpl::GetXPath()
{
    return ArkWebStringClassToStruct(nweb_item_->GetXPath());
}
 
ArkWebString ArkWebAutoFillItemImpl::GetContent()
{
    return ArkWebStringClassToStruct(nweb_item_->GetContent());
}
 
int32_t ArkWebAutoFillItemImpl::GetIndex()
{
    return nweb_item_->GetIndex();
}
 
int32_t ArkWebAutoFillItemImpl::GetMode()
{
    return static_cast<int32_t>(nweb_item_->GetMode());
}
 
} // namespace OHOS::ArkWeb
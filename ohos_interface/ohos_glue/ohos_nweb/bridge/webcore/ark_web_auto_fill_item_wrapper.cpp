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
 
#include "ohos_nweb/bridge/ark_web_auto_fill_item_wrapper.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebAutoFillItemWrapper::ArkWebAutoFillItemWrapper(
    ArkWebRefPtr<ArkWebAutoFillItem> ark_web_item)
    : ark_web_item_(ark_web_item) {}
 
std::string ArkWebAutoFillItemWrapper::GetXPath() const
{
    ArkWebString str = ark_web_item_->GetXPath();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}
 
std::string ArkWebAutoFillItemWrapper::GetContent() const
{
    ArkWebString str = ark_web_item_->GetContent();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}
 
int32_t ArkWebAutoFillItemWrapper::GetIndex() const
{
    return ark_web_item_->GetIndex();
}
 
OHOS::NWeb::AutoFillMode ArkWebAutoFillItemWrapper::GetMode() const
{
    return static_cast<OHOS::NWeb::AutoFillMode>(ark_web_item_->GetMode());
}
 
} // namespace OHOS::ArkWeb
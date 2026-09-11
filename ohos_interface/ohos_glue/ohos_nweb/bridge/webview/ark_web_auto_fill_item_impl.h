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
 
#ifndef ARK_WEB_AUTO_FILL_ITEM_IMPL_H_
#define ARK_WEB_AUTO_FILL_ITEM_IMPL_H_
#pragma once
 
#include "include/nweb_command_action_info.h"
#include "ohos_nweb/include/ark_web_auto_fill_item.h"
 
namespace OHOS::ArkWeb {
 
class ArkWebAutoFillItemImpl : public ArkWebAutoFillItem {
    IMPLEMENT_REFCOUNTING(ArkWebAutoFillItemImpl);
 
public:
    explicit ArkWebAutoFillItemImpl(std::shared_ptr<OHOS::NWeb::AutoFillItem> nweb_item);
    ~ArkWebAutoFillItemImpl() = default;
 
    ArkWebString GetXPath() override;
    ArkWebString GetContent() override;
    int32_t GetIndex() override;
    int32_t GetMode() override;
 
private:
    std::shared_ptr<OHOS::NWeb::AutoFillItem> nweb_item_;
};
 
} // namespace OHOS::ArkWeb
 
#endif // ARK_WEB_AUTO_FILL_ITEM_IMPL_H_
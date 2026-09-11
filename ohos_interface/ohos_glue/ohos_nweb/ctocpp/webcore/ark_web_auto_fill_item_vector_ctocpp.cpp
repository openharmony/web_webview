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

#include "ohos_nweb/ctocpp/ark_web_auto_fill_item_vector_ctocpp.h"

#include "ohos_nweb/bridge/ark_web_auto_fill_item_wrapper.h"
#include "ohos_nweb/ctocpp/ark_web_auto_fill_item_ctocpp.h"

namespace OHOS::ArkWeb {

std::vector<std::shared_ptr<OHOS::NWeb::AutoFillItem>> ArkWebAutoFillItemVectorStructToClass(
    const ArkWebAutoFillItemVector& struct_value)
{
    std::vector<std::shared_ptr<OHOS::NWeb::AutoFillItem>> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            std::shared_ptr<OHOS::NWeb::AutoFillItem> auto_fill_item =
                std::make_shared<ArkWebAutoFillItemWrapper>(
                    ArkWebAutoFillItemCToCpp::Invert(struct_value.value[count]));
            class_value.push_back(auto_fill_item);
        }
    }

    return class_value;
}

void ArkWebAutoFillItemVectorStructRelease(ArkWebAutoFillItemVector& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

} // namespace OHOS::ArkWeb

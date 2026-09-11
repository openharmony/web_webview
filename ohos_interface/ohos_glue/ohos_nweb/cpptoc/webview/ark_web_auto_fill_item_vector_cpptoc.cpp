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

#include "ohos_nweb/cpptoc/ark_web_auto_fill_item_vector_cpptoc.h"

#include "ohos_nweb/bridge/ark_web_auto_fill_item_impl.h"
#include "ohos_nweb/cpptoc/ark_web_auto_fill_item_cpptoc.h"

namespace OHOS::ArkWeb {

ArkWebAutoFillItemVector ArkWebAutoFillItemVectorClassToStruct(
    const std::vector<std::shared_ptr<OHOS::NWeb::AutoFillItem>>& class_value)
{
    ArkWebAutoFillItemVector struct_value = { .size = class_value.size(),
        .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value =
            (ark_web_auto_fill_item_t**)ArkWebMemMalloc(
                sizeof(ark_web_auto_fill_item_t*) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            ArkWebRefPtr<ArkWebAutoFillItem> ark_web_auto_fill_item =
                new ArkWebAutoFillItemImpl(*it);
            struct_value.value[count] = ArkWebAutoFillItemCppToC::Invert(ark_web_auto_fill_item);
            count++;
        }
    }

    return struct_value;
}

} // namespace OHOS::ArkWeb

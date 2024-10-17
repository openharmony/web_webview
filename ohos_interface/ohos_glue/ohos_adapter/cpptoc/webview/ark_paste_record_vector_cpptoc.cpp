/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/cpptoc/ark_paste_record_vector_cpptoc.h"

#include "ohos_adapter/bridge/ark_paste_data_record_adapter_impl.h"
#include "ohos_adapter/cpptoc/ark_paste_data_record_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ArkPasteRecordVector ArkPasteRecordVectorClassToStruct(const NWeb::PasteRecordVector& class_value)
{
    ArkPasteRecordVector struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (ark_paste_data_record_adapter_t**)ArkWebMemMalloc(
            sizeof(ark_paste_data_record_adapter_t*) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            ArkWebRefPtr<ArkPasteDataRecordAdapter> adapter = new ArkPasteDataRecordAdapterImpl(*it);
            struct_value.value[count] = ArkPasteDataRecordAdapterCppToC::Invert(adapter);
            count++;
        }
    }
    return struct_value;
}

} // namespace OHOS::ArkWeb

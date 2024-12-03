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

#include "ohos_adapter/ctocpp/ark_paste_record_vector_ctocpp.h"

#include "ohos_adapter/bridge/ark_paste_data_record_adapter_wrapper.h"
#include "ohos_adapter/ctocpp/ark_paste_data_record_adapter_ctocpp.h"
#include "base/include/ark_web_macros.h"

namespace OHOS::ArkWeb {

NWeb::PasteRecordVector ArkPasteRecordVectorStructToClass(const ArkPasteRecordVector& struct_value)
{
    NWeb::PasteRecordVector class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            std::shared_ptr<NWeb::PasteDataRecordAdapter> adapter = std::make_shared<ArkPasteDataRecordAdapterWrapper>(
                ArkPasteDataRecordAdapterCToCpp::Invert(struct_value.value[count]));
            class_value.push_back(adapter);
        }
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkPasteRecordVectorStructRelease(ArkPasteRecordVector& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

} // namespace OHOS::ArkWeb

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#include "ohos_nweb/cpptoc/ark_web_cookie_vector_cpptoc.h"
 
#include "ohos_nweb/bridge/ark_web_cookie_impl.h"
#include "ohos_nweb/cpptoc/ark_web_cookie_cpptoc.h"
 
namespace OHOS::ArkWeb {

ArkWebCookieVector ArkWebCookieVectorClassToStruct(
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebCookie>>& class_value)
{
    ArkWebCookieVector struct_value = { .size = class_value.size(),
        .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value =
            (ark_web_cookie_t**)ArkWebMemMalloc(
                sizeof(ark_web_cookie_t*) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            ArkWebRefPtr<ArkWebCookie> ark_web_cookie =
                new ArkWebCookieImpl(*it);
            struct_value.value[count] = ArkWebCookieCppToC::Invert(ark_web_cookie);
            count++;
        }
    }

    return struct_value;
}

void ArkWebCookieVectorStructRelease(ArkWebCookieVector& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

} // namespace OHOS::ArkWeb

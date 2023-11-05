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

#include "registerarkjsfunction_fuzzer.h"

#include "nweb.h"
#include "nweb_create_window.h"

namespace OHOS {
    std::shared_ptr<OHOS::NWeb::NWeb> g_nweb = nullptr;
    bool RegisterArkJSfunctionFuzzTest(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return true;
        }
        g_nweb = NWeb::GetNwebForTest();
        if (g_nweb == nullptr) {
            return true;
        }
        const std::string object_name((const char *)data, size);
        const std::vector<std::string> method_list;
        g_nweb->RegisterArkJSfunction(object_name, method_list);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::RegisterArkJSfunctionFuzzTest(data, size);
    return 0;
}

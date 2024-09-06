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

#include "getnweb_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "nweb_helper.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool GetNwebFuzzTest(const uint8_t* data, size_t size)
{
    int32_t nweb_id = static_cast<int32_t>(size);

    NWebHelper::Instance().GetNWeb(nweb_id);

    NWebHelper::Instance().SetWebTag(nweb_id, "NWeb");

    NWebHelper::Instance().PrepareForPageLoad("", true, size);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GetNwebFuzzTest(data, size);
    return 0;
}

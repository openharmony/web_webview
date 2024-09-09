/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "initandrun_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "nweb_helper.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool InitAndRunFuzzTest(const uint8_t* data, size_t size)
{
    const std::string path("/data/app/el1/bundle/public/com.ohos.nweb");
    NWebHelper::Instance().SetBundlePath(path);

    NWebHelper::Instance().InitAndRun(false);

    if (data == nullptr) {
        return false;
    }
    std::string pathTmp(reinterpret_cast<const char*>(data), size);
    NWebHelper::Instance().SetBundlePath(pathTmp);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::InitAndRunFuzzTest(data, size);
    return 0;
}

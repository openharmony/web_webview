/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ashmemcreate_fuzzer.h"

#include <cstring>
#include <dlfcn.h>
#include <securec.h>
#include <unistd.h>

#include "graphic_adapter.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool AshmemCreateFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    AshmemAdapter ashmem;
    char* name = new (std::nothrow) char[size + 1] { 0 };
    if (name == nullptr) {
        return false;
    }
    if (memcpy_s(name, size, data, size) != 0) {
        delete[] name;
        return false;
    }
    int fd = ashmem.AshmemCreate(name, size);
    if (fd >= 0) {
        close(fd);
    }
    delete[] name;
    name = nullptr;
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AshmemCreateFuzzTest(data, size);
    return 0;
}

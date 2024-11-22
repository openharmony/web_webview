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

#include "base/include/ark_web_memory.h"

#include "base/include/ark_web_log_utils.h"

void ArkWebMemFree(void* mem)
{
    ARK_WEB_BASE_DV_LOG("address is %{public}ld", reinterpret_cast<long>(mem));

    if (mem) {
        free(mem);
    }
}

void* ArkWebMemMalloc(int size)
{
    void* mem = nullptr;
    if (size > 0) {
        mem = malloc(size);
        if (mem == nullptr) {
            ARK_WEB_BASE_DV_LOG("failed to malloc memery,size is %{public}d", size);
        } else {
            ARK_WEB_BASE_DV_LOG("address is %{public}ld, size is %{public}d", reinterpret_cast<long>(mem), size);
        }
    } else {
        ARK_WEB_BASE_DV_LOG("size %{public}d is invalid", size);
    }

    return mem;
}

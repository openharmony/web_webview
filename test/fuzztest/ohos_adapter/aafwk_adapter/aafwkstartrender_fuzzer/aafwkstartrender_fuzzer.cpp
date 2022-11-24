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

#include "aafwkstartrender_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "aafwk_render_scheduler_impl.h"
#include "aafwk_app_mgr_client_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size < sizeof(int32_t))) {
            return false;
        }
        AafwkAppMgrClientAdapterImpl render;
        int32_t ipcFd;
        int32_t sharedFd = 0;
        std::string renderParam = (char *)data;
        pid_t renderPid = 0;
        if (memcpy_s(&ipcFd, sizeof(int32_t), data, sizeof(int32_t)) != 0) {
            return false;
        }
        render.StartRenderProcess(renderParam, ipcFd, sharedFd, renderPid);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

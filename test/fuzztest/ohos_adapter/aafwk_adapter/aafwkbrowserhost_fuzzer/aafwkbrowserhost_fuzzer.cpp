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

#include "aafwkbrowserhost_fuzzer.h"

#include <cstring>
#include <securec.h>

#define private public
#include "aafwk_app_mgr_client_adapter_impl.h"
#include "aafwk_browser_host_impl.h"
#include "aafwk_render_scheduler_impl.h"
#undef private

#include "aafwk_render_scheduler_host_adapter.h"
#include "app_mgr_client.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool AafwkBrowserHostFuzzTest(const uint8_t* data, size_t size)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    uint32_t code = 0;
    int32_t surface_id = 0;
    MessageParcel newdata;
    MessageParcel reply;
    MessageOption option;
    host->OnRemoteRequest(code, newdata, reply, option);
    code = 0;
    host->OnRemoteRequest(code, newdata, reply, option);
    host->HandleQueryRenderSurface(newdata, reply);
    host->HandleReportThread(newdata, reply);
    host->HandlePassSurface(newdata, reply);
    host->HandleDestroyRenderSurface(newdata, reply);
    sptr<Surface> surface;
    host->PassSurface(surface, surface_id);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AafwkBrowserHostFuzzTest(data, size);
    return 0;
}

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

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "aafwk_app_mgr_client_adapter_impl.h"
#include "aafwk_browser_client_adapter_impl.h"
#include "aafwk_browser_host_impl.h"
#include "aafwk_render_scheduler_impl.h"
#undef private

#include "aafwk_browser_client_adapter.h"
#include "aafwk_render_scheduler_host_adapter.h"
#include "app_mgr_client.h"
#include "ibrowser.h"
#include "iconsumer_surface.h"
#include "iremote_proxy.h"
#include "irender_scheduler.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

class MockBrowserClient : public BrowserClient {
    explicit MockBrowserClient(const sptr<IRemoteObject>& impl);

    sptr<IRemoteObject> QueryRenderSurface(int32_t surface_id);

    void ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role);

    void PassSurface(sptr<Surface> surface, int64_t surface_id);

    void DestroyRenderSurface(int32_t surface_id);
};

sptr<IRemoteObject> MockBrowserClient::QueryRenderSurface(int32_t surface_id)
{
    (void)surface_id;
    return nullptr;
}

void MockBrowserClient::ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role)
{
    (void)status;
    (void)process_id;
    (void)thread_id;
    (void)role;
}

void MockBrowserClient::PassSurface(sptr<Surface> surface, int64_t surface_id)
{
    (void)surface;
    (void)surface_id;
}

void MockBrowserClient::DestroyRenderSurface(int32_t surface_id)
{
    (void)surface_id;
}

bool AafwkBrowserClientAdapterFuzzTest(const uint8_t* data, size_t size)
{
    sptr<IRemoteObject> impl;
    auto client = new BrowserClient(impl);
    int32_t surface_id = 0;
    client->QueryRenderSurface(surface_id);
    int32_t status = 0;
    int32_t process_id = 0;
    int32_t thread_id = 0;
    FuzzedDataProvider dataProvider(data, size);
    int32_t role = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);

    sptr<Surface> surface;
    client->ReportThread(status, process_id, thread_id, role);
    client->PassSurface(surface, surface_id);
    client->DestroyRenderSurface(surface_id);

    std::shared_ptr<AafwkBrowserClientAdapterImpl> clientAdapter = std::make_shared<AafwkBrowserClientAdapterImpl>();
    clientAdapter->QueryRenderSurface(surface_id);
    ResSchedStatusAdapter newstatus = ResSchedStatusAdapter::THREAD_CREATED;
    ResSchedRoleAdapter newrole = ResSchedRoleAdapter::USER_INTERACT;
    clientAdapter->ReportThread(newstatus, process_id, thread_id, newrole);
    clientAdapter->PassSurface(surface_id);
    clientAdapter->DestroyRenderSurface(surface_id);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AafwkBrowserClientAdapterFuzzTest(data, size);
    return 0;
}

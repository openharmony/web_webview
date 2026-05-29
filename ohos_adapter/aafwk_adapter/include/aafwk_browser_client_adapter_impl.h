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

#ifndef AAFWK_BROWSER_CLIENT_ADAPTER_IMPL_H
#define AAFWK_BROWSER_CLIENT_ADAPTER_IMPL_H

#include <memory>
#include "aafwk_render_scheduler_host_adapter.h"
#include "aafwk_browser_client_adapter.h"
#include "ibrowser.h"
#include "iconsumer_surface.h"
#include "irender_scheduler.h"
#include "iremote_proxy.h"

namespace OHOS::NWeb {
class BrowserClient : public IRemoteProxy<IBrowser> {
public:
    explicit BrowserClient(const sptr<IRemoteObject> &impl);

    virtual ~BrowserClient() = default;

    virtual sptr<IRemoteObject> QueryRenderSurface(int32_t surface_id) override;

    virtual void ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role) override;

    virtual void PassSurface(sptr<Surface> surface, int64_t surface_id) override;

    virtual void DestroyRenderSurface(int32_t surface_id) override;
private:
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<BrowserClient> delegator_;
};

class AafwkBrowserClientAdapterImpl : public AafwkBrowserClientAdapter {
public:
    explicit AafwkBrowserClientAdapterImpl();

    static AafwkBrowserClientAdapterImpl& GetInstance();

    ~AafwkBrowserClientAdapterImpl() override = default;

    void* QueryRenderSurface(int32_t surface_id) override;

    void ReportThread(ResSchedStatusAdapter status, int32_t process_id, int32_t thread_id,
                      ResSchedRoleAdapter role) override;

    void PassSurface(int64_t surface_id) override;

    void DestroyRenderSurface(int32_t surface_id) override;

    sptr<IBrowser> browserHost_;
private:
    std::map<int32_t, OHNativeWindow*> window_map_;
    std::mutex window_map_mutex_;
};
} // namespace content
#endif // AAFWK_BROWSER_CLIENT_ADAPTER_IMPL_H
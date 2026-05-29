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

#ifndef AAFWK_BROWSER_HOST_IMPL_H
#define AAFWK_BROWSER_HOST_IMPL_H

#include <cstdint>
#include <memory>
#include <map>

#include "aafwk_browser_host_adapter.h"
#include <functional>
#include "iremote_object.h"
#include "iremote_stub.h"
#include "iconsumer_surface.h"
#include "nocopyable.h"
#include "ibrowser.h"
#include "res_sched_client_adapter.h"

namespace OHOS::NWeb {
class BrowserHost : public IRemoteStub<IBrowser> {
public:
    BrowserHost();

    virtual ~BrowserHost();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
private:
    int32_t HandleQueryRenderSurface(MessageParcel &data, MessageParcel &reply);

    int32_t HandleReportThread(MessageParcel &data, MessageParcel &reply);

    int32_t HandlePassSurface(MessageParcel &data, MessageParcel &reply);

    int32_t HandleDestroyRenderSurface(MessageParcel &data, MessageParcel &reply);

    using BrowserHostFunc = std::function<int32_t(BrowserHost*, MessageParcel &data, MessageParcel &reply)>;

    std::map<uint32_t, BrowserHostFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BrowserHost);
};

class AafwkBrowserHostImpl : public BrowserHost {
public:
    explicit AafwkBrowserHostImpl(std::shared_ptr<AafwkBrowserHostAdapter> adapter);

    ~AafwkBrowserHostImpl() override = default;

    sptr<IRemoteObject> QueryRenderSurface(int32_t surface_id) override;

    void ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role) override;

    void PassSurface(sptr<Surface> surface, int64_t surface_id) override;

    void DestroyRenderSurface(int32_t surface_id) override;
private:
    std::map<int64_t, sptr<Surface>> surface_map_;
    std::shared_ptr<AafwkBrowserHostAdapter> browserHostAdapter_;
    std::mutex map_mutex_;
};
} //namespace content

#endif // AAFWK_BROWSER_HOST_IMPL_H
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

#ifndef AAFWK_APP_MGR_CLIENT_ADAPTER_IMPL_H
#define AAFWK_APP_MGR_CLIENT_ADAPTER_IMPL_H

#include <memory>
#include <string>
#include <sys/types.h>
#include "aafwk_app_mgr_client_adapter.h"
#include "aafwk_render_scheduler_host_adapter.h"
#include "app_mgr_client.h"

namespace OHOS::NWeb {
class AafwkAppMgrClientAdapterImpl : public AafwkAppMgrClientAdapter {
public:
    AafwkAppMgrClientAdapterImpl();

    ~AafwkAppMgrClientAdapterImpl() override = default;

    int StartRenderProcess(const std::string &renderParam, int32_t ipcFd, int32_t sharedFd, pid_t &renderPid) override;

    void AttachRenderProcess(std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter) override;

    int GetRenderProcessTerminationStatus(pid_t renderPid, int &status) override;

private:
    std::unique_ptr<AppExecFwk::AppMgrClient> appMgrClient_;
};
}  // namespace OHOS::NWeb

#endif  // AAFWK_APP_MGR_CLIENT_ADAPTER_IMPL_H
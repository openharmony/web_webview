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

#ifndef AAFWK_APP_MGR_CLIENT_ADAPTER_H
#define AAFWK_APP_MGR_CLIENT_ADAPTER_H

#include <cstdint>
#include <string>
#include <sys/types.h>
#include "aafwk_render_scheduler_host_adapter.h"

namespace OHOS::NWeb {
class AafwkAppMgrClientAdapter {
public:
    AafwkAppMgrClientAdapter() = default;

    virtual ~AafwkAppMgrClientAdapter() = default;

    virtual int StartRenderProcess(const std::string &renderParam, int32_t ipcFd,
        int32_t sharedFd, pid_t &renderPid) = 0;

    virtual void AttachRenderProcess(std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter) = 0;

    virtual int GetRenderProcessTerminationStatus(pid_t renderPid, int &status) = 0;
};
}  // namespace OHOS::NWeb

#endif  // AAFWK_APP_MGR_CLIENT_ADAPTER_H
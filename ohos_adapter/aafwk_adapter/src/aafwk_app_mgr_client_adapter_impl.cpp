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

#include "aafwk_app_mgr_client_adapter_impl.h"

#include "aafwk_render_scheduler_impl.h"
#include "nweb_log.h"

namespace {
constexpr int GET_TERMINATION_STATUS_MAX_CNT = 5;
}

namespace OHOS::NWeb {
AafwkAppMgrClientAdapterImpl::AafwkAppMgrClientAdapterImpl() :
    appMgrClient_(std::make_unique<AppExecFwk::AppMgrClient>()) {}

int AafwkAppMgrClientAdapterImpl::StartRenderProcess(const std::string &renderParam, int32_t ipcFd,
    int32_t sharedFd, pid_t &renderPid)
{
    if (appMgrClient_ == nullptr) {
        WVLOG_E("app mgr client is nullptr.");
        return -1;
    }

    int ret = appMgrClient_->StartRenderProcess(renderParam, ipcFd, sharedFd, renderPid);
    if (ret != 0) {
        WVLOG_E("app mgr client start render process failed, ret = %{public}d.", ret);
        return -1;
    }

    return 0;
}

void AafwkAppMgrClientAdapterImpl::AttachRenderProcess(std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter)
{
    if (appMgrClient_ == nullptr) {
        WVLOG_E("app mgr client is nullptr.");
        return;
    }

    if (adapter == nullptr) {
        WVLOG_E("render scheduler is nullptr.");
        return;
    }

    AafwkRenderSchedulerImpl *renderScheduler = new (std::nothrow) AafwkRenderSchedulerImpl(adapter);
    if (renderScheduler == nullptr) {
        WVLOG_E("new AafwkRenderSchedulerImpl failed.");
        return;
    }

    appMgrClient_->AttachRenderProcess(renderScheduler);
}

int AafwkAppMgrClientAdapterImpl::GetRenderProcessTerminationStatus(pid_t renderPid, int &status)
{
    if (appMgrClient_ == nullptr) {
        WVLOG_E("app mgr client is nullptr.");
        return -1;
    }

    int retryCnt = 0;
    do {
        int ret = appMgrClient_->GetRenderProcessTerminationStatus(renderPid, status);
        if (ret != 0) {
            WVLOG_E("app mgr client get render process termination status failed, ret = %{public}d.", ret);
            return -1;
        }
    } while (status < 0 && ++retryCnt < GET_TERMINATION_STATUS_MAX_CNT);

    if (status < 0) {
        WVLOG_E("render process termination status invalid, status = %{public}d.", status);
        return -1;
    }

    return 0;
}
}  // namespace OHOS::NWeb

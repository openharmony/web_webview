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

#include <chrono>
#include <thread>

#include "aafwk_render_scheduler_impl.h"
#include "nweb_log.h"

namespace {
constexpr int GET_TERMINATION_STATUS_MAX_CNT = 5;
constexpr int START_RENDER_PROCESS_MAX_CNT = 10;
constexpr int SLEEP_FOR_MILLI_SECONDS_CNT = 10;
constexpr int RET_ALREADY_EXIST_RENDER = 8454244; // copy from ability_runtime
} // namespace

namespace OHOS::NWeb {
const std::string GPU_PROCESS_TYPE = "gpu-process";
AafwkAppMgrClientAdapterImpl::AafwkAppMgrClientAdapterImpl()
    : appMgrClient_(std::make_unique<AppExecFwk::AppMgrClient>())
{}

int AafwkAppMgrClientAdapterImpl::StartRenderProcess(
    const std::string& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd, pid_t& renderPid)
{
    if (appMgrClient_ == nullptr) {
        WVLOG_E("app mgr client is nullptr.");
        return -1;
    }

    int retryCnt = 0;
    int ret;
    do {
        ret = appMgrClient_->StartRenderProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid, false);
        if (ret == RET_ALREADY_EXIST_RENDER) {
            WVLOG_E("app mgr client start render process failed, render process already exist.");
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_FOR_MILLI_SECONDS_CNT));
            continue;
        }
        if (ret != 0) {
            WVLOG_E("app mgr client start render process failed, ret = %{public}d.", ret);
            return -1;
        }
    } while (++retryCnt < START_RENDER_PROCESS_MAX_CNT && ret != 0);

    if (ret != 0) {
        WVLOG_E("over max retry times, app mgr client start render process failed, ret = %{public}d.", ret);
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

int AafwkAppMgrClientAdapterImpl::StartChildProcess(
    const std::string& renderParam, int32_t ipcFd, int32_t sharedFd,
    int32_t crashFd, pid_t& renderPid, const std::string& processType)
{
    if (appMgrClient_ == nullptr) {
        WVLOG_E("app mgr client is nullptr.");
        return -1;
    }

    bool isGPU = false;
    if (processType == GPU_PROCESS_TYPE) {
        isGPU = true;
    }

    int retryCnt = 0;
    int ret;
    do {
        ret = appMgrClient_->StartRenderProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid, isGPU);
        if (ret == RET_ALREADY_EXIST_RENDER) {
            WVLOG_E("app mgr client start %{public}s process failed, process already exist.", processType.c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_FOR_MILLI_SECONDS_CNT));
            continue;
        }
        if (ret != 0) {
            WVLOG_E("app mgr client start render process failed, ret = %{public}d.", ret);
            return -1;
        }
    } while (++retryCnt < START_RENDER_PROCESS_MAX_CNT && ret != 0);

    if (ret != 0) {
        WVLOG_E("over max retry times, app mgr client start render process failed, ret = %{public}d.", ret);
        return -1;
    }

    return 0;
}

void AafwkAppMgrClientAdapterImpl::SaveBrowserConnect(std::shared_ptr<AafwkBrowserHostAdapter> adapter)
{
    if (appMgrClient_ == nullptr) {
        WVLOG_E("app mgr client is nullptr.");
        return;
    }

    if (adapter == nullptr) {
        WVLOG_E("browser connect is nullptr.");
        return;
    }

    aafwkBrowserHostImpl_ = new (std::nothrow) AafwkBrowserHostImpl(adapter);
    if (aafwkBrowserHostImpl_ == nullptr) {
        WVLOG_E("create new AafwkBrowserHostImpl failed!");
    }
    WVLOG_I("AafwkAppMgrClientAdapterImpl SaveBrowserConnect success!");
    appMgrClient_->SaveBrowserChannel(aafwkBrowserHostImpl_);
}
} // namespace OHOS::NWeb

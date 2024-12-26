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

#include <AbilityKit/native_child_process.h>
#include "nweb_log.h"

namespace {
const char *IPC_FD_NAME = "IPC_FD";
const char *SHARED_FD_NAME = "SHARED_FD";
const char *CRASH_FD_NAME = "CRASH_FD";
} // namespace

namespace OHOS::NWeb {
const std::string GPU_PROCESS_TYPE = "gpu-process";

int AafwkAppMgrClientAdapterImpl::StartRenderProcess(
    const std::string& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd, pid_t& renderPid)
{
    WVLOG_D("[adapter not used] AafwkAppMgrClientAdapterImpl::StartRenderProcess");
    return 0;
}

void AafwkAppMgrClientAdapterImpl::AttachRenderProcess(std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter)
{
    WVLOG_D("[adapter not used] AafwkAppMgrClientAdapterImpl::AttachRenderProcess");
}

int AafwkAppMgrClientAdapterImpl::GetRenderProcessTerminationStatus(pid_t renderPid, int &status)
{
    WVLOG_D("[adapter not used] AafwkAppMgrClientAdapterImpl::GetRenderProcessTerminationStatus");
    return 0;
}

int AafwkAppMgrClientAdapterImpl::StartChildProcess(
    const std::string& renderParam, int32_t ipcFd, int32_t sharedFd,
    int32_t crashFd, pid_t& renderPid, const std::string& processType)
{
    if (processType == GPU_PROCESS_TYPE) {
        WVLOG_D("[adapter mock] AafwkAppMgrClientAdapterImpl::StartChildProcess for gpu-process");
        return 0;
    }
    WVLOG_D("AafwkAppMgrClientAdapterImpl::StartChildProcess for %{public}s", processType.c_str());
    const char* entry = "libweb_engine.so:NWebRenderMain";

    NativeChildProcess_Args args;
    args.entryParams = const_cast<char*>(renderParam.c_str());
    NativeChildProcess_Fd ipcFdStruct{const_cast<char*>(IPC_FD_NAME), ipcFd, nullptr};
    NativeChildProcess_Fd sharedFdStruct{const_cast<char*>(SHARED_FD_NAME), sharedFd, &ipcFdStruct};
    NativeChildProcess_Fd crashFdStruct{const_cast<char*>(CRASH_FD_NAME), crashFd, &sharedFdStruct};
    args.fdList.head = &crashFdStruct;

    NativeChildProcess_Options option{NCP_ISOLATION_MODE_ISOLATED, 0};
    auto ret = OH_Ability_StartNativeChildProcess(entry, args, option, &renderPid);
    if (ret != NCP_NO_ERROR) {
        WVLOG_E("start native child process failed, ret = %{public}d.", ret);
        return -1;
    }
    return 0;
}

void AafwkAppMgrClientAdapterImpl::SaveBrowserConnect(std::shared_ptr<AafwkBrowserHostAdapter> adapter)
{
    WVLOG_D("[adapter not used] AafwkAppMgrClientAdapterImpl::SaveBrowserConnect");
}
} // namespace OHOS::NWeb

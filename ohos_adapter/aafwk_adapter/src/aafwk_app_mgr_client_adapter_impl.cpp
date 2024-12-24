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

#include <mutex>
#include <condition_variable>

#include <AbilityKit/native_child_process.h>
#include <native_window/external_window.h>
#include <IPCKit/ipc_kit.h>
#include "nweb_log.h"

namespace OHOS::NWeb {
namespace {
const char *IPC_FD_NAME = "IPC_FD";
const char *SHARED_FD_NAME = "SHARED_FD";
const char *CRASH_FD_NAME = "CRASH_FD";

// for singel GPU process
const char *GPU_MAIN_STUB_DESCRIPTOR = "GPU_MAIN_STUB";
const std::string GPU_PROCESS_TYPE = "gpu-process";
const uint32_t IPC_B2G_CODE_INIT_DATA = 10001;
const uint32_t IPC_G2B_CODE_QUERY_WINDOW = 10002;
const uint32_t IPC_G2B_CODE_DESTROY_WINDOW = 10003;
OHIPCRemoteProxy* g_remote_proxy = nullptr;
std::shared_ptr<AafwkBrowserHostAdapter> g_browser_host_adapter = nullptr;
std::mutex mtx;
std::condition_variable cv;
} // namespace

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
    WVLOG_D("AafwkAppMgrClientAdapterImpl::GetRenderProcessTerminationStatus");
    status = 0;
    return 0;
}

int OnRemoteRequestForGpuProcess(uint32_t code, const OHIPCParcel* data, OHIPCParcel* reply, void* user_data)
{
    WVLOG_D("receive a native IPC remote request from GPU process, code=%{public}d", code);
    if (g_browser_host_adapter == nullptr) {
        WVLOG_E("browser host adapter is null");
        return -1;
    }
    if (code == IPC_G2B_CODE_QUERY_WINDOW) {
        int windowId;
        if (OH_IPCParcel_ReadInt32(data, &windowId) != 0) {
            WVLOG_E("failed to read window id");
            return -1;
        }
        void* window = g_browser_host_adapter->GetSurfaceFromKernel(windowId);
        if (window == nullptr) {
            WVLOG_E("failed to get window from kernel");
            return -1;
        }
        OHNativeWindow* ohNativeWindow = reinterpret_cast<OHNativeWindow*>(window);
        if (OH_NativeWindow_WriteToParcel(ohNativeWindow, reply) != 0) {
            WVLOG_E("failed to write window");
            return -1;
        }
        return 0;
    } else if (code == IPC_G2B_CODE_DESTROY_WINDOW) {
        int windowId;
        if (OH_IPCParcel_ReadInt32(data, &windowId) != 0) {
            WVLOG_E("failed to read window id");
            return -1;
        }
        g_browser_host_adapter->DestroySurfaceFromKernel(windowId);
        return 0;
    } else {
        WVLOG_E("unknow code from GPU process, code=%{public}d", code);
        return -1;
    }
}

int WriteInitDataToParcel(OHIPCParcel* data, OHIPCRemoteStub* stub, const std::string& renderParam)
{
    if (OH_IPCParcel_WriteRemoteStub(data, stub) != 0) {
        WVLOG_E("failed to write remote stub");
        return -1;
    }
    if (OH_IPCParcel_WriteString(data, renderParam.c_str()) != 0) {
        WVLOG_E("failed to write render param");
        return -1;
    }
    return 0;
}

int WriteFdsToParcel(OHIPCParcel* data, int32_t ipcFd, int32_t sharedFd, int32_t crashFd)
{
    if (OH_IPCParcel_WriteFileDescriptor(data, ipcFd) != 0) {
        WVLOG_E("failed to write ipc fd");
        return -1;
    }
    if (OH_IPCParcel_WriteFileDescriptor(data, sharedFd) != 0) {
        WVLOG_E("failed to write shared fd");
        return -1;
    }
    if (OH_IPCParcel_WriteFileDescriptor(data, crashFd) != 0) {
        WVLOG_E("failed to write crash fd");
        return -1;
    }
    return 0;
}

int SendInitDataToGpuProcess(const std::string& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd,
    pid_t& renderPid)
{
    OHIPCRemoteStub* stub = OH_IPCRemoteStub_Create(
        GPU_MAIN_STUB_DESCRIPTOR, &OnRemoteRequestForGpuProcess, nullptr, nullptr);
    if (stub == nullptr) {
        WVLOG_E("failed to create stub");
        return -1;
    }

    OHIPCParcel* data = OH_IPCParcel_Create();
    OHIPCParcel* reply = OH_IPCParcel_Create();
    if (data == nullptr || reply == nullptr) {
        WVLOG_E("failed to create parcel");
        return -1;
    }

    if (WriteInitDataToParcel(data, stub, renderParam) != 0) {
        OH_IPCParcel_Destroy(data);
        OH_IPCParcel_Destroy(reply);
        return -1;
    }

    if (WriteFdsToParcel(data, ipcFd, sharedFd, crashFd) != 0) {
        OH_IPCParcel_Destroy(data);
        OH_IPCParcel_Destroy(reply);
        return -1;
    }

    if (g_remote_proxy != nullptr &&
        OH_IPCRemoteProxy_SendRequest(g_remote_proxy, IPC_B2G_CODE_INIT_DATA, data, reply, nullptr) != 0) {
        WVLOG_E("failed to send request of init data");
        OH_IPCParcel_Destroy(data);
        OH_IPCParcel_Destroy(reply);
        return -1;
    }

    if (OH_IPCParcel_ReadInt32(reply, &renderPid) != 0) {
        WVLOG_E("failed to get render pid");
        OH_IPCParcel_Destroy(data);
        OH_IPCParcel_Destroy(reply);
        return -1;
    }

    OH_IPCParcel_Destroy(data);
    OH_IPCParcel_Destroy(reply);
    return 0;
}

void onGpuProcessStarted(int err_code, OHIPCRemoteProxy* remote_proxy)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (remote_proxy == nullptr) {
        WVLOG_E("Initialize IPC remote proxy failed, which is nullptr.");
        return;
    }
    WVLOG_D("onGpuProcessStarted for gpu-process");
    g_remote_proxy = remote_proxy;
    cv.notify_all();
}

int AafwkAppMgrClientAdapterImpl::StartChildProcess(
    const std::string& renderParam, int32_t ipcFd, int32_t sharedFd,
    int32_t crashFd, pid_t& renderPid, const std::string& processType)
{
    WVLOG_D("AafwkAppMgrClientAdapterImpl::StartChildProcess for %{public}s", processType.c_str());
    if (processType == GPU_PROCESS_TYPE) {
        std::unique_lock<std::mutex> lock(mtx);
        g_remote_proxy = nullptr;
        if (OH_Ability_CreateNativeChildProcess("libweb_engine.so", &onGpuProcessStarted) != 0) {
            WVLOG_E("failed to startChildProcess for gpu-process");
            return -1;
        }
        if (!cv.wait_for(lock, std::chrono::seconds(3), [] { // wait for 3sec
                return g_remote_proxy != nullptr;
            })) {
            WVLOG_E("startChildProcess for gpu-process: wait failed");
            return -1;
        }
        if (SendInitDataToGpuProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid) != 0) {
            return -1;
        }
        return 0;
    }
    
    // for Render process
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
    WVLOG_D("AafwkAppMgrClientAdapterImpl::SaveBrowserConnect");
    g_browser_host_adapter = adapter;
}
} // namespace OHOS::NWeb

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

#include "ohos_adapter/bridge/ark_aafwk_app_mgr_client_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_aafwk_browser_host_adapter_impl.h"
#include "ohos_adapter/bridge/ark_aafwk_render_scheduler_host_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAafwkAppMgrClientAdapterWrapper::ArkAafwkAppMgrClientAdapterWrapper(ArkWebRefPtr<ArkAafwkAppMgrClientAdapter> ref)
    : ctocpp_(ref)
{}

int ArkAafwkAppMgrClientAdapterWrapper::StartRenderProcess(
    const std::string& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd, pid_t& renderPid)
{
    ArkWebString ark_renderParam = ArkWebStringClassToStruct(renderParam);
    int result = ctocpp_->StartRenderProcess(ark_renderParam, ipcFd, sharedFd, crashFd, renderPid);

    ArkWebStringStructRelease(ark_renderParam);

    return result;
}

void ArkAafwkAppMgrClientAdapterWrapper::AttachRenderProcess(
    std::shared_ptr<NWeb::AafwkRenderSchedulerHostAdapter> adapter)
{
    if (CHECK_SHARED_PTR_IS_NULL(adapter)) {
        ctocpp_->AttachRenderProcess(nullptr);
        return;
    }

    ctocpp_->AttachRenderProcess(new ArkAafwkRenderSchedulerHostAdapterImpl(adapter));
}

int ArkAafwkAppMgrClientAdapterWrapper::GetRenderProcessTerminationStatus(pid_t renderPid, int& status)
{
    return ctocpp_->GetRenderProcessTerminationStatus(renderPid, status);
}

int ArkAafwkAppMgrClientAdapterWrapper::StartChildProcess(const std::string& renderParam, int32_t ipcFd,
    int32_t sharedFd, int32_t crashFd, pid_t& renderPid, const std::string& processType)
{
    ArkWebString ark_renderParam = ArkWebStringClassToStruct(renderParam);
    ArkWebString ark_processType = ArkWebStringClassToStruct(processType);
    int result = ctocpp_->StartChildProcess(ark_renderParam, ipcFd, sharedFd, crashFd, renderPid, ark_processType);

    ArkWebStringStructRelease(ark_renderParam);
    ArkWebStringStructRelease(ark_processType);

    return result;
}

void ArkAafwkAppMgrClientAdapterWrapper::SaveBrowserConnect(std::shared_ptr<NWeb::AafwkBrowserHostAdapter> adapter)
{
    if (CHECK_SHARED_PTR_IS_NULL(adapter)) {
        ctocpp_->SaveBrowserConnect(nullptr);
        return;
    }

    ctocpp_->SaveBrowserConnect(new ArkAafwkBrowserHostAdapterImpl(adapter));
}

bool ArkAafwkAppMgrClientAdapterWrapper::IsRenderProcessByUid(int uid)
{
    return ctocpp_->IsRenderProcessByUid(uid);
}
} // namespace OHOS::ArkWeb

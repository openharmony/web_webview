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

#include "ohos_adapter/bridge/ark_aafwk_app_mgr_client_adapter_impl.h"

#include "ohos_adapter/bridge/ark_aafwk_browser_host_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_aafwk_render_scheduler_host_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAafwkAppMgrClientAdapterImpl::ArkAafwkAppMgrClientAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AafwkAppMgrClientAdapter> ref)
    : real_(ref)
{}

int ArkAafwkAppMgrClientAdapterImpl::StartRenderProcess(
    const ArkWebString& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd, pid_t& renderPid)
{
    return real_->StartRenderProcess(ArkWebStringStructToClass(renderParam), ipcFd, sharedFd, crashFd, renderPid);
}

void ArkAafwkAppMgrClientAdapterImpl::AttachRenderProcess(ArkWebRefPtr<ArkAafwkRenderSchedulerHostAdapter> adapter)
{
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return real_->AttachRenderProcess(nullptr);
    }

    return real_->AttachRenderProcess(std::make_shared<ArkAafwkRenderSchedulerHostAdapterWrapper>(adapter));
}

int ArkAafwkAppMgrClientAdapterImpl::GetRenderProcessTerminationStatus(pid_t renderPid, int& status)
{
    return real_->GetRenderProcessTerminationStatus(renderPid, status);
}

int ArkAafwkAppMgrClientAdapterImpl::StartChildProcess(const ArkWebString& renderParam, int32_t ipcFd, int32_t sharedFd,
    int32_t crashFd, pid_t& renderPid, const ArkWebString& processType)
{
    return real_->StartChildProcess(ArkWebStringStructToClass(renderParam), ipcFd, sharedFd, crashFd, renderPid,
        ArkWebStringStructToClass(processType));
}

void ArkAafwkAppMgrClientAdapterImpl::SaveBrowserConnect(ArkWebRefPtr<ArkAafwkBrowserHostAdapter> adapter)
{
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return real_->SaveBrowserConnect(nullptr);
    }

    return real_->SaveBrowserConnect(std::make_shared<ArkAafwkBrowserHostAdapterWrapper>(adapter));
}

bool ArkAafwkAppMgrClientAdapterImpl::IsRenderProcessByUid(int uid)
{
    return real_->IsRenderProcessByUid(uid);
}
} // namespace OHOS::ArkWeb

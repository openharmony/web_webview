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

#ifndef ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_IMPL_H
#define ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_IMPL_H
#pragma once

#include "aafwk_app_mgr_client_adapter.h"
#include "ohos_adapter/include/ark_aafwk_app_mgr_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkAafwkAppMgrClientAdapterImpl : public ArkAafwkAppMgrClientAdapter {
public:
    explicit ArkAafwkAppMgrClientAdapterImpl(std::shared_ptr<OHOS::NWeb::AafwkAppMgrClientAdapter>);

    int StartRenderProcess(
        const ArkWebString& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd, pid_t& renderPid) override;

    void AttachRenderProcess(ArkWebRefPtr<ArkAafwkRenderSchedulerHostAdapter> adapter) override;

    int GetRenderProcessTerminationStatus(pid_t renderPid, int& status) override;

    int StartChildProcess(const ArkWebString& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd,
        pid_t& renderPid, const ArkWebString& processType) override;

    void SaveBrowserConnect(ArkWebRefPtr<ArkAafwkBrowserHostAdapter> adapter) override;

    bool IsRenderProcessByUid(int uid) override;

private:
    std::shared_ptr<OHOS::NWeb::AafwkAppMgrClientAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAafwkAppMgrClientAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_IMPL_H

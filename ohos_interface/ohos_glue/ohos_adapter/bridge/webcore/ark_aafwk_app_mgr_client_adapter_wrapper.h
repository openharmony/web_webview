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

#ifndef ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_WRAPPER_H
#define ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_WRAPPER_H
#pragma once

#include "aafwk_app_mgr_client_adapter.h"
#include "ohos_adapter/include/ark_aafwk_app_mgr_client_adapter.h"

namespace OHOS::ArkWeb {
class ArkAafwkAppMgrClientAdapterWrapper : public OHOS::NWeb::AafwkAppMgrClientAdapter {
public:
    explicit ArkAafwkAppMgrClientAdapterWrapper(ArkWebRefPtr<ArkAafwkAppMgrClientAdapter>);

    int StartRenderProcess(
        const std::string& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd, pid_t& renderPid) override;

    void AttachRenderProcess(std::shared_ptr<NWeb::AafwkRenderSchedulerHostAdapter> adapter) override;

    int GetRenderProcessTerminationStatus(pid_t renderPid, int& status) override;

    int StartChildProcess(const std::string& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd,
        pid_t& renderPid, const std::string& processType) override;

    void SaveBrowserConnect(std::shared_ptr<NWeb::AafwkBrowserHostAdapter> adapter) override;

    bool IsRenderProcessByUid(int uid) override;

private:
    ArkWebRefPtr<ArkAafwkAppMgrClientAdapter> ctocpp_;
};
} // namespace OHOS::ArkWeb

#endif // ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_WRAPPER_H

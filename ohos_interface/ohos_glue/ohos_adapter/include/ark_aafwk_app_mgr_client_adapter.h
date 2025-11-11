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

#ifndef ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_H
#define ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_aafwk_browser_host_adapter.h"
#include "ohos_adapter/include/ark_aafwk_render_scheduler_host_adapter.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkAafwkAppMgrClientAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int StartRenderProcess(
        const ArkWebString& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd, pid_t& renderPid) = 0;

    /*--ark web()--*/
    virtual void AttachRenderProcess(ArkWebRefPtr<ArkAafwkRenderSchedulerHostAdapter> adapter) = 0;

    /*--ark web()--*/
    virtual int GetRenderProcessTerminationStatus(pid_t renderPid, int& status) = 0;

    /*--ark web()--*/
    virtual int StartChildProcess(const ArkWebString& renderParam, int32_t ipcFd, int32_t sharedFd, int32_t crashFd,
        pid_t& renderPid, const ArkWebString& processType) = 0;

    /*--ark web()--*/
    virtual void SaveBrowserConnect(ArkWebRefPtr<ArkAafwkBrowserHostAdapter> adapter) = 0;

    /*--ark web()--*/
    virtual bool IsRenderProcessByUid(int uid) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_AAFWK_APP_MGR_CLIENT_ADAPTER_H

/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_RES_SCHED_CLIENT_ADAPTER_H
#define ARK_RES_SCHED_CLIENT_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkResSchedClientAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    static bool ReportKeyThread(int32_t statusAdapter, pid_t pid, pid_t tid, int32_t roleAdapter);

    /*--ark web()--*/
    static bool ReportWindowStatus(int32_t statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId);

    /*--ark web()--*/
    static bool ReportScene(int32_t statusAdapter, int32_t sceneAdapter, int32_t nwebId);

    /*--ark web()--*/
    static bool ReportAudioData(int32_t statusAdapter, pid_t pid, pid_t tid);

    /*--ark web()--*/
    static void ReportWindowId(int32_t windowId, int32_t nwebId);

    /*--ark web()--*/
    static void ReportNWebInit(int32_t statusAdapter, int32_t nweb_id);

    /*--ark web()--*/
    static void ReportRenderProcessStatus(int32_t statusAdapter, pid_t pid);

    /*--ark web()--*/
    static bool ReportScreenCapture(int32_t statusAdapter, pid_t pid);

    /*--ark web()--*/
    static bool ReportVideoPlaying(int32_t statusAdapter, pid_t pid);

    /*--ark web()--*/
    static void ReportProcessInUse(pid_t pid);

    /*--ark web()--*/
    static void ReportSiteIsolationMode(bool mode);

    /*--ark web()--*/
    static bool ReportSubwindowCall(int32_t statusAdapter, pid_t pid, pid_t tid);
};

} // namespace OHOS::ArkWeb

#endif // ARK_RES_SCHED_CLIENT_ADAPTER_H

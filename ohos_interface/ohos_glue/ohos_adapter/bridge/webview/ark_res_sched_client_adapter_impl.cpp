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

#include "ohos_adapter/include/ark_res_sched_client_adapter.h"
#include "res_sched_client_adapter.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

bool ArkResSchedClientAdapter::ReportKeyThread(int32_t statusAdapter, pid_t pid, pid_t tid, int32_t roleAdapter)
{
    return NWeb::ResSchedClientAdapter::ReportKeyThread(
        (NWeb::ResSchedStatusAdapter)statusAdapter, pid, tid, (NWeb::ResSchedRoleAdapter)roleAdapter);
}

bool ArkResSchedClientAdapter::ReportWindowStatus(int32_t statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId)
{
    return NWeb::ResSchedClientAdapter::ReportWindowStatus(
        (NWeb::ResSchedStatusAdapter)statusAdapter, pid, windowId, nwebId);
}

bool ArkResSchedClientAdapter::ReportScene(int32_t statusAdapter, int32_t sceneAdapter, int32_t nwebId)
{
    return NWeb::ResSchedClientAdapter::ReportScene(
        (NWeb::ResSchedStatusAdapter)statusAdapter, (NWeb::ResSchedSceneAdapter)sceneAdapter, nwebId);
}

bool ArkResSchedClientAdapter::ReportAudioData(int32_t statusAdapter, pid_t pid, pid_t tid)
{
    return NWeb::ResSchedClientAdapter::ReportAudioData((NWeb::ResSchedStatusAdapter)statusAdapter, pid, tid);
}

void ArkResSchedClientAdapter::ReportWindowId(int32_t windowId, int32_t nwebId)
{
    return NWeb::ResSchedClientAdapter::ReportWindowId(windowId, nwebId);
}

void ArkResSchedClientAdapter::ReportNWebInit(int32_t statusAdapter, int32_t nweb_id)
{
    NWeb::ResSchedClientAdapter::ReportNWebInit((NWeb::ResSchedStatusAdapter)statusAdapter, nweb_id);
}

void ArkResSchedClientAdapter::ReportRenderProcessStatus(int32_t statusAdapter, pid_t pid)
{
    NWeb::ResSchedClientAdapter::ReportRenderProcessStatus((NWeb::ResSchedStatusAdapter)statusAdapter, pid);
}

bool ArkResSchedClientAdapter::ReportScreenCapture(int32_t statusAdapter, pid_t pid)
{
    return NWeb::ResSchedClientAdapter::ReportScreenCapture((NWeb::ResSchedStatusAdapter)statusAdapter, pid);
}

bool ArkResSchedClientAdapter::ReportVideoPlaying(int32_t statusAdapter, pid_t pid)
{
    return NWeb::ResSchedClientAdapter::ReportVideoPlaying((NWeb::ResSchedStatusAdapter)statusAdapter, pid);
}

void ArkResSchedClientAdapter::ReportProcessInUse(pid_t pid)
{
    NWeb::ResSchedClientAdapter::ReportProcessInUse(pid);
}

void ArkResSchedClientAdapter::ReportSiteIsolationMode(bool mode)
{
    NWeb::ResSchedClientAdapter::ReportSiteIsolationMode(mode);
}

bool ArkResSchedClientAdapter::ReportSubwindowCall(int32_t statusAdapter, pid_t pid, pid_t tid)
{
    return NWeb::ResSchedClientAdapter::ReportSubwindowCall((NWeb::ResSchedStatusAdapter)statusAdapter, pid, tid);
}

} // namespace OHOS::ArkWeb

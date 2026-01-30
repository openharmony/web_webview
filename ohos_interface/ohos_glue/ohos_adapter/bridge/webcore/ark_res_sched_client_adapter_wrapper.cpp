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

namespace OHOS::NWeb {

bool ResSchedClientAdapter::ReportKeyThread(
    ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid, ResSchedRoleAdapter roleAdapter)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportKeyThread(
        (int32_t)statusAdapter, pid, tid, (int32_t)roleAdapter);
}

bool ResSchedClientAdapter::ReportWindowStatus(
    ResSchedStatusAdapter statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportWindowStatus((int32_t)statusAdapter, pid, windowId, nwebId);
}

bool ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter statusAdapter, ResSchedSceneAdapter sceneAdapter, int32_t nwebId)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportScene((int32_t)statusAdapter, (int32_t)sceneAdapter, nwebId);
}

bool ResSchedClientAdapter::ReportAudioData(ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportAudioData((int32_t)statusAdapter, pid, tid);
}

void ResSchedClientAdapter::ReportWindowId(int32_t windowId, int32_t nwebId)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportWindowId(windowId, nwebId);
}

void ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter statusAdapter, int32_t nweb_id)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportNWebInit((int32_t)statusAdapter, nweb_id);
}

void ResSchedClientAdapter::ReportRenderProcessStatus(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportRenderProcessStatus((int32_t)statusAdapter, pid);
}

bool ResSchedClientAdapter::ReportScreenCapture(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportScreenCapture((int32_t)statusAdapter, pid);
}

bool ResSchedClientAdapter::ReportVideoPlaying(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportVideoPlaying((int32_t)statusAdapter, pid);
}

void ResSchedClientAdapter::ReportProcessInUse(pid_t pid)
{
    OHOS::ArkWeb::ArkResSchedClientAdapter::ReportProcessInUse(pid);
}

void ResSchedClientAdapter::ReportSiteIsolationMode(bool mode)
{
    OHOS::ArkWeb::ArkResSchedClientAdapter::ReportSiteIsolationMode(mode);
}

bool ResSchedClientAdapter::ReportSubwindowCall(ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid)
{
    return OHOS::ArkWeb::ArkResSchedClientAdapter::ReportSubwindowCall((int32_t)statusAdapter, pid, tid);
}
} // namespace OHOS::NWeb

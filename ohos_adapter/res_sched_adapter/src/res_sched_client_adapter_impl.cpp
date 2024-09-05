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

#include <chrono>
#include <mutex>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>
#include <unordered_set>

#include "nweb_log.h"
#include "res_sched_client_adapter.h"

namespace OHOS::NWeb {
bool ResSchedClientAdapter::ReportKeyThread(
    ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid, ResSchedRoleAdapter roleAdapter)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return true;
}

bool ResSchedClientAdapter::ReportAudioData(ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return true;
}

void ResSchedClientAdapter::ReportProcessInUse(pid_t pid)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

void ResSchedClientAdapter::ReportSiteIsolationMode(bool mode)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

bool ResSchedClientAdapter::ReportWindowStatus(
    ResSchedStatusAdapter statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return true;
}

bool ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter statusAdapter, ResSchedSceneAdapter sceneAdapter, int32_t nwebId)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return false;
}

void ResSchedClientAdapter::ReportWindowId(int32_t windowId, int32_t nwebId)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

void ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter statusAdapter, int32_t nwebId)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

void ResSchedClientAdapter::ReportRenderProcessStatus(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

bool ResSchedClientAdapter::ReportScreenCapture(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return true;
}

bool ResSchedClientAdapter::ReportVideoPlaying(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return true;
}
} // namespace OHOS::NWeb

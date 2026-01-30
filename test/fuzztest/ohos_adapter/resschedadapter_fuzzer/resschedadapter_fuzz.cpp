/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "resschedadapter_fuzz.h"

#include <cstdint>
#include <cstring>
#include <mutex>
#include <securec.h>
#include <unordered_map>
#include <vector>
#include <fuzzer/FuzzedDataProvider.h>

#include "res_sched_client_adapter.h"

using namespace OHOS::NWeb;

namespace OHOS {
namespace NWeb {
constexpr int MAX_SET_NUMBER = 1000;

bool ResSchedAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    FuzzedDataProvider dataProvider(data, size);

    pid_t pid = 1;
    pid_t tid = 1;
    int32_t nwebId = dataProvider.ConsumeIntegralInRange<int32_t>(0, MAX_SET_NUMBER);
    uint32_t windowId = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_SET_NUMBER);

    std::vector<ResSchedStatusAdapter> statuses = { ResSchedStatusAdapter::WEB_ACTIVE,
        ResSchedStatusAdapter::WEB_INACTIVE, ResSchedStatusAdapter::THREAD_CREATED,
        ResSchedStatusAdapter::THREAD_DESTROYED, ResSchedStatusAdapter::VIDEO_PLAYING_START,
        ResSchedStatusAdapter::VIDEO_PLAYING_STOP, ResSchedStatusAdapter::SCREEN_CAPTURE_START,
        ResSchedStatusAdapter::SCREEN_CAPTURE_STOP, ResSchedStatusAdapter::AUDIO_STATUS_START,
        ResSchedStatusAdapter::AUDIO_STATUS_STOP, ResSchedStatusAdapter::WEB_SCENE_ENTER,
        ResSchedStatusAdapter::WEB_SCENE_EXIT };

    std::vector<ResSchedRoleAdapter> roles = { ResSchedRoleAdapter::USER_INTERACT, ResSchedRoleAdapter::NORMAL_DISPLAY,
        ResSchedRoleAdapter::IMPORTANT_DISPLAY, ResSchedRoleAdapter::NORMAL_AUDIO, ResSchedRoleAdapter::IMPORTANT_AUDIO,
        ResSchedRoleAdapter::IMAGE_DECODE };

    ResSchedClientAdapter adapter;

    for (auto status : statuses) {
        adapter.ReportVideoPlaying(status, pid);
        adapter.ReportScreenCapture(status, pid);
        adapter.ReportRenderProcessStatus(status, pid);
        adapter.ReportNWebInit(status, nwebId);
        adapter.ReportWindowId(windowId, nwebId);

        for (auto role : roles) {
            adapter.ReportKeyThread(status, pid, tid, role);
        }

        adapter.ReportAudioData(status, pid, tid);
        adapter.ReportProcessInUse(pid);
        adapter.ReportSiteIsolationMode(true);
        adapter.ReportWindowStatus(status, pid, windowId, nwebId);
        adapter.ReportScene(status, ResSchedSceneAdapter::CLICK, nwebId);
        adapter.ReportScene(status, ResSchedSceneAdapter::LOAD_URL, nwebId);
        adapter.ReportSubwindowCall(status, pid, tid);
    }

    adapter.ReportProcessInUse(0);
    adapter.ReportWindowId(0, nwebId);
    adapter.ReportKeyThread(statuses[0], pid, tid, static_cast<ResSchedRoleAdapter>(-1));
    adapter.ReportAudioData(statuses[0], pid, 0);
    adapter.ReportSiteIsolationMode(true);
    adapter.ReportSubwindowCall(statuses[0], pid, 0);

    adapter.ReportWindowId(0, nwebId);
    adapter.ReportKeyThread(statuses[0], -1, tid, ResSchedRoleAdapter::USER_INTERACT);
    adapter.ReportAudioData(statuses[0], pid, -1);
    adapter.ReportWindowStatus(statuses[0], pid, windowId, -1);
    adapter.ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT, -1);
    adapter.ReportSubwindowCall(statuses[0], pid, -1);

    return true;
}
} // namespace NWeb
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::NWeb::ResSchedAdapterFuzzTest(data, size);
    return 0;
}

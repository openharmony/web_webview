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

#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "nweb_log.h"
#include "res_sched_client.h"
#include "res_sched_client_adapter.h"
#include "res_type.h"

namespace OHOS::NWeb {
using namespace OHOS::ResourceSchedule;

const std::unordered_map<ResSchedTypeAdapter, uint32_t> RES_TYPE_MAP = {
    { ResSchedTypeAdapter::RES_TYPE_KEY_THREAD, ResType::RES_TYPE_REPORT_KEY_THREAD },
    { ResSchedTypeAdapter::RES_TYPE_WEB_STATUS_CHANGE, ResType::RES_TYPE_REPORT_WINDOW_STATE },
    { ResSchedTypeAdapter::RES_TYPE_WEB_SCENE, ResType::RES_TYPE_REPORT_SCENE_SCHED },
};

const std::unordered_map<ResSchedStatusAdapter, int64_t> RES_STATUS_MAP = {
    { ResSchedStatusAdapter::THREAD_CREATED, ResType::ReportChangeStatus::CREATE },
    { ResSchedStatusAdapter::THREAD_DESTROYED, ResType::ReportChangeStatus::REMOVE },
    { ResSchedStatusAdapter::WEB_ACTIVE, ResType::WindowStates::ACTIVE },
    { ResSchedStatusAdapter::WEB_INACTIVE, ResType::WindowStates::INACTIVE },
    { ResSchedStatusAdapter::WEB_SCENE_ENTER, ResType::SceneControl::SCENE_IN },
    { ResSchedStatusAdapter::WEB_SCENE_EXIT, ResType::SceneControl::SCENE_OUT },
};

const std::unordered_map<ResSchedRoleAdapter, ResType::ThreadRole> RES_ROLE_MAP = {
    { ResSchedRoleAdapter::USER_INTERACT, ResType::ThreadRole::USER_INTERACT },
    { ResSchedRoleAdapter::NORMAL_DISPLAY, ResType::ThreadRole::NORMAL_DISPLAY },
    { ResSchedRoleAdapter::IMPORTANT_DISPLAY, ResType::ThreadRole::IMPORTANT_DISPLAY },
    { ResSchedRoleAdapter::NORMAL_AUDIO, ResType::ThreadRole::NORMAL_AUDIO },
    { ResSchedRoleAdapter::IMPORTANT_AUDIO, ResType::ThreadRole::IMPORTANT_AUDIO },
};

const std::unordered_map<ResSchedSceneAdapter, int32_t> RES_SCENE_MAP = {
    { ResSchedSceneAdapter::LOAD_URL, ResType::WebScene::WEB_SCENE_LOAD_URL },
    { ResSchedSceneAdapter::CLICK, ResType::WebScene::WEB_SCENE_CLICK },
    { ResSchedSceneAdapter::SLIDE, ResType::WebScene::WEB_SCENE_SLIDE },
    { ResSchedSceneAdapter::RESIZE, ResType::WebScene::WEB_SCENE_RESIZE },
    { ResSchedSceneAdapter::VISIBLE, ResType::WebScene::WEB_SCENE_VISIBLE },
};

constexpr char PID[] = "pid";
constexpr char UID[] = "uid";
constexpr char TID[] = "tid";
constexpr char ROLE[] = "role";
constexpr char WINDOW_ID[] = "windowId";
constexpr char SERIAL_NUMBER[] = "serialNum";
constexpr char SCENE_ID[] = "sceneId";
constexpr char STATE[] = "state";
class RenderData;
std::unordered_map<int32_t, RenderData> g_renderMap;
std::unordered_map<pid_t, pid_t> g_renderPidMap;

std::string GetUidString()
{
    static std::string uidString = std::to_string(getuid());
    return uidString;
}

bool ConvertStatus(ResSchedStatusAdapter statusAdapter, int64_t& status)
{
    if (auto it = RES_STATUS_MAP.find(statusAdapter); it == RES_STATUS_MAP.end()) {
        WVLOG_E("invalid status: %{public}d", statusAdapter);
        return false;
    } else {
        status = it->second;
    }
    return true;
}

bool ReportSceneInternal(ResSchedStatusAdapter statusAdapter, ResSchedSceneAdapter sceneAdapter)
{
    static uint32_t resType = ResType::RES_TYPE_REPORT_SCENE_SCHED;

    int64_t status;
    bool ret = ConvertStatus(statusAdapter, status);
    if (!ret) {
        return false;
    }

    int32_t scene_id;
    if (auto it = RES_SCENE_MAP.find(sceneAdapter); it == RES_SCENE_MAP.end()) {
        WVLOG_E("invalid scene id: %{public}d", sceneAdapter);
        return false;
    } else {
        scene_id = it->second;
    }

    std::unordered_map<std::string, std::string> mapPayload { { UID, GetUidString() },
        { SCENE_ID, std::to_string(scene_id) } };
    WVLOG_D("ReportScene status: %{public}d, uid: %{public}s, scene_id: %{public}d", static_cast<int32_t>(status),
        GetUidString().c_str(), scene_id);
    ResSchedClient::GetInstance().ReportData(resType, status, mapPayload);
    return true;
}

class RenderData {
public:
    RenderData() = default;
    RenderData(int32_t nweb_id) : nweb_id_(nweb_id) {};
    ~RenderData()
    {
        WVLOG_D("report RenderData dtor");
    };
    void SetNWebId(int32_t nweb_id)
    {
        nweb_id_ = nweb_id;
    };
    void SetMainTid(pid_t main_tid)
    {
        main_tid_ = main_tid;
    };
    void SetCompositorTid(pid_t compositor_tid)
    {
        compositor_tid_ = compositor_tid;
    };
    void SetActive(bool is_active)
    {
        is_active_ = is_active;
    };
    bool IsSet()
    {
        return (main_tid_ != -1 && compositor_tid_ != -1);
    };
    void AddScene(ResSchedSceneAdapter scene)
    {
        scene_list_.emplace_back(scene);
    };
    void ReportScene()
    {
        if (!ShouldReportScene()) {
            WVLOG_D("should not report scene");
            return;
        }
        for (auto& scene : scene_list_) {
            ReportSceneInternal(ResSchedStatusAdapter::WEB_SCENE_ENTER, scene);
        }
        scene_list_.clear();
    };
    bool ShouldReportScene()
    {
        return (main_tid_ != -1 && compositor_tid_ != -1 && is_active_);
    };
    void SetLoaded()
    {
        is_loaded_ = true;
    };
    bool IsLoaded()
    {
        return is_loaded_;
    };

private:
    int32_t nweb_id_ = -1;
    pid_t main_tid_ = -1;
    pid_t compositor_tid_ = -1;
    std::vector<ResSchedSceneAdapter> scene_list_;
    bool is_active_ = false;
    bool is_loaded_ = false;
};

bool ResSchedClientAdapter::ReportKeyThread(
    ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid, ResSchedRoleAdapter roleAdapter)
{
    static uint32_t resType = ResType::RES_TYPE_REPORT_KEY_THREAD;

    int64_t status;
    bool ret = ConvertStatus(statusAdapter, status);
    if (!ret)
        return false;

    ResType::ThreadRole role;
    if (auto it = RES_ROLE_MAP.find(roleAdapter); it == RES_ROLE_MAP.end()) {
        WVLOG_E("invalid role: %{public}d", roleAdapter);
        return false;
    } else {
        role = it->second;
    }

    std::unordered_map<std::string, std::string> mapPayload { { UID, GetUidString() }, { PID, std::to_string(pid) },
        { TID, std::to_string(tid) }, { ROLE, std::to_string(role) } };
    WVLOG_D("ReportKeyThread status: %{public}d, uid: %{public}s, pid: %{public}d, tid:%{public}d, role: %{public}d",
        static_cast<int32_t>(status), GetUidString().c_str(), pid, tid, static_cast<int32_t>(role));
    ResSchedClient::GetInstance().ReportData(resType, status, mapPayload);

    if (pid == tid) {
        g_renderPidMap[pid] = -1;
    } else if (g_renderPidMap.find(pid) == g_renderPidMap.end()) {
        g_renderPidMap[pid] = tid;
    } else {
        auto nweb_id = g_renderPidMap[pid];
        if (g_renderMap.find(nweb_id) != g_renderMap.end()) {
            auto& renderData = g_renderMap[nweb_id];
            renderData.SetCompositorTid(tid);
            renderData.ReportScene();
        }
    }
    return true;
}

bool ResSchedClientAdapter::ReportWindowStatus(
    ResSchedStatusAdapter statusAdapter, pid_t pid, uint32_t windowId, int32_t nweb_id)
{
    static uint32_t resType = ResType::RES_TYPE_REPORT_WINDOW_STATE;
    static uint32_t serial_num = 0;
    static constexpr uint32_t SERIAL_NUM_MAX = 10000;

    if (g_renderMap.find(nweb_id) == g_renderMap.end()) {
        WVLOG_E(
            "ReportWindowStatus nweb_id %{public}d not exist in render map, status %{public}d", nweb_id, statusAdapter);
        return false;
    }

    auto& renderData = g_renderMap[nweb_id];
    if (!renderData.IsSet()) {
        renderData.SetMainTid(pid);
        if (g_renderPidMap[pid] == -1) {
            g_renderPidMap[pid] = nweb_id;
        } else {
            renderData.SetCompositorTid(g_renderPidMap[pid]);
        }
    }

    int64_t status;
    bool ret = ConvertStatus(statusAdapter, status);
    if (!ret)
        return false;

    std::unordered_map<std::string, std::string> mapPayload { { UID, GetUidString() }, { PID, std::to_string(pid) },
        { WINDOW_ID, std::to_string(windowId) }, { SERIAL_NUMBER, std::to_string(serial_num) },
        { STATE, std::to_string(status) } };
    WVLOG_D("ReportWindowStatus status: %{public}d, uid: %{public}s, pid: %{public}d, windowId:%{public}d, sn: "
            "%{public}d",
        static_cast<int32_t>(status), GetUidString().c_str(), pid, windowId, serial_num);
    ResSchedClient::GetInstance().ReportData(resType, ResType::ReportChangeStatus::CREATE, mapPayload);
    serial_num = (serial_num + 1) % SERIAL_NUM_MAX;

    if (statusAdapter == ResSchedStatusAdapter::WEB_ACTIVE) {
        renderData.SetActive(true);
        renderData.ReportScene();
    } else if (statusAdapter == ResSchedStatusAdapter::WEB_INACTIVE) {
        renderData.SetActive(false);
    }
    return true;
}

bool ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter statusAdapter, ResSchedSceneAdapter sceneAdapter, int32_t nweb_id)
{
    if (nweb_id == -1) {
        return ReportSceneInternal(statusAdapter, sceneAdapter);
    }

    if (g_renderMap.find(nweb_id) == g_renderMap.end()) {
        WVLOG_E("ReportScene nweb_id %{public}d not exist in render map", nweb_id);
        return false;
    }

    auto& renderData = g_renderMap[nweb_id];
    if (sceneAdapter == ResSchedSceneAdapter::LOAD_URL) {
        if (renderData.IsLoaded()) {
            g_renderMap.erase(nweb_id);
            g_renderMap.emplace(nweb_id, RenderData(nweb_id));
        }
        renderData.SetLoaded();
    }

    if (!renderData.ShouldReportScene()) {
        renderData.AddScene(sceneAdapter);
        return true;
    }
    return ReportSceneInternal(statusAdapter, sceneAdapter);
}

void ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter statusAdapter, int32_t nweb_id)
{
    if (statusAdapter == ResSchedStatusAdapter::WEB_SCENE_ENTER) {
        WVLOG_D("ReportNWebInit %{public}d", nweb_id);
        g_renderMap.emplace(nweb_id, RenderData(nweb_id));
    } else if (statusAdapter == ResSchedStatusAdapter::WEB_SCENE_EXIT) {
        g_renderMap.erase(nweb_id);
    }
}
} // namespace OHOS::NWeb

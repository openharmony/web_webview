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

#include "app_mgr_constants.h"
#include "app_mgr_client.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "nweb_log.h"
#include "res_sched_client.h"
#include "res_sched_client_adapter.h"
#include "res_type.h"
#include "singleton.h"
#include "system_ability_definition.h"
#include "system_properties_adapter_impl.h"

namespace OHOS::NWeb {
using namespace OHOS::ResourceSchedule;

const std::unordered_map<ResSchedTypeAdapter, uint32_t> RES_TYPE_MAP = {
    { ResSchedTypeAdapter::RES_TYPE_KEY_THREAD, ResType::RES_TYPE_REPORT_KEY_THREAD },
    { ResSchedTypeAdapter::RES_TYPE_WEB_STATUS_CHANGE, ResType::RES_TYPE_REPORT_WINDOW_STATE },
    { ResSchedTypeAdapter::RES_TYPE_WEB_SCENE, ResType::RES_TYPE_REPORT_SCENE_SCHED },
    { ResSchedTypeAdapter::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE, ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE },
    { ResSchedTypeAdapter::RES_TYPE_WEBVIEW_SCREEN_CAPTURE, ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE },
    { ResSchedTypeAdapter::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE, ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE },
    { ResSchedTypeAdapter::RES_TYPE_WEB_SUBWIN_CALL_STATUS_CHANGE, ResType::RES_TYPE_WEB_SUBWIN_TASK },
};

const std::unordered_map<ResSchedStatusAdapter, int64_t> RES_STATUS_MAP = {
    { ResSchedStatusAdapter::THREAD_CREATED, ResType::ReportChangeStatus::CREATE },
    { ResSchedStatusAdapter::THREAD_DESTROYED, ResType::ReportChangeStatus::REMOVE },
    { ResSchedStatusAdapter::WEB_ACTIVE, ResType::WindowStates::ACTIVE },
    { ResSchedStatusAdapter::WEB_INACTIVE, ResType::WindowStates::INACTIVE },
    { ResSchedStatusAdapter::WEB_SCENE_ENTER, ResType::SceneControl::SCENE_IN },
    { ResSchedStatusAdapter::WEB_SCENE_EXIT, ResType::SceneControl::SCENE_OUT },
    { ResSchedStatusAdapter::AUDIO_STATUS_START, ResType::AudioStatus::START },
    { ResSchedStatusAdapter::AUDIO_STATUS_STOP, ResType::AudioStatus::STOP },
    { ResSchedStatusAdapter::SCREEN_CAPTURE_START, ResType::WebScreenCapture::WEB_SCREEN_CAPTURE_START },
    { ResSchedStatusAdapter::SCREEN_CAPTURE_STOP, ResType::WebScreenCapture::WEB_SCREEN_CAPTURE_STOP },
    { ResSchedStatusAdapter::VIDEO_PLAYING_START, ResType::WebVideoState::WEB_VIDEO_PLAYING_START },
    { ResSchedStatusAdapter::VIDEO_PLAYING_STOP, ResType::WebVideoState::WEB_VIDEO_PLAYING_STOP },
    { ResSchedStatusAdapter::WEB_SUBWIN_CALL_START, ResType::WebVideoState::WEB_SUBWIN_CALL_START },
    { ResSchedStatusAdapter::WEB_SUBWIN_CALL_STOP, ResType::WebVideoState::WEB_SUBWIN_CALL_STOP },
};

const std::unordered_map<ResSchedRoleAdapter, ResType::ThreadRole> RES_ROLE_MAP = {
    { ResSchedRoleAdapter::USER_INTERACT, ResType::ThreadRole::USER_INTERACT },
    { ResSchedRoleAdapter::NORMAL_DISPLAY, ResType::ThreadRole::NORMAL_DISPLAY },
    { ResSchedRoleAdapter::IMPORTANT_DISPLAY, ResType::ThreadRole::IMPORTANT_DISPLAY },
    { ResSchedRoleAdapter::NORMAL_AUDIO, ResType::ThreadRole::NORMAL_AUDIO },
    { ResSchedRoleAdapter::IMPORTANT_AUDIO, ResType::ThreadRole::IMPORTANT_AUDIO },
    { ResSchedRoleAdapter::IMAGE_DECODE, ResType::ThreadRole::IMAGE_DECODE },
};

const std::unordered_map<ResSchedSceneAdapter, int32_t> RES_SCENE_MAP = {
    { ResSchedSceneAdapter::LOAD_URL, ResType::WebScene::WEB_SCENE_LOAD_URL },
    { ResSchedSceneAdapter::CLICK, ResType::WebScene::WEB_SCENE_CLICK },
    { ResSchedSceneAdapter::SLIDE, ResType::WebScene::WEB_SCENE_SLIDE },
    { ResSchedSceneAdapter::RESIZE, ResType::WebScene::WEB_SCENE_RESIZE },
    { ResSchedSceneAdapter::VISIBLE, ResType::WebScene::WEB_SCENE_VISIBLE },
    { ResSchedSceneAdapter::KEYBOARD_CLICK, ResType::WebScene::WEB_SCENE_KEYBOARD_CLICK },
    { ResSchedSceneAdapter::KEY_TASK, ResType::WebScene::WEB_SCENE_KEY_TASK },
    { ResSchedSceneAdapter::IMAGE_DECODE, ResType::WebScene::WEB_SCENE_IMAGE_DECODE },
};

const int32_t INVALID_NUMBER = -1;
const int64_t INVALID_NUMBER_INT64 = -1;
const int64_t SLIDE_PERIOD_MS = 300;
const pid_t INVALID_PID = -1;
constexpr char PID[] = "pid";
constexpr char UID[] = "uid";
constexpr char TID[] = "tid";
constexpr char ROLE[] = "role";
constexpr char WINDOW_ID[] = "windowId";
constexpr char SERIAL_NUMBER[] = "serialNum";
constexpr char SCENE_ID[] = "sceneId";
constexpr char STATE[] = "state";
std::set<int32_t> g_nwebSet;
std::unordered_map<pid_t, std::unordered_map<int32_t, ResSchedStatusAdapter>> g_pidNwebMap;
std::unordered_map<int32_t, std::vector<pid_t>> g_nwebProcessMap;
std::unordered_set<pid_t> g_processInUse;
std::mutex g_windowIdMutex {};
std::mutex g_processInUseMutex {};
int32_t g_windowId = INVALID_NUMBER;
int32_t g_nwebId = INVALID_NUMBER;
pid_t g_lastPid = INVALID_PID;
int64_t g_lastStatus = INVALID_NUMBER_INT64;
int64_t g_timeStamp = 0;
int64_t g_preTimeStamp = 0;
pid_t g_lastRenderPid = INVALID_PID;
int64_t g_lastRenderStatus = INVALID_NUMBER_INT64;
bool g_siteIsolationMode = false;

std::string GetUidString()
{
    static std::string uidString = std::to_string(getuid());
    return uidString;
}

std::string GetBundleNameByUid(int32_t uid)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        WVLOG_E("get SystemAbilityManager failed");
        return "";
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        WVLOG_E("get Bundle Manager failed");
        return "";
    }
    auto bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (bundleMgr == nullptr) {
        return "";
    }
    std::string bundle {""};
    bundleMgr->GetNameForUid(uid, bundle);
    return bundle;
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

bool NeedReportScene(ResSchedSceneAdapter sceneAdapter)
{
    if (sceneAdapter != ResSchedSceneAdapter::SLIDE) {
        return true;
    }
    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    g_timeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();
    if (g_timeStamp - g_preTimeStamp > SLIDE_PERIOD_MS) {
        g_preTimeStamp = g_timeStamp;
        return true;
    }
    return false;
}

bool ReportSceneInternal(ResSchedStatusAdapter statusAdapter, ResSchedSceneAdapter sceneAdapter)
{
    // To limit the frequency of events reported in some scenarios
    if (!NeedReportScene(sceneAdapter)) {
        return false;
    }

    int64_t status;
    bool ret = ConvertStatus(statusAdapter, status);
    if (!ret) {
        return false;
    }

    int32_t sceneId;
    if (auto it = RES_SCENE_MAP.find(sceneAdapter); it == RES_SCENE_MAP.end()) {
        WVLOG_E("invalid scene id: %{public}d", sceneAdapter);
        return false;
    } else {
        sceneId = it->second;
    }

    std::unordered_map<std::string, std::string> mapPayload { { UID, GetUidString() },
        { SCENE_ID, std::to_string(sceneId) } };
    ResSchedClient::GetInstance().ReportData(ResType::RES_TYPE_REPORT_SCENE_SCHED, status, mapPayload);
    WVLOG_D("ReportScene status: %{public}d, uid: %{public}s, sceneId: %{public}d", static_cast<int32_t>(status),
        GetUidString().c_str(), sceneId);
    return true;
}

bool IsSameSourceWebSiteActive(ResSchedStatusAdapter statusAdapter, pid_t pid, int32_t nwebId)
{
    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    g_pidNwebMap[pid][nwebId] = statusAdapter;
    if (statusAdapter == ResSchedStatusAdapter::WEB_INACTIVE) {
        auto nwebMap = g_pidNwebMap[pid];
        for (auto it : nwebMap) {
            if (it.second == ResSchedStatusAdapter::WEB_ACTIVE) {
                WVLOG_D("IsSameSourceWebSiteActive, current nwebId: %{public}d, active nwebId: %{public}d, "
                        "pid: %{public}d", nwebId, it.first, pid);
                return true;
            }
        }
        g_pidNwebMap.erase(pid);
    }
    return false;
}

void ReportStatusData(ResSchedStatusAdapter statusAdapter,
                      pid_t pid, uint32_t windowId, int32_t nwebId, bool isSiteManage)
{
    static uint32_t serialNum = 0;
    static constexpr uint32_t serialNumMax = 10000;

    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    if (g_nwebSet.find(nwebId) == g_nwebSet.end() || pid == 0) {
        WVLOG_D("Don't report window status, nwebId: %{public}d, pid: %{public}d", nwebId, pid);
        return;
    }

    int64_t status;
    bool ret = ConvertStatus(statusAdapter, status);
    if (!ret) {
        return;
    }

    ProductDeviceType deviceType = SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType();
    {
        std::lock_guard<std::mutex> lock(g_processInUseMutex);
        if (deviceType == ProductDeviceType::DEVICE_TYPE_MOBILE || g_processInUse.count(pid)) {
            if (isSiteManage && IsSameSourceWebSiteActive(statusAdapter, pid, nwebId)) {
                return;
            }
        }
    }

    if (pid == g_lastPid && status == g_lastStatus) {
        return;
    }
    g_lastPid = pid;
    g_lastStatus = status;

    std::unordered_map<std::string, std::string> mapPayload { { UID, GetUidString() }, { PID, std::to_string(pid) },
        { WINDOW_ID, std::to_string(windowId) }, { SERIAL_NUMBER, std::to_string(serialNum) },
        { STATE, std::to_string(status) } };
    ResSchedClient::GetInstance().ReportData(
        ResType::RES_TYPE_REPORT_WINDOW_STATE, ResType::ReportChangeStatus::CREATE, mapPayload);

    WVLOG_I("ReportWindowStatus status: %{public}d, uid: %{public}s, pid: %{public}d, windowId: %{public}d, "
            "nwebId: %{public}d, sn: %{public}d",
            static_cast<int32_t>(status), GetUidString().c_str(), pid, windowId, nwebId, serialNum);
    serialNum = (serialNum + 1) % serialNumMax;

    // Report visible scene event again when tab becomes active to solve timing problem
    if (statusAdapter == ResSchedStatusAdapter::WEB_ACTIVE) {
        ReportSceneInternal(statusAdapter, ResSchedSceneAdapter::VISIBLE);
    }
}

bool ResSchedClientAdapter::ReportKeyThread(
    ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid, ResSchedRoleAdapter roleAdapter)
{
    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    int64_t status;
    bool ret = ConvertStatus(statusAdapter, status);
    if (!ret) {
        return false;
    }

    ResType::ThreadRole role;
    if (auto it = RES_ROLE_MAP.find(roleAdapter); it == RES_ROLE_MAP.end()) {
        WVLOG_E("invalid role: %{public}d", roleAdapter);
        return false;
    } else {
        role = it->second;
    }

    std::unordered_map<std::string, std::string> mapPayload { { UID, GetUidString() }, { PID, std::to_string(pid) },
        { TID, std::to_string(tid) }, { ROLE, std::to_string(role) } };

    // Report process creation event first when render process is created
    if (pid == tid) {
        mapPayload["processType"] = std::to_string(static_cast<uint32_t>(AppExecFwk::ProcessType::RENDER));
        mapPayload["bundleName"] = GetBundleNameByUid(getuid());
    }

    ResSchedClient::GetInstance().ReportData(ResType::RES_TYPE_REPORT_KEY_THREAD, status, mapPayload);
    WVLOG_I("ReportKeyThread status: %{public}d, uid: %{public}s, pid: %{public}d, tid:%{public}d, role: %{public}d",
        static_cast<int32_t>(status), GetUidString().c_str(), pid, tid, static_cast<int32_t>(role));

    if (pid == tid && g_windowId != INVALID_NUMBER && g_nwebId != INVALID_NUMBER) {
        std::lock_guard<std::mutex> lock(g_windowIdMutex);
        ReportStatusData(ResSchedStatusAdapter::WEB_ACTIVE, pid, g_windowId, g_nwebId, false);
    }

    if (pid == tid && statusAdapter == ResSchedStatusAdapter::THREAD_DESTROYED) {
        std::lock_guard<std::mutex> lock(g_processInUseMutex);
        g_processInUse.erase(pid);
    }

    // Load url may create new render process, repeat report load url event when
    // render key thread create to solve timing problem. Later events will overwrite previous events
    if (statusAdapter == ResSchedStatusAdapter::THREAD_CREATED && pid != getprocpid()) {
        ReportSceneInternal(statusAdapter, ResSchedSceneAdapter::LOAD_URL);
    }
    return true;
}

bool ResSchedClientAdapter::ReportAudioData(ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid)
{
    static uint32_t resType = ResType::RES_TYPE_WEBVIEW_AUDIO_STATUS_CHANGE;

    int64_t status;
    if (!ConvertStatus(statusAdapter, status)) {
        return false;
    }

    uid_t uid = getuid();
    std::unordered_map<std::string, std::string> mapPayload { { UID, std::to_string(uid) },
        { PID, std::to_string(pid) }, { TID, std::to_string(tid) } };
    WVLOG_D("ReportAudioData status: %{public}d, uid: %{public}d, pid: %{public}d, tid:%{public}d",
        static_cast<int32_t>(status), uid, pid, tid);
    ResSchedClient::GetInstance().ReportData(resType, status, mapPayload);

    return true;
}

void ResSchedClientAdapter::ReportProcessInUse(pid_t pid)
{
    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    int32_t nwebId = g_nwebId;
    if (g_pidNwebMap.count(pid)) {
        nwebId = g_pidNwebMap[pid].begin()->first;
    }

    {
        std::lock_guard<std::mutex> lock(g_processInUseMutex);
        g_processInUse.insert(pid);
    }

    if (nwebId != INVALID_NUMBER) {
        g_nwebProcessMap[nwebId].push_back(pid);
        WVLOG_D("ReportProcessInUse nwebId: %{public}d, pid: %{public}d", nwebId, pid);

        if (g_siteIsolationMode) {
            ReportStatusData(ResSchedStatusAdapter::WEB_ACTIVE, pid, g_windowId, nwebId, true);
        }
    }
}

void ResSchedClientAdapter::ReportSiteIsolationMode(bool mode)
{
    g_siteIsolationMode = mode;
    WVLOG_D("ReportSiteIsolationMode g_siteIsolationMode: %{public}d", mode);
}

bool ResSchedClientAdapter::ReportWindowStatus(
    ResSchedStatusAdapter statusAdapter, pid_t pid, uint32_t windowId, int32_t nwebId)
{
    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    if (g_nwebSet.find(nwebId) == g_nwebSet.end() || pid == 0) {
        WVLOG_D("Don't report window status, nwebId: %{public}d, pid: %{public}d", nwebId, pid);
        return false;
    }

    int64_t status;
    if (!ConvertStatus(statusAdapter, status)) {
        return false;
    }
    if (statusAdapter == ResSchedStatusAdapter::WEB_ACTIVE) {
        ReportWindowId(windowId, nwebId);
    }

    ReportStatusData(statusAdapter, pid, windowId, nwebId, true);

    ProductDeviceType deviceType = SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType();
    if (deviceType == ProductDeviceType::DEVICE_TYPE_TABLET || deviceType == ProductDeviceType::DEVICE_TYPE_2IN1) {
        for (auto pidInNweb : g_nwebProcessMap[nwebId]) {
            if (pidInNweb == pid) {
                continue;
            }
            ReportStatusData(statusAdapter, pidInNweb, windowId, nwebId, true);
        }
    }

    return true;
}

bool ResSchedClientAdapter::ReportScene(
    ResSchedStatusAdapter statusAdapter, ResSchedSceneAdapter sceneAdapter, int32_t nwebId)
{
    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    if (nwebId == -1) {
        return ReportSceneInternal(statusAdapter, sceneAdapter);
    }

    if (g_nwebSet.find(nwebId) == g_nwebSet.end()) {
        WVLOG_E("ReportScene nwebId %{public}d not exist in render set", nwebId);
        return false;
    }
    return ReportSceneInternal(statusAdapter, sceneAdapter);
}

void ResSchedClientAdapter::ReportWindowId(int32_t windowId, int32_t nwebId)
{
    std::lock_guard<std::mutex> lock(g_windowIdMutex);
    g_windowId = windowId;
    g_nwebId = nwebId;
    WVLOG_D("ReportWindowId windowId: %{public}d, nwebId: %{public}d", g_windowId, g_nwebId);
}

void ResSchedClientAdapter::ReportNWebInit(ResSchedStatusAdapter statusAdapter, int32_t nwebId)
{
    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    if (statusAdapter == ResSchedStatusAdapter::WEB_SCENE_ENTER) {
        WVLOG_D("ReportNWebInit %{public}d", nwebId);
        g_nwebSet.emplace(nwebId);
    } else if (statusAdapter == ResSchedStatusAdapter::WEB_SCENE_EXIT) {
        g_nwebSet.erase(nwebId);
        g_nwebProcessMap.erase(nwebId);
        for (auto nwebMap : g_pidNwebMap) {
            auto it = nwebMap.second.find(nwebId);
            if (it != nwebMap.second.end()) {
                nwebMap.second.erase(it);
            }
        }
    }
}

void ResSchedClientAdapter::ReportRenderProcessStatus(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    int64_t status;
    bool ret = ConvertStatus(statusAdapter, status);
    if (!ret) {
        WVLOG_E("ReportRenderProcessStatus get appMgrClient failed");
        return;
    }

    if (pid == g_lastRenderPid && status == g_lastRenderStatus) {
        WVLOG_D("ReportRenderProcessStatus same status and process");
        return;
    }
    g_lastRenderPid = pid;
    g_lastRenderStatus = status;

    auto appMgrClient = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient == nullptr) {
        WVLOG_E("ReportRenderProcessStatus get appMgrClient failed");
        return;
    }
    appMgrClient->UpdateRenderState(pid, status);
    WVLOG_D("ReportRenderProcessStatus status: %{public}d, pid: %{public}d", static_cast<int32_t>(status), pid);
}

bool ResSchedClientAdapter::ReportScreenCapture(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    static uint32_t resType = ResType::RES_TYPE_WEBVIEW_SCREEN_CAPTURE;

    int64_t status;
    if (!ConvertStatus(statusAdapter, status)) {
        return false;
    }

    uid_t uid = getuid();
    std::unordered_map<std::string, std::string> mapPayload { { UID, std::to_string(uid) },
        { PID, std::to_string(pid) } };
    WVLOG_D("ReportScreenCapture status: %{public}d, uid: %{public}d, pid: %{public}d",
        static_cast<int32_t>(status), uid, pid);
    ResSchedClient::GetInstance().ReportData(resType, status, mapPayload);

    return true;
}

bool ResSchedClientAdapter::ReportVideoPlaying(ResSchedStatusAdapter statusAdapter, pid_t pid)
{
    static uint32_t resType = ResType::RES_TYPE_WEBVIEW_VIDEO_STATUS_CHANGE;

    int64_t status;
    if (!ConvertStatus(statusAdapter, status)) {
        return false;
    }

    uid_t uid = getuid();
    std::unordered_map<std::string, std::string> mapPayload { { UID, std::to_string(uid) },
        { PID, std::to_string(pid) } };
    WVLOG_D("ReportVideoPlaying status: %{public}d, uid: %{public}d, pid: %{public}d",
        static_cast<int32_t>(status), uid, pid);
    ResSchedClient::GetInstance().ReportData(resType, status, mapPayload);

    return true;
}

bool ResSchedClientAdapter::ReportSubwindowCall(ResSchedStatusAdapter statusAdapter, pid_t pid, pid_t tid)
{
    static uint32_t resType = ResType::RES_TYPE_WEB_SUBWIN_TASK;

    int64_t status;
    if (!ConvertStatus(statusAdapter, status)) {
        return false;
    }

    uid_t uid = getuid();
    std::unordered_map<std::string, std::string> mapPayload { { UID, std::to_string(uid) },
        { PID, std::to_string(pid) }, { TID, std::to_string(tid) } };
    WVLOG_D("ReportSubwindowCall status: %{public}d, uid: %{public}d, pid: %{public}d, tid:%{public}d",
        static_cast<int32_t>(status), uid, pid, tid);
    ResSchedClient::GetInstance().ReportData(resType, status, mapPayload);

    return true;
}
} // namespace OHOS::NWeb

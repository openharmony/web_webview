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

#include "app_fwk_update_service.h"

#include "appspawn.h"
#include "bundle_mgr_client.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "event_handler.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "nweb_log.h"
#include "parameter.h"
#include "parameters.h"
#include "system_ability_definition.h"
#include "sysversion.h"

namespace OHOS::NWeb {

namespace {
const std::string ARK_WEB_BUNDLE_NAME = "com.ohos.nweb";
const int RETRY_COUNT = 2;
const int FOUNDATION_UID = 5523;
REGISTER_SYSTEM_ABILITY_BY_ID(AppFwkUpdateService, SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, false);

constexpr int32_t TASK_DELAY_TIME = 60000; // 1min = 1*60*1000
const std::string TASK_ID = "unload";
const std::string PERSIST_ARKWEBCORE_PACKAGE_NAME = "persist.arkwebcore.package_name";
const std::set<std::string> ARK_WEB_DEFAULT_BUNDLE_NAME_SET = { "com.ohos.nweb", "com.ohos.arkwebcore" };
const std::string NWEB_HAP_PATH_MODULE_UPDATE = "/module_update/ArkWebCore/app/com.ohos.nweb/NWeb.hap";
} // namespace

PackageChangedReceiver::PackageChangedReceiver(
    const EventFwk::CommonEventSubscribeInfo& subscribeInfo, const PackageCommonEventCallback& callback)
    : EventFwk::CommonEventSubscriber(subscribeInfo), callback_(callback)
{}

void PackageChangedReceiver::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    std::string action = data.GetWant().GetAction();
    if (action.empty()) {
        WVLOG_I("action is empty");
        return;
    }

    std::string bundleName = data.GetWant().GetBundle();
    if (!ARK_WEB_DEFAULT_BUNDLE_NAME_SET.count(bundleName)) {
        WVLOG_I("Bundle name is not nweb.");
        return;
    }

    WVLOG_I("packagechangeReceiver OnReceiveEvent, ret = %{public}s.", action.c_str());
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED) {
        std::string hapPath;
        OHOS::AppExecFwk::BundleInfo bundleInfo;
        OHOS::AppExecFwk::BundleMgrClient client;

        bool result = client.GetBundleInfo(bundleName, OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo,
            AppExecFwk::Constants::ALL_USERID);
        if (result) {
            if (bundleInfo.hapModuleInfos.size() > 0) {
                hapPath = bundleInfo.hapModuleInfos[0].hapPath;
                WVLOG_I("PackagechangeReceiver hapPath is %{public}s.", hapPath.c_str());
            }
            callback_.OnPackageChangedEvent(bundleName, hapPath);
        } else {
            WVLOG_I("Failed to get bundleInfo.");
        }
    }
}

AppFwkUpdateService::AppFwkUpdateService(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate) {}

AppFwkUpdateService::~AppFwkUpdateService() {}

ErrCode AppFwkUpdateService::NotifyFWKAfterBmsStart()
{
    const std::string bundleName = OHOS::system::GetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "");
    if (bundleName.empty()) {
        WVLOG_E("NotifyFWKAfterBmsStart bundleName is empty");
        return ERR_INVALID_VALUE;
    }
    WVLOG_I("NotifyFWKAfterBmsStart bundleName: %{public}s", bundleName.c_str());
    int ret = SendAppSpawnMessage(bundleName, MSG_LOAD_WEBLIB_IN_APPSPAWN);
    if (ret != 0) {
        return ERR_INVALID_VALUE;
    }
    ret = OHOS::system::SetParameter("web.engine.install.completed", "true");
    if (ret != 0) {
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

ErrCode AppFwkUpdateService::NotifyArkWebInstallSuccess()
{
    int preloadMode = OHOS::system::GetIntParameter("const.startup.nwebspawn.preloadMode", 0);
    bool bootCompleted = OHOS::system::GetBoolParameter("bootevent.boot.completed", false);
    WVLOG_I("NwebSpawn preload render lib mode: %{public}d, boot completed: %{public}d", preloadMode, bootCompleted);
    if (preloadMode && bootCompleted) {
        auto ret = OHOS::system::SetParameter("web.engine.install.completed", "true");
        if (!ret) {
            WVLOG_E("Set parameter web.engine.install.completed failed");
            return ERR_INVALID_VALUE;
        }
    }
    return ERR_OK;
}

ErrCode AppFwkUpdateService::VerifyPackageInstall(
    const std::string& bundleName, const std::string& hapPath, int32_t& isSuccess)
{
    if (IPCSkeleton::GetCallingUid() != FOUNDATION_UID) {
        return ERR_INVALID_VALUE;
    }
    int ret = 0;
    ret = SendAppSpawnMessage(bundleName, MSG_UNLOAD_WEBLIB_IN_APPSPAWN);
    if (ret != 0) {
        WVLOG_I("SendAppSpawnMessage MSG_UNLOAD_WEBLIB_IN_APPSPAWN happened error: %{public}d", isSuccess);
        return ERR_INVALID_VALUE;
    }

    isSuccess = 0;
    if (OHOS::system::GetParameter("persist.arkwebcore.install_path", "") == hapPath) {
        WVLOG_I("OnPackageChangedEvent install path not changed.");
        return ERR_OK;
    }

    ret = SetWebInstallPath(hapPath);
    if (ret != 1) {
        isSuccess = -1;
        WVLOG_I("SetWebInstallPath happend error: %{public}d", isSuccess);
        return ERR_INVALID_VALUE;
    }

    ret = SetWebCorePackageName(bundleName);
    if (ret != 1) {
        isSuccess = -1;
        WVLOG_I("SetWebInstallPath happend error: %{public}d", isSuccess);
        return ERR_INVALID_VALUE;
    }

    ret = SendAppSpawnMessage(bundleName, MSG_UPDATE_MOUNT_POINTS);
    if (ret != 0) {
        isSuccess = -1;
        WVLOG_I("SendAppSpawnMessage happend error: %{public}d", isSuccess);
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}

void AppFwkUpdateService::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    WVLOG_I("Servie on start.");
    if (registerToService_) {
        WVLOG_I("App fwk update service is running.");
    }
    if (!Init(startReason)) {
        return;
    }
}

bool AppFwkUpdateService::Init(const SystemAbilityOnDemandReason& startReason)
{
    std::string reasonName = startReason.GetName();
    std::string reasonValue = startReason.GetValue();
    WVLOG_I("AppFwkUpdateService reasonName: %{public}s", reasonName.c_str());
    PostDelayUnloadTask();
    bool ret = Publish(this);
    if (!ret) {
        WVLOG_I("Service publish failed.");
        return false;
    }
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);

    std::string bundleName = "";
    if (startReason.HasExtraData()) {
        auto reasonMap = startReason.GetExtraData().GetWant();
        for (auto i = reasonMap.begin(); i != reasonMap.end(); ++i) {
            std::string key = std::string(i->first.data());
            std::string value = std::string(i->second.data());
            if (key == "bundleName") {
                bundleName = value;
                WVLOG_I("AppFwkUpdateService bundleName: %{public}s", bundleName.c_str());
                break;
            }
        }
    }
    if (!ARK_WEB_DEFAULT_BUNDLE_NAME_SET.count(bundleName)) {
        WVLOG_I("Bundle name is not nweb.");
        return false;
    }
    registerToService_ = true;
    WVLOG_I("Service init success.");
    return true;
}

int AppFwkUpdateService::SendAppSpawnMessage(const std::string& bundleName, AppSpawnMsgType msgType)
{
    WVLOG_I("Send appspawn message start,uid = %{public}d.", getuid());
    int ret = 0;
    int retryCount = 0;
    AppSpawnClientHandle clientHandle = nullptr;
    AppSpawnReqMsgHandle reqHandle = 0;
    AppSpawnResult result = {};
    do {
        ret = AppSpawnClientInit(APPSPAWN_SERVER_NAME, &clientHandle);
        if (ret != 0) {
            WVLOG_I("Failed to create reqMgr,retry count = %{public}d.", retryCount);
            continue;
        }
        ret = AppSpawnReqMsgCreate(msgType, bundleName.c_str(), &reqHandle);
        if (ret != 0) {
            WVLOG_I("Failed to create req,retry count = %{public}d.", retryCount);
            continue;
        }
        ret = AppSpawnClientSendMsg(clientHandle, reqHandle, &result);
    } while (++retryCount < RETRY_COUNT && ret != 0);
    AppSpawnClientDestroy(clientHandle);
    if (result.result != 0) {
        WVLOG_E("recv apsppawn result msg, result: %{public}d", result.result);
        return result.result;
    }
    WVLOG_I("Send appspawn message success.");
    return ret;
}

int AppFwkUpdateService::SendNWebSpawnMesage(const std::string& bundleName)
{
    WVLOG_I("Send nweb spawn messagestart,uid = %{public}d.", getuid());
    int ret = 0;
    int retryCount = 0;
    AppSpawnClientHandle clientHandle = nullptr;
    AppSpawnReqMsgHandle reqHandle = 0;
    do {
        ret = AppSpawnClientInit(NWEBSPAWN_SERVER_NAME, &clientHandle);
        if (ret != 0) {
            WVLOG_I("Failed to create reqMgr,retry count = %{public}d.", retryCount);
            continue;
        }
        ret = AppSpawnReqMsgCreate(MSG_RESTART_SPAWNER, bundleName.c_str(), &reqHandle);
        if (ret != 0) {
            WVLOG_I("Failed to create req,retry count = %{public}d.", retryCount);
            continue;
        }
        AppSpawnResult result = {};
        ret = AppSpawnClientSendMsg(clientHandle, reqHandle, &result);
    } while (++retryCount < RETRY_COUNT && ret != 0);
    AppSpawnClientDestroy(clientHandle);
    WVLOG_I("SendNWebSpawnMesage  res = %{public}d.", ret);
    return ret;
}

int AppFwkUpdateService::SetWebInstallPath(const std::string& path)
{
    int res = OHOS::system::SetParameter("persist.arkwebcore.install_path", path);
    WVLOG_I("SetWebInstallPath  res = %{public}d.", res);
    return res;
}
int AppFwkUpdateService::SetWebCorePackageName(const std::string& packageName)
{
    int res = OHOS::system::SetParameter("persist.arkwebcore.package_name", packageName);
    WVLOG_I("SetWebCorePackageName  res = %{public}d.", res);
    return res;
}

void AppFwkUpdateService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    WVLOG_I("OnAddSystemAbility systemAbilityId:%{public}d added!", systemAbilityId);
    if (systemAbilityId == COMMON_EVENT_SERVICE_ID) {
        SubscribePackageChangedEvent();
        return;
    }
}

void AppFwkUpdateService::SubscribePackageChangedEvent()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    PackageCommonEventCallback callback = { std::bind(
        &AppFwkUpdateService::OnPackageChangedEvent, this, std::placeholders::_1, std::placeholders::_2) };
    pkgSubscriber_ = std::make_shared<PackageChangedReceiver>(subscribeInfo, callback);
    if (pkgSubscriber_ == nullptr) {
        WVLOG_I("Create package changed subscriber failed");
        return;
    }
    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(pkgSubscriber_)) {
        WVLOG_I("Subscribe package changed event fail");
    }
}

void AppFwkUpdateService::OnPackageChangedEvent(const std::string& bunldeName, const std::string& hapPath)
{
    SendNWebSpawnMesage(bunldeName);
}
void AppFwkUpdateService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        WVLOG_I("systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }
    if (pkgSubscriber_ == nullptr) {
        WVLOG_I("OnRemoveSystemAbility subscribeer is nullptr");
        return;
    }
    bool result = OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(pkgSubscriber_);
    WVLOG_I("UnSubscribeCommonEvent subscriber result = %{public}d", result);
}

void AppFwkUpdateService::OnStop()
{
    WVLOG_I("Ready to stop service.");
    registerToService_ = false;
}

void AppFwkUpdateService::PostDelayUnloadTask()
{
    if (runner_ == nullptr) {
        runner_ = AppExecFwk::EventRunner::Create("unload");
    }
    if (unloadHandler_ == nullptr) {
        unloadHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    }
    if (unloadHandler_ == nullptr) {
        WVLOG_I("PostDelayUnloadTask invoke failed return.");
        return;
    }

    auto task = [this]() {
        auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgrProxy == nullptr) {
            WVLOG_I("Get samgrProxy failed.");
            return;
        }
        int32_t ret = samgrProxy->UnloadSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID);
        if (ret != 0) {
            WVLOG_I("Unload sa failed.");
            return;
        }
        WVLOG_I("Do unload task done.");
    };
    unloadHandler_->RemoveTask(TASK_ID);
    unloadHandler_->PostTask(task, TASK_ID, TASK_DELAY_TIME);
}
} // namespace OHOS::NWeb

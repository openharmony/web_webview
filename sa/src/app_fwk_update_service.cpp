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
#include "iremote_object.h"
#include "iservice_registry.h"
#include "nweb_log.h"
#include "parameter.h"
#include "parameters.h"
#include "system_ability_definition.h"
#include "sysversion.h"

namespace OHOS::NWeb {

const std::string PACKAGE_CHANGE_EVENT = "usual.event.PACKAGE_CHANGED";
const std::string ARK_WEB_DEFAULT_BUNDLE_NAME = "com.ohos.nweb";
const int RETRY_COUNT = 2;
REGISTER_SYSTEM_ABILITY_BY_ID(AppFwkUpdateService, SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, false);

constexpr int32_t TASK_DELAY_TIME = 60000; // 1min = 1*60*1000
const std::string TASK_ID = "unload";
PackageChangedReceiver::PackageChangedReceiver(
    const EventFwk::CommonEventSubscribeInfo& subscribeInfo, const PackageCommonEventCallback& callback)
    : EventFwk::CommonEventSubscriber(subscribeInfo), callback_(callback)
{}

void PackageChangedReceiver::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    WVLOG_I("PackageChangedReceiver invoked.");
    std::string action = data.GetWant().GetAction();
    if (action.empty()) {
        WVLOG_I("action is empty");
        return;
    }
    std::string bundleName = data.GetWant().GetBundle();
    std::string package_name =
        OHOS::system::GetParameter("persist.arkwebcore.package_name", ARK_WEB_DEFAULT_BUNDLE_NAME);
    if (bundleName != package_name) {
        WVLOG_I("Bundle name is not nweb.");
        return;
    }
    WVLOG_I("packagechangeReceiver OnReceiveEvent, ret = %{public}s.", action.c_str());
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED) {
        std::string hapPath;
        OHOS::AppExecFwk::BundleInfo bundleInfo;
        OHOS::AppExecFwk::BundleMgrClient client;
        WVLOG_I("packagechangeReceiver GetBundleInfo, ret = %{public}s.", bundleName.c_str());

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

ErrCode AppFwkUpdateService::RequestUpdateService(const std::string& bundleName)
{
    WVLOG_I("request update service invoked.");
    return ERR_OK;
}

void AppFwkUpdateService::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    WVLOG_I("Servie on start.");
    if (registerToService_) {
        WVLOG_I("App fwk update service is running.");
    }
    if (!Init(startReason)) {
        WVLOG_I("failed to init app_fwk_update_service.");
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
    std::string package_name =
        OHOS::system::GetParameter("persist.arkwebcore.package_name", ARK_WEB_DEFAULT_BUNDLE_NAME);
    if (bundleName != package_name) {
        WVLOG_I("Bundle name is not nweb.");
        return false;
    }
    if (reasonName == PACKAGE_CHANGE_EVENT) {
        OHOS::AppExecFwk::BundleInfo bundleInfo;
        std::string hapPath;
        OHOS::AppExecFwk::BundleMgrClient client;

        bool result = client.GetBundleInfo(bundleName, OHOS::AppExecFwk::BundleFlag::GET_BUNDLE_DEFAULT, bundleInfo,
            AppExecFwk::Constants::ALL_USERID);
        if (result) {
            if (bundleInfo.hapModuleInfos.size() > 0) {
                hapPath = bundleInfo.hapModuleInfos[0].hapPath;
                WVLOG_I("Hap path is %{public}s.", hapPath.c_str());
            }
        } else {
            WVLOG_W("Failed to get bundle info.");
        }
        OnPackageChangedEvent(bundleName, hapPath);
    }
    registerToService_ = true;
    WVLOG_I("Service init success.");
    return true;
}

void AppFwkUpdateService::SendAppSpawnMessage(const std::string& bundleName)
{
    WVLOG_I("Send appspawn message start,uid = %{public}d.", getuid());
    int ret = 0;
    int retryCount = 0;
    AppSpawnClientHandle clientHandle = nullptr;
    AppSpawnReqMsgHandle reqHandle = 0;
    do {
        ret = AppSpawnClientInit(APPSPAWN_SERVER_NAME, &clientHandle);
        if (ret != 0) {
            WVLOG_I("Failed to create reqMgr,retry count = %{public}d.", retryCount);
            continue;
        }
        ret = AppSpawnReqMsgCreate(MSG_UPDATE_MOUNT_POINTS, bundleName.c_str(), &reqHandle);
        if (ret != 0) {
            WVLOG_I("Failed to create req,retry count = %{public}d.", retryCount);
            continue;
        }
        AppSpawnResult result = {};
        ret = AppSpawnClientSendMsg(clientHandle, reqHandle, &result);
    } while (++retryCount < RETRY_COUNT && ret != 0);
    AppSpawnClientDestroy(clientHandle);
    WVLOG_I("Send appspawn message success.");
}

void AppFwkUpdateService::SendNWebSpawnMesage(const std::string& bundleName)
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
    WVLOG_I("Send nweb spawn message success.");
}

void AppFwkUpdateService::SetWebInstallPath(const std::string& path)
{
    OHOS::system::SetParameter("persist.arkwebcore.install_path", path);
    return;
}
void AppFwkUpdateService::SetWebCorePackageName(const std::string& packageName)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", packageName);
    return;
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
    if (OHOS::system::GetParameter("persist.arkwebcore.install_path", "") == hapPath) {
        WVLOG_I("OnPackageChangedEvent install path not changed.");
        return;
    }
    SetWebInstallPath(hapPath);
    SetWebCorePackageName(bunldeName);
    SendNWebSpawnMesage(bunldeName);
    SendAppSpawnMessage(bunldeName);
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

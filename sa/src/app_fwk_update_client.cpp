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

#include "app_fwk_update_client.h"

#include <fcntl.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "app_fwk_update_load_callback.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "isystem_ability_load_callback.h"
#include "nweb_log.h"
#include "system_ability_definition.h"

#include "base/web/webview/sa/app_fwk_update_service_proxy.h"
#include "base/web/webview/sa/iapp_fwk_update_service.h"

namespace OHOS::NWeb {
namespace {
const int LOAD_SA_TIMEOUT_MS = 4 * 1000;
const int FOUNDATION_UID = 5523;
const std::set<std::string> ARK_WEB_DEFAULT_BUNDLE_NAME_SET = { "com.ohos.nweb", "com.ohos.arkwebcore" };
} // namespace

AppFwkUpdateClient::AppFwkUpdateClient()
{
    appFwkUpdateDiedRecipient_ = new (std::nothrow) AppFwkUpdateDiedRecipient();
    if (appFwkUpdateDiedRecipient_ == nullptr) {
        WVLOG_I("create fwk update service died recipient failed");
    }
}

AppFwkUpdateClient& AppFwkUpdateClient::GetInstance()
{
    static AppFwkUpdateClient singleAppFwkUpdateClient;
    return singleAppFwkUpdateClient;
}
void AppFwkUpdateClient::SetFwkUpdate(const sptr<IRemoteObject>& remoteObject)
{
    std::lock_guard<std::mutex> lock(mutex_);
    fwkUpdateProxy_ = iface_cast<IAppFwkUpdateService>(remoteObject);
    if (fwkUpdateProxy_) {
        WVLOG_I("SetFwkUpdate is not null");
    }
}

sptr<IAppFwkUpdateService> AppFwkUpdateClient::GetFwkUpdate()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return fwkUpdateProxy_;
}

sptr<IAppFwkUpdateService> AppFwkUpdateClient::GetFwkUpdateProxy()
{
    auto fwkUpdateProxy = GetFwkUpdate();
    if (fwkUpdateProxy != nullptr) {
        return fwkUpdateProxy;
    }
    auto sam = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        WVLOG_E("load fwk service sam is null");
        return {};
    }

    auto remoteObj = sam->CheckSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID);
    if (remoteObj != nullptr) {
        fwkUpdateProxy = iface_cast<IAppFwkUpdateService>(remoteObj);
        return fwkUpdateProxy;
    }
    if (!LoadFwkService()) {
        WVLOG_I("get fwk update service is null");
        return nullptr;
    }
    fwkUpdateProxy = GetFwkUpdate();
    if (fwkUpdateProxy == nullptr) {
        WVLOG_I("get fwk update service proxy is null");
        return nullptr;
    }
    WVLOG_I("load fwk service sa finished");
    return fwkUpdateProxy;
}

void AppFwkUpdateClient::NotifyFWKAfterBmsStart()
{
    WVLOG_I("NotifyFWKAfterBmsStart received message");
    auto proxy = GetFwkUpdateProxy();
    if (proxy == nullptr) {
        WVLOG_E("NotifyFWKAfterBmsStart failed, proxy is null");
        return;
    }
    proxy->NotifyFWKAfterBmsStart();
}

int AppFwkUpdateClient::VerifyPackageInstall(const std::string& bundleName, const std::string& hapPath)
{
    WVLOG_I("verify package install callingUid: %{public}d", IPCSkeleton::GetCallingUid());
    if (IPCSkeleton::GetCallingUid() != FOUNDATION_UID) {
        return -1;
    }
    if (!ARK_WEB_DEFAULT_BUNDLE_NAME_SET.count(bundleName)) {
        WVLOG_I("bundle name is not nweb.");
        return 0;
    }

    auto proxy = GetFwkUpdateProxy();
    if (proxy == nullptr) {
        WVLOG_E("verify package install failed, proxy is null");
        return -1;
    }
    int32_t isSuccess = -1;
    proxy->VerifyPackageInstall(bundleName, hapPath, isSuccess);
    WVLOG_I("verify package install result: %{public}d", isSuccess);
    return isSuccess;
}

bool AppFwkUpdateClient::LoadFwkService()
{
    {
        std::unique_lock<std::mutex> lock(loadSaMutex_);
        loadSaFinished_ = false;
    }
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        WVLOG_I("failed to get system ability manager");
        return false;
    }
    sptr<AppFwkUpdateLoadCallback> loadCallback = new (std::nothrow) AppFwkUpdateLoadCallback();
    if (loadCallback == nullptr) {
        WVLOG_I("failed to create load callback");
        return false;
    }
    auto ret = systemAbilityMgr->LoadSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, loadCallback);
    if (ret != 0) {
        WVLOG_W("load fwk update service failed.");
        return false;
    }
    {
        std::unique_lock<std::mutex> lock(loadSaMutex_);
        auto waitStatus = loadSaCondition_.wait_for(
            lock, std::chrono::milliseconds(LOAD_SA_TIMEOUT_MS), [this]() { return loadSaFinished_; });
        if (!waitStatus) {
            auto remoteObj = systemAbilityMgr->CheckSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID);
            if (remoteObj != nullptr) {
                SetFwkUpdate(remoteObj);
                return true;
            }
            WVLOG_I("load fwk service timeout.");
            return false;
        }
        WVLOG_I("load fwk service success.");
        return true;
    }
}

void AppFwkUpdateClient::OnLoadSystemAbilitySuccess(const sptr<IRemoteObject>& remoteObject)
{
    WVLOG_I("on load systemAbility success");
    if (appFwkUpdateDiedRecipient_ == nullptr) {
        WVLOG_E("register fwk update service died recipient failed");
        return;
    }
    if (!remoteObject->AddDeathRecipient(appFwkUpdateDiedRecipient_)) {
        WVLOG_E("add fwk update service died recipient failed");
        return;
    }
    SetFwkUpdate(remoteObject);
    std::unique_lock<std::mutex> lock(loadSaMutex_);
    loadSaFinished_ = true;
    loadSaCondition_.notify_one();
}

void AppFwkUpdateClient::OnLoadSystemAbilityFail()
{
    SetFwkUpdate(nullptr);
    std::unique_lock<std::mutex> lock(loadSaMutex_);
    loadSaFinished_ = true;
    loadSaCondition_.notify_one();
}

void AppFwkUpdateClient::AppFwkUpdateDiedRecipient::OnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    if (remoteObject == nullptr) {
        WVLOG_E("remote object of fwk update service died recipient is nullptr");
        return;
    }
    AppFwkUpdateClient::GetInstance().AppFwkUpdateOnRemoteDied(remoteObject);
}

void AppFwkUpdateClient::AppFwkUpdateOnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    WVLOG_I("remote object of fwk update service died recipient is died");
    auto fwkUpdateProxy = GetFwkUpdate();
    if (fwkUpdateProxy == nullptr) {
        WVLOG_E("app fwk update proxy is nullptr");
        return;
    }
    sptr<IRemoteObject> remotePromote = remoteObject.promote();
    if (remotePromote == nullptr) {
        WVLOG_E("remote object of fwk update service promote fail");
        return;
    }
    if (fwkUpdateProxy->AsObject() != remotePromote) {
        WVLOG_E("app fwk update died recipient not find remote object");
        return;
    }
    remotePromote->RemoveDeathRecipient(appFwkUpdateDiedRecipient_);
    SetFwkUpdate(nullptr);
}
} // namespace OHOS::NWeb

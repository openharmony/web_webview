/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "web_native_messaging_manager.h"

#include "web_native_messaging_log.h"
#include "i_web_extension_connection_callback.h"
#include "iservice_registry.h"
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"
#include "connect_native_request.h"
#include <fcntl.h>
#include "system_ability.h"
#include "web_native_messaging_common.h"
#include "system_ability_definition.h"
#include "ability_manager_client.h"
#include "bundle_info.h"
#include "if_system_ability_manager.h"
#include "bundle_mgr_client.h"
#include "bundlemgr/bundle_mgr_interface.h"

namespace OHOS::NWeb {
namespace {
const std::string PERMISSION_WEB_NATIVE_MESSAGING = "ohos.permission.WEB_NATIVE_MESSAGING";
const std::string WANT_EXTENSION_ORIGIN_PARAM_KEY = "ohos.arkweb.extensionOrigin";
const std::string WANT_READ_PIPE_PARAM_KEY = "ohos.arkweb.messageReadPipe";
const std::string WANT_WRITE_PIPE_PARAM_KEY = "ohos.arkweb.messageWritePipe";
static int32_t g_ConnectionIdStart = -1;
static const int32_t MAX_REQUEST_SIZE = 10000;
constexpr int32_t UID_TRANSFORM_DIVISOR = 200000;
constexpr int32_t RANDOM_CONNECT_ID_BASE_RANGE = 1000000;
} // namespace

static uint32_t GetRandomUint32FromUrandom(void)
{
    uint32_t random;
    int32_t fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return 0;
    }
    ssize_t len = read(fd, &random, sizeof(random));
    close(fd);
    if (len != sizeof(random)) {
        return 0;
    }
    return random % RANDOM_CONNECT_ID_BASE_RANGE;
}

static int32_t CreateUniqueConnectionID()
{
    if (g_ConnectionIdStart == -1) {
        g_ConnectionIdStart = GetRandomUint32FromUrandom();
    } else {
        if (g_ConnectionIdStart < INT32_MAX) {
            g_ConnectionIdStart++;
        } else {
            g_ConnectionIdStart = 0;
        }
    }
    return g_ConnectionIdStart;
}

void WebNativeMessagingManager::DeleteIpcConnect(Security::AccessToken::AccessTokenID id, std::string& bundleName)
{
    std::lock_guard<std::mutex> lock(AbilityConnectMutex_);
    auto iter = AbilityConnectMap_.find(std::pair<Security::AccessToken::AccessTokenID, std::string>(id, bundleName));
    if (iter != AbilityConnectMap_.end()) {
        AbilityConnectMap_.erase(iter);
    }
}

bool WebNativeMessagingManager::IsIpcConnectExist()
{
    std::lock_guard<std::mutex> lock(AbilityConnectMutex_);
    return AbilityConnectMap_.size() > 0;
}

void WebNativeMessagingManager::CleanAbilityConnection(Security::AccessToken::AccessTokenID tokenId,
    std::string& bundleName)
{
    DeleteIpcConnect(tokenId, bundleName);
    if (!IsIpcConnectExist() && delayExitTask_) {
        delayExitTask_->Start();
    }
}

bool WebNativeMessagingManager::GetPidExtensionBundleName(int32_t pid, std::string& bundleName)
{
    if (pid <= 0) {
        return false;
    }
    std::lock_guard<std::mutex> lock(AbilityConnectMutex_);
    auto iter = AbilityConnectMap_.begin();
    while (iter != AbilityConnectMap_.end()) {
        if (iter->second && iter->second->GetTargetExtensionPid() == pid) {
            bundleName = iter->second->GetTargetBundleName();
            return true;
        }
        iter++;
    }
    return false;
}

sptr<ExtensionIpcConnection> WebNativeMessagingManager::NewIpcConnectionUnlock(
    Security::AccessToken::AccessTokenID tokenId,
    std::string& bundleName, std::string& abilityName, sptr<IRemoteObject> token)
{
    sptr<ExtensionIpcConnection> ipcConnect =
        new (std::nothrow) ExtensionIpcConnection(tokenId, bundleName, abilityName, token, serviceHandler_);
    if (ipcConnect == nullptr) {
        WNMLOG_E("new ExtensionIpcConnection failed");
        return nullptr;
    }

    wptr<ExtensionIpcConnection> wpIpcConnect = ipcConnect;
    std::shared_ptr<IWebNativeMessagingManager> weakPtrManager = shared_from_this();
    ipcConnect->SetThisWptr(wpIpcConnect);
    ipcConnect->SetManagerWptr(weakPtrManager);

    AbilityConnectMap_.insert(std::pair<std::pair<Security::AccessToken::AccessTokenID, std::string>,
        sptr<ExtensionIpcConnection>>({tokenId, bundleName}, ipcConnect));
    return ipcConnect;
}

sptr<ExtensionIpcConnection> WebNativeMessagingManager::NewIpcConnection(
    Security::AccessToken::AccessTokenID tokenId,
    std::string& bundleName, std::string& abilityName, sptr<IRemoteObject> token)
{
    std::lock_guard<std::mutex> lock(AbilityConnectMutex_);
    return NewIpcConnectionUnlock(tokenId, bundleName, abilityName, token);
}

sptr<ExtensionIpcConnection> WebNativeMessagingManager::LookUpIpcConnectionUnlock(
    Security::AccessToken::AccessTokenID tokenId, std::string& bundleName)
{
    auto iter = AbilityConnectMap_.find(
        std::pair<Security::AccessToken::AccessTokenID, std::string>(tokenId, bundleName));
    if (iter != AbilityConnectMap_.end()) {
        return iter->second;
    }
    return nullptr;
}

sptr<ExtensionIpcConnection> WebNativeMessagingManager::LookUpIpcConnection(
    Security::AccessToken::AccessTokenID tokenId, std::string& bundleName)
{
    std::lock_guard<std::mutex> lock(AbilityConnectMutex_);
    return LookUpIpcConnectionUnlock(tokenId, bundleName);
}

sptr<ExtensionIpcConnection> WebNativeMessagingManager::LookUpOrNewIpcConnection(
    Security::AccessToken::AccessTokenID tokenId,
    std::string& bundleName, std::string& abilityName, sptr<IRemoteObject> token)
{
    std::lock_guard<std::mutex> lock(AbilityConnectMutex_);
    auto connect = LookUpIpcConnectionUnlock(tokenId, bundleName);
    if (connect) {
        return connect;
    }
    return NewIpcConnectionUnlock(tokenId, bundleName, abilityName, token);
}

static sptr<AppExecFwk::IBundleMgr> GetBundleMgr()
{
    auto saMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!saMgr) {
        return nullptr;
    }
    auto bundleMgrObj = saMgr->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!bundleMgrObj) {
        return nullptr;
    }
    return iface_cast<AppExecFwk::IBundleMgr>(bundleMgrObj);
}

static bool CheckAbilityIsUIAbility(const std::string& bundleName,
    const std::string& abilityName, int32_t userId)
{
    if (bundleName.empty() || abilityName.empty() || userId <= 0) {
        WNMLOG_E("check ability type: params is error");
        return false;
    }
    auto bundleMgr = GetBundleMgr();
    if (!bundleMgr) {
        WNMLOG_E("check ability type: bundle manager is null");
        return false;
    }
    AppExecFwk::BundleInfo bundleInfo;
    auto ret = bundleMgr->GetBundleInfo(bundleName, AppExecFwk::BundleFlag::GET_BUNDLE_WITH_ABILITIES,
        bundleInfo, userId);
    if (!ret) {
        WNMLOG_E("check ability type: GetBundleInfo fail, error: %{public}d", ret);
        return false;
    }
    for (auto ability : bundleInfo.abilityInfos) {
        if (ability.name == abilityName) {
            if (ability.type == AppExecFwk::AbilityType::PAGE) {
                return true;
            }
            WNMLOG_E("check ability type: ability is found, but type is not page");
            return false;
        }
    }

    WNMLOG_E("check ability type: ability is not found");
    return false;
}

int32_t WebNativeMessagingManager::GetAndCheckConnectParams(const sptr<IRemoteObject>& token,
    const sptr<IRemoteObject>& connectionCallback, int32_t connectionId,
    ConnectNativeParams& params)
{
    if (token == nullptr) {
        WNMLOG_E("Check token failed");
        return ConnectNativeRet::CONTEXT_ERROR;
    }
    params.token = token;
    if (connectionCallback == nullptr) {
        WNMLOG_E("Check connectionCallback failed");
        return ConnectNativeRet::CALLBACK_ERROR;
    }
    params.connectionCallback = connectionCallback;

    params.callerTokenId = IPCSkeleton::GetCallingTokenID();
    params.callerPid = IPCSkeleton::GetCallingPid();
    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(params.callerTokenId,
        PERMISSION_WEB_NATIVE_MESSAGING) != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        WNMLOG_E("Check permission %{public}s failed", PERMISSION_WEB_NATIVE_MESSAGING.c_str());
        return ConnectNativeRet::PERMISSION_CHECK_ERROR;
    }

    Security::AccessToken::HapTokenInfo hapInfo;
    if (Security::AccessToken::AccessTokenKit::GetHapTokenInfo(params.callerTokenId, hapInfo) != 0) {
        WNMLOG_E("get hap token info failed");
        return ConnectNativeRet::PERMISSION_CHECK_ERROR;
    }
    params.callerBundleName = hapInfo.bundleName;

    if (ConnectionNativeRequest::GetInnerConnectionIdFromMap(
        params.callerTokenId, params.callerPid, connectionId) != -1) {
        WNMLOG_E("connectId is exist");
        return ConnectNativeRet::CONNECTION_ID_EXIST;
    }
    params.callerConnectId = connectionId;

    if (ConnectionNativeRequest::GetAliveSize() > MAX_REQUEST_SIZE) {
        WNMLOG_E("connect count is too large");
        return ConnectNativeRet::REQUEST_SIZE_TOO_LARGE;
    }
    params.innerConnectId = CreateUniqueConnectionID();
    return ConnectNativeRet::SUCCESS;
}

std::shared_ptr<ConnectionNativeRequest> WebNativeMessagingManager::CreateNativeRequest(const AAFwk::Want& want,
    ConnectNativeParams& params, int32_t& errorNum)
{
    std::shared_ptr<ConnectionNativeRequest> request = std::make_shared<ConnectionNativeRequest>();
    if (request == nullptr) {
        WNMLOG_E("new connection native request failed");
        errorNum = ConnectNativeRet::MEMORY_ERROR;
        return nullptr;
    }

    if (request->FillRequestWithWant(want) != ConnectNativeRet::SUCCESS) {
        WNMLOG_E("check want format error");
        errorNum = ConnectNativeRet::WANT_FORMAT_ERROR;
        return nullptr;
    }

    request->SetInnerConnectionId(params.innerConnectId);
    request->SetCallerConnectionId(params.callerConnectId);
    request->SetCallerPid(params.callerPid);
    request->SetAppConnectCallback(params.connectionCallback);
    request->SetCallerTokenId(params.callerTokenId);
    request->SetCallerBundleName(params.callerBundleName);

    ConnectionNativeRequest::InsertRequestMap(request);
    return request;
}

void WebNativeMessagingManager::ConnectWebNativeMessagingExtension(const sptr<IRemoteObject>& token,
    const AAFwk::Want& want, const sptr<IRemoteObject>& connectionCallback,
    int32_t connectionId, int32_t& errorNum)
{
    ConnectNativeParams params;
    int ret = GetAndCheckConnectParams(token, connectionCallback, connectionId, params);
    if (ret != ConnectNativeRet::SUCCESS) {
        errorNum = ret;
        return;
    }

    std::shared_ptr<ConnectionNativeRequest> request = CreateNativeRequest(want, params, errorNum);
    if (request == nullptr) {
        return;
    }

    sptr<ExtensionIpcConnection> ipcConnect =
        LookUpOrNewIpcConnection(params.callerTokenId, request->GetTargetBundleName(),
        request->GetTargetAbilityName(), token);
    if (!ipcConnect) {
        WNMLOG_E("create ipc connect failed");
        errorNum = ConnectNativeRet::MEMORY_ERROR;
        return;
    }
    int32_t res = ipcConnect->ConnectNative(request);
    if (res == ConnectNativeRet::CONNECT_STATUS_ERROR) {
        // retry to connect it.
        DeleteIpcConnect(params.callerTokenId, request->GetTargetBundleName());
        ipcConnect = NewIpcConnection(params.callerTokenId, request->GetTargetBundleName(),
            request->GetTargetAbilityName(), token);
        if (ipcConnect) {
            WNMLOG_E("new ipc connection failed");
            errorNum = ConnectNativeRet::MEMORY_ERROR;
            return;
        }
        if (ipcConnect->ConnectNative(request) != ConnectNativeRet::SUCCESS) {
            WNMLOG_E("retry connect native failed");
            if (ipcConnect->IsRequestListEmpty()) {
                CleanAbilityConnection(params.callerTokenId, ipcConnect->GetTargetBundleName());
            }
            errorNum = res;
            return;
        }
    } else if (res != ConnectNativeRet::SUCCESS) {
        WNMLOG_E("connect native failed");
        if (ipcConnect->IsRequestListEmpty()) {
            CleanAbilityConnection(params.callerTokenId, ipcConnect->GetTargetBundleName());
        }
        errorNum = res;
        return;
    }
    if (delayExitTask_) {
        delayExitTask_->Stop();
    }
    errorNum = ConnectNativeRet::SUCCESS;
}

void WebNativeMessagingManager::DisconnectWebNativeMessagingExtension(int32_t connectionId, int32_t& errorNum)
{
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetCallingTokenID();
    int32_t pid = IPCSkeleton::GetCallingPid();
    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, PERMISSION_WEB_NATIVE_MESSAGING) !=
        Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        WNMLOG_E("Check permission %{public}s failed", PERMISSION_WEB_NATIVE_MESSAGING.c_str());
        errorNum = ConnectNativeRet::PERMISSION_CHECK_ERROR;
        return;
    }
    int32_t innerId = ConnectionNativeRequest::GetInnerConnectionIdFromMap(tokenId, pid, connectionId);
    if (innerId == -1) {
        WNMLOG_E("Get inner id failed");
        errorNum = ConnectNativeRet::CONNECTION_NOT_EXIST;
        return;
    }
    auto request = ConnectionNativeRequest::GetExistConnectId(innerId);
    if (!request) {
        WNMLOG_E("connectionId is not exist");
        errorNum = ConnectNativeRet::CONNECTION_NOT_EXIST;
        return;
    }

    auto ipcConnect = LookUpIpcConnection(tokenId, request->GetTargetBundleName());
    if (!ipcConnect) {
        WNMLOG_E("connection is not connected");
        errorNum = ConnectNativeRet::CONNECTION_NOT_EXIST;
        return;
    }

    bool ipcConnectNeedDelete = false;
    int32_t res = ipcConnect->DisconnectNative(innerId, ipcConnectNeedDelete);
    if (ipcConnectNeedDelete) {
        DeleteIpcConnect(tokenId, request->GetTargetBundleName());
        if (!IsIpcConnectExist() && delayExitTask_) {
            delayExitTask_->Start();
        }
    }
    if (res != ConnectNativeRet::SUCCESS) {
        WNMLOG_E("disconnect native failed.");
        errorNum = res;
        return;
    }
    errorNum = ConnectNativeRet::SUCCESS;
}

void WebNativeMessagingManager::StartAbility(const sptr<IRemoteObject>& token,
    const AAFwk::Want& want, const AAFwk::StartOptions& startOptions, int32_t& errorNum)
{
    int32_t userId = IPCSkeleton::GetCallingUid() / UID_TRANSFORM_DIVISOR;
    int32_t pid = IPCSkeleton::GetCallingPid();
    std::string extensionBundleName;
    if (!GetPidExtensionBundleName(pid, extensionBundleName)) {
        WNMLOG_E("pid %{public}d is not native messaging extension.", pid);
        errorNum = ConnectNativeRet::PERMISSION_CHECK_ERROR;
        return;
    }

    if (extensionBundleName != want.GetBundle()) {
        WNMLOG_E("start ability is not extension app.");
        errorNum = ConnectNativeRet::PERMISSION_CHECK_ERROR;
        return;
    }

    if (!CheckAbilityIsUIAbility(want.GetBundle(), want.GetElement().GetAbilityName(), userId)) {
        WNMLOG_E("start ability is not UIAbility.");
        errorNum = ConnectNativeRet::PERMISSION_CHECK_ERROR;
        return;
    }

    auto client = AAFwk::AbilityManagerClient::GetInstance();
    if (client == nullptr) {
        WNMLOG_E("client is null.");
        errorNum = ConnectNativeRet::IPC_ERROR;
        return;
    }
    ErrCode err = client->StartAbility(want, startOptions, token, userId);
    if (err != ERR_OK) {
        WNMLOG_E("StartAbility failed: %{public}d.", err);
        errorNum = err;
        return;
    }
    errorNum = ConnectNativeRet::SUCCESS;
}

void WebNativeMessagingManager::StopNativeConnectionFromExtension(int32_t innerConnectId, int32_t& errorNum)
{
    int32_t pid = IPCSkeleton::GetCallingPid();
    auto request = ConnectionNativeRequest::GetExistConnectId(innerConnectId);
    if (!request) {
        WNMLOG_E("innerConnectId is not exist");
        errorNum = ConnectNativeRet::CONNECTION_NOT_EXIST;
        return;
    }

    if (pid != request->GetExtensionPid()) {
        WNMLOG_E("innerConnectId is not belong to this application");
        errorNum = ConnectNativeRet::CONNECTION_NOT_EXIST;
        return;
    }

    auto ipcConnect = LookUpIpcConnection(request->GetCallerTokenId(), request->GetTargetBundleName());
    if (!ipcConnect) {
        WNMLOG_E("connection is not connected");
        errorNum = ConnectNativeRet::CONNECTION_NOT_EXIST;
        return;
    }

    bool ipcConnectNeedDelete = false;
    int32_t res = ipcConnect->DisconnectNative(innerConnectId, ipcConnectNeedDelete);
    if (ipcConnectNeedDelete) {
        DeleteIpcConnect(request->GetCallerTokenId(), request->GetTargetBundleName());
        if (!IsIpcConnectExist() && delayExitTask_) {
            delayExitTask_->Start();
        }
    }
    if (res != ConnectNativeRet::SUCCESS) {
        WNMLOG_E("disconnect native failed.");
        errorNum = res;
        return;
    }
    errorNum = ConnectNativeRet::SUCCESS;
}

void WebNativeMessagingManager::ExitSaProcess()
{
    if (IsIpcConnectExist()) {
        WNMLOG_E("App using service still exist, not exit sa.");
        return;
    }
    WNMLOG_I("All processes using extension died, start sa exit.");
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        WNMLOG_E("Failed to get SystemAbilityManager.");
        return;
    }
    int32_t ret = systemAbilityMgr->UnloadSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID);
    if (ret != 0) {
        WNMLOG_E("Failed to UnloadSystemAbility errcode=%{public}d.", ret);
        return;
    }
    WNMLOG_I("UnloadSystemAbility successfully.");
}

int32_t WebNativeMessagingManager::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (fd < 0) {
        return ERR_INVALID_VALUE;
    }
    dprintf(fd, "WebNativeMessagingService Dump:\n");
    std::string arg0 = (args.size() == 0) ? "" : Str16ToStr8(args.at(0));
    if (arg0.compare("-h") == 0) {
        dprintf(fd, "Usage:\n");
        dprintf(fd, "      -h: command help\n");
        dprintf(fd, "      -d: default dump\n");
    } else if (arg0.compare("-d") == 0 || arg0.compare("") == 0) {
        {
            std::lock_guard<std::mutex> lock(AbilityConnectMutex_);
            dprintf(fd, "ExtensionAbilityConnection:\n");
            for (auto iter = AbilityConnectMap_.begin(); iter != AbilityConnectMap_.end(); iter++) {
                auto info = iter->second;
                if (info) {
                    info->DumpMesg(fd);
                }
            }
        }
        dprintf(fd, "AllNativeRequest:\n");
        ConnectionNativeRequest::DumpAllRequest(fd);
        dprintf(fd, "IdMap:\n");
        ConnectionNativeRequest::DumpInnerIdMap(fd);
    }
    return ERR_OK;
}

bool WebNativeMessagingManager::Init()
{
    runner_ = AppExecFwk::EventRunner::Create(true, AppExecFwk::ThreadMode::FFRT);
    if (!runner_) {
        WNMLOG_E("failed to create a runner");
        return false;
    }
    serviceHandler_ = std::make_shared<ServiceEventHandler>(runner_);
    if (!serviceHandler_) {
        WNMLOG_E("failed to create service handler.");
        return false;
    }
    delayExitTask_ = std::make_shared<ServiceDelayExitTask>(serviceHandler_, [this]() {
        this->ExitSaProcess();
    });
    if (!delayExitTask_) {
        WNMLOG_E("failed to create a delay exit task");
        return false;
    }
    delayExitTask_->Start();
    WNMLOG_I("WebNativeMessagingManager init success.");
    return true;
}

} // namespace OHOS::NWeb

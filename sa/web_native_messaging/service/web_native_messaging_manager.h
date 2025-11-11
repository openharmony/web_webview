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
#ifndef NWEB_WEB_NATIVE_MESSAGING_MANAGER_H
#define NWEB_WEB_NATIVE_MESSAGING_MANAGER_H

#include <string>
#include <map>
#include <mutex>

#include "connect_native_request.h"
#include "event_handler.h"
#include "extension_ipc_connection.h"
#include "i_web_native_messaging_manager.h"
#include "refbase.h"
#include "service_delay_exit_task.h"
#include "start_options.h"
#include "web_native_messaging_common.h"

namespace OHOS::NWeb {

using AbilityCallbackMapType =
    std::map<std::pair<Security::AccessToken::AccessTokenID, std::string>, sptr<ExtensionIpcConnection>>;

struct ConnectNativeParams {
    Security::AccessToken::AccessTokenID callerTokenId;
    int32_t callerPid;
    int32_t callerConnectId;
    int32_t innerConnectId;
    std::string callerBundleName;
    sptr<IRemoteObject> connectionCallback;
    sptr<IRemoteObject> token;
    int32_t callerUserId;
};

class WebNativeMessagingManager : public IWebNativeMessagingManager,
    public std::enable_shared_from_this<WebNativeMessagingManager> {
public:
    WebNativeMessagingManager() = default;
    virtual ~WebNativeMessagingManager() = default;

    void ConnectWebNativeMessagingExtension(const sptr<IRemoteObject>& token,
        const AAFwk::Want& want, const sptr<IRemoteObject>& connectionCallback,
        int32_t connectionId, int32_t& errorNum);
    void DisconnectWebNativeMessagingExtension(int32_t connectionId, int32_t& errorNum);
    void StartAbility(const sptr<IRemoteObject>& token,
        const AAFwk::Want& want, const AAFwk::StartOptions& startOptions, int32_t& errorNum);
    void StopNativeConnectionFromExtension(int32_t innerConnectId, int32_t& errorNum);

    void CleanAbilityConnection(Security::AccessToken::AccessTokenID tokenId, std::string& bundleName) override;

    void DeleteIpcConnect(Security::AccessToken::AccessTokenID id, std::string& bundleName);
    void ExitSaProcess();
    bool Init();

    int32_t Dump(int fd, const std::vector<std::u16string>& args);

private:
    bool IsIpcConnectExist();
    sptr<ExtensionIpcConnection> LookUpIpcConnectionUnlock(Security::AccessToken::AccessTokenID tokenId,
        std::string& bundleName);
    sptr<ExtensionIpcConnection> LookUpIpcConnection(Security::AccessToken::AccessTokenID tokenId,
        std::string& bundleName);
    sptr<ExtensionIpcConnection> NewIpcConnectionUnlock(Security::AccessToken::AccessTokenID tokenId,
        std::string& bundleName, std::string& abilityName, sptr<IRemoteObject> token, int32_t userId);
    sptr<ExtensionIpcConnection> NewIpcConnection(Security::AccessToken::AccessTokenID tokenId,
        std::string& bundleName, std::string& abilityName, sptr<IRemoteObject> token, int32_t userId);
    sptr<ExtensionIpcConnection> LookUpOrNewIpcConnection(Security::AccessToken::AccessTokenID tokenId,
        std::string& bundleName, std::string& abilityName, sptr<IRemoteObject> token, int32_t userId);

    bool GetPidExtensionBundleName(int32_t pid, std::string& bundleName);
    int32_t GetAndCheckConnectParams(const sptr<IRemoteObject>& token,
        const sptr<IRemoteObject>& connectionCallback, int32_t connectionId,
        ConnectNativeParams& param);
    std::shared_ptr<ConnectionNativeRequest> CreateNativeRequest(const AAFwk::Want& want,
        ConnectNativeParams& params, int32_t& errorNum);

    std::shared_ptr<ServiceEventHandler> serviceHandler_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<ServiceDelayExitTask> delayExitTask_;

    std::mutex AbilityConnectMutex_;
    AbilityCallbackMapType AbilityConnectMap_;
};
} // namespace OHOS::NWeb
#endif // NWEB_WEB_NATIVE_MESSAGING_MANAGER_H

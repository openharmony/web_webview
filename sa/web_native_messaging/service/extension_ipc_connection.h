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
#ifndef WEB_NWEB_EXTENSION_IPC_CONNECTION_H
#define WEB_NWEB_EXTENSION_IPC_CONNECTION_H

#include <mutex>
#include <string>

#include "ability_connection.h"
#include "accesstoken_kit.h"
#include "connect_native_request.h"
#include "element_name.h"
#include "i_web_native_messaging_manager.h"
#include "iremote_object.h"
#include "refbase.h"
#include "service_event_handler.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
class ExtensionIpcConnection : public AbilityRuntime::AbilityConnection {
public:
    ExtensionIpcConnection(Security::AccessToken::AccessTokenID callerTokenId,
        std::string targetBundleName, std::string targetAbilityName, sptr<IRemoteObject> token,
        std::shared_ptr<ServiceEventHandler> eventHandler) :
        callerTokenId_(callerTokenId), targetBundleName_(targetBundleName),
        targetAbilityName_(targetAbilityName), token_(token), eventHandler_(eventHandler) {}

    ~ExtensionIpcConnection()
    {
        WNMLOG_D("~ExtensionIpcConnection");
    }

    void OnAbilityConnectDone(
        const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override;

    int32_t ConnectNative(std::shared_ptr<ConnectionNativeRequest> info);
    int32_t DisconnectNative(int32_t connectionId, bool& resDeleted);

    void PostRequestOnConnect(std::shared_ptr<ConnectionNativeRequest> request);
    void PostRequestOnDisconnect(std::shared_ptr<ConnectionNativeRequest> request);
    void PostRequestOnFailed(std::shared_ptr<ConnectionNativeRequest> request, int32_t errorNum);

    void HandleCallerDeath();

    bool IsRequestListEmpty();

    void DumpMesg(int fd);

    void SetThisWptr(wptr<ExtensionIpcConnection> wp)
    {
        wpThis_ = wp;
    }

    void SetManagerWptr(std::shared_ptr<IWebNativeMessagingManager> wp)
    {
        wpManager_ = wp;
    }

    std::string& GetTargetBundleName()
    {
        return targetBundleName_;
    }

    std::string& GetTargetAbilityName()
    {
        return targetAbilityName_;
    }

    int32_t GetTargetExtensionPid()
    {
        return targetExtensionPid_;
    }

    Security::AccessToken::AccessTokenID GetCallerTokenId()
    {
        return callerTokenId_;
    }

private:
    int32_t DoConnectNative(std::shared_ptr<ConnectionNativeRequest> request);
    int32_t DoDisconnectNative(std::shared_ptr<ConnectionNativeRequest> request);

    std::shared_ptr<ConnectionNativeRequest> LookForAndRemoveRequest(int32_t connectionId);
    void RemovePendingRequestUnlock(std::shared_ptr<ConnectionNativeRequest> request);

    enum IpcConnectStatus : int32_t {
        INIT,
        CONNECTING,
        CONNECTED,
        DISCONNECTED,
    };

    Security::AccessToken::AccessTokenID callerTokenId_;
    std::string targetBundleName_;
    std::string targetAbilityName_;
    int32_t targetExtensionPid_;
    sptr<IRemoteObject> extensionIpcRemote_;
    sptr<IRemoteObject> token_;
    IpcConnectStatus status_ = IpcConnectStatus::INIT;

    std::mutex mutex_;
    std::vector<std::shared_ptr<ConnectionNativeRequest>> pendingRequests_;
    std::vector<std::shared_ptr<ConnectionNativeRequest>> connectedRequests_;

    wptr<ExtensionIpcConnection> wpThis_;
    std::weak_ptr<IWebNativeMessagingManager> wpManager_;
    std::shared_ptr<ServiceEventHandler> eventHandler_;
};
} // namespace OHOS::NWeb
#endif // WEB_NWEB_EXTENSION_IPC_CONNECTION_H

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
#include "extension_ipc_connection.h"

#include "ability_manager_client.h"
#include "connect_native_request.h"
#include "i_web_extension_connection_callback.h"
#include "iweb_native_messaging_extension.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
namespace {
static const std::string REQUEST_ON_CONNECT = "RequestOnConnect";
static const std::string REQUEST_ON_DISCONNECT = "RequestOnDisconnect";
static const std::string REQUEST_ON_FAILED = "RequestOnFailed";
}

void ExtensionIpcConnection::PostRequestOnConnect(std::shared_ptr<ConnectionNativeRequest> request)
{
    if (!eventHandler_) {
        WNMLOG_W("event handler is null");
        return;
    }
    eventHandler_->ProxyPostTask([requestTmp = request] () {
        if (requestTmp) {
            requestTmp->OnConnect();
        }
    }, REQUEST_ON_CONNECT, 0);
}

void ExtensionIpcConnection::PostRequestOnDisconnect(std::shared_ptr<ConnectionNativeRequest> request)
{
    if (!eventHandler_) {
        WNMLOG_W("event handler is null");
        return;
    }
    eventHandler_->ProxyPostTask([requestTmp = request] () {
        if (requestTmp) {
            requestTmp->OnDisconnect();
        }
    }, REQUEST_ON_DISCONNECT, 0);
}

void ExtensionIpcConnection::PostRequestOnFailed(std::shared_ptr<ConnectionNativeRequest> request, int32_t errorNum)
{
    if (!eventHandler_) {
        WNMLOG_W("event handler is null");
        return;
    }
    eventHandler_->ProxyPostTask([requestTmp = request, errorNum] () {
        if (requestTmp) {
            requestTmp->OnFailed(errorNum);
        }
    }, REQUEST_ON_FAILED, 0);
}

static void FillWNMEConnectionInfo(WNMEConnectionInfo& info, ConnectionNativeRequest& request)
{
    info.connectionId = request.GetInnerConnectionId();
    info.bundleName = request.GetCallerBundleName();
    info.extensionOrigin = request.GetCallerExtensionOrigin();
    info.fdRead = request.GetFdRead();
    info.fdWrite = request.GetFdWrite();
}

int32_t ExtensionIpcConnection::DoConnectNative(std::shared_ptr<ConnectionNativeRequest> request)
{
    if (!request) {
        WNMLOG_E("DoConnectNative request is null");
        return -1;
    }

    auto connectNativeExtension = iface_cast<IWebNativeMessagingExtension>(extensionIpcRemote_);
    if (!connectNativeExtension) {
        WNMLOG_E("DoConnectNative cast interface error");
        return -1;
    }

    WNMEConnectionInfo info;
    FillWNMEConnectionInfo(info, *request);

    // send request to extension side
    int32_t pid = connectNativeExtension->ConnectNative(info);
    if (pid < 0) {
        WNMLOG_E("call extension onConnectNative failed");
        return -1;
    }
    targetExtensionPid_ = pid;
    request->SetExtensionPid(targetExtensionPid_);

    // close fd in service
    request->ResetAllFileDescriptors();

    // notify caller callback
    PostRequestOnConnect(request);
    return 0;
}

int32_t ExtensionIpcConnection::DoDisconnectNative(std::shared_ptr<ConnectionNativeRequest> request)
{
    if (!request) {
        WNMLOG_E("DoDisconnectNative request is null");
        return -1;
    }
    auto connectNativeExtension = iface_cast<IWebNativeMessagingExtension>(extensionIpcRemote_);
    if (!connectNativeExtension) {
        WNMLOG_E("DoDisconnectNative cast interface error");
        return -1;
    }
    WNMEConnectionInfo info;
    FillWNMEConnectionInfo(info, *request);
    connectNativeExtension->DisconnectNative(info);
    PostRequestOnDisconnect(request);
    return 0;
}

void ExtensionIpcConnection::HandleCallerDeath()
{
    WNMLOG_I("handle caller death");
    std::vector<std::shared_ptr<ConnectionNativeRequest>> popDeleteList;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (status_ == IpcConnectStatus::CONNECTED) {
            popDeleteList.swap(connectedRequests_);
        }
        status_ = IpcConnectStatus::DISCONNECTED;
    }
    for (auto& request : popDeleteList) {
        DoDisconnectNative(request);
    }

    auto sp = wpThis_.promote();
    if (sp) {
        ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(sp);
        if (err != ERR_OK) {
            WNMLOG_E("call ability disconnect ability failed, err %{public}d", err);
        }
    }

    auto spManager = wpManager_.lock();
    if (spManager != nullptr) {
        spManager->CleanAbilityConnection(callerTokenId_, targetBundleName_);
    }
}

void ExtensionIpcConnection::OnAbilityConnectDone(const AppExecFwk::ElementName& element,
    const sptr<IRemoteObject>& remoteObject, int resultCode)
{
    WNMLOG_I("ability connect done");
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ != IpcConnectStatus::CONNECTING) {
        WNMLOG_W("connection is not connecting");
        return;
    }
    status_ = IpcConnectStatus::CONNECTED;
    extensionIpcRemote_ = remoteObject;

    // send pending request
    auto iter = pendingRequests_.begin();
    while (iter != pendingRequests_.end()) {
        std::shared_ptr<ConnectionNativeRequest> request = *iter;
        if (!request) {
            WNMLOG_W("pending request is null");
            iter = pendingRequests_.erase(iter);
            continue;
        }

        if (resultCode == ERR_OK) {
            if (DoConnectNative(request) == 0) {
                connectedRequests_.emplace_back(request);
            } else {
                PostRequestOnFailed(request, ConnectNativeRet::ABILITY_CONNECTION_ERROR);
            }
        } else {
            PostRequestOnFailed(request, resultCode);
        }
        iter = pendingRequests_.erase(iter);
    }
}

void ExtensionIpcConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName& element,
    int resultCode)
{
    WNMLOG_I("ability disconnect done");
    std::vector<std::shared_ptr<ConnectionNativeRequest>> popList;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        status_ = IpcConnectStatus::DISCONNECTED;
        popList.insert(popList.end(), pendingRequests_.begin(), pendingRequests_.end());
        popList.insert(popList.end(), connectedRequests_.begin(), connectedRequests_.end());
        pendingRequests_.clear();
        connectedRequests_.clear();
    }
    for (auto& request : popList) {
        PostRequestOnDisconnect(request);
    }

    auto spManager = wpManager_.lock();
    if (spManager != nullptr) {
        spManager->CleanAbilityConnection(callerTokenId_, targetBundleName_);
    }
}

int32_t ExtensionIpcConnection::ConnectNative(std::shared_ptr<ConnectionNativeRequest> request)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (status_ == IpcConnectStatus::DISCONNECTED) {
        WNMLOG_W("ipc connection status is disconnected");
        return ConnectNativeRet::CONNECT_STATUS_ERROR;
    }
    if (status_ == IpcConnectStatus::INIT) {
        status_ = IpcConnectStatus::CONNECTING;
        pendingRequests_.emplace_back(request);
        AAFwk::Want want;
        want.SetElementName(targetBundleName_, targetAbilityName_);
        auto spThis = wpThis_.promote();
        auto abilityClient = AAFwk::AbilityManagerClient::GetInstance();
        if (!abilityClient) {
            WNMLOG_E("ability manager is null");
            RemovePendingRequestUnlock(request);
            return ConnectNativeRet::ABILITY_CONNECTION_ERROR;
        }
        ErrCode err = abilityClient->ConnectAbilityWithExtensionType(want, spThis, token_, callerUserId_,
            AppExecFwk::ExtensionAbilityType::WEB_NATIVE_MESSAGING);
        if (err != ERR_OK) {
            WNMLOG_E("call ability manager connect extension failed, err %{public}d", err);
            RemovePendingRequestUnlock(request);
            status_ = IpcConnectStatus::INIT;
            return err;
        }
    } else if (status_ == IpcConnectStatus::CONNECTING) {
        pendingRequests_.emplace_back(request);
    } else if (status_ == IpcConnectStatus::CONNECTED) {
        if (DoConnectNative(request) != 0) {
            return ConnectNativeRet::ABILITY_CONNECTION_ERROR;
        }
        connectedRequests_.emplace_back(request);
    }

    request->AddDeathRecipientCallback([wp = wpThis_] () {
        auto ipcConnect = wp.promote();
        if (ipcConnect) {
            ipcConnect->HandleCallerDeath();
        }
    });
    return ConnectNativeRet::SUCCESS;
}

std::shared_ptr<ConnectionNativeRequest> ExtensionIpcConnection::LookForAndRemoveRequest(int32_t connectionId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::shared_ptr<ConnectionNativeRequest> result = nullptr;
    auto iter = std::find_if(pendingRequests_.begin(), pendingRequests_.end(),
        [connectionId] (std::shared_ptr<ConnectionNativeRequest> request) {
            return (request && request->GetInnerConnectionId() == connectionId);
        });
    if (iter != pendingRequests_.end()) {
        result = *iter;
        pendingRequests_.erase(iter);
        return result;
    }
    iter = std::find_if(connectedRequests_.begin(), connectedRequests_.end(),
        [connectionId] (std::shared_ptr<ConnectionNativeRequest> request) {
            return (request && request->GetInnerConnectionId() == connectionId);
        });
    if (iter != connectedRequests_.end()) {
        result = *iter;
        connectedRequests_.erase(iter);
        return result;
    }
    return nullptr;
}

void ExtensionIpcConnection::RemovePendingRequestUnlock(std::shared_ptr<ConnectionNativeRequest> request)
{
    for (auto iter = pendingRequests_.begin(); iter != pendingRequests_.end(); iter++) {
        if (*iter == request) {
            pendingRequests_.erase(iter);
            return;
        }
    }
    return;
}

bool ExtensionIpcConnection::IsRequestListEmpty()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return (pendingRequests_.size() == 0) && (connectedRequests_.size() == 0);
}

int32_t ExtensionIpcConnection::DisconnectNative(int32_t connectionId, bool& resDeleted)
{
    std::shared_ptr<ConnectionNativeRequest> deleteReq = LookForAndRemoveRequest(connectionId);
    if (!deleteReq) {
        WNMLOG_E("can not find disconnect connectId %{public}d", connectionId);
        return ConnectNativeRet::CONNECTION_NOT_EXIST;
    }
    if (deleteReq->IsConnected()) {
        if (DoDisconnectNative(deleteReq)) {
            WNMLOG_E("connectId %{public}d send extension disconnect failed", connectionId);
        }
    }
    if (IsRequestListEmpty()) {
        WNMLOG_I("need disconnect extension ability.");
        auto sp = wpThis_.promote();
        if (!sp) {
            return ConnectNativeRet::MEMORY_ERROR;
        }
        auto abilityClient = AAFwk::AbilityManagerClient::GetInstance();
        if (!abilityClient) {
            WNMLOG_E("ability manager is null");
            return ConnectNativeRet::ABILITY_CONNECTION_ERROR;
        }
        ErrCode err = abilityClient->DisconnectAbility(sp);
        if (err != ERR_OK) {
            WNMLOG_E("call ability manager disconnect ability failed, err %{public}d", err);
            return err;
        }
        resDeleted = true;
    }
    return ConnectNativeRet::SUCCESS;
}

void ExtensionIpcConnection::DumpMesg(int fd)
{
    dprintf(fd, "    callerTokenId:%d;targetBundleName:%s;targetAbilityName:%s;targetExtensionPid:%d;status:%d\n",
        callerTokenId_, targetBundleName_.c_str(), targetAbilityName_.c_str(), targetExtensionPid_, status_);
    std::lock_guard<std::mutex> lock(mutex_);
    dprintf(fd, "    pendingList:\n");
    for (auto& request : pendingRequests_) {
        if (request) {
            dprintf(fd, "        request:%s\n", request->ToString().c_str());
        }
    }
    dprintf(fd, "    connectedList:\n");
    for (auto& request : connectedRequests_) {
        if (request) {
            dprintf(fd, "        request:%s\n", request->ToString().c_str());
        }
    }
}

} // namespace OHOS::NWeb

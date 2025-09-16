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
#ifndef WEB_NWEB_CONNECT_NATIVE_REQUEST_H
#define WEB_NWEB_CONNECT_NATIVE_REQUEST_H

#include <string>
#include <unordered_map>

#include "accesstoken_kit.h"
#include "refbase.h"
#include "want.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
class InnerConnectIdMapKey {
public:
    InnerConnectIdMapKey(Security::AccessToken::AccessTokenID id, int32_t pid, int32_t connectId) :
        callerTokenId(id), callerPid(pid), callerConnectId(connectId) {}
    bool operator<(const InnerConnectIdMapKey& other) const {
        if (this->callerTokenId != other.callerTokenId) {
            return this->callerTokenId < other.callerTokenId;
        }
        if (this->callerPid != other.callerPid) {
            return this->callerPid < other.callerPid;
        }
        return this->callerConnectId < other.callerConnectId;
    }

    Security::AccessToken::AccessTokenID callerTokenId;
    int32_t callerPid;
    int32_t callerConnectId;
};
using InnerConnectionIdMapType = std::map<InnerConnectIdMapKey, int32_t>;

class ConnectionNativeRequest {
public:
    ConnectionNativeRequest() {}
    ~ConnectionNativeRequest();

    static void InsertRequestMap(std::shared_ptr<ConnectionNativeRequest> request);
    static void RemoveRequestMap(int32_t id);
    static std::shared_ptr<ConnectionNativeRequest> GetExistConnectId(int32_t id);
    static int32_t GetAliveSize();

    static int32_t GetInnerConnectionIdFromMap(Security::AccessToken::AccessTokenID tokenId,
        int32_t pid, int32_t connectionId);
    static void InsertInnerConnectionIdMap(Security::AccessToken::AccessTokenID tokenId,
        int32_t pid, int32_t connectionId, int32_t innerId);
    static void RemoveInnerConnectionIdMap(Security::AccessToken::AccessTokenID tokenId,
        int32_t pid, int32_t connectionId);
    static void DumpAllRequest(int fd);
    static void DumpInnerIdMap(int fd);

    ConnectNativeRet FillRequestWithWant(const AAFwk::Want& want);

    void OnConnect();
    void OnDisconnect();
    void OnFailed(int32_t errorNum);

    std::string ToString();

    void SetCallerConnectionId(int32_t connectionId)
    {
        callerConnectionId_ = connectionId;
    }

    int32_t GetCallerConnectionId()
    {
        return callerConnectionId_;
    }

    void SetInnerConnectionId(int32_t connectionId)
    {
        innerConnectionId_ = connectionId;
    }

    int32_t GetInnerConnectionId()
    {
        return innerConnectionId_;
    }

    class RequestClientDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        using RemoteDiedHandler = std::function<void ()>;
        explicit RequestClientDeathRecipient(RemoteDiedHandler handler) : handler_(handler) {}
        ~RequestClientDeathRecipient() {}
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override
        {
            if (handler_) {
                handler_();
            }
        }
    private:
        RemoteDiedHandler handler_;
    };

    void SetAppConnectCallback(sptr<IRemoteObject> callback)
    {
        appConnectCallback_ = callback;
    }

    void AddDeathRecipientCallback(RequestClientDeathRecipient::RemoteDiedHandler handler)
    {
        if (!appConnectCallback_) {
            WNMLOG_W("appConnectCallback_ is null, set death recipient failed.");
            return;
        }
        deathRecipient_ = new (std::nothrow) RequestClientDeathRecipient(handler);
        if (deathRecipient_) {
            appConnectCallback_->AddDeathRecipient(deathRecipient_);
        }
    }

    void SetCallerTokenId(Security::AccessToken::AccessTokenID tokenId)
    {
        callerTokenId_ = tokenId;
    }

    Security::AccessToken::AccessTokenID GetCallerTokenId()
    {
        return callerTokenId_;
    }

    void SetCallerBundleName(std::string bundleName)
    {
        callerBundleName_ = bundleName;
    }

    std::string& GetCallerBundleName()
    {
        return callerBundleName_;
    }

    void SetCallerPid(int32_t pid)
    {
        callerPid_ = pid;
    }

    int32_t GetCallerPid()
    {
        return callerPid_;
    }

    void SetExtensionPid(int32_t pid)
    {
        targetExtensionPid_ = pid;
    }

    int32_t GetExtensionPid()
    {
        return targetExtensionPid_;
    }

    std::string& GetTargetBundleName()
    {
        return targetBundleName_;
    }

    std::string& GetTargetAbilityName()
    {
        return targetAbilityName_;
    }

    std::string& GetCallerExtensionOrigin()
    {
        return callerExtensionOrigin_;
    }

    int32_t GetFdRead()
    {
        return fdRead_;
    }

    int32_t GetFdWrite()
    {
        return fdWrite_;
    }

    void ResetAllFileDescriptors()
    {
        if (fdRead_ >= 0) {
            close(fdRead_);
            fdRead_ = -1;
        }

        if (fdWrite_ >= 0) {
            close(fdWrite_);
            fdWrite_ = -1;
        }
    }

    bool IsConnected()
    {
        return isConnected_;
    }
private:
    int32_t callerConnectionId_;
    int32_t innerConnectionId_;
    int32_t callerPid_;
    Security::AccessToken::AccessTokenID callerTokenId_;
    std::string callerBundleName_;
    std::string callerExtensionOrigin_;
    std::string targetBundleName_;
    std::string targetAbilityName_;
    int32_t targetExtensionPid_ = -1;
    int32_t fdRead_;
    int32_t fdWrite_;
    sptr<IRemoteObject> appConnectCallback_;
    bool isConnected_ = false;
    sptr<RequestClientDeathRecipient> deathRecipient_;

    static std::mutex connectIdMutex_;
    static std::unordered_map<int32_t, std::weak_ptr<ConnectionNativeRequest>> connectIdMap_;
    static std::mutex InnerIdMapMutex_;
    static InnerConnectionIdMapType InnerIdMap_;
};
} // namespace OHOS::NWeb
#endif // WEB_NWEB_CONNECT_NATIVE_REQUEST_H

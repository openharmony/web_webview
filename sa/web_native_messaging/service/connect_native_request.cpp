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
#include "connect_native_request.h"
#include "i_web_extension_connection_callback.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
namespace {
const std::string WANT_EXTENSION_ORIGIN_PARAM_KEY = "ohos.arkweb.extensionOrigin";
const std::string WANT_READ_PIPE_PARAM_KEY = "ohos.arkweb.messageReadPipe";
const std::string WANT_WRITE_PIPE_PARAM_KEY = "ohos.arkweb.messageWritePipe";
} // namespace

std::mutex ConnectionNativeRequest::connectIdMutex_;
std::unordered_map<int32_t, std::weak_ptr<ConnectionNativeRequest>> ConnectionNativeRequest::connectIdMap_;
std::mutex ConnectionNativeRequest::InnerIdMapMutex_;
InnerConnectionIdMapType ConnectionNativeRequest::InnerIdMap_;

ConnectionNativeRequest::~ConnectionNativeRequest()
{
    ResetAllFileDescriptors();
    ConnectionNativeRequest::RemoveRequestMap(this->innerConnectionId_);
    ConnectionNativeRequest::RemoveInnerConnectionIdMap(this->callerTokenId_,
        this->callerPid_, this->callerConnectionId_);
    WNMLOG_D("~ConnectionNativeRequest");
}

void ConnectionNativeRequest::InsertRequestMap(std::shared_ptr<ConnectionNativeRequest> request)
{
    if (!request) {
        WNMLOG_E("InsertRequestMap request is null");
        return;
    }
    std::lock_guard<std::mutex> lock(connectIdMutex_);
    std::weak_ptr<ConnectionNativeRequest> weak = request;
    connectIdMap_[request->GetInnerConnectionId()] = weak;

    InsertInnerConnectionIdMap(request->GetCallerTokenId(), request->GetCallerPid(),
        request->GetCallerConnectionId(), request->GetInnerConnectionId());
}

void ConnectionNativeRequest::RemoveRequestMap(int32_t id)
{
    std::lock_guard<std::mutex> lock(connectIdMutex_);
    connectIdMap_.erase(id);
}

int32_t ConnectionNativeRequest::GetInnerConnectionIdFromMap(Security::AccessToken::AccessTokenID tokenId,
    int32_t pid, int32_t connectionId)
{
    std::lock_guard<std::mutex> lock(InnerIdMapMutex_);
    InnerConnectIdMapKey key(tokenId, pid, connectionId);
    auto iter = InnerIdMap_.find(key);
    return iter == InnerIdMap_.end() ? -1 : iter->second;
}

void ConnectionNativeRequest::InsertInnerConnectionIdMap(Security::AccessToken::AccessTokenID tokenId,
    int32_t pid, int32_t connectionId, int32_t innerId)
{
    std::lock_guard<std::mutex> lock(InnerIdMapMutex_);
    InnerConnectIdMapKey key(tokenId, pid, connectionId);
    InnerIdMap_[key] = innerId;
}

void ConnectionNativeRequest::RemoveInnerConnectionIdMap(Security::AccessToken::AccessTokenID tokenId,
    int32_t pid, int32_t connectionId)
{
    std::lock_guard<std::mutex> lock(InnerIdMapMutex_);
    InnerConnectIdMapKey key(tokenId, pid, connectionId);
    auto iter = InnerIdMap_.find(key);
    if (iter != InnerIdMap_.end()) {
        InnerIdMap_.erase(iter);
    }
}

int32_t ConnectionNativeRequest::GetAliveSize()
{
    std::lock_guard<std::mutex> lock(connectIdMutex_);
    return connectIdMap_.size();
}

std::shared_ptr<ConnectionNativeRequest> ConnectionNativeRequest::GetExistConnectId(int32_t id)
{
    std::lock_guard<std::mutex> lock(connectIdMutex_);
    auto iter = connectIdMap_.find(id);
    if (iter != connectIdMap_.end()) {
        std::shared_ptr<ConnectionNativeRequest> request = iter->second.lock();
        return request;
    }
    return nullptr;
}

void ConnectionNativeRequest::DumpAllRequest(int fd)
{
    std::lock_guard<std::mutex> lock(connectIdMutex_);
    for (auto iter = connectIdMap_.begin(); iter != connectIdMap_.end(); iter++) {
        auto request = iter->second.lock();
        if (request) {
            dprintf(fd, "    Request:%s\n", request->ToString().c_str());
        }
    }
}

void ConnectionNativeRequest::DumpInnerIdMap(int fd)
{
    std::lock_guard<std::mutex> lock(InnerIdMapMutex_);
    for (auto iter = InnerIdMap_.begin(); iter != InnerIdMap_.end(); iter++) {
        auto key = iter->first;
        auto innerId = iter->second;
        dprintf(fd, "   Entry:callerTokenId:%d;callerPid:%d;callerConnectId:%d;innerConnectionId:%d\n",
            key.callerTokenId, key.callerPid, key.callerConnectId, innerId);
    }
}

ConnectNativeRet ConnectionNativeRequest::FillRequestWithWant(const AAFwk::Want& want)
{
    std::string bundleName = want.GetBundle();
    if (bundleName.empty()) {
        WNMLOG_E("want bundle name is invalid");
        return ConnectNativeRet::WANT_FORMAT_ERROR;
    }
    targetBundleName_ = bundleName;

    std::string abilityName = want.GetElement().GetAbilityName();
    if (abilityName.empty()) {
        WNMLOG_E("want ability name is invalid");
        return ConnectNativeRet::WANT_FORMAT_ERROR;
    }
    targetAbilityName_ = abilityName;

    std::string extensionOrigin = want.GetParams().GetStringParam(WANT_EXTENSION_ORIGIN_PARAM_KEY);
    if (extensionOrigin.empty()) {
        WNMLOG_E("want extensionOrigin is invalid");
        return ConnectNativeRet::WANT_FORMAT_ERROR;
    }
    callerExtensionOrigin_ = extensionOrigin;

    int32_t readFd = -1;
    AAFwk::WantParams readPipeParams = want.GetParams().GetWantParams(WANT_READ_PIPE_PARAM_KEY);
    if (readPipeParams.IsEmpty() || readPipeParams.GetStringParam("type") != "FD" ||
        (readFd = readPipeParams.GetIntParam("value", -1)) == -1) {
        WNMLOG_E("want readFd is invalid");
        return ConnectNativeRet::WANT_FORMAT_ERROR;
    }
    fdRead_ = readFd;

    int32_t writeFd = -1;
    AAFwk::WantParams writePipeParams = want.GetParams().GetWantParams(WANT_WRITE_PIPE_PARAM_KEY);
    if (writePipeParams.IsEmpty() || writePipeParams.GetStringParam("type") != "FD" ||
        (writeFd = writePipeParams.GetIntParam("value", -1)) == -1) {
        WNMLOG_E("want writeFd is invalid");
        return ConnectNativeRet::WANT_FORMAT_ERROR;
    }
    fdWrite_ = writeFd;
    return ConnectNativeRet::SUCCESS;
}

void ConnectionNativeRequest::OnConnect()
{
    if (isConnected_) {
        return;
    }
    isConnected_ = true;
    if (appConnectCallback_ == nullptr) {
        WNMLOG_E("app connect callback is null");
        return;
    }
    auto callback = iface_cast<IWebExtensionConnectionCallback>(appConnectCallback_);
    if (!callback)  {
        WNMLOG_E("cast interface error");
        return;
    }
    ConnectionNativeInfo info;
    info.connectionId = callerConnectionId_;
    info.bundleName = targetBundleName_;
    info.extensionOrigin = callerExtensionOrigin_;
    info.extensionPid = targetExtensionPid_;
    callback->OnConnect(info);
}

void ConnectionNativeRequest::OnDisconnect()
{
    if (!isConnected_) {
        return;
    }
    isConnected_ = false;
    if (appConnectCallback_ == nullptr) {
        WNMLOG_E("app connect callback is null");
        return;
    }
    auto callback = iface_cast<IWebExtensionConnectionCallback>(appConnectCallback_);
    if (!callback) {
        WNMLOG_E("cast interface error");
        return;
    }

    ConnectionNativeInfo info;
    info.connectionId = callerConnectionId_;
    info.bundleName = targetBundleName_;
    info.extensionOrigin = callerExtensionOrigin_;
    info.extensionPid = targetExtensionPid_;
    callback->OnDisconnect(info);
}

void ConnectionNativeRequest::OnFailed(int32_t errorNum)
{
    if (appConnectCallback_ == nullptr) {
        WNMLOG_E("app connect callback is null");
        return;
    }
    auto callback = iface_cast<IWebExtensionConnectionCallback>(appConnectCallback_);
    if (!callback) {
        WNMLOG_E("cast interface error");
        return;
    }
    callback->OnFailed(errorNum);
}

std::string ConnectionNativeRequest::ToString()
{
    return "callerConnectionId:" + std::to_string(callerConnectionId_) +
        ";innerConnectionId:" + std::to_string(innerConnectionId_) +
        ";callerPid:" + std::to_string(callerPid_) +
        ";callerTokenId:" + std::to_string(callerTokenId_) +
        ";callerBundleName:" + callerBundleName_ +
        ";callerExtensionOrigin:" + callerExtensionOrigin_ +
        ";targetBundleName:" + targetBundleName_ +
        ";targetAbilityName:" + targetAbilityName_ +
        ";targetExtensionPid:" + std::to_string(targetExtensionPid_) +
        ";fdRead:" + std::to_string(fdRead_) +
        ";fdWrite:" + std::to_string(fdWrite_) +
        ";isConnected:" + std::to_string(isConnected_);
}
} // namespace OHOS::NWeb

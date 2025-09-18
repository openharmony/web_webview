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
#include "web_native_messaging_client.h"

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "isystem_ability_load_callback.h"
#include "web_native_messaging_log.h"
#include "system_ability_definition.h"
#include "web_native_messaging_load_callback.h"

#include "base/web/webview/sa/web_native_messaging/iweb_native_messaging_service.h"
#include "base/web/webview/sa/web_native_messaging/web_native_messaging_service_proxy.h"

namespace OHOS::NWeb {
namespace {
const int LOAD_SA_TIMEOUT_MS = 2 * 1000;
} // namespace

WebNativeMessagingClient::WebNativeMessagingClient()
{
    webNativeMessagingDiedRecipient_ = new (std::nothrow) WebNativeMessagingDiedRecipient();
    if (webNativeMessagingDiedRecipient_ == nullptr) {
        WNMLOG_E("create web native messaging service died recipient failed");
    }
}
WebNativeMessagingClient& WebNativeMessagingClient::GetInstance()
{
    static WebNativeMessagingClient client;
    return client;
}

void WebNativeMessagingClient::SetWebNativeMessagingProxy(const sptr<IRemoteObject>& remoteObject)
{
    std::lock_guard<std::mutex> lock(mutex_);
    webNativeMessagingProxy_ = iface_cast<IWebNativeMessagingService>(remoteObject);
    if (webNativeMessagingProxy_) {
        WNMLOG_I("SetWebNativeMessagingProxy is not null");
    }
}

sptr<IWebNativeMessagingService> WebNativeMessagingClient::GetWebNativeMessaging()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return webNativeMessagingProxy_;
}

sptr<IWebNativeMessagingService> WebNativeMessagingClient::GetWebNativeMessagingProxy()
{
    auto webNativeMessagingProxy = GetWebNativeMessaging();
    if (webNativeMessagingProxy != nullptr) {
        return webNativeMessagingProxy;
    }
    auto sam = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        WNMLOG_E("load web native messaging service sam is null");
        return nullptr;
    }

    auto remoteObj = sam->CheckSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID);
    if (remoteObj != nullptr) {
        webNativeMessagingProxy = iface_cast<IWebNativeMessagingService>(remoteObj);
        return webNativeMessagingProxy;
    }
    if (!LoadNativeMessagingService()) {
        WNMLOG_I("get web native messaging service is null");
        return nullptr;
    }
    webNativeMessagingProxy = GetWebNativeMessaging();
    if (webNativeMessagingProxy == nullptr) {
        WNMLOG_I("get web native messaging service proxy is null");
        return nullptr;
    }
    WNMLOG_I("load web native messaging service sa finished");
    return webNativeMessagingProxy;
}

int WebNativeMessagingClient::ConnectWebNativeMessagingExtension(
    const sptr<IRemoteObject>& token, const AAFwk::Want& want,
    const sptr<IRemoteObject>& connectCallback, int32_t connectionId)
{
    if (token == nullptr) {
        WNMLOG_E("token is invalid");
        return ConnectNativeRet::CONTEXT_ERROR;
    }
    if (connectCallback == nullptr) {
        WNMLOG_E("connectCallback is invalid");
        return ConnectNativeRet::CALLBACK_ERROR;
    }
    auto proxy = GetWebNativeMessagingProxy();
    if (proxy == nullptr) {
        WNMLOG_E("connect web native messaging failed, proxy is null");
        return ConnectNativeRet::IPC_ERROR;
    }

    int32_t errorNum = 0;
    int32_t ret =
        proxy->ConnectWebNativeMessagingExtension(token, want, connectCallback, connectionId, errorNum);
    if (ret != 0) {
        WNMLOG_E("connect web extension result: %{public}d", ret);
        return ConnectNativeRet::IPC_ERROR;
    }
    return errorNum;
}

int WebNativeMessagingClient::DisconnectWebNativeMessagingExtension(int32_t connectionId)
{
    auto proxy = GetWebNativeMessagingProxy();
    if (proxy == nullptr) {
        WNMLOG_E("disconnect web native messaging failed, proxy is null");
        return ConnectNativeRet::IPC_ERROR;
    }

    int32_t errorNum = 0;
    int32_t ret =
        proxy->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    if (ret != 0) {
        WNMLOG_E("disconnect web extension result: %{public}d", ret);
        return ConnectNativeRet::IPC_ERROR;
    }
    return errorNum;
}

int WebNativeMessagingClient::StartAbility(const sptr<IRemoteObject>& token,
    const AAFwk::Want& want, const AAFwk::StartOptions& options)
{
    auto proxy = GetWebNativeMessagingProxy();
    if (proxy == nullptr) {
        WNMLOG_E("start ability failed, proxy is null");
        return ConnectNativeRet::IPC_ERROR;
    }

    int32_t errorNum = 0;
    int32_t ret =
        proxy->StartAbility(token, want, options, errorNum);
    if (ret != 0) {
        WNMLOG_E("Start ability result: %{public}d", ret);
        return ConnectNativeRet::IPC_ERROR;
    }
    return errorNum;
}

int WebNativeMessagingClient::StopNativeConnectionFromExtension(int32_t connectionId)
{
    auto proxy = GetWebNativeMessagingProxy();
    if (proxy == nullptr) {
        WNMLOG_E("stop native connection failed, proxy is null");
        return ConnectNativeRet::IPC_ERROR;
    }

    int32_t errorNum = 0;
    int32_t ret =
        proxy->StopNativeConnectionFromExtension(connectionId, errorNum);
    if (ret != 0) {
        WNMLOG_E("Stop native connection from extension result: %{public}d", ret);
        return ConnectNativeRet::IPC_ERROR;
    }
    return errorNum;
}

bool WebNativeMessagingClient::LoadNativeMessagingService()
{
    {
        std::unique_lock<std::mutex> lock(loadSaMutex_);
        loadSaFinished_ = false;
    }
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        WNMLOG_I("failed to get system ability manager");
        return false;
    }
    sptr<WebNativeMessagingLoadCallback> loadCallback = new (std::nothrow) WebNativeMessagingLoadCallback();
    if (loadCallback == nullptr) {
        WNMLOG_I("failed to create load callback");
        return false;
    }
    auto ret = systemAbilityMgr->LoadSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID, loadCallback);
    if (ret != 0) {
        WNMLOG_W("load native messaging service failed.");
        return false;
    }
    {
        std::unique_lock<std::mutex> lock(loadSaMutex_);
        auto waitStatus = loadSaCondition_.wait_for(
            lock, std::chrono::milliseconds(LOAD_SA_TIMEOUT_MS), [this]() { return loadSaFinished_; });
        if (!waitStatus) {
            auto remoteObj = systemAbilityMgr->CheckSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID);
            if (remoteObj != nullptr) {
                SetWebNativeMessagingProxy(remoteObj);
                return true;
            }
            WNMLOG_I("load web native messaging service timeout.");
            return false;
        }
        WNMLOG_I("load web native messaging service success.");
        return true;
    }
}

void WebNativeMessagingClient::OnLoadSystemAbilitySuccess(const sptr<IRemoteObject>& remoteObject)
{
    WNMLOG_I("on load systemAbility success");
    if (webNativeMessagingDiedRecipient_ == nullptr) {
        WNMLOG_E("register web native messaging service died recipient failed");
        return;
    }
    if (!remoteObject->AddDeathRecipient(webNativeMessagingDiedRecipient_)) {
        WNMLOG_E("add web native messaging service died recipient failed");
        return;
    }
    SetWebNativeMessagingProxy(remoteObject);
    std::unique_lock<std::mutex> lock(loadSaMutex_);
    loadSaFinished_ = true;
    loadSaCondition_.notify_one();
}

void WebNativeMessagingClient::OnLoadSystemAbilityFail()
{
    SetWebNativeMessagingProxy(nullptr);
    std::unique_lock<std::mutex> lock(loadSaMutex_);
    loadSaFinished_ = true;
    loadSaCondition_.notify_one();
}

void WebNativeMessagingClient::WebNativeMessagingDiedRecipient::OnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    if (remoteObject == nullptr) {
        WNMLOG_E("remote object of web native messaging service died recipient is nullptr");
        return;
    }
    WebNativeMessagingClient::GetInstance().WebNativeMessagingOnRemoteDied(remoteObject);
}

void WebNativeMessagingClient::WebNativeMessagingOnRemoteDied(const wptr<IRemoteObject>& remoteObject)
{
    WNMLOG_I("remote object of web native messaging service died recipient is died");
    auto webNativeMessagingProxy = GetWebNativeMessaging();
    if (webNativeMessagingProxy == nullptr) {
        WNMLOG_E("web native messaging proxy is nullptr");
        return;
    }
    sptr<IRemoteObject> remotePromote = remoteObject.promote();
    if (remotePromote == nullptr) {
        WNMLOG_E("remote object of web native messaging service promote fail");
        return;
    }
    if (webNativeMessagingProxy->AsObject() != remotePromote) {
        WNMLOG_E("web native messaging died recipient not find remote object");
        return;
    }
    remotePromote->RemoveDeathRecipient(webNativeMessagingDiedRecipient_);
    SetWebNativeMessagingProxy(nullptr);
    if (deathCallback_) {
        deathCallback_();
    }
}
} // namespace OHOS::NWeb

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
#include "web_native_messaging_service.h"

#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "sysversion.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
namespace {
REGISTER_SYSTEM_ABILITY_BY_ID(WebNativeMessagingService, SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID, true);
} // namespace

WebNativeMessagingService::WebNativeMessagingService(int32_t saId, bool runOnCreate) :
    SystemAbility(saId, runOnCreate) {}

WebNativeMessagingService::~WebNativeMessagingService() {}

ErrCode WebNativeMessagingService::ConnectWebNativeMessagingExtension(const sptr<IRemoteObject>& token,
    const AAFwk::Want& want, const sptr<IRemoteObject>& connectionCallback,
    int32_t connectionId, int32_t& errorNum)
{
    if (!manager_) {
        errorNum = ConnectNativeRet::SERVICE_INIT_ERROR;
        return ERR_OK;
    }
    manager_->ConnectWebNativeMessagingExtension(token, want, connectionCallback, connectionId, errorNum);
    return ERR_OK;
}

ErrCode WebNativeMessagingService::DisconnectWebNativeMessagingExtension(int32_t connectionId,
    int32_t& errorNum)
{
    if (!manager_) {
        errorNum = ConnectNativeRet::SERVICE_INIT_ERROR;
        return ERR_OK;
    }
    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    return ERR_OK;
}

ErrCode WebNativeMessagingService::StartAbility(const sptr<IRemoteObject>& token,
    const AAFwk::Want& want, const AAFwk::StartOptions& startOptions, int32_t& errorNum)
{
    if (!manager_) {
        errorNum = ConnectNativeRet::SERVICE_INIT_ERROR;
        return ERR_OK;
    }
    manager_->StartAbility(token, want, startOptions, errorNum);
    return ERR_OK;
}

ErrCode WebNativeMessagingService::StopNativeConnectionFromExtension(int32_t connectionId, int32_t& errorNum)
{
    if (!manager_) {
        errorNum = ConnectNativeRet::SERVICE_INIT_ERROR;
        return ERR_OK;
    }
    manager_->StopNativeConnectionFromExtension(connectionId, errorNum);
    return ERR_OK;
}

int32_t WebNativeMessagingService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (!manager_) {
        return ERR_INVALID_VALUE;
    }
    return manager_->Dump(fd, args);
}

void WebNativeMessagingService::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    WNMLOG_I("Service on start.");
    if (registerToService_) {
        WNMLOG_I("Web native messaging service is running.");
    }
    if (!Init(startReason)) {
        return;
    }
}

bool WebNativeMessagingService::Init(const SystemAbilityOnDemandReason& startReason)
{
    std::string reasonName = startReason.GetName();
    std::string reasonValue = startReason.GetValue();
    WNMLOG_I("WebNativeMessagingService reasonName: %{public}s", reasonName.c_str());

    manager_ = std::make_shared<WebNativeMessagingManager>();
    if (!manager_) {
        WNMLOG_I("create manager failed");
        return false;
    }
    manager_->Init();

    bool ret = Publish(this);
    if (!ret) {
        WNMLOG_I("Service publish failed.");
        return false;
    }

    registerToService_ = true;
    WNMLOG_I("Service init success.");
    return true;
}

void WebNativeMessagingService::OnStop()
{
    WNMLOG_I("Ready to stop service.");
    registerToService_ = false;
}
} // namespace OHOS::NWeb

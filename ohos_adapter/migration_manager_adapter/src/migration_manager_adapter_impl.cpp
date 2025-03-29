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
#include "migration_manager_adapter_impl.h"

#include <cmath>

#include "ability_manager_client.h"
#include "nweb_log.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
static const uint32_t CMD_CODE = 1;
static sptr<OHOS::IRemoteObject> remoteObject_ = nullptr;
static const std::string REPLY_CODE = "code";
static const std::string REPLY_ERROR_LIST = "errorList";
static const std::string REPLY_SUCCESS_COUNT = "successCount";
static const std::string REPLY_INDEX = "index";
std::atomic<bool> MigrationManagerAdapterImpl::isConnectSystemUI_(false);

void MigrationManagerAdapterImpl::MigrationListenerAdapterImpl::OnMigrationReply(int32_t errorCode,
    int32_t succussCount, const std::vector<int32_t>& errorIndex, const std::vector<int32_t>& codeList)
{
    if (listener_) {
        WVLOG_I("on migration replay will be invoked");
        listener_->OnMigrationReply(errorCode, succussCount, errorIndex, codeList);
    }
}

void MigrationManagerAdapterImpl::MigrationListenerAdapterImpl::OnAbilityConnectDone(
    const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int resultCode)
{
    WVLOG_I("send migration request begin");
    if (!remoteObject) {
        return;
    }
    if (remoteObject_ == nullptr) {
        remoteObject_ = remoteObject;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    isConnectSystemUI_.store(true);

    data.WriteInterfaceToken(Str8ToStr16(token_));
    data.WriteString16(Str8ToStr16(jsonData_));
    int32_t ret = remoteObject_->SendRequest(CMD_CODE, data, reply, option);
    if (ret != ERR_OK) {
        WVLOG_W("send request failed: %{public}d", ret);
        return;
    }
    std::string replyData = Str16ToStr8(reply.ReadString16());
    WVLOG_I("send migration request reply res: %{public}s", replyData.c_str());

    int errorCode = -1;
    int successCount = 0;
    std::vector<int> errorIndexList;
    std::vector<int> codeList;
    nlohmann::json jsonObj = nlohmann::json::parse(replyData);
    if (jsonObj.contains(REPLY_CODE) && jsonObj[REPLY_CODE].is_number_integer()) {
        errorCode = jsonObj[REPLY_CODE];
    }
    if (jsonObj.contains(REPLY_ERROR_LIST)) {
        auto errorList = jsonObj[REPLY_ERROR_LIST];
        for (size_t i = 0; i < errorList.size(); ++i) {
            if (errorList[i].contains(REPLY_CODE) && errorList[i].contains(REPLY_INDEX)) {
                int errorCode = errorList[i][REPLY_CODE];
                int errorIndex = errorList[i][REPLY_INDEX];
                codeList.push_back(errorCode);
                errorIndexList.push_back(errorIndex);
            }
        }
    }
    if (jsonObj.contains(REPLY_SUCCESS_COUNT) && jsonObj[REPLY_SUCCESS_COUNT].is_number_integer()) {
        successCount = jsonObj[REPLY_SUCCESS_COUNT];
    }
    WVLOG_I("send migration request successCount: %{public}d", successCount);
    OnMigrationReply(errorCode, successCount, errorIndexList, codeList);
    WVLOG_I("send migration request finish");
}

void MigrationManagerAdapterImpl::MigrationListenerAdapterImpl::SetJsonData(const std::string& jsonData)
{
    WVLOG_D("set json data invoke");
    jsonData_ = jsonData;
}

void MigrationManagerAdapterImpl::MigrationListenerAdapterImpl::SetInterfaceToken(const std::string& token)
{
    WVLOG_I("SetInterfaceToken invoke");
    token_ = token;
}

void MigrationManagerAdapterImpl::MigrationListenerAdapterImpl::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName& element, int resultCode)
{
    WVLOG_I("onAbilityDisconnectDone, resultCode is %{public}d", resultCode);
    std::vector<int32_t> vec(0);
    OnMigrationReply(0, 0, vec, vec);
    isConnectSystemUI_.store(false);
}

void MigrationManagerAdapterImpl::SetMigrationParam(
    const std::string& bundleName, const std::string& abilityName, const std::string& token)
{
    WVLOG_I("setMigrationParam start.");
    bundleName_ = bundleName;
    abilityName_ = abilityName;
    if (callback_) {
        callback_->SetInterfaceToken(token);
    }
}

bool MigrationManagerAdapterImpl::SendMigrationRequest(std::shared_ptr<std::string> jsonData)
{
    WVLOG_I("send migration reqeust start.");
    if (callback_) {
        callback_->SetJsonData(*jsonData);
    }
    if (isConnectSystemUI_.load() && callback_ != nullptr) {
        AppExecFwk::ElementName element;
        callback_->OnAbilityConnectDone(element, remoteObject_, AppExecFwk::Constants::INVALID_USERID);
        WVLOG_I("connectExtension has been invoked once.");
        return true;
    }

    AAFwk::Want want;
    want.SetElementName(bundleName_, abilityName_);
    auto abilityManager = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManager == nullptr) {
        WVLOG_W("ability Manager Client is nullptr");
        return false;
    }
    auto ret = abilityManager->ConnectAbility(want, callback_, AppExecFwk::Constants::INVALID_USERID);
    if (ret != ERR_OK) {
        WVLOG_E("connectExtensionAbility failed.");
        return false;
    }
    WVLOG_I("connectExtensionAbility end.");
    return true;
}

uint32_t MigrationManagerAdapterImpl::RegisterMigrationListener(std::shared_ptr<MigrationListenerAdapter> listener)
{
    if (!listener) {
        WVLOG_E("registerMigrationListener make failed");
    }
    WVLOG_I("connectExtensionAbility RegisterMigrationListener.");
    callback_ = new (std::nothrow) MigrationListenerAdapterImpl(listener);
    if (!callback_) {
        WVLOG_E("make_shared failed");
        return -1;
    }
    return 0;
}

} // namespace OHOS::NWeb

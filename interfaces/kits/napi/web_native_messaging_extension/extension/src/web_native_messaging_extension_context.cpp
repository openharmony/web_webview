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

#include "web_native_messaging_extension_context.h"

#include "ability_manager_client.h"
#include "web_native_messaging_client.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {
ErrCode WebNativeMessagingExtensionContext::StartAbility(const OHOS::AAFwk::Want& want,
    const OHOS::AAFwk::StartOptions startOptions)
{
    ErrCode err = WebNativeMessagingClient::GetInstance().StartAbility(token_, want, startOptions);
    if (err != ERR_OK) {
        WNMLOG_E("failed %{public}d", err);
    }
    return err;
}

ErrCode WebNativeMessagingExtensionContext::StartAbilityForResult(const OHOS::AAFwk::Want& want,
    const OHOS::AAFwk::StartOptions startOptions, int requestCode)
{
    ErrCode err = WebNativeMessagingClient::GetInstance().StartAbilityForResult(
        token_, want, startOptions, requestCode);
    if (err != ERR_OK) {
        WNMLOG_E("failed %{public}d", err);
    }
    return err;
}

ErrCode WebNativeMessagingExtensionContext::TerminateSelf()
{
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->TerminateAbility(token_, -1, nullptr);
    if (err != ERR_OK) {
        WNMLOG_E("failed %{public}d", err);
    }
    return err;
}

ErrCode WebNativeMessagingExtensionContext::StopNativeConnection(int32_t connectionId)
{
    ErrCode err = WebNativeMessagingClient::GetInstance().StopNativeConnectionFromExtension(connectionId);
    if (err != ERR_OK) {
        WNMLOG_E("failed %{public}d", err);
    }
    return err;
}

ErrCode WebNativeMessagingExtensionContext::StartAbilityForResult(const OHOS::AAFwk::Want& want,
    const OHOS::AAFwk::StartOptions startOptions, int requestCode, RuntimeTask&& task)
{
    WNMLOG_I(" Context::StartAbilityForResult, requestCode: %{public}d", requestCode);
    resultCallbacks_.insert(std::make_pair(requestCode, std::move(task)));

    OHOS::AAFwk::Want modifiedWant = want;
    modifiedWant.SetParam(OHOS::AAFwk::Want::PARAM_RESV_FOR_RESULT, true);

    ErrCode err = WebNativeMessagingClient::GetInstance().StartAbilityForResult(
        token_, modifiedWant, startOptions, requestCode);
    if (err != ERR_OK) {
        WNMLOG_E("Client StartAbilityForResult failed: %{public}d, removing callback", err);
        resultCallbacks_.erase(requestCode);
    } else {
        WNMLOG_I("Client StartAbilityForResult succeeded, callback registered, "
            "requestCode: %{public}d", requestCode);
    }
    return err;
}

void WebNativeMessagingExtensionContext::OnAbilityResult(
    int requestCode, int resultCode, const AAFwk::Want& resultData)
{
    WNMLOG_I("Context::OnAbilityResult , requestCode: %{public}d ", requestCode);

    auto callback = resultCallbacks_.find(requestCode);
    if (callback != resultCallbacks_.end()) {
        if (callback->second) {
            callback->second(resultCode, resultData, false);
        }
        resultCallbacks_.erase(callback);
    } else {
        WNMLOG_W("No callback found for requestCode: %{public}d", requestCode);
    }
}

int WebNativeMessagingExtensionContext::GenerateCurRequestCode()
{
    curRequestCode_ = (curRequestCode_ == INT_MAX) ? 0 : (curRequestCode_ + 1);
    return curRequestCode_;
}

} // namespace NWeb
} // namespace OHOS

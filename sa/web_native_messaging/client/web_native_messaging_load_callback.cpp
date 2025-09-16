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

#include "web_native_messaging_load_callback.h"

#include "system_ability_definition.h"
#include "web_native_messaging_client.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
void WebNativeMessagingLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject>& remoteObject)
{
    if (systemAbilityId != SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID) {
        WNMLOG_E("system ability id mismatch, sa id: %{public}d", systemAbilityId);
        return;
    }

    if (remoteObject == nullptr) {
        WNMLOG_E("remoteObject is nullptr");
        return;
    }
    WNMLOG_D("load system ability success %{public}d", systemAbilityId);
    WebNativeMessagingClient::GetInstance().OnLoadSystemAbilitySuccess(remoteObject);
}

void WebNativeMessagingLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    if (systemAbilityId != SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID) {
        WNMLOG_E("system ability id mismatch, sa id: %{public}d", systemAbilityId);
        return;
    }

    WNMLOG_D("load system ability failed %{public}d", systemAbilityId);
    WebNativeMessagingClient::GetInstance().OnLoadSystemAbilityFail();
}
} // namespace OHOS::NWeb

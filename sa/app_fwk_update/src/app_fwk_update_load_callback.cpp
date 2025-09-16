/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "app_fwk_update_load_callback.h"

#include "app_fwk_update_client.h"
#include "nweb_log.h"
#include "system_ability_definition.h"

namespace OHOS::NWeb {
void AppFwkUpdateLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject>& remoteObject)
{
    if (systemAbilityId != SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID) {
        WVLOG_E("system ability id mismatch, sa id: %{public}d", systemAbilityId);
        return;
    }

    if (remoteObject == nullptr) {
        WVLOG_E("remoteObject is nullptr");
        return;
    }
    WVLOG_D("load system ability success %{public}d", systemAbilityId);
    AppFwkUpdateClient::GetInstance().OnLoadSystemAbilitySuccess(remoteObject);
}

void AppFwkUpdateLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    if (systemAbilityId != SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID) {
        WVLOG_E("system ability id mismatch, sa id: %{public}d", systemAbilityId);
        return;
    }

    WVLOG_D("load system ability failed %{public}d", systemAbilityId);
    AppFwkUpdateClient::GetInstance().OnLoadSystemAbilityFail();
}
} // namespace OHOS::NWeb
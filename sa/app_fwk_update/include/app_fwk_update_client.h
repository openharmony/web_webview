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

#ifndef OHOS_NWEB_APP_FWK_UPDATE_CLIENT_H
#define OHOS_NWEB_APP_FWK_UPDATE_CLIENT_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <set>
#include <string>

#include "singleton.h"

#include "iapp_fwk_update_service.h"

namespace OHOS::NWeb {
class AppFwkUpdateClient {
public:
    AppFwkUpdateClient();
    virtual ~AppFwkUpdateClient() = default;
    static AppFwkUpdateClient& GetInstance();
    int VerifyPackageInstall(const std::string& bundleName, const std::string& hapPath);
    int NotifyFWKAfterBmsStart();
    void NotifyArkWebInstallSuccess();
    void OnLoadSystemAbilitySuccess(const sptr<IRemoteObject>& object);
    void OnLoadSystemAbilityFail();
    void AppFwkUpdateOnRemoteDied(const wptr<IRemoteObject>& remoteObject);

private:
    bool LoadFwkService();
    void SetFwkUpdate(const sptr<IRemoteObject>& remoteObject);
    sptr<IAppFwkUpdateService> GetFwkUpdate();
    sptr<IAppFwkUpdateService> GetFwkUpdateProxy();
    class AppFwkUpdateDiedRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    };

private:
    std::condition_variable loadSaCondition_;
    std::mutex loadSaMutex_;
    bool loadSaFinished_ { false };
    std::mutex mutex_;
    sptr<IAppFwkUpdateService> fwkUpdateProxy_ = nullptr;
    sptr<AppFwkUpdateDiedRecipient> appFwkUpdateDiedRecipient_ = nullptr;
    DISALLOW_COPY_AND_MOVE(AppFwkUpdateClient);
};
} // namespace OHOS::NWeb
#endif // OHOS_NWEB_APP_FWK_UPDATE_CLIENT_H

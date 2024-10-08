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

#ifndef APP_FWK_UPDATE_SERVICE_H
#define APP_FWK_UPDATE_SERVICE_H

#include <string>

#include "app_fwk_update_service_stub.h"
#include "common_event_manager.h"
#include "event_handler.h"
#include "refbase.h"
#include "system_ability.h"

namespace OHOS::NWeb {

struct PackageCommonEventCallback {
    std::function<void(const std::string bundleName, const std::string hapPath)> OnPackageChangedEvent;
};

class PackageChangedReceiver : public EventFwk::CommonEventSubscriber {
public:
    explicit PackageChangedReceiver(
        const EventFwk::CommonEventSubscribeInfo& subscribeInfo, const PackageCommonEventCallback& callback);
    ~PackageChangedReceiver() = default;
    void OnReceiveEvent(const EventFwk::CommonEventData& data) override;

private:
    PackageCommonEventCallback callback_;
};

class AppFwkUpdateService : public SystemAbility, public AppFwkUpdateServiceStub {
    DECLARE_SYSTEM_ABILITY(AppFwkUpdateService);

public:
    AppFwkUpdateService(int32_t saId, bool runOnCreate);
    ~AppFwkUpdateService();

    // add an unused funcation to verify adapting sa IDL template.
    ErrCode RequestUpdateService(const std::string& bundleName) override;
    void SubscribePackageChangedEvent();
    void OnPackageChangedEvent(const std::string& bunldeName, const std::string& hapPath);

protected:
    void OnStart(const SystemAbilityOnDemandReason& startReason) override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void PostDelayUnloadTask();

private:
    bool Init(const SystemAbilityOnDemandReason& startReason);
    void SetWebInstallPath(const std::string& path);
    void SetWebCorePackageName(const std::string& packageName);
    void SendAppSpawnMessage(const std::string& packageName);
    void SendNWebSpawnMesage(const std::string& packageName);
    std::shared_ptr<AppExecFwk::EventHandler> unloadHandler_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    bool registerToService_ = false;
    std::shared_ptr<EventFwk::CommonEventSubscriber> pkgSubscriber_;
};
} // namespace OHOS::NWeb

#endif // APP_FWK_UPDATE_SERVICE_H

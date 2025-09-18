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
#ifndef NWEB_WEB_NATIVE_MESSAGING_SERVICE_H
#define NWEB_WEB_NATIVE_MESSAGING_SERVICE_H

#include <string>

#include "refbase.h"
#include "system_ability.h"
#include "web_native_messaging_manager.h"
#include "web_native_messaging_service_stub.h"

namespace OHOS::NWeb {
class WebNativeMessagingService : public SystemAbility, public WebNativeMessagingServiceStub {
    DECLARE_SYSTEM_ABILITY(WebNativeMessagingService);

public:
    WebNativeMessagingService(int32_t saId, bool runOnCreate);
    ~WebNativeMessagingService();

    ErrCode ConnectWebNativeMessagingExtension(const sptr<IRemoteObject>& token,
        const AAFwk::Want& want, const sptr<IRemoteObject>& connectionCallback,
        int32_t connectionId, int32_t& errorNum) override;
    ErrCode DisconnectWebNativeMessagingExtension(int32_t connectionId, int32_t& errorNum) override;
    ErrCode StartAbility(const sptr<IRemoteObject>& token,
        const AAFwk::Want& want, const AAFwk::StartOptions& startOptions, int32_t& errorNum) override;
    ErrCode StopNativeConnectionFromExtension(int32_t connectionId, int32_t& errorNum) override;
    int32_t Dump(int fd, const std::vector<std::u16string>& args) override;

protected:
    void OnStart(const SystemAbilityOnDemandReason& startReason) override;
    void OnStop() override;

private:
    bool Init(const SystemAbilityOnDemandReason& startReason);
    bool registerToService_ = false;

    std::shared_ptr<WebNativeMessagingManager> manager_;
};
} // namespace OHOS::NWeb
#endif // NWEB_WEB_NATIVE_MESSAGING_SERVICE_H

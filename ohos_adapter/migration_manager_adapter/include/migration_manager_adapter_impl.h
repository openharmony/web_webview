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

#ifndef MIGRATION_MANAGER_ADAPTER_IMPL_H
#define MIGRATION_MANAGER_ADAPTER_IMPL_H

#include <map>
#include <mutex>

#include "ability_connect_callback_interface.h"
#include "ability_connect_callback_stub.h"
#include "migration_manager_adapter.h"

namespace OHOS::NWeb {

class MigrationManagerAdapterImpl : public MigrationManagerAdapter {
public:
    MigrationManagerAdapterImpl() = default;
    ~MigrationManagerAdapterImpl() override = default;
    uint32_t RegisterMigrationListener(std::shared_ptr<MigrationListenerAdapter> listener) override;
    void SetMigrationParam(
        const std::string& bundleName, const std::string& abilityName, const std::string& token) override;
    bool SendMigrationRequest(std::shared_ptr<std::string> jsonData) override;

    class MigrationListenerAdapterImpl : public OHOS::AAFwk::AbilityConnectionStub {
    public:
        MigrationListenerAdapterImpl(std::shared_ptr<MigrationListenerAdapter> listener) : listener_(listener) {}
        virtual ~MigrationListenerAdapterImpl() = default;
        void OnAbilityConnectDone(
            const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int resultCode) override;
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override;
        void OnMigrationReply(int32_t errorCode, int32_t succussCount, const std::vector<int32_t>& errorIndex,
            const std::vector<int32_t>& codeList);
        void SetJsonData(const std::string& jsonData);
        void SetInterfaceToken(const std::string& token);
        friend class MigrationManagerAdapterImpl;

    private:
        std::shared_ptr<MigrationListenerAdapter> listener_;
        std::string jsonData_;
        std::string token_;
        std::mutex mutex_;
    };

private:
    static std::atomic<bool> isConnectSystemUI_;
    sptr<MigrationListenerAdapterImpl> callback_;
    std::string jsonUrl_;
    std::string abilityName_;
    std::string bundleName_;
};
} // namespace OHOS::NWeb

#endif
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

#include "ohos_adapter/bridge/ark_migration_manager_adapter_impl.h"

#include "ohos_adapter/bridge/ark_fold_status_listener_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_migration_listener_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMigrationManagerAdapterImpl::ArkMigrationManagerAdapterImpl(std::shared_ptr<OHOS::NWeb::MigrationManagerAdapter> ref)
    : real_(ref)
{}

void ArkMigrationManagerAdapterImpl::SetMigrationParam(
    const ArkWebString& bundleName, const ArkWebString& abilityName, const ArkWebString& token)
{
    std::string str_bundle = ArkWebStringStructToClass(bundleName);
    std::string str_ability = ArkWebStringStructToClass(abilityName);
    std::string str_token = ArkWebStringStructToClass(token);
    real_->SetMigrationParam(str_bundle, str_ability, str_token);
}

bool ArkMigrationManagerAdapterImpl::SendMigrationRequest(void* jsonData)
{
    std::shared_ptr<std::string>* temp = static_cast<std::shared_ptr<std::string>*>(jsonData);
    return real_->SendMigrationRequest(*temp);
}

uint32_t ArkMigrationManagerAdapterImpl::RegisterMigrationListener(ArkWebRefPtr<ArkMigrationListenerAdapter> listener)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->RegisterMigrationListener(nullptr);
    }

    return real_->RegisterMigrationListener(std::make_shared<ArkMigrationListenerAdapterWrapper>(listener));
}

} // namespace OHOS::ArkWeb

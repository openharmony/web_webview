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

#include "ohos_adapter/bridge/ark_migration_manager_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_migration_listener_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMigrationManagerAdapterWrapper::ArkMigrationManagerAdapterWrapper(ArkWebRefPtr<ArkMigrationManagerAdapter> ref)
    : ctocpp_(ref)
{}

void ArkMigrationManagerAdapterWrapper::SetMigrationParam(
    const std::string& bundleName, const std::string& abilityName, const std::string& token)
{
    ArkWebString ark_str_bundle = ArkWebStringClassToStruct(bundleName);
    ArkWebString ark_str_ability = ArkWebStringClassToStruct(abilityName);
    ArkWebString ark_str_token = ArkWebStringClassToStruct(token);
    ctocpp_->SetMigrationParam(ark_str_bundle, ark_str_ability, ark_str_token);

    ArkWebStringStructRelease(ark_str_bundle);
    ArkWebStringStructRelease(ark_str_ability);
    ArkWebStringStructRelease(ark_str_token);
}

bool ArkMigrationManagerAdapterWrapper::SendMigrationRequest(std::shared_ptr<std::string> jsonData)
{
    return ctocpp_->SendMigrationRequest((void*)(&jsonData));
}

uint32_t ArkMigrationManagerAdapterWrapper::RegisterMigrationListener(
    std::shared_ptr<OHOS::NWeb::MigrationListenerAdapter> listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->RegisterMigrationListener(nullptr);
    }

    return ctocpp_->RegisterMigrationListener(new ArkMigrationListenerAdapterImpl(listener));
}
} // namespace OHOS::ArkWeb

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

#ifndef ARK_MIGRATION_LISTENER_ADAPTER_WRAPPER_H
#define ARK_MIGRATION_LISTENER_ADAPTER_WRAPPER_H
#pragma once

#include "migration_manager_adapter.h"
#include "ohos_adapter/include/ark_migration_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkMigrationListenerAdapterWrapper : public OHOS::NWeb::MigrationListenerAdapter {
public:
    explicit ArkMigrationListenerAdapterWrapper(ArkWebRefPtr<ArkMigrationListenerAdapter>);

    void OnMigrationReply(int32_t errorCode, int32_t succussCount, const std::vector<int32_t>& errorIndex,
        const std::vector<int32_t>& codeIndex) override;

private:
    ArkWebRefPtr<ArkMigrationListenerAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MIGRATION_LISTENER_ADAPTER_WRAPPER_H

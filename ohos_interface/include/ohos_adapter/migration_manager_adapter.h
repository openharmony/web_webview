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

#ifndef MIGRATION_MANAGER_ADAPTER_H
#define MIGRATION_MANAGER_ADAPTER_H

#include <memory>

namespace OHOS::NWeb {

using ListenerId = uint32_t;

class MigrationListenerAdapter {
public:
    MigrationListenerAdapter() = default;

    virtual ~MigrationListenerAdapter() = default;

    virtual void OnMigrationReply(int32_t errorCode, int32_t succussCount, const std::vector<int32_t>& errorIndex,
        const std::vector<int32_t>& codeList) = 0;
};

class MigrationManagerAdapter {
public:
    MigrationManagerAdapter() = default;

    virtual ~MigrationManagerAdapter() = default;

    virtual void SetMigrationParam(
        const std::string& bundleName, const std::string& abilityName, const std::string& token) = 0;

    virtual bool SendMigrationRequest(std::shared_ptr<std::string> jsonData) = 0;

    virtual ListenerId RegisterMigrationListener(std::shared_ptr<MigrationListenerAdapter> listener) = 0;
};

} // namespace OHOS::NWeb

#endif // MIGRATION_MANAGER_ADAPTER_H

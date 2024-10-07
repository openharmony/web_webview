/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_WEB_PERMISSION_DATA_BASE_ADAPTER_IMPL_H
#define OHOS_WEB_PERMISSION_DATA_BASE_ADAPTER_IMPL_H

#include "ohos_web_data_base_adapter.h"

#include <string>

#include <database/rdb/relational_store.h>
#include <database/rdb/relational_store_error_code.h>
#include <AbilityKit/ability_runtime/context_constant.h>
#include <AbilityKit/ability_runtime/ability_runtime_common.h>

namespace OHOS::NWeb {

class OhosWebPermissionDataBaseAdapterImpl : public OhosWebPermissionDataBaseAdapter {
public:
    static OhosWebPermissionDataBaseAdapterImpl& GetInstance(const std::string& cachePath = "");

    ~OhosWebPermissionDataBaseAdapterImpl() override;

    bool ExistPermissionByOrigin(const std::string& origin, const WebPermissionType& key) override;

    bool GetPermissionResultByOrigin(const std::string& origin, const WebPermissionType& key,
        bool &result) override;

    void SetPermissionByOrigin(const std::string& origin, const WebPermissionType& key, bool result) override;

    void ClearPermissionByOrigin(const std::string& origin, const WebPermissionType& key) override;

    void ClearAllPermission(const WebPermissionType& key) override;

    void GetOriginsByPermission(const WebPermissionType& key, std::vector<std::string>& origins) override;

private:
    OhosWebPermissionDataBaseAdapterImpl(const std::string& cachePath);

    OhosWebPermissionDataBaseAdapterImpl(const OhosWebPermissionDataBaseAdapterImpl& other) = delete;

    OhosWebPermissionDataBaseAdapterImpl& operator=(const OhosWebPermissionDataBaseAdapterImpl&) = delete;

    std::string KeyToTableName(const WebPermissionType& key) const;

    Rdb_SecurityArea GetAreaMode(AbilityRuntime_AreaMode areaMode);

    void GetOrOpen(const OH_Rdb_Config *config, int *errCode);

    OH_Rdb_Store *rdbStore_;
};
} // namespace
#endif // OHOS_WEB_PERMISSION_DATA_BASE_ADAPTER_IMPL_H

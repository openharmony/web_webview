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

#ifndef OHOS_WEB_DATA_BASE_ADAPTER_IMPL_H
#define OHOS_WEB_DATA_BASE_ADAPTER_IMPL_H

#include "ohos_web_data_base_adapter.h"

#include <string>

#include <database/rdb/relational_store.h>
#include <database/rdb/relational_store_error_code.h>
#include <AbilityKit/ability_runtime/context_constant.h>
#include <AbilityKit/ability_runtime/ability_runtime_common.h>

namespace OHOS::NWeb {

class OhosWebDataBaseAdapterImpl : public OhosWebDataBaseAdapter {
public:
    static OhosWebDataBaseAdapterImpl& GetInstance();

    ~OhosWebDataBaseAdapterImpl() override;

    bool ExistHttpAuthCredentials() override;

    void DeleteHttpAuthCredentials() override;

    void SaveHttpAuthCredentials(const std::string& host, const std::string& realm,
        const std::string& username, const char* password) override;

    void GetHttpAuthCredentials(const std::string& host, const std::string& realm,
        std::string& username, char* password, uint32_t passwordSize) override;


private:
    OhosWebDataBaseAdapterImpl();

    OhosWebDataBaseAdapterImpl(const OhosWebDataBaseAdapterImpl& other) = delete;

    OhosWebDataBaseAdapterImpl& operator=(const OhosWebDataBaseAdapterImpl&) = delete;

    Rdb_SecurityArea GetAreaMode(AbilityRuntime_AreaMode areaMode);

    void GetOrOpen(const OH_Rdb_Config *config, int *errCode);

    OH_Rdb_Store *rdbStore_;
};
} // namespace
#endif // OHOS_WEB_DATA_BASE_ADAPTER_IMPL_H

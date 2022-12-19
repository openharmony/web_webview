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

#ifndef OHOS_WEB_DNS_DATA_BASE_ADAPTER_IMPL_H
#define OHOS_WEB_DNS_DATA_BASE_ADAPTER_IMPL_H

#include "ohos_web_data_base_adapter.h"

#include <string>

#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "rdb_store_config.h"
#include "rdb_types.h"

namespace OHOS::NWeb {
    class DnsDataBaseRdbOpenCallBack : public OHOS::NativeRdb::RdbOpenCallback {
    public:
        int32_t OnCreate(OHOS::NativeRdb::RdbStore& rdbStore) override;

        int32_t OnUpgrade(OHOS::NativeRdb::RdbStore& rdbStore, int32_t currentVersion, int32_t targetVersion) override;
    };

    class OhosWebDnsDataBaseAdapterImpl : public OhosWebDnsDataBaseAdapter {
    public:
        static OhosWebDnsDataBaseAdapterImpl& GetInstance();

        ~OhosWebDnsDataBaseAdapterImpl() override = default;

        bool ExistHostname(const std::string& hostname) const override;

        void InsertHostname(const std::string& hostname) override;

        void GetHostnames(std::vector<std::string>& hostnames) const override;

        void ClearAllHostname() override;

    private:
        OhosWebDnsDataBaseAdapterImpl();

        OhosWebDnsDataBaseAdapterImpl(const OhosWebDnsDataBaseAdapterImpl& other) = delete;

        OhosWebDnsDataBaseAdapterImpl& operator=(const OhosWebDnsDataBaseAdapterImpl&) = delete;

        std::shared_ptr<OHOS::NativeRdb::RdbStore> CreateDataBase(const std::string& dataBeseName,
                                                                  OHOS::NativeRdb::RdbOpenCallback& callBack);

        std::shared_ptr<OHOS::NativeRdb::RdbStore> rdbStore_;
    };
} // namespace
#endif // OHOS_WEB_DNS_DATA_BASE_ADAPTER_IMPL_H
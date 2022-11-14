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

#include "databaseoncreate_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "ohos_web_data_base_adapter_impl.h"
#include "sqlite_database_utils.h"

using namespace OHOS::NWeb;
using namespace OHOS::NativeRdb;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }
        std::string name = "web_test.db";
        std::string bundleName = "com.example";
        std::string databaseDir = "/data";
        int32_t errorCode = E_OK;
        std::string realPath = SqliteDatabaseUtils::GetDefaultDatabasePath(databaseDir, name, errorCode);
        RdbStoreConfig config("");
        config.SetPath(std::move(realPath));
        config.SetBundleName(bundleName);
        config.SetName(std::move(name));
        config.SetArea(1);
        errorCode = NativeRdb::E_OK;
        DataBaseRdbOpenCallBack callBack;
        auto rdbStore = NativeRdb::RdbHelper::GetRdbStore(config, 1, callBack, errorCode);
        callBack.OnCreate(*(rdbStore.get()));
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

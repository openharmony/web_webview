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

#include "pmsdatabaseonupgrade_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "ohos_web_permission_data_base_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool PmsDatabaseOnUpgradeFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    NativeRdb::RdbStoreConfig config("");
    int32_t errorCode = NativeRdb::E_OK;
    PermissionDataBaseRdbOpenCallBack callBack;
    auto rdbStore = NativeRdb::RdbHelper::GetRdbStore(config, 1, callBack, errorCode);
    int32_t currentVersion = static_cast<int32_t>(size);
    int32_t targetVersion = static_cast<int32_t>(size);
    callBack.OnUpgrade(*(rdbStore.get()), currentVersion, targetVersion);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::PmsDatabaseOnUpgradeFuzzTest(data, size);
    return 0;
}

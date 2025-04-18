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

#include "getoriginbypermission_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "ohos_web_permission_data_base_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool GetOriginByPermissionFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    WebPermissionType key = WebPermissionType::GEOLOCATION;
    std::vector<std::string> origins;
    std::string origin1((const char*)data, size);
    std::string origin2((const char*)data, size);
    origins.push_back(origin1);
    origins.push_back(origin2);
    OhosWebPermissionDataBaseAdapterImpl::GetInstance().GetOriginsByPermission(key, origins);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GetOriginByPermissionFuzzTest(data, size);
    return 0;
}

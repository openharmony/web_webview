/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "accesstoken_fuzzer.h"
#include "access_token_adapter_impl.h"
#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 125;
bool AccessTokenAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int))) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string permissionName = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);

    auto& instance = AccessTokenAdapterImpl::GetInstance();
    bool result = instance.VerifyAccessToken(permissionName);

    return result;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AccessTokenAdapterFuzzTest(data, size);
    return 0;
}
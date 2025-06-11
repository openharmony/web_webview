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

#include "getusercertdata_fuzzer.h"
#include <fuzzer/FuzzedDataProvider.h>

#include "cert_mgr_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_ARRAY_SIZE = 10;
bool GetUserCertDataFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    const size_t arraySize = dataProvider.ConsumeIntegralInRange<size_t>(1, MAX_ARRAY_SIZE);
    uint8_t* certData = new uint8_t[arraySize];
    for (size_t i = 0; i < arraySize; ++i) {
        certData[i] = dataProvider.ConsumeIntegral<uint8_t>();
    }
    CertManagerAdapterImpl cerImpl;
    uint32_t certSum = cerImpl.GetUserRootCertSum();
    cerImpl.GetUserRootCertData(certSum, const_cast<uint8_t*>(data));

    delete[] certData;
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GetUserCertDataFuzzTest(data, size);
    return 0;
}

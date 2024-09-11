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

#include "keystoreadapterimpl_fuzzer.h"

#include <securec.h>

#include "keystore_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool ApplyKeystoreAdapterImplFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return false;
    }
    struct HksParamSet* paramSet;
    struct HksParam decryptParams[] = { { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC } };
    int32_t result = KeystoreAdapterImpl::GetInstance().InitParamSet(
        nullptr, decryptParams, sizeof(decryptParams) / sizeof(HksParam));
    result =
        KeystoreAdapterImpl::GetInstance().InitParamSet(&paramSet, nullptr, sizeof(decryptParams) / sizeof(HksParam));
    result = KeystoreAdapterImpl::GetInstance().InitParamSet(
        &paramSet, decryptParams, sizeof(decryptParams) / sizeof(HksParam));
    std::string alias = "test";
    std::string plainData = "web_test";
    std::string encryptString = KeystoreAdapterImpl::GetInstance().EncryptKey(alias, plainData);
    std::string DecryptString = KeystoreAdapterImpl::GetInstance().DecryptKey(alias, encryptString);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ApplyKeystoreAdapterImplFuzzTest(data, size);
    return 0;
}

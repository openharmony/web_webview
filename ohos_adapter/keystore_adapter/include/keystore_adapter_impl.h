/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef KEYSTORE_ADAPTER_IMPL_H
#define KEYSTORE_ADAPTER_IMPL_H

#include "keystore_adapter.h"
#include "hks_param.h"
#include "hks_api.h"
#include "hks_type.h"

namespace OHOS::NWeb {

class KeystoreAdapterImpl : public KeystoreAdapter {
public:
    static KeystoreAdapterImpl& GetInstance();

    ~KeystoreAdapterImpl() override = default;

    std::string EncryptKey(const std::string alias, const std::string plainData) override;
    std::string DecryptKey(const std::string alias, const std::string encryptedData) override;
    int32_t InitParamSet(
        struct HksParamSet **paramSet,
        const struct HksParam *params,
        uint32_t paramCount);

private:
    KeystoreAdapterImpl() = default;

    KeystoreAdapterImpl(const KeystoreAdapterImpl& other) = delete;

    KeystoreAdapterImpl& operator=(const KeystoreAdapterImpl&) = delete;
};

}  // namespace OHOS::NWeb

#endif  // KEYSTORE_ADAPTER_IMPL_H
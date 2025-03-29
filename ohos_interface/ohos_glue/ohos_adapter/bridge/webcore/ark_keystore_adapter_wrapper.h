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

#ifndef ARK_KEYSTORE_ADAPTER_WRAPPER_H
#define ARK_KEYSTORE_ADAPTER_WRAPPER_H
#pragma once

#include "keystore_adapter.h"
#include "ohos_adapter/include/ark_keystore_adapter.h"

namespace OHOS::ArkWeb {

class ArkKeystoreAdapterWrapper : public OHOS::NWeb::KeystoreAdapter {
public:
    explicit ArkKeystoreAdapterWrapper(ArkWebRefPtr<ArkKeystoreAdapter>);

    std::string EncryptKey(const std::string alias, const std::string plainData) override;
    std::string DecryptKey(const std::string alis, const std::string encryptedData) override;
    std::string AssetQuery(const std::string assetHandle) override;

private:
    ArkWebRefPtr<ArkKeystoreAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_KEYSTORE_ADAPTER_WRAPPER_H

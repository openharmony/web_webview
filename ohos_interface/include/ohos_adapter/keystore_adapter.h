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

#ifndef KEYSTORE_ADAPTER_H
#define KEYSTORE_ADAPTER_H

#include <cstdint>
#include <string>

namespace OHOS::NWeb {

class KeystoreAdapter {
public:
    KeystoreAdapter() = default;

    virtual ~KeystoreAdapter() = default;

    virtual std::string EncryptKey(const std::string alias, const std::string plainData) = 0;
    virtual std::string DecryptKey(const std::string alis, const std::string encryptedData) = 0;
    virtual std::string AssetQuery(const std::string assetHandle) = 0;
};

} // namespace OHOS::NWeb

#endif // KEYSTORE_ADAPTER_H
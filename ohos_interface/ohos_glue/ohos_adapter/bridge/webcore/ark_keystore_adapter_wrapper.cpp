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

#include "ohos_adapter/bridge/ark_keystore_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkKeystoreAdapterWrapper::ArkKeystoreAdapterWrapper(ArkWebRefPtr<ArkKeystoreAdapter> ref) : ctocpp_(ref) {}

std::string ArkKeystoreAdapterWrapper::EncryptKey(const std::string alias, const std::string plainData)
{
    if (!ctocpp_) {
        return "";
    }

    ArkWebString ark_alias = ArkWebStringClassToStruct(alias);
    ArkWebString ark_plainData = ArkWebStringClassToStruct(plainData);
    ArkWebString ark_result = ctocpp_->EncryptKey(ark_alias, ark_plainData);

    std::string result = ArkWebStringStructToClass(ark_result);
    ArkWebStringStructRelease(ark_alias);
    ArkWebStringStructRelease(ark_plainData);
    ArkWebStringStructRelease(ark_result);

    return result;
}

std::string ArkKeystoreAdapterWrapper::DecryptKey(const std::string alias, const std::string encryptedData)
{
    if (!ctocpp_) {
        return "";
    }

    ArkWebString ark_alias = ArkWebStringClassToStruct(alias);
    ArkWebString ark_encryptedData = ArkWebStringClassToStruct(encryptedData);
    ArkWebString ark_result = ctocpp_->DecryptKey(ark_alias, ark_encryptedData);

    std::string result = ArkWebStringStructToClass(ark_result);
    ArkWebStringStructRelease(ark_alias);
    ArkWebStringStructRelease(ark_encryptedData);
    ArkWebStringStructRelease(ark_result);

    return result;
}

std::string ArkKeystoreAdapterWrapper::AssetQuery(const std::string assetHandle)
{
    if (!ctocpp_) {
        return "";
    }

    ArkWebString ark_assetHandle = ArkWebStringClassToStruct(assetHandle);
    ArkWebString ark_result = ctocpp_->AssetQuery(ark_assetHandle);

    std::string result = ArkWebStringStructToClass(ark_result);
    ArkWebStringStructRelease(ark_assetHandle);
    ArkWebStringStructRelease(ark_result);

    return result;
}

} // namespace OHOS::ArkWeb

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

#include "ohos_adapter/bridge/ark_keystore_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkKeystoreAdapterImpl::ArkKeystoreAdapterImpl(NWeb::KeystoreAdapter& ref) : real_(ref) {}

ArkWebString ArkKeystoreAdapterImpl::EncryptKey(const ArkWebString& alias, const ArkWebString& plainData)
{
    std::string s_alias = ArkWebStringStructToClass(alias);
    std::string s_plainData = ArkWebStringStructToClass(plainData);

    std::string result = real_.EncryptKey(s_alias, s_plainData);

    return ArkWebStringClassToStruct(result);
}

ArkWebString ArkKeystoreAdapterImpl::DecryptKey(const ArkWebString& alias, const ArkWebString& encryptedData)
{
    std::string s_alias = ArkWebStringStructToClass(alias);
    std::string s_encryptedData = ArkWebStringStructToClass(encryptedData);

    std::string result = real_.DecryptKey(s_alias, s_encryptedData);

    return ArkWebStringClassToStruct(result);
}

ArkWebString ArkKeystoreAdapterImpl::AssetQuery(const ArkWebString& assetHandle)
{
    std::string s_assetHandle = ArkWebStringStructToClass(assetHandle);
    std::string result = real_.AssetQuery(s_assetHandle);

    return ArkWebStringClassToStruct(result);
}

} // namespace OHOS::ArkWeb

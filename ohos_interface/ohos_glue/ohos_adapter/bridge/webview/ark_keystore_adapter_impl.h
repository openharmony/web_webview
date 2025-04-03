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

#ifndef ARK_KEYSTORE_ADAPTER_IMPL_H
#define ARK_KEYSTORE_ADAPTER_IMPL_H
#pragma once

#include "keystore_adapter.h"
#include "ohos_adapter/include/ark_keystore_adapter.h"

namespace OHOS::ArkWeb {

class ArkKeystoreAdapterImpl : public ArkKeystoreAdapter {
public:
    explicit ArkKeystoreAdapterImpl(NWeb::KeystoreAdapter&);

    ArkWebString EncryptKey(const ArkWebString& alias, const ArkWebString& plainData) override;

    ArkWebString DecryptKey(const ArkWebString& alis, const ArkWebString& encryptedData) override;

    ArkWebString AssetQuery(const ArkWebString& assetHandle) override;

private:
    NWeb::KeystoreAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkKeystoreAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_KEYSTORE_ADAPTER_IMPL_H

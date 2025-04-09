/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_DRM_CALLBACK_ADAPTER_IMPL_H
#define ARK_DRM_CALLBACK_ADAPTER_IMPL_H
#pragma once

#include "drm_adapter.h"
#include "ohos_adapter/include/ark_drm_adapter.h"

namespace OHOS::ArkWeb {

class ArkDrmCallbackAdapterImpl : public ArkDrmCallbackAdapter {
public:
    ArkDrmCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::DrmCallbackAdapter>);

    void OnSessionMessage(const ArkWebString& sessionId, int32_t& type, const ArkWebUint8Vector& message) override;

    void OnProvisionRequest(const ArkWebString& defaultUrl, const ArkWebString& requestData) override;

    void OnProvisioningComplete(bool success) override;

    void OnMediaKeySessionReady(void* session) override;

    void OnPromiseRejected(uint32_t promiseId, const ArkWebString& errorMessage) override;

    void OnPromiseResolved(uint32_t promiseId) override;

    void OnPromiseResolvedWithSession(uint32_t promiseId, const ArkWebString& sessionId) override;

    void OnSessionClosed(const ArkWebString& sessionId) override;

    void OnSessionKeysChange(const ArkWebString& sessionId, const ArkWebStringVector& keyIdArray,
        const ArkWebUint32Vector& statusArray, bool hasAdditionalUsableKey, bool isKeyRelease) override;

    void OnSessionExpirationUpdate(const ArkWebString& sessionId, uint64_t expirationTime) override;

    void OnStorageProvisioned() override;

    void OnStorageSaveInfo(const ArkWebUint8Vector& ketSetId, const ArkWebString& mimeType,
        const ArkWebString& sessionId, int32_t keyType) override;

    void OnStorageLoadInfo(const ArkWebString& sessionId) override;

    void OnStorageClearInfoForKeyRelease(const ArkWebString& sessionId) override;

    void OnStorageClearInfoForLoadFail(const ArkWebString& sessionId) override;

    void OnMediaLicenseReady(bool success) override;

private:
    std::shared_ptr<OHOS::NWeb::DrmCallbackAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkDrmCallbackAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_DRM_CALLBACK_ADAPTER_IMPL_H

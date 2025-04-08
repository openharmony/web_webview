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

#ifndef ARK_DRM_CALLBACK_ADAPTER_WRAPPER_H
#define ARK_DRM_CALLBACK_ADAPTER_WRAPPER_H
#pragma once

#include "drm_adapter.h"
#include "ohos_adapter/include/ark_drm_adapter.h"

namespace OHOS::ArkWeb {

class ArkDrmCallbackAdapterWrapper : public OHOS::NWeb::DrmCallbackAdapter {
public:
    ArkDrmCallbackAdapterWrapper(ArkWebRefPtr<ArkDrmCallbackAdapter>);

    void OnSessionMessage(const std::string& sessionId, int32_t& type, const std::vector<uint8_t>& message) override;

    void OnProvisionRequest(const std::string& defaultUrl, const std::string& requestData) override;

    void OnProvisioningComplete(bool success) override;

    void OnMediaKeySessionReady(void* session) override;

    void OnPromiseRejected(uint32_t promiseId, const std::string& errorMessage) override;

    void OnPromiseResolved(uint32_t promiseId) override;

    void OnPromiseResolvedWithSession(uint32_t promiseId, const std::string& sessionId) override;

    void OnSessionClosed(const std::string& sessionId) override;

    void OnSessionKeysChange(const std::string& sessionId, const std::vector<std::string>& keyIdArray,
        const std::vector<uint32_t>& statusArray, bool hasAdditionalUsableKey, bool isKeyRelease) override;

    void OnSessionExpirationUpdate(const std::string& sessionId, uint64_t expirationTime) override;

    void OnStorageProvisioned() override;

    void OnStorageSaveInfo(const std::vector<uint8_t>& ketSetId, const std::string& mimeType,
        const std::string& sessionId, int32_t keyType) override;

    void OnStorageLoadInfo(const std::string& sessionId) override;

    void OnStorageClearInfoForKeyRelease(const std::string& sessionId) override;

    void OnStorageClearInfoForLoadFail(const std::string& sessionId) override;

    void OnMediaLicenseReady(bool success) override;

private:
    ArkWebRefPtr<ArkDrmCallbackAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DRM_CALLBACK_ADAPTER_WRAPPER_H

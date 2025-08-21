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

#ifndef DRM_ADAPTER_H
#define DRM_ADAPTER_H

#include <memory>
#include <string>

namespace OHOS::NWeb {

using OHOSMediaKeySession = void*;

class DrmCallbackAdapter {
public:
    virtual ~DrmCallbackAdapter() = default;

    virtual void OnSessionMessage(const std::string& sessionId, int32_t& type, const std::vector<uint8_t>& message) = 0;

    virtual void OnProvisionRequest(const std::string& defaultUrl, const std::string& requestData) = 0;

    virtual void OnProvisioningComplete(bool success) = 0;

    virtual void OnMediaKeySessionReady(void* session) = 0;

    virtual void OnPromiseRejected(uint32_t promiseId, const std::string& errorMessage) = 0;

    virtual void OnPromiseResolved(uint32_t promiseId) = 0;

    virtual void OnPromiseResolvedWithSession(uint32_t promiseId, const std::string& sessionId) = 0;

    virtual void OnSessionClosed(const std::string& sessionId) = 0;

    virtual void OnSessionKeysChange(const std::string& sessionId, const std::vector<std::string>& keyIdArray,
        const std::vector<uint32_t>& statusArray, bool hasAdditionalUsableKey, bool isKeyRelease) = 0;

    virtual void OnSessionExpirationUpdate(const std::string& sessionId, uint64_t expirationTime) = 0;

    virtual void OnStorageProvisioned() = 0;

    virtual void OnStorageSaveInfo(const std::vector<uint8_t>& ketSetId, const std::string& mimeType,
        const std::string& sessionId, int32_t keyType) = 0;

    virtual void OnStorageLoadInfo(const std::string& sessionId) = 0;

    virtual void OnStorageClearInfoForKeyRelease(const std::string& sessionId) = 0;

    virtual void OnStorageClearInfoForLoadFail(const std::string& sessionId) = 0;

    virtual void OnMediaLicenseReady(bool success) {}
};

class DrmAdapter {
public:
    DrmAdapter() = default;

    virtual ~DrmAdapter() = default;

    virtual bool IsSupported(const std::string& name) = 0;

    virtual bool IsSupported2(const std::string& name, const std::string& mimeType) = 0;

    virtual bool IsSupported3(const std::string& name, const std::string& mimeType, int32_t level) = 0;

    virtual std::vector<uint8_t> GetUUID(const std::string& name) = 0;

    virtual void StorageProvisionedResult(bool result) = 0;

    virtual void StorageSaveInfoResult(bool result, int32_t type) = 0;

    virtual void StorageLoadInfoResult(const std::string& sessionId, const std::vector<uint8_t>& keySetId,
        const std::string& mimeType, uint32_t keyType) = 0;

    virtual void StorageClearInfoResult(bool result, int32_t type) = 0;

    virtual int32_t ReleaseMediaKeySystem() = 0;

    virtual int32_t ReleaseMediaKeySession() = 0;

    virtual int32_t CreateKeySystem(const std::string& name, const std::string& origin, int32_t securityLevel) = 0;

    virtual int32_t SetConfigurationString(const std::string& configName, const std::string& value) = 0;

    virtual int32_t GetConfigurationString(const std::string& configName, char* value, int32_t valueLen) = 0;

    virtual int32_t SetConfigurationByteArray(
        const std::string& configName, const uint8_t* value, int32_t valueLen) = 0;

    virtual int32_t GetConfigurationByteArray(const std::string& configName, uint8_t* value, int32_t* valueLen) = 0;

    virtual int32_t GetMaxContentProtectionLevel(int32_t& level) = 0;

    virtual int32_t ProcessKeySystemResponse(const std::string& response, bool isResponseReceived) = 0;

    virtual int32_t GetCertificateStatus(int32_t& certStatus) = 0;

    virtual int32_t RegistDrmCallback(std::shared_ptr<DrmCallbackAdapter> callbackAdapter) = 0;

    virtual int32_t UpdateSession(uint32_t promiseId, const std::string& sessionId, std::vector<uint8_t> response) = 0;

    virtual int32_t CloseSession(uint32_t promiseId, const std::string& sessionId) = 0;

    virtual int32_t RemoveSession(uint32_t promiseId, const std::string& sessionId) = 0;

    virtual int32_t LoadSession(uint32_t promiseId, const std::string& sessionId) = 0;

    virtual int32_t ClearMediaKeys() = 0;

    virtual int32_t GetSecurityLevel() = 0;

    virtual int32_t RequireSecureDecoderModule(const std::string& mimeType, bool& status) = 0;

    virtual int32_t GenerateMediaKeyRequest(const std::string& sessionId, int32_t type, int32_t initDataLen,
        const std::vector<uint8_t>& initData, const std::string& mimeType, uint32_t promiseId) = 0;
};
} // namespace OHOS::NWeb
#endif // DRM_ADAPTER_H
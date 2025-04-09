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

#ifndef ARK_DRM_ADAPTER_H
#define ARK_DRM_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkDrmCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnSessionMessage(const ArkWebString& sessionId, int32_t& type, const ArkWebUint8Vector& message) = 0;

    /*--ark web()--*/
    virtual void OnProvisionRequest(const ArkWebString& defaultUrl, const ArkWebString& requestData) = 0;

    /*--ark web()--*/
    virtual void OnProvisioningComplete(bool success) = 0;

    /*--ark web()--*/
    virtual void OnMediaKeySessionReady(void* session) = 0;

    /*--ark web()--*/
    virtual void OnPromiseRejected(uint32_t promiseId, const ArkWebString& errorMessage) = 0;

    /*--ark web()--*/
    virtual void OnPromiseResolved(uint32_t promiseId) = 0;

    /*--ark web()--*/
    virtual void OnPromiseResolvedWithSession(uint32_t promiseId, const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual void OnSessionClosed(const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual void OnSessionKeysChange(const ArkWebString& sessionId, const ArkWebStringVector& keyIdArray,
        const ArkWebUint32Vector& statusArray, bool hasAdditionalUsableKey, bool isKeyRelease) = 0;

    /*--ark web()--*/
    virtual void OnSessionExpirationUpdate(const ArkWebString& sessionId, uint64_t expirationTime) = 0;

    /*--ark web()--*/
    virtual void OnStorageProvisioned() = 0;

    /*--ark web()--*/
    virtual void OnStorageSaveInfo(const ArkWebUint8Vector& ketSetId, const ArkWebString& mimeType,
        const ArkWebString& sessionId, int32_t keyType) = 0;

    /*--ark web()--*/
    virtual void OnStorageLoadInfo(const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual void OnStorageClearInfoForKeyRelease(const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual void OnStorageClearInfoForLoadFail(const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual void OnMediaLicenseReady(bool success) = 0;
};

/*--ark web(source=webview)--*/
class ArkDrmAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool IsSupported(const ArkWebString& name) = 0;

    /*--ark web()--*/
    virtual bool IsSupported2(const ArkWebString& name, const ArkWebString& mimeType) = 0;

    /*--ark web()--*/
    virtual bool IsSupported3(const ArkWebString& name, const ArkWebString& mimeType, int32_t level) = 0;

    /*--ark web()--*/
    virtual ArkWebUint8Vector GetUUID(const ArkWebString& name) = 0;

    /*--ark web()--*/
    virtual void StorageProvisionedResult(bool result) = 0;

    /*--ark web()--*/
    virtual void StorageSaveInfoResult(bool result, int32_t type) = 0;

    /*--ark web()--*/
    virtual void StorageLoadInfoResult(const ArkWebString& sessionId, const ArkWebUint8Vector& keySetId,
        const ArkWebString& mimeType, uint32_t keyType) = 0;

    /*--ark web()--*/
    virtual void StorageClearInfoResult(bool result, int32_t type) = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseMediaKeySystem() = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseMediaKeySession() = 0;

    /*--ark web()--*/
    virtual int32_t CreateKeySystem(const ArkWebString& name, const ArkWebString& origin, int32_t securityLevel) = 0;

    /*--ark web()--*/
    virtual int32_t SetConfigurationString(const ArkWebString& configName, const ArkWebString& value) = 0;

    /*--ark web()--*/
    virtual int32_t GetConfigurationString(const ArkWebString& configName, char* value, int32_t valueLen) = 0;

    /*--ark web()--*/
    virtual int32_t SetConfigurationByteArray(
        const ArkWebString& configName, const uint8_t* value, int32_t valueLen) = 0;

    /*--ark web()--*/
    virtual int32_t GetConfigurationByteArray(const ArkWebString& configName, uint8_t* value, int32_t* valueLen) = 0;

    /*--ark web()--*/
    virtual int32_t GetMaxContentProtectionLevel(int32_t& level) = 0;

    /*--ark web()--*/
    virtual int32_t ProcessKeySystemResponse(const ArkWebString& response, bool isResponseReceived) = 0;

    /*--ark web()--*/
    virtual int32_t GetCertificateStatus(int32_t& certStatus) = 0;

    /*--ark web()--*/
    virtual int32_t RegistDrmCallback(ArkWebRefPtr<ArkDrmCallbackAdapter> callbackAdapter) = 0;

    /*--ark web()--*/
    virtual int32_t UpdateSession(uint32_t promiseId, const ArkWebString& sessionId, ArkWebUint8Vector response) = 0;

    /*--ark web()--*/
    virtual int32_t CloseSession(uint32_t promiseId, const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual int32_t RemoveSession(uint32_t promiseId, const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual int32_t LoadSession(uint32_t promiseId, const ArkWebString& sessionId) = 0;

    /*--ark web()--*/
    virtual int32_t ClearMediaKeys() = 0;

    /*--ark web()--*/
    virtual int32_t GetSecurityLevel() = 0;

    /*--ark web()--*/
    virtual int32_t RequireSecureDecoderModule(const ArkWebString& mimeType, bool& status) = 0;

    /*--ark web()--*/
    virtual int32_t GenerateMediaKeyRequest(const ArkWebString& sessionId, int32_t type, int32_t initDataLen,
        const ArkWebUint8Vector& initData, const ArkWebString& mimeType, uint32_t promiseId) = 0;
};
} // namespace OHOS::ArkWeb

#endif // ARK_DRM_ADAPTER_H

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

#include "ohos_adapter/bridge/ark_drm_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {
ArkDrmCallbackAdapterWrapper::ArkDrmCallbackAdapterWrapper(ArkWebRefPtr<ArkDrmCallbackAdapter> ref) : ctocpp_(ref) {}

void ArkDrmCallbackAdapterWrapper::OnSessionMessage(
    const std::string& sessionId, int32_t& type, const std::vector<uint8_t>& message)
{
    ArkWebUint8Vector arkMessage = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(message);
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnSessionMessage(arkSessionId, type, arkMessage);
    ArkWebBasicVectorStructRelease(arkMessage);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnProvisionRequest(const std::string& defaultUrl, const std::string& requestData)
{
    ArkWebString arkDefaultUrl = ArkWebStringClassToStruct(defaultUrl);
    ArkWebString arkRequestData = ArkWebStringClassToStruct(requestData);
    ctocpp_->OnProvisionRequest(arkDefaultUrl, arkRequestData);
    ArkWebStringStructRelease(arkDefaultUrl);
    ArkWebStringStructRelease(arkRequestData);
}

void ArkDrmCallbackAdapterWrapper::OnProvisioningComplete(bool success)
{
    ctocpp_->OnProvisioningComplete(success);
}

void ArkDrmCallbackAdapterWrapper::OnMediaKeySessionReady(void* session)
{
    ctocpp_->OnMediaKeySessionReady(session);
}

void ArkDrmCallbackAdapterWrapper::OnPromiseRejected(uint32_t promiseId, const std::string& errorMessage)
{
    ArkWebString arkErrorMessage = ArkWebStringClassToStruct(errorMessage);
    ctocpp_->OnPromiseRejected(promiseId, arkErrorMessage);
    ArkWebStringStructRelease(arkErrorMessage);
}

void ArkDrmCallbackAdapterWrapper::OnPromiseResolved(uint32_t promiseId)
{
    ctocpp_->OnPromiseResolved(promiseId);
}

void ArkDrmCallbackAdapterWrapper::OnPromiseResolvedWithSession(uint32_t promiseId, const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnPromiseResolvedWithSession(promiseId, arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnSessionClosed(const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnSessionClosed(arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnSessionKeysChange(const std::string& sessionId,
    const std::vector<std::string>& keyIdArray, const std::vector<uint32_t>& statusArray, bool hasAdditionalUsableKey,
    bool isKeyRelease)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ArkWebStringVector arkKeyIdArray = ArkWebStringVectorClassToStruct(keyIdArray);
    ArkWebUint32Vector arkStatusArray = ArkWebBasicVectorClassToStruct<uint32_t, ArkWebUint32Vector>(statusArray);
    ctocpp_->OnSessionKeysChange(arkSessionId, arkKeyIdArray, arkStatusArray, hasAdditionalUsableKey, isKeyRelease);
    ArkWebStringStructRelease(arkSessionId);
    ArkWebStringVectorStructRelease(arkKeyIdArray);
    ArkWebBasicVectorStructRelease(arkStatusArray);
}

void ArkDrmCallbackAdapterWrapper::OnSessionExpirationUpdate(const std::string& sessionId, uint64_t expirationTime)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnSessionExpirationUpdate(arkSessionId, expirationTime);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnStorageProvisioned()
{
    ctocpp_->OnStorageProvisioned();
}

void ArkDrmCallbackAdapterWrapper::OnStorageSaveInfo(
    const std::vector<uint8_t>& ketSetId, const std::string& mimeType, const std::string& sessionId, int32_t keyType)
{
    ArkWebUint8Vector arkKetSetId = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(ketSetId);
    ArkWebString arkMimeType = ArkWebStringClassToStruct(mimeType);
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnStorageSaveInfo(arkKetSetId, arkMimeType, arkSessionId, keyType);
    ArkWebBasicVectorStructRelease(arkKetSetId);
    ArkWebStringStructRelease(arkMimeType);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnStorageLoadInfo(const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnStorageLoadInfo(arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnStorageClearInfoForKeyRelease(const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnStorageClearInfoForKeyRelease(arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnStorageClearInfoForLoadFail(const std::string& sessionId)
{
    ArkWebString arkSessionId = ArkWebStringClassToStruct(sessionId);
    ctocpp_->OnStorageClearInfoForLoadFail(arkSessionId);
    ArkWebStringStructRelease(arkSessionId);
}

void ArkDrmCallbackAdapterWrapper::OnMediaLicenseReady(bool success)
{
    ctocpp_->OnMediaLicenseReady(success);
}
} // namespace OHOS::ArkWeb

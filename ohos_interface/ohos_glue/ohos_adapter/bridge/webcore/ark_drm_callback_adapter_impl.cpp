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

#include "ohos_adapter/bridge/ark_drm_callback_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkDrmCallbackAdapterImpl::ArkDrmCallbackAdapterImpl(std::shared_ptr<OHOS::NWeb::DrmCallbackAdapter> ref) : real_(ref)
{}

void ArkDrmCallbackAdapterImpl::OnSessionMessage(
    const ArkWebString& sessionId, int32_t& type, const ArkWebUint8Vector& msg)
{
    real_->OnSessionMessage(
        ArkWebStringStructToClass(sessionId), type, ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(msg));
}

void ArkDrmCallbackAdapterImpl::OnProvisionRequest(const ArkWebString& defaultUrl, const ArkWebString& requestData)
{
    real_->OnProvisionRequest(ArkWebStringStructToClass(defaultUrl), ArkWebStringStructToClass(requestData));
}

void ArkDrmCallbackAdapterImpl::OnProvisioningComplete(bool success)
{
    real_->OnProvisioningComplete(success);
}

void ArkDrmCallbackAdapterImpl::OnMediaKeySessionReady(void* session)
{
    real_->OnMediaKeySessionReady(session);
}

void ArkDrmCallbackAdapterImpl::OnPromiseRejected(uint32_t promiseId, const ArkWebString& errorMessage)
{
    real_->OnPromiseRejected(promiseId, ArkWebStringStructToClass(errorMessage));
}

void ArkDrmCallbackAdapterImpl::OnPromiseResolved(uint32_t promiseId)
{
    real_->OnPromiseResolved(promiseId);
}

void ArkDrmCallbackAdapterImpl::OnPromiseResolvedWithSession(uint32_t promiseId, const ArkWebString& sessionId)
{
    real_->OnPromiseResolvedWithSession(promiseId, ArkWebStringStructToClass(sessionId));
}

void ArkDrmCallbackAdapterImpl::OnSessionClosed(const ArkWebString& sessionId)
{
    real_->OnSessionClosed(ArkWebStringStructToClass(sessionId));
}

void ArkDrmCallbackAdapterImpl::OnSessionKeysChange(const ArkWebString& sessionId, const ArkWebStringVector& keyIdArray,
    const ArkWebUint32Vector& statusArray, bool hasAdditionalUsableKey, bool isRelease)
{
    real_->OnSessionKeysChange(ArkWebStringStructToClass(sessionId), ArkWebStringVectorStructToClass(keyIdArray),
        ArkWebBasicVectorStructToClass<uint32_t, ArkWebUint32Vector>(statusArray), hasAdditionalUsableKey, isRelease);
}

void ArkDrmCallbackAdapterImpl::OnSessionExpirationUpdate(const ArkWebString& sessionId, uint64_t expirationTime)
{
    real_->OnSessionExpirationUpdate(ArkWebStringStructToClass(sessionId), expirationTime);
}

void ArkDrmCallbackAdapterImpl::OnStorageProvisioned()
{
    real_->OnStorageProvisioned();
}

void ArkDrmCallbackAdapterImpl::OnStorageSaveInfo(
    const ArkWebUint8Vector& ketSetId, const ArkWebString& mimeType, const ArkWebString& sessionId, int32_t keyType)
{
    real_->OnStorageSaveInfo(ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(ketSetId),
        ArkWebStringStructToClass(mimeType), ArkWebStringStructToClass(sessionId), keyType);
}

void ArkDrmCallbackAdapterImpl::OnStorageLoadInfo(const ArkWebString& sessionId)
{
    real_->OnStorageLoadInfo(ArkWebStringStructToClass(sessionId));
}

void ArkDrmCallbackAdapterImpl::OnStorageClearInfoForKeyRelease(const ArkWebString& sessionId)
{
    real_->OnStorageClearInfoForKeyRelease(ArkWebStringStructToClass(sessionId));
}

void ArkDrmCallbackAdapterImpl::OnStorageClearInfoForLoadFail(const ArkWebString& sessionId)
{
    real_->OnStorageClearInfoForLoadFail(ArkWebStringStructToClass(sessionId));
}

void ArkDrmCallbackAdapterImpl::OnMediaLicenseReady(bool success)
{
    real_->OnMediaLicenseReady(success);
}
} // namespace OHOS::ArkWeb

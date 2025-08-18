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

#include "drm_adapter_impl.h"

#include <atomic>
#include <clocale>
#include <cstddef>
#include <cstring.h>
#include <map>
#include <memory>
#include <sstream>
#include <typeinfo>
#include <unistd.h>

#include "native_mediakeysession.h"
#include "native_mediakeysystem.h"

namespace OHOS::NWeb {
std::unordered_map<MediaKeySystem*, std::shared_ptr<DrmCallbackImpl>> DrmAdapterImpl::mediaKeySystemCallbackMap_;
std::unordered_map<MediaKeySession*, std::shared_ptr<DrmCallbackImpl>> DrmAdapterImpl::mediaKeySessionCallbackMap_;
std::mutex DrmAdapterImpl::mediaKeySystemCallbackMapMutex_;
std::mutex DrmAdapterImpl::mediaKeySessionCallbackMutex_;

static std::unordered_map<std::string, uint32_t> KeyStatusMap {
    { "USABLE", static_cast<uint32_t>(KeyStatus::KEY_STATUS_USABLE) },
    { "EXPIRED", static_cast<uint32_t>(KeyStatus::KEY_STATUS_EXPIRED) },
    { "OUTPUT_NOT_ALLOWED", static_cast<uint32_t>(KeyStatus::KEY_STATUS_OUTPUT_NOT_ALLOWED) },
    { "PENDING", static_cast<uint32_t>(KeyStatus::KEY_STATUS_PENDING) },
    { "INTERNAL_ERROR", static_cast<uint32_t>(KeyStatus::KEY_STATUS_INTERNAL_ERROR) },
    { "USABLE_IN_FUTURE", static_cast<uint32_t>(KeyStatus::KEY_STATUS_USABLE_IN_FUTURE) },
};

DRM_ContentProtectionLevel GetContentProtectionLevelFromSecurityLevel(int32_t levelData)
{
    DRM_ContentProtectionLevel contentProtectionLevel = CONTENT_PROTECTION_LEVEL_UNKNOWN;
    switch (levelData) {
        case SECURITY_LEVEL_UNKNOWN:
            contentProtectionLevel = CONTENT_PROTECTION_LEVEL_UNKNOWN;
            break;
        case SECURITY_LEVEL_1:
            contentProtectionLevel = CONTENT_PROTECTION_LEVEL_HW_CRYPTO;
            break;
        case SECURITY_LEVEL_3:
            contentProtectionLevel = CONTENT_PROTECTION_LEVEL_SW_CRYPTO;
            break;
        default:
            break;
    }
    return contentProtectionLevel;
}

int32_t GetSecurityLevelFromContentProtectionLevel(int32_t levelData)
{
    int32_t securityLevel = SECURITY_LEVEL_3;
    switch (levelData) {
        case CONTENT_PROTECTION_LEVEL_UNKNOWN:
            securityLevel = SECURITY_LEVEL_UNKNOWN;
            break;
        case CONTENT_PROTECTION_LEVEL_SW_CRYPTO:
            securityLevel = SECURITY_LEVEL_3;
            break;
        case CONTENT_PROTECTION_LEVEL_HW_CRYPTO:
        case CONTENT_PROTECTION_LEVEL_ENHANCED_HW_CRYPTO:
            securityLevel = SECURITY_LEVEL_1;
            break;
        case CONTENT_PROTECTION_LEVEL_MAX:
        default:
            break;
    }
    return securityLevel;
}

std::string toHexString(const uint8_t* data, size_t length)
{
    static const char hexDigits[] = "0123456789ABCDEF";
    std::string hexString;
    hexString.reserve(length + length);
    for (size_t i = 0; i < length; ++i) {
        hexString.push_back(hexDigits[data[i] >> HEX_OFFSET]);
        hexString.push_back(hexDigits[data[i] & 0x0F]);
    }
    return hexString;
}

bool EndsWithAndRemove(std::string& str, const std::string& suffix)
{
    if (str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0) {
        str.erase(str.size() - suffix.size(), suffix.size());
        return true;
    }
    return false;
}

bool IsValidNumber(const std::string& str)
{
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

DrmCallbackImpl::DrmCallbackImpl(std::shared_ptr<DrmCallbackAdapter> callbackAdapter)
    : callbackAdapter_(callbackAdapter)
{}

void DrmCallbackImpl::OnSessionMessage(const std::string& sessionId, int32_t& type, const std::vector<uint8_t>& message)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnSessionMessage(sessionId, type, message);
    }
}

void DrmCallbackImpl::OnProvisionRequest(const std::string& defaultUrl, const std::string& requestData)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnProvisionRequest(defaultUrl, requestData);
    }
}

void DrmCallbackImpl::OnProvisioningComplete(bool success)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnProvisioningComplete(success);
    }
}

void DrmCallbackImpl::OnMediaKeySessionReady(void* session)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnMediaKeySessionReady(session);
    }
}

void DrmCallbackImpl::OnPromiseRejected(uint32_t promiseId, const std::string& errorMessage)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnPromiseRejected(promiseId, errorMessage);
    }
}

void DrmCallbackImpl::OnPromiseResolved(uint32_t promiseId)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnPromiseResolved(promiseId);
    }
}

void DrmCallbackImpl::OnPromiseResolvedWithSession(uint32_t promiseId, const std::string& sessionId)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnPromiseResolvedWithSession(promiseId, sessionId);
    }
}

void DrmCallbackImpl::OnSessionClosed(const std::string& sessionId)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnSessionClosed(sessionId);
    }
}

void DrmCallbackImpl::OnSessionKeysChange(const std::string& sessionId, const std::vector<std::string>& keyIdArray,
    const std::vector<uint32_t>& statusArray, bool hasAdditionalUsableKey, bool isKeyRelease)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnSessionKeysChange(sessionId, keyIdArray, statusArray, hasAdditionalUsableKey, isKeyRelease);
    }
}

void DrmCallbackImpl::OnSessionExpirationUpdate(const std::string& sessionId, uint64_t expirationTime)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnSessionExpirationUpdate(sessionId, expirationTime);
    }
}

void DrmCallbackImpl::OnStorageProvisioned()
{
    if (callbackAdapter_) {
        callbackAdapter_->OnStorageProvisioned();
    }
}

void DrmCallbackImpl::OnStorageSaveInfo(
    const std::vector<uint8_t>& ketSetId, const std::string& mimeType, const std::string& sessionId, int32_t keyType)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnStorageSaveInfo(ketSetId, mimeType, sessionId, keyType);
    }
}

void DrmCallbackImpl::OnStorageLoadInfo(const std::string& sessionId)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnStorageLoadInfo(sessionId);
    }
}

void DrmCallbackImpl::OnStorageClearInfoForKeyRelease(const std::string& sessionId)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnStorageClearInfoForKeyRelease(sessionId);
    }
}

void DrmCallbackImpl::OnStorageClearInfoForLoadFail(const std::string& sessionId)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnStorageClearInfoForLoadFail(sessionId);
    }
}

void DrmCallbackImpl::OnMediaLicenseReady(bool success)
{
    if (callbackAdapter_) {
        callbackAdapter_->OnMediaLicenseReady(success);
    }
}

void DrmCallbackImpl::UpdateMediaKeySessionInfoMap(MediaKeySession* keySession,
    std::shared_ptr<SessionInfo> sessionInfo)
{
    WVLOG_I("[DRM]DrmCallbackImpl::UpdateMediaKeySessionInfoMap enter.");
    std::lock_guard<std::mutex> lock(mediaKeySessionInfoMutex_);
    mediaKeySessionInfoMap_[keySession] = sessionInfo;
}

std::shared_ptr<SessionInfo> DrmCallbackImpl::GetMediaKeySessionInfo(MediaKeySession* keySession)
{
    WVLOG_I("[DRM]DrmCallbackImpl::GetMediaKeySessionInfo enter.");
    if (keySession == nullptr) {
        WVLOG_E("[DRM]DrmCallbackImpl::GetMediaKeySessionInfo error, keySession is nullptr.");
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(mediaKeySessionInfoMutex_);
    auto iter = mediaKeySessionInfoMap_.find(keySession);
    if (iter != mediaKeySessionInfoMap_.end()) {
        return iter->second;
    }
    WVLOG_I("[DRM]DrmCallbackImpl::GetMediaKeySessionInfo, keySession not found.");
    return nullptr;
}

void DrmCallbackImpl::RemoveMediaKeySessionInfo(MediaKeySession* keySession)
{
    WVLOG_I("[DRM]DrmCallbackImpl::RemoveMediaKeySessionInfo enter.");
    std::lock_guard<std::mutex> lock(mediaKeySessionInfoMutex_);
    mediaKeySessionInfoMap_.erase(keySession);
}

void DrmCallbackImpl::ClearMediaKeySessionInfo()
{
    WVLOG_I("[DRM]DrmCallbackImpl::ClearMediaKeySessionInfo enter.");
    std::lock_guard<std::mutex> lock(mediaKeySessionInfoMutex_);
    mediaKeySessionInfoMap_.clear();
}

DrmAdapterImpl::~DrmAdapterImpl()
{
    WVLOG_I("[DRM]DrmAdapterImpl::~DrmAdapterImpl enter.");
    if (callback_) {
        callback_->ClearMediaKeySessionInfo();
    }
    if (drmKeySession_ != nullptr) {
        ReleaseMediaKeySession();
    }
    if (keySystemType_ == KeySystemType::WIDEVINE) {
        std::lock_guard<std::mutex> lock(mediaKeySessionMutex_);
        for (auto iter = emeMediaKeySessionMap_.begin(); iter != emeMediaKeySessionMap_.end();) {
            ReleaseMediaKeySession(iter->second);
            emeMediaKeySessionMap_.erase(iter++);
        }
    }
    if (drmKeySystem_ != nullptr) {
        ReleaseMediaKeySystem();
    }
}

bool DrmAdapterImpl::IsSupported(const std::string& name)
{
    WVLOG_I("[DRM]DrmAdapterImpl::IsSupported");
    if (name.empty()) {
        WVLOG_E("[DRM]name is empty!");
        return false;
    }

    bool isSupported = OH_MediaKeySystem_IsSupported(name.c_str());
    WVLOG_I("[DRM]DrmAdapterImpl::IsSupported: %{public}d", isSupported);
    return isSupported;
}

bool DrmAdapterImpl::IsSupported2(const std::string& name, const std::string& mimeType)
{
    WVLOG_I("[DRM]DrmAdapterImpl::IsSupported2 enter");
    if (name.empty()) {
        WVLOG_E("[DRM]name is empty!");
        return false;
    }
    if (mimeType.empty()) {
        WVLOG_E("[DRM]mimeType is empty!");
        return false;
    }

    bool isSupported = OH_MediaKeySystem_IsSupported2(name.c_str(), mimeType.c_str());
    WVLOG_I("[DRM]DrmAdapterImpl::IsSupported2: %{public}d", isSupported);
    return isSupported;
}

bool DrmAdapterImpl::IsSupported3(const std::string& name, const std::string& mimeType, int32_t level)
{
    WVLOG_I("[DRM]DrmAdapterImpl::IsSupported3 enter");
    if (name.empty()) {
        WVLOG_E("[DRM]name is empty!");
        return false;
    }
    if (mimeType.empty()) {
        WVLOG_E("[DRM]mimeType is empty!");
        return false;
    }
    bool isSupported =
        OH_MediaKeySystem_IsSupported3(name.c_str(), mimeType.c_str(), static_cast<DRM_ContentProtectionLevel>(level));
    if (isSupported != true) {
        WVLOG_E("[DRM]The device does not support the content protection level.");
    }
    return isSupported;
}

std::vector<uint8_t> DrmAdapterImpl::GetUUID(const std::string& name)
{
    WVLOG_I("[DRM]DrmAdapterImpl::GetUUID enter, name = %{public}s:", name.c_str());
    std::vector<uint8_t> uuid;
    uuid.clear();
    uint32_t count = 10;
    DRM_MediaKeySystemDescription infos[10];
    (void)memset_s(infos, sizeof(infos), 0, sizeof(infos));
    Drm_ErrCode errNo = OH_MediaKeySystem_GetMediaKeySystems(infos, &count);
    if (errNo != DRM_ERR_OK) {
        WVLOG_E("[DRM]DRMAdapterImpl::GetMediaKeySystems failed.");
        return uuid;
    }
    WVLOG_I("[DRM]DrmAdapterImpl::GetUUID, name = %{public}s, count = %{public}d", name.c_str(), count);
    for (uint32_t i = 0; i < count; i++) {
        if (name == infos[i].name) {
            uuid.insert(uuid.begin(), infos[i].uuid, infos[i].uuid + DRM_UUID_LEN);
            break;
        }
    }
    WVLOG_I("[DRM]DrmAdapterImpl::GetUUID, name = %{public}s", name.c_str());
    return uuid;
}

int32_t DrmAdapterImpl::ReleaseMediaKeySystem()
{
    WVLOG_I("[DRM]DrmAdapterImpl::ReleaseMediaKeySystem enter");
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    {
        std::lock_guard<std::mutex> lock(mediaKeySystemCallbackMapMutex_);
        auto iter = mediaKeySystemCallbackMap_.find(drmKeySystem_);
        if (iter != mediaKeySystemCallbackMap_.end()) {
            mediaKeySystemCallbackMap_.erase(iter);
        }
    }

    Drm_ErrCode ret = OH_MediaKeySystem_Destroy(drmKeySystem_);
    drmKeySystem_ = nullptr;
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]Failed to release MediaKeySystem.");
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::ReleaseMediaKeySession()
{
    WVLOG_I("[DRM]DrmAdapterImpl::ReleaseMediaKeySession enter");
    if (drmKeySession_ == nullptr) {
        WVLOG_E("[DRM]drmKeySession_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    {
        std::lock_guard<std::mutex> lock(mediaKeySessionCallbackMutex_);
        auto iter = mediaKeySessionCallbackMap_.find(drmKeySession_);
        if (iter != mediaKeySessionCallbackMap_.end()) {
            mediaKeySessionCallbackMap_.erase(iter);
        }
    }

    Drm_ErrCode ret = OH_MediaKeySession_Destroy(drmKeySession_);
    drmKeySession_ = nullptr;
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]Failed to release MediaKeySession.");
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

Drm_ErrCode DrmAdapterImpl::SystemCallBackWithObj(
    MediaKeySystem* mediaKeySystem, DRM_EventType eventType, uint8_t* info, int32_t infoLen, char* extra)
{
    WVLOG_I("[DRM]DrmAdapterImpl::SystemCallBackWithObj enter. eventType = %{public}d.", eventType);
    if (mediaKeySystem == nullptr) {
        WVLOG_E("[DRM]DrmAdapterImpl::SystemCallBackWithObj error, mediaKeySystem is nullptr.");
        return DRM_ERR_INVALID_VAL;
    }

    std::shared_ptr<DrmCallbackImpl> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(mediaKeySystemCallbackMapMutex_);
        auto iter = mediaKeySystemCallbackMap_.find(mediaKeySystem);
        if (iter == mediaKeySystemCallbackMap_.end()) {
            WVLOG_E("[DRM]DrmAdapterImpl::SystemCallBackWithObj error, mediaKeySystem not found.");
            return DRM_ERR_INVALID_VAL;
        }
        callback = iter->second;
    }

    if (callback == nullptr) {
        WVLOG_E("[DRM]DrmAdapterImpl::SystemCallBackWithObj error, callback is nullptr.");
        return DRM_ERR_INVALID_VAL;
    }

    Drm_ErrCode ret = DRM_ERR_OK;
    if (eventType == EVENT_PROVISION_REQUIRED) {
        uint8_t request[MAX_REQUEST_LENGTH] = { 0x00 };
        int32_t requestLen = MAX_REQUEST_LENGTH;
        char defaultUrl[MAX_URL_LENGTH] = { 0x00 };
        int32_t defaultUrlLen = MAX_URL_LENGTH;
        ret =
            OH_MediaKeySystem_GenerateKeySystemRequest(mediaKeySystem, request, &requestLen, defaultUrl, defaultUrlLen);
        WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySystem_GenerateKeySystemRequest, ret = %{public}d.", ret);
        if (ret == DRM_ERR_OK) {
            if (requestLen > MAX_REQUEST_LENGTH) {
                WVLOG_E("[DRM]OH_MediaKeySystem_GenerateKeySystemRequest error, invalid requestLen.");
                return DRM_ERR_INVALID_VAL;
            }
            std::vector<uint8_t> requestData;
            std::string requestString;
            requestData.insert(requestData.begin(), request, request + requestLen);
            requestString.assign(requestData.begin(), requestData.end());
            callback->OnProvisionRequest(std::string(defaultUrl), requestString);
        }
    }
    return ret;
}

void DrmAdapterImpl::OnSessionExpirationUpdate(MediaKeySession* drmKeySession, uint8_t* info, int32_t infoLen)
{
    WVLOG_I("[DRM]DrmAdapterImpl::OnSessionExpirationUpdate enter. infoLen = %{public}d", infoLen);
    if (!drmKeySession) {
        WVLOG_I("[DRM]DrmAdapterImpl::OnSessionExpirationUpdate drmKeySession is nullptr.");
        return;
    }
    std::shared_ptr<DrmCallbackImpl> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(mediaKeySessionCallbackMutex_);
        auto iter = mediaKeySessionCallbackMap_.find(drmKeySession);
        if (iter == mediaKeySessionCallbackMap_.end()) {
            WVLOG_E("[DRM]DrmAdapterImpl::OnSessionExpirationUpdate error, mediaKeySession not found.");
            return;
        }
        callback = iter->second;
    }

    if (callback == nullptr) {
        WVLOG_E("[DRM]DrmAdapterImpl::OnSessionExpirationUpdate error, callback is nullptr.");
        return;
    }

    uint64_t timeStamp = 0;
    if (info != nullptr && infoLen > 0 && infoLen <= EXPIRATION_INFO_MAX_LEN) {
        std::vector<uint8_t> infoData;
        std::string infoString;
        infoData.insert(infoData.begin(), info, info + infoLen);
        infoString.assign(infoData.begin(), infoData.end());
        WVLOG_I("[DRM]DrmAdapterImpl::OnSessionExpirationUpdate. infoString = %{public}s", infoString.c_str());
        EndsWithAndRemove(infoString, "ms");
        if (IsValidNumber(infoString)) {
            if (infoString.size() > MILLISECOND_DIGITS) {
                infoString.erase(infoString.size() - MILLISECOND_DIGITS, MILLISECOND_DIGITS);
            }
            timeStamp = std::strtoull(infoString.c_str(), nullptr, EXPIRATION_INFO_BASE);
        }
    }

    auto sessionInfo = callback->GetMediaKeySessionInfo(drmKeySession);
    if (sessionInfo) {
        auto sessionId = sessionInfo->GetSessionId();
        if (sessionId) {
            WVLOG_I("[DRM]DrmAdapterImpl::SessionKeyChangeCallBackWithObj emeId = %{public}s.",
                sessionId->EmeId().c_str());
            callback->OnSessionExpirationUpdate(sessionId->EmeId(), timeStamp * MILLISECOND_IN_SECOND);
        }
    }
}

void DrmAdapterImpl::GetKeyRequest(MediaKeySession* drmKeySession, uint8_t* info, int32_t infoLen)
{
    WVLOG_I("[DRM]DrmAdapterImpl::GetKeyRequest enter.");
    if (drmKeySession == nullptr) {
        return;
    }
    std::shared_ptr<DrmCallbackImpl> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(mediaKeySessionCallbackMutex_);
        auto iter = mediaKeySessionCallbackMap_.find(drmKeySession);
        if (iter == mediaKeySessionCallbackMap_.end()) {
            return;
        }
        callback = iter->second;
    }
    if (callback == nullptr) {
        return;
    }
    DRM_MediaKeyRequestInfo reqInfo;
    DRM_MediaKeyRequest mediaKeyRequest;
    auto sessionInfo = callback->GetMediaKeySessionInfo(drmKeySession);
    if (!sessionInfo) {
        return;
    }
    reqInfo.type = static_cast<DRM_MediaKeyType>(sessionInfo->KeyType());
    reqInfo.initDataLen = infoLen;
    reqInfo.optionsCount = 0;
    errno_t retCopy =
        memcpy_s(reqInfo.mimeType, MAX_MIMETYPE_LEN, sessionInfo->MimeType().c_str(), sessionInfo->MimeType().length());
    if (retCopy != 0) {
        return;
    }
    if (infoLen > 0) {
        retCopy = memcpy_s(reqInfo.initData, MAX_INIT_DATA_LEN, info, infoLen);
        if (retCopy != 0) {
            return;
        }
    }
    Drm_ErrCode ret = OH_MediaKeySession_GenerateMediaKeyRequest(drmKeySession, &reqInfo, &mediaKeyRequest);
    WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySession_GenerateMediaKeyRequest, ret = %{public}d", ret);
    if (ret != DRM_ERR_OK) {
        return;
    }
    int32_t requestType = static_cast<int32_t>(mediaKeyRequest.type);
    std::vector<uint8_t> requestData;
    requestData.insert(requestData.begin(), mediaKeyRequest.data, mediaKeyRequest.data + mediaKeyRequest.dataLen);
    auto sessionId = sessionInfo->GetSessionId();
    if (sessionId) {
        callback->OnSessionMessage(sessionId->EmeId(), requestType, requestData);
    }
}

Drm_ErrCode DrmAdapterImpl::SessionEventCallBackWithObj(
    MediaKeySession* mediaKeySession, DRM_EventType eventType, uint8_t* info, int32_t infoLen, char* extra)
{
    WVLOG_I("[DRM]DrmAdapterImpl::SessionEventCallBackWithObj: %{public}d, infoLen = %{public}d",
        static_cast<int32_t>(eventType), infoLen);
    switch (eventType) {
        case EVENT_KEY_REQUIRED:
            GetKeyRequest(mediaKeySession, info, infoLen);
            break;
        case EVENT_EXPIRATION_UPDATE:
            OnSessionExpirationUpdate(mediaKeySession, info, infoLen);
            break;
        case EVENT_DRM_BASE:
        case EVENT_PROVISION_REQUIRED:
        case EVENT_KEY_EXPIRED:
        case EVENT_VENDOR_DEFINED:
        default:
            break;
    }
    return DRM_ERR_OK;
}

Drm_ErrCode DrmAdapterImpl::SessionKeyChangeCallBackWithObj(
    MediaKeySession* mediaKeySession, DRM_KeysInfo* keysInfo, bool newKeysAvailable)
{
    WVLOG_I("[DRM]DrmAdapterImpl::SessionKeyChangeCallBackWithObj enter.");
    if (keysInfo == nullptr) {
        WVLOG_E("[DRM]DrmAdapterImpl::SessionKeyChangeCallBackWithObj: keysInfo is nullptr.");
        return DRM_ERR_INVALID_VAL;
    }

    std::shared_ptr<DrmCallbackImpl> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(mediaKeySessionCallbackMutex_);
        auto sessionIter = mediaKeySessionCallbackMap_.find(mediaKeySession);
        if (sessionIter == mediaKeySessionCallbackMap_.end()) {
            WVLOG_E("[DRM]DrmAdapterImpl::SessionKeyChangeCallBackWithObj: mediaKeySession is invalid.");
            return DRM_ERR_INVALID_VAL;
        }
        callback = sessionIter->second;
    }
    std::vector<std::string> keyIdArray;
    std::vector<uint32_t> statusArray;
    for (uint32_t i = 0; i < keysInfo->keysInfoCount; i++) {
        std::string statusStr = std::string(keysInfo->statusValue[i]);
        uint32_t statusCode = static_cast<uint32_t>(KeyStatus::KEY_STATUS_INTERNAL_ERROR);

        auto iter = KeyStatusMap.find(statusStr);
        if (iter != KeyStatusMap.end()) {
            statusCode = iter->second;
        }

        std::string keyIdStr = toHexString(keysInfo->keyId[i], MAX_KEY_ID_LEN);
        keyIdArray.push_back(keyIdStr);
        statusArray.push_back(statusCode);
    }

    if (callback) {
        auto sessionInfo = callback->GetMediaKeySessionInfo(mediaKeySession);
        if (sessionInfo) {
            auto sessionId = sessionInfo->GetSessionId();
            if (sessionId) {
                WVLOG_I("[DRM]DrmAdapterImpl::SessionKeyChangeCallBackWithObj emeId: %{public}s, isRelease: %{public}d",
                    sessionId->EmeId().c_str(), sessionInfo->IsRelease());
                callback->OnSessionKeysChange(
                    sessionId->EmeId(), keyIdArray, statusArray, newKeysAvailable, sessionInfo->IsRelease());
            }
        }
    } else {
        WVLOG_E("[DRM]DrmAdapterImpl::SessionKeyChangeCallBackWithObj, callback is nullptr.");
    }
    return DRM_ERR_OK;
}

int32_t DrmAdapterImpl::CreateKeySystem(const std::string& name, const std::string& origin, int32_t securityLevel)
{
    WVLOG_I("[DRM]DrmAdapterImpl::CreateKeySystem enter.");
    if (name.empty()) {
        WVLOG_E("[DRM]name is empty!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    Drm_ErrCode ret = OH_MediaKeySystem_Create(name.c_str(), &drmKeySystem_);
    WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySystem_Create name: %{public}s, ret: %{public}d.", name.c_str(), ret);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::CreateKeySystem failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    {
        std::lock_guard<std::mutex> lock(mediaKeySystemCallbackMapMutex_);
        mediaKeySystemCallbackMap_[drmKeySystem_] = callback_;
    }

    if (name == WIDEVINE_NAME) {
        SetConfigurationString(PRIVACY_MODE, ENABLE);
        SetConfigurationString(SESSION_SHARING, ENABLE);
        int32_t configRet = SetConfigurationString(ORIGIN, origin);
        if (configRet != static_cast<int32_t>(DrmResult::DRM_RESULT_OK)) {
            WVLOG_E("[DRM]DrmAdapterImpl::CreateKeySystem ORIGIN set failed.");
        }
        keySystemType_ = KeySystemType::WIDEVINE;
    } else if (name == WISEPLAY_NAME) {
        keySystemType_ = KeySystemType::WISEPLAY;
    }

    ret = OH_MediaKeySystem_SetCallback(drmKeySystem_, SystemCallBackWithObj);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]OH_MediaKeySystem_SetCallback failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    contentProtectionLevel_ = GetContentProtectionLevelFromSecurityLevel(securityLevel);
    int32_t iRet = CreateMediaKeySession();
    if (iRet != 0) {
        WVLOG_E("[DRM]OH_MediaKeySystem_CreateMediaKeySession failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    WVLOG_I("[DRM]DrmAdapterImpl::CreateKeySystem exit.");
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::CreateMediaKeySession(std::string emeId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::CreateMediaKeySession enter.");
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    Drm_ErrCode ret = DRM_ERR_OK;
    MediaKeySession* drmKeySession = nullptr;
    ret = OH_MediaKeySystem_CreateMediaKeySession(drmKeySystem_, &contentProtectionLevel_, &drmKeySession);
    WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySystem_CreateMediaKeySession ret: %{public}d.", ret);
    if (ret != DRM_ERR_OK || (drmKeySession == nullptr)) {
        WVLOG_E("[DRM]DrmAdapterImpl::CreateMediaKeySession failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    {
        std::lock_guard<std::mutex> lock(mediaKeySessionCallbackMutex_);
        mediaKeySessionCallbackMap_[drmKeySession] = callback_;
    }
    OH_MediaKeySession_Callback sessionCallback = { SessionEventCallBackWithObj, SessionKeyChangeCallBackWithObj };
    ret = OH_MediaKeySession_SetCallback(drmKeySession, &sessionCallback);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::CreateMediaKeySession failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (emeId.empty()) {
        drmKeySession_ = drmKeySession;
        // Notify the app that the CDM is ready.
        if (callback_) {
            callback_->OnMediaKeySessionReady(reinterpret_cast<OHOSMediaKeySession>(drmKeySession));
        }
    } else {
        std::lock_guard<std::mutex> lock(mediaKeySessionMutex_);
        emeMediaKeySessionMap_[emeId] = drmKeySession;
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::ReleaseMediaKeySession(MediaKeySession* drmKeySession)
{
    WVLOG_I("[DRM]DrmAdapterImpl::ReleaseMediaKeySession enter");
    if (drmKeySession == nullptr) {
        WVLOG_E("[DRM]drmKeySession is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    {
        std::lock_guard<std::mutex> lock(mediaKeySessionCallbackMutex_);
        auto iter = mediaKeySessionCallbackMap_.find(drmKeySession);
        if (iter != mediaKeySessionCallbackMap_.end()) {
            mediaKeySessionCallbackMap_.erase(iter);
        }
    }

    Drm_ErrCode ret = OH_MediaKeySession_Destroy(drmKeySession);
    drmKeySession = nullptr;
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]Failed to release MediaKeySession.");
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

MediaKeySession* DrmAdapterImpl::GetMediaKeySession(std::string emeId)
{
    if (keySystemType_ == KeySystemType::WISEPLAY) {
        return drmKeySession_;
    }
    MediaKeySession* drmKeySession = nullptr;
    if (emeId.empty()) {
        return drmKeySession;
    }
    std::lock_guard<std::mutex> lock(mediaKeySessionMutex_);
    auto iter = emeMediaKeySessionMap_.find(emeId);
    if (iter != emeMediaKeySessionMap_.end()) {
        drmKeySession = iter->second;
    } else {
        WVLOG_E("[DRM]MediaKeySession not found.");
    }
    return drmKeySession;
}

int32_t DrmAdapterImpl::SetConfigurationString(const std::string& configName, const std::string& value)
{
    WVLOG_I("[DRM]DrmAdapterImpl::SetConfigurationString configName: %{public}s, vale: %{public}s", configName.c_str(),
        value.c_str());
    if (configName.empty()) {
        WVLOG_E("[DRM]configName is empty!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (value.empty()) {
        WVLOG_E("[DRM]value is empty!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    Drm_ErrCode ret = OH_MediaKeySystem_SetConfigurationString(drmKeySystem_, configName.c_str(), value.c_str());
    WVLOG_I("[DRM]DrmAdapterImpl::SetConfigurationString configName: %{public}s, vale: %{public}s, ret: %{public}d.",
        configName.c_str(), value.c_str(), ret);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::SetConfigurationString failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::GetConfigurationString(const std::string& configName, char* value, int32_t valueLen)
{
    WVLOG_I("[DRM]DrmAdapterImpl::GetConfigurationString");

    if (configName.empty()) {
        WVLOG_E("[DRM]configName is empty!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (value == nullptr) {
        WVLOG_E("[DRM]value is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    Drm_ErrCode ret = OH_MediaKeySystem_GetConfigurationString(drmKeySystem_, configName.c_str(), value, valueLen);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::GetConfigurationString failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::SetConfigurationByteArray(const std::string& configName, const uint8_t* value, int32_t valueLen)
{
    WVLOG_I("[DRM]DrmAdapterImpl::SetConfigurationByteArray");

    if (configName.empty()) {
        WVLOG_E("[DRM]configName is empty!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (value == nullptr) {
        WVLOG_E("[DRM]value is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    uint8_t* nonConstPtr = const_cast<uint8_t*>(value);
    Drm_ErrCode ret =
        OH_MediaKeySystem_SetConfigurationByteArray(drmKeySystem_, configName.c_str(), nonConstPtr, valueLen);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::SetConfigurationByteArray failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    return ret;
}

int32_t DrmAdapterImpl::GetConfigurationByteArray(const std::string& configName, uint8_t* value, int32_t* valueLen)
{
    WVLOG_I("[DRM]DrmAdapterImpl::GetConfigurationByteArray");
    if (configName.empty()) {
        WVLOG_E("[DRM]configName is empty!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (value == nullptr) {
        WVLOG_E("[DRM]value is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (valueLen == nullptr) {
        WVLOG_E("[DRM]valueLen is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    Drm_ErrCode ret = OH_MediaKeySystem_GetConfigurationByteArray(drmKeySystem_, configName.c_str(), value, valueLen);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::GetConfigurationByteArray failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::GetMaxContentProtectionLevel(int32_t& level)
{
    WVLOG_I("[DRM]DrmAdapterImpl::GetMaxContentProtectionLevel enter.");
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    DRM_ContentProtectionLevel contentProtectionLevel = CONTENT_PROTECTION_LEVEL_UNKNOWN;

    Drm_ErrCode ret = OH_MediaKeySystem_GetMaxContentProtectionLevel(drmKeySystem_, &contentProtectionLevel);
    level = contentProtectionLevel;
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::GetMaxContentProtectionLevel failed.");
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

void DrmAdapterImpl::StorageProvisionedResult(bool result)
{
    WVLOG_I("[DRM]DrmAdapterImpl::StorageProvisionedResult enter, result = %{public}d: ", result);
    if (drmKeySession_ == nullptr) {
        if (result) {
            CreateMediaKeySession();
        }
    }
}

void DrmAdapterImpl::StorageSaveInfoResult(bool result, int32_t type)
{
    WVLOG_I("[DRM]DrmAdapterImpl::StorageSaveInfoResult enter, result = %{public}d: ", result);
    if (!result) {
        if (callback_) {
            callback_->OnPromiseRejected(removeSessionPromiseId_, "Fail to update persistent storage");
        }
        return;
    }
    if (type != static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE)) {
        HandleKeyUpdatedCallback(updateSessionPromiseId_, result);
        WVLOG_I("[DRM]DrmAdapterImpl::StorageSaveInfoResult result = %{public}d: ", result);
        return;
    }
    if (!drmKeySystem_) {
        return;
    }
    uint8_t releaseRequest[MAX_MEDIA_KEY_REQUEST_DATA_LEN];
    int32_t releaseRequestLen = MAX_MEDIA_KEY_REQUEST_DATA_LEN;
    std::shared_ptr<SessionId> sessionId = GetSessionIdByEmeId(releaseEmeId_);
    if (sessionId == nullptr) {
        if (callback_) {
            callback_->OnPromiseRejected(removeSessionPromiseId_, "Session doesn't exist");
        }
        return;
    }
    MediaKeySession* drmKeySession = GetMediaKeySession(releaseEmeId_);
    if (!drmKeySession) {
        return;
    }
    Drm_ErrCode ret = OH_MediaKeySession_GenerateOfflineReleaseRequest(
        drmKeySession, sessionId->KeySetId(), sessionId->KeySetIdLen(), releaseRequest, &releaseRequestLen);
    WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySession_GenerateOfflineReleaseRequest ret = %{public}d: ", ret);
    if (ret != DRM_ERR_OK) {
        if (callback_) {
            callback_->OnPromiseRejected(removeSessionPromiseId_, "Fail to generate key release request");
        }
        return;
    }
    if (releaseRequestLen > MAX_MEDIA_KEY_REQUEST_DATA_LEN) {
        WVLOG_E("[DRM]OH_MediaKeySession_GenerateOfflineReleaseRequest error, invalid releaseRequestLen.");
        return;
    }
    std::vector<uint8_t> requestData;
    requestData.insert(requestData.begin(), releaseRequest, releaseRequest + releaseRequestLen);
    int32_t requestType = static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE);
    if (callback_) {
        callback_->OnPromiseResolved(removeSessionPromiseId_);
        callback_->OnSessionMessage(releaseEmeId_, requestType, requestData);
    }
}

void DrmAdapterImpl::StorageLoadInfoResult(
    const std::string& emeId, const std::vector<uint8_t>& keySetId, const std::string& mimeType, uint32_t keyType)
{
    WVLOG_I("[DRM]DrmAdapterImpl::StorageLoadInfoResult enter, emeId = %{public}s: ", emeId.c_str());
    if (keySetId.size() == 0) {
        WVLOG_I("[DRM]DrmAdapterImpl::StorageLoadInfoResult emeId = %{public}s: ", emeId.c_str());
        if (callback_) {
            callback_->OnPromiseResolvedWithSession(loadSessionPromiseId_, "");
        }
        return;
    }

    // Loading same persistent license into different sessions isn't
    // supported.
    if (GetSessionIdByEmeId(emeId) != nullptr) {
        return;
    }

    std::shared_ptr<SessionId> sessionId = std::make_shared<SessionId>(emeId, keySetId.data(), keySetId.size());
    PutSessionInfo(sessionId, mimeType, keyType);
    LoadSessionWithLoadedStorage(sessionId, loadSessionPromiseId_);
}

void DrmAdapterImpl::StorageClearInfoResult(bool result, int32_t type)
{
    WVLOG_I("[DRM]DrmAdapterImpl::StorageClearInfoResult enter.");
    if (type == static_cast<int32_t>(ClearInfoType::KEY_RELEASE)) {
        HandleKeyUpdatedCallback(updateSessionPromiseId_, result);
    } else if (type == static_cast<int32_t>(ClearInfoType::LOAD_FAIL)) {
        if (!result) {
            WVLOG_W("[DRM]Failed to clear persistent storage for non-exist license");
        }
        if (callback_) {
            callback_->OnPromiseResolvedWithSession(loadSessionPromiseId_, "");
        }
    }
}

int32_t DrmAdapterImpl::ProcessKeySystemResponse(const std::string& response, bool isResponseReceived)
{
    WVLOG_I("[DRM]DrmAdapterImpl::ProcessKeySystemResponse enter, isResponseReceived : %{public}d", isResponseReceived);
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    bool success = true;
    if (isResponseReceived) {
        int32_t responseLen = static_cast<int32_t>(response.size());
        std::vector<uint8_t> vec(responseLen);
        errno_t retCopy = memcpy_s(vec.data(), responseLen, response.data(), response.size());
        if (retCopy != 0) {
            WVLOG_E("[DRM]memcpy_s failed with error.");
            return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
        }
        Drm_ErrCode ret = OH_MediaKeySystem_ProcessKeySystemResponse(drmKeySystem_, vec.data(), responseLen);
        WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySystem_ProcessKeySystemResponse ret : %{public}d", ret);
        if (ret != DRM_ERR_OK) {
            WVLOG_E("[DRM]DrmAdapterImpl::ProcessKeySystemResponse failed.");
            success = false;
        }
    } else {
        success = false;
    }

    if (!success) {
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (callback_) {
        callback_->OnStorageProvisioned();
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::GetCertificateStatus(int32_t& certStatus)
{
    WVLOG_I("[DRM]DrmAdapterImpl::GetCertificateStatus");
    if (drmKeySystem_ == nullptr) {
        WVLOG_E("[DRM]drmKeySystem_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    DRM_CertificateStatus cert = CERT_STATUS_INVALID;
    Drm_ErrCode ret = OH_MediaKeySystem_GetCertificateStatus(drmKeySystem_, &cert);
    certStatus = cert;
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::GetCertificateStatus failed.");
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::RegistDrmCallback(std::shared_ptr<DrmCallbackAdapter> callbackAdapter)
{
    callback_ = std::make_shared<DrmCallbackImpl>(callbackAdapter);
    return DRM_ERR_OK;
}

void DrmAdapterImpl::UpdateSessionResult(
    bool isKeyRelease, std::shared_ptr<SessionId> sessionId, uint8_t* mediaKeyId, int32_t mediaKeyIdLen)
{
    WVLOG_I("[DRM]DrmAdapterImpl::UpdateSessionResult enter.");
    if (sessionId == nullptr) {
        return;
    }

    std::shared_ptr<SessionInfo> info = GetSessionInfo(sessionId);
    if (info == nullptr) {
        WVLOG_E(
            "[DRM]DrmAdapterImpl::UpdateSessionResult, info is nullptr, emeId: %{public}s", sessionId->EmeId().c_str());
        return;
    }

    if (isKeyRelease) {
        WVLOG_I("[DRM]DrmAdapterImpl::UpdateSessionResult, emeId: %{public}s", sessionId->EmeId().c_str());
        ClearPersistentSessionInfoFroKeyRelease(sessionId);
    } else if (info->KeyType() == static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE) && mediaKeyIdLen > 0) {
        WVLOG_I("[DRM]DrmAdapterImpl::UpdateSessionResult, emeId: %{public}s", sessionId->EmeId().c_str());
        SetKeySetId(sessionId, mediaKeyId, mediaKeyIdLen);
    } else {
        WVLOG_I("[DRM]DrmAdapterImpl::UpdateSessionResult, emeId: %{public}s", sessionId->EmeId().c_str());
        HandleKeyUpdatedCallback(updateSessionPromiseId_, true);
    }
}

int32_t DrmAdapterImpl::UpdateSession(uint32_t promiseId, const std::string& emeId, std::vector<uint8_t> response)
{
    WVLOG_I("[DRM]DrmAdapterImpl::UpdateSession enter, emeId: %{public}s", emeId.c_str());
    MediaKeySession* drmKeySession = GetMediaKeySession(emeId);
    if (!drmKeySession) {
        WVLOG_E("[DRM]drmKeySession is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    updateSessionPromiseId_ = promiseId;
    std::shared_ptr<SessionId> sessionId = GetSessionIdByEmeId(emeId);
    if (sessionId == nullptr) {
        if (callback_) {
            callback_->OnPromiseRejected(promiseId, "Invalid session in updateSession: " + emeId);
        }
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    std::shared_ptr<SessionInfo> info = GetSessionInfo(sessionId);
    if (info == nullptr) {
        WVLOG_E("[DRM]DrmAdapterImpl::UpdateSession, info is nullptr, emeId: %{public}s", emeId.c_str());
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    bool isKeyRelease = info->KeyType() == static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE);
    int32_t mediaKeyIdLen = MAX_KEY_SET_ID_LEN;
    uint8_t mediaKeyId[MAX_KEY_SET_ID_LEN] = { 0x00 };
    if (isKeyRelease) {
        Drm_ErrCode ret = OH_MediaKeySession_ProcessOfflineReleaseResponse(
            drmKeySession, sessionId->KeySetId(), sessionId->KeySetIdLen(), response.data(), response.size());
        WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySession_ProcessOfflineReleaseResponse, ret: %{public}d", ret);
        if (ret != DRM_ERR_OK) {
            if (callback_) {
                callback_->OnPromiseRejected(promiseId, "Update session failed.");
            }
            return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
        }
    } else {
        Drm_ErrCode ret = OH_MediaKeySession_ProcessMediaKeyResponse(
            drmKeySession, response.data(), response.size(), mediaKeyId, &mediaKeyIdLen);
        WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySession_ProcessMediaKeyResponse result. ret: %{public}d", ret);
        if (ret != DRM_ERR_OK) {
            if (callback_) {
                callback_->OnPromiseRejected(promiseId, "Update session failed.");
            }
            return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
        }
        if ((keySystemType_ == KeySystemType::WISEPLAY) && callback_) {
            callback_->OnMediaLicenseReady(true);
        }
    }
    UpdateSessionResult(isKeyRelease, sessionId, mediaKeyId, mediaKeyIdLen);
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::CloseSession(uint32_t promiseId, const std::string& emeId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::CloseSession enter.");
    if (drmKeySystem_ == nullptr) {
        if (callback_) {
            callback_->OnPromiseRejected(promiseId, "closeSession() called when MediaDrm is null.");
        }
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    std::shared_ptr<SessionId> sessionId = GetSessionIdByEmeId(emeId);
    if (sessionId == nullptr) {
        if (callback_) {
            callback_->OnPromiseRejected(promiseId, "Invalid sessionId in closeSession(): " + emeId);
        }
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (keySystemType_ == KeySystemType::WIDEVINE) {
        MediaKeySession* drmKeySession = GetMediaKeySession(emeId);
        if (!drmKeySession) {
            return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
        }
        if (callback_) {
            callback_->RemoveMediaKeySessionInfo(drmKeySession);
        }
        {
            std::lock_guard<std::mutex> lock(mediaKeySessionMutex_);
            ReleaseMediaKeySession(drmKeySession);
            emeMediaKeySessionMap_.erase(emeId);
        }
    }
    RemoveSessionInfo(sessionId);
    if (callback_) {
        callback_->OnPromiseResolved(promiseId);
        callback_->OnSessionClosed(emeId);
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::RemoveSession(uint32_t promiseId, const std::string& emeId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::RemoveSession enter.");
    if (callback_ == nullptr) {
        WVLOG_E("[DRM]DrmAdapterImpl::RemoveSession, callback_ is nullptr.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    std::shared_ptr<SessionId> sessionId = GetSessionIdByEmeId(emeId);
    if (sessionId == nullptr) {
        callback_->OnPromiseRejected(promiseId, "Session doesn't exist");
        WVLOG_W("[DRM]DrmAdapterImpl::RemoveSession, Session doesn't exist.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }

    std::shared_ptr<SessionInfo> sessionInfo = GetSessionInfo(sessionId);
    if (sessionInfo == nullptr) {
        callback_->OnPromiseRejected(promiseId, "SessionInfo doesn't exist");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (sessionInfo->KeyType() == static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_ONLINE)) {
        callback_->OnPromiseRejected(promiseId, "Removing temporary session isn't implemented");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    removeSessionPromiseId_ = promiseId;

    releaseEmeId_ = emeId;

    SetKeyType(sessionId, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::LoadSession(uint32_t promiseId, const std::string& sessionId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::LoadSession enter.");
    loadSessionPromiseId_ = promiseId;
    LoadSessionInfo(sessionId);
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::ClearMediaKeys()
{
    WVLOG_I("[DRM]DrmAdapterImpl::ClearMediaKeys enter.");
    if (drmKeySession_ == nullptr) {
        WVLOG_E("[DRM]drmKeySession_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    Drm_ErrCode ret = OH_MediaKeySession_ClearMediaKeys(drmKeySession_);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::ClearMediaKeys failed.");
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::GetSecurityLevel()
{
    WVLOG_I("[DRM]DrmAdapterImpl::GetSecurityLevel enter.");
    if (drmKeySession_ == nullptr) {
        WVLOG_E("[DRM]drmKeySession_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    DRM_ContentProtectionLevel levelData = CONTENT_PROTECTION_LEVEL_SW_CRYPTO;
    Drm_ErrCode ret = OH_MediaKeySession_GetContentProtectionLevel(drmKeySession_, &levelData);
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::GetSecurityLevel failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    int32_t securityLevel = GetSecurityLevelFromContentProtectionLevel(static_cast<int32_t>(levelData));
    return securityLevel;
}

int32_t DrmAdapterImpl::RequireSecureDecoderModule(const std::string& mimeType, bool& status)
{
    WVLOG_I("[DRM]DrmAdapterImpl::RequireSecureDecoderModule enter.");
    if (mimeType.empty()) {
        WVLOG_E("[DRM]mimeType is empty!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (drmKeySession_ == nullptr) {
        WVLOG_E("[DRM]drmKeySession_ is nullptr!");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    bool stas = false;
    Drm_ErrCode ret = OH_MediaKeySession_RequireSecureDecoderModule(drmKeySession_, mimeType.c_str(), &stas);
    status = stas;
    if (ret != DRM_ERR_OK) {
        WVLOG_E("[DRM]DrmAdapterImpl::RequireSecureDecoderModule failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

int32_t DrmAdapterImpl::GenerateMediaKeyRequest(const std::string& emeId, int32_t type, int32_t initDataLen,
    const std::vector<uint8_t>& initData, const std::string& mimeType, uint32_t promiseId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::GenerateMediaKeyRequest enter, emeId = %{public}s", emeId.c_str());
    if (!callback_) {
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (drmKeySystem_ == nullptr || drmKeySession_ == nullptr) {
        callback_->OnPromiseRejected(promiseId, "DrmKeySystem released previously.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    std::shared_ptr<SessionId> sessionId = SessionId::CreateSessionId(emeId);
    DRM_MediaKeyRequestInfo info;
    DRM_MediaKeyRequest mediaKeyRequest;
    info.type = static_cast<DRM_MediaKeyType>(type);
    info.initDataLen = initDataLen;
    info.optionsCount = 0;
    if (memcpy_s(info.mimeType, MAX_MIMETYPE_LEN, mimeType.c_str(), mimeType.length()) != 0) {
        WVLOG_E("[DRM]memcpy_s failed with error.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (memcpy_s(info.initData, MAX_INIT_DATA_LEN, initData.data(), initData.size()) != 0) {
        WVLOG_E("[DRM]memcpy_s failed with error.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    if (keySystemType_ == KeySystemType::WIDEVINE) {
        int32_t iRet = CreateMediaKeySession(emeId);
        if (iRet != 0) {
            return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
        }
    }
    MediaKeySession* drmKeySession = GetMediaKeySession(emeId);
    if (!drmKeySession) {
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    Drm_ErrCode ret = OH_MediaKeySession_GenerateMediaKeyRequest(drmKeySession, &info, &mediaKeyRequest);
    WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySession_GenerateMediaKeyRequest, ret = %{public}d", ret);
    if (ret != DRM_ERR_OK) {
        callback_->OnPromiseRejected(promiseId, "Generate request failed.");
        return static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR);
    }
    int32_t requestType = static_cast<int32_t>(mediaKeyRequest.type);
    std::vector<uint8_t> requestData;
    requestData.insert(requestData.begin(), mediaKeyRequest.data, mediaKeyRequest.data + mediaKeyRequest.dataLen);
    PutSessionInfo(sessionId, mimeType, type);
    std::shared_ptr<SessionInfo> sessionInfo = GetSessionInfo(sessionId);
    callback_->UpdateMediaKeySessionInfoMap(drmKeySession, sessionInfo);
    callback_->OnPromiseResolvedWithSession(promiseId, emeId);
    callback_->OnSessionMessage(emeId, requestType, requestData);
    return static_cast<int32_t>(DrmResult::DRM_RESULT_OK);
}

// private
void DrmAdapterImpl::PutSessionInfo(std::shared_ptr<SessionId> sessionId, const std::string& mimeType, int32_t type)
{
    if (sessionId == nullptr) {
        return;
    }
    std::shared_ptr<SessionInfo> info = std::make_shared<SessionInfo>(sessionId, mimeType, type);
    WVLOG_I("[DRM]DrmAdapterImpl::PutSessionInfo, emeId = %{public}s", sessionId->EmeId().c_str());
    emeSessionInfoMap_[sessionId->EmeId()] = info;
}

std::shared_ptr<SessionInfo> DrmAdapterImpl::GetSessionInfo(std::shared_ptr<SessionId> sessionId)
{
    if (sessionId == nullptr) {
        WVLOG_I("[DRM]DrmAdapterImpl::GetSessionInfo, sessionId is nullptr.");
        return nullptr;
    }
    WVLOG_I("[DRM]DrmAdapterImpl::GetSessionInfo, emeId = %{public}s", sessionId->EmeId().c_str());
    auto iter = emeSessionInfoMap_.find(sessionId->EmeId());
    if (iter != emeSessionInfoMap_.end()) {
        WVLOG_I("[DRM]DrmAdapterImpl::GetSessionInfo, find.");
        return iter->second;
    }
    WVLOG_I("[DRM]DrmAdapterImpl::GetSessionInfo, ret is nullptr.");
    return nullptr;
}

std::shared_ptr<SessionId> DrmAdapterImpl::GetSessionIdByEmeId(const std::string& emeId)
{
    auto iter = emeSessionInfoMap_.find(emeId);
    if (iter != emeSessionInfoMap_.end()) {
        std::shared_ptr<SessionInfo> info = iter->second;
        if (info != nullptr) {
            return info->GetSessionId();
        }
    }
    return nullptr;
}

void DrmAdapterImpl::RemoveSessionInfo(std::shared_ptr<SessionId> sessionId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::RemoveSessionInfo.");
    /**
     * Remove session and related infomration from memory, but doesn't touch
     * persistent storage.
     */
    if (sessionId == nullptr) {
        return;
    }
    std::shared_ptr<SessionInfo> info = GetSessionInfo(sessionId);
    if (info != nullptr) {
        emeSessionInfoMap_.erase(sessionId->EmeId());
    }
}

void DrmAdapterImpl::LoadSessionInfo(const std::string& emeId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::LoadSessionInfo.");
    if (callback_) {
        callback_->OnStorageLoadInfo(emeId);
    }
}

void DrmAdapterImpl::LoadSessionWithLoadedStorage(std::shared_ptr<SessionId> sessionId, uint32_t promiseId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::LoadSessionWithLoadedStorage.");
    if (sessionId == nullptr || callback_ == nullptr) {
        return;
    }
    std::shared_ptr<SessionInfo> info = GetSessionInfo(sessionId);
    if (info == nullptr) {
        WVLOG_E("[DRM]DrmAdapterImpl::LoadSessionWithLoadedStorage, info is null.");
        return;
    }
    if (info->KeyType() == static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE)) {
        callback_->OnPromiseResolvedWithSession(promiseId, sessionId->EmeId());
        std::vector<std::string> dummyKeyId;
        std::vector<uint32_t> dummyStatus;
        dummyKeyId.push_back("");
        dummyStatus.push_back(static_cast<uint32_t>(KeyStatus::KEY_STATUS_INTERNAL_ERROR));
        callback_->OnSessionKeysChange(sessionId->EmeId(), dummyKeyId, dummyStatus, false, true);
        return;
    }

    if (info->KeyType() != static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE)) {
        return;
    }

    if (keySystemType_ == KeySystemType::WIDEVINE) {
        int32_t iRet = CreateMediaKeySession(sessionId->EmeId());
        if (iRet != 0) {
            WVLOG_E("[DRM]OH_MediaKeySystem_CreateMediaKeySession failed.");
            return;
        }
    }
    MediaKeySession* drmKeySession = GetMediaKeySession(sessionId->EmeId());
    callback_->UpdateMediaKeySessionInfoMap(drmKeySession, info);
    if (drmKeySession != nullptr) {
        Drm_ErrCode ret =
            OH_MediaKeySession_RestoreOfflineMediaKeys(drmKeySession, sessionId->KeySetId(), sessionId->KeySetIdLen());
        WVLOG_I("[DRM]DrmAdapterImpl::OH_MediaKeySession_RestoreOfflineMediaKeys, ret = %{public}d", ret);
        if (ret != DRM_ERR_OK) {
            ClearPersistentSessionInfoForLoadFail(sessionId);
            return;
        }
        if (keySystemType_ == KeySystemType::WISEPLAY) {
            callback_->OnMediaLicenseReady(true);
        }
        callback_->OnPromiseResolvedWithSession(promiseId, sessionId->EmeId());
    }
    WVLOG_I("[DRM]DrmAdapterImpl::LoadSessionWithLoadedStorage.");
}

// remove && release
void DrmAdapterImpl::SetKeyType(std::shared_ptr<SessionId> sessionId, int32_t keyType)
{
    WVLOG_I("[DRM]DrmAdapterImpl::SetKeyType.");
    std::shared_ptr<SessionInfo> info = GetSessionInfo(sessionId);
    if (info == nullptr) {
        return;
    }
    info->SetKeyType(keyType);

    if (info->GetSessionId() != nullptr && info->GetSessionId()->KeySetId() != nullptr) {
        std::vector<uint8_t> keySetIdVec;
        keySetIdVec.insert(keySetIdVec.begin(), info->GetSessionId()->KeySetId(),
            info->GetSessionId()->KeySetId() + info->GetSessionId()->KeySetIdLen());
        if (callback_) {
            WVLOG_I("[DRM]DrmAdapterImpl::OnStorageSaveInfo.");
            callback_->OnStorageSaveInfo(keySetIdVec, info->MimeType(), sessionId->EmeId(), keyType);
        }
    }
}

// update
void DrmAdapterImpl::SetKeySetId(std::shared_ptr<SessionId> sessionId, uint8_t* mediaKeyId, int32_t mediaKeyIdLen)
{
    WVLOG_I("[DRM]DrmAdapterImpl::SetKeySetId.");
    if (sessionId == nullptr) {
        HandleKeyUpdatedCallback(updateSessionPromiseId_, false);
        return;
    }
    sessionId->SetKeySetId(mediaKeyId, mediaKeyIdLen);
    if (callback_) {
        std::shared_ptr<SessionInfo> info = GetSessionInfo(sessionId);
        if (info) {
            std::vector<uint8_t> keySetIdVec;
            keySetIdVec.insert(keySetIdVec.begin(), info->GetSessionId()->KeySetId(),
                info->GetSessionId()->KeySetId() + info->GetSessionId()->KeySetIdLen());
            callback_->OnStorageSaveInfo(keySetIdVec, info->MimeType(), sessionId->EmeId(), info->KeyType());
        }
    } else {
        HandleKeyUpdatedCallback(updateSessionPromiseId_, false);
    }
}

void DrmAdapterImpl::ClearPersistentSessionInfoFroKeyRelease(std::shared_ptr<SessionId> sessionId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::ClearPersistentSessionInfoFroKeyRelease.");
    if (sessionId != nullptr) {
        sessionId->SetKeySetId(nullptr, 0);
        if (callback_) {
            WVLOG_I("[DRM]OnStorageClearInfoForKeyRelease.");
            callback_->OnStorageClearInfoForKeyRelease(sessionId->EmeId());
        }
    }
}

void DrmAdapterImpl::ClearPersistentSessionInfoForLoadFail(std::shared_ptr<SessionId> sessionId)
{
    WVLOG_I("[DRM]DrmAdapterImpl::ClearPersistentSessionInfoFroKeyRelease.");
    if (sessionId != nullptr) {
        sessionId->SetKeySetId(nullptr, 0);
        if (callback_) {
            WVLOG_I("[DRM]OnStorageClearInfoForLoadFail.");
            callback_->OnStorageClearInfoForLoadFail(sessionId->EmeId());
        }
    }
}

void DrmAdapterImpl::HandleKeyUpdatedCallback(uint32_t promiseId, bool result)
{
    WVLOG_I("[DRM]DrmAdapterImpl::HandleKeyUpdatedCallback, result: %{public}d", result);
    if (callback_) {
        if (!result) {
            callback_->OnPromiseRejected(promiseId, "failed to update key after response accepted");
            return;
        }
        callback_->OnPromiseResolved(promiseId);
    }
}
} // namespace OHOS::NWeb
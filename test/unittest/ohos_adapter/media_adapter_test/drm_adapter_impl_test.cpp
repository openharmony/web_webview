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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "native_mediakeysession.h"
#include "native_mediakeysystem.h"
#define private public
#include "drm_adapter.h"
#include "drm_adapter_impl.h"

using namespace testing;
using namespace testing::ext;
using ::testing::NiceMock;

namespace OHOS::NWeb {
namespace {
std::shared_ptr<SessionInfo> g_sessioninfo;
std::shared_ptr<DrmCallbackImpl> g_callback;
std::shared_ptr<DrmAdapterImpl> g_adapter;
bool g_isWisePlay = false;
bool g_isSupportDrm = true;
} // namespace

static const std::string GetKeySystemName()
{
    if (OH_MediaKeySystem_IsSupported("com.clearplay.drm")) {
        g_isSupportDrm = true;
        return "com.clearplay.drm";
    } else if (OH_MediaKeySystem_IsSupported("com.wiseplay.drm")) {
        g_isWisePlay = true;
        g_isSupportDrm = true;
        return "com.wiseplay.drm";
    } else {
        g_isSupportDrm = false;
        return "ERROR";
    }
}

class DrmCallbackAdapterMock : public DrmCallbackAdapter {
public:
    MOCK_METHOD(void, OnSessionMessage, (const std::string&, int32_t&, const std::vector<uint8_t>&), (override));
    MOCK_METHOD(void, OnProvisionRequest, (const std::string&, const std::string&), (override));
    MOCK_METHOD(void, OnProvisioningComplete, (bool), (override));
    MOCK_METHOD(void, OnMediaKeySessionReady, (void*), (override));
    MOCK_METHOD(void, OnPromiseRejected, (uint32_t, const std::string&), (override));
    MOCK_METHOD(void, OnPromiseResolved, (uint32_t), (override));
    MOCK_METHOD(void, OnPromiseResolvedWithSession, (uint32_t, const std::string&), (override));
    MOCK_METHOD(void, OnStorageProvisioned, (), (override));
    MOCK_METHOD(void, OnStorageSaveInfo, (const std::vector<uint8_t>&, const std::string&, const std::string&, int32_t),
        (override));
    MOCK_METHOD(void, OnSessionClosed, (const std::string&), (override));
    MOCK_METHOD(void, OnStorageLoadInfo, (const std::string&), (override));
    MOCK_METHOD(void, OnSessionKeysChange,
        (const std::string&, const std::vector<std::string>&, const std::vector<uint32_t>&, bool, bool), (override));
    MOCK_METHOD(void, OnSessionExpirationUpdate, (const std::string&, uint64_t), (override));
    MOCK_METHOD(void, OnStorageClearInfoForKeyRelease, (const std::string&), (override));
    MOCK_METHOD(void, OnStorageClearInfoForLoadFail, (const std::string&), (override));
    MOCK_METHOD(void, OnMediaLicenseReady, (bool), (override));
};

class SessionIdTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class SessionInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class DrmCallbackImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<DrmCallbackAdapterMock> mockCallback_;
};

class DrmAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    std::shared_ptr<DrmCallbackAdapterMock> mockCallback_;
};

void SessionIdTest::SetUpTestCase(void) {}

void SessionIdTest::TearDownTestCase(void) {}

void SessionIdTest::SetUp(void) {}

void SessionIdTest::TearDown(void) {}

/**
 * @tc.name: SessionIdTest_SessionId_001.
 * @tc.desc: test of SessionId :: CreateSessionId
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SessionIdTest, SessionIdTest_SessionId_001, TestSize.Level1)
{
    auto sessionId = SessionId::CreateSessionId("testSessionId");
    EXPECT_EQ(sessionId->EmeId(), "testSessionId");
    EXPECT_EQ(sessionId->KeySetIdLen(), 0);
}

/**
 * @tc.name: SessionIdTest_SessionId_002.
 * @tc.desc: test of SessionId :: CreateSessionId
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SessionIdTest, SessionIdTest_SessionId_002, TestSize.Level1)
{
    auto sessionId = SessionId::CreateSessionId("tempSessionId02");
    EXPECT_NE(sessionId->EmeId(), "tempSessionId");
    EXPECT_EQ(sessionId->KeySetIdLen(), 0);
}

/**
 * @tc.name: SessionIdTest_SessionId_003.
 * @tc.desc: test of SessionId :: emeId & keySetId & keySetIdLen
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SessionIdTest, SessionIdTest_SessionId_003, TestSize.Level1)
{
    unsigned char keySetId[] = { 1, 2, 3, 4, 5 };
    SessionId sessionId("emeIdTest", keySetId, 5);

    EXPECT_EQ(sessionId.EmeId(), "emeIdTest");
    EXPECT_EQ(sessionId.KeySetIdLen(), 5);
    EXPECT_EQ(memcmp(sessionId.KeySetId(), keySetId, 5), 0);
}

/**
 * @tc.name: SessionIdTest_SessionId_004.
 * @tc.desc: test of SessionId :: SetKeySetId
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SessionIdTest, SessionIdTest_SessionId_004, TestSize.Level1)
{
    unsigned char keySetId[] = { 10, 20, 30 };
    unsigned char newKeySetId[] = { 40, 50, 60 };
    SessionId sessionId("emeIdTest", keySetId, 3);

    sessionId.SetKeySetId(newKeySetId, 3);
    EXPECT_EQ(memcmp(sessionId.KeySetId(), newKeySetId, 3), 0);
}

void SessionInfoTest::SetUpTestCase(void) {}

void SessionInfoTest::TearDownTestCase(void) {}

void SessionInfoTest::SetUp(void)
{
    unsigned char keySetId[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    auto sessionId = SessionId::CreateSessionId("session123");
    sessionId->SetKeySetId(keySetId, sizeof(keySetId));
    g_sessioninfo = std::make_shared<SessionInfo>(
        sessionId, "video/mp4", static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
}

void SessionInfoTest::TearDown(void)
{
    g_sessioninfo = nullptr;
}

/**
 * @tc.name: SessionInfoTest_SessionInfo_001.
 * @tc.desc: test of SessionInfo :: mimeType
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SessionInfoTest, SessionInfoTest_SessionInfo_001, TestSize.Level1)
{
    EXPECT_EQ(g_sessioninfo->MimeType(), "video/mp4");
    EXPECT_EQ(g_sessioninfo->KeyType(), static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
}

/**
 * @tc.name: SessionInfoTest_SessionInfo_002.
 * @tc.desc: test of SessionInfo :: SetKeyType & keyType
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SessionInfoTest, SessionInfoTest_SessionInfo_002, TestSize.Level1)
{
    g_sessioninfo->SetKeyType(MEDIA_KEY_TYPE_ONLINE);
    EXPECT_EQ(g_sessioninfo->KeyType(), MEDIA_KEY_TYPE_ONLINE);
}

/**
 * @tc.name: SessionInfoTest_SessionInfo_003.
 * @tc.desc: test of SessionInfo :: sessionId
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(SessionInfoTest, SessionInfoTest_SessionInfo_003, TestSize.Level1)
{
    auto sessionId = g_sessioninfo->GetSessionId();
    EXPECT_EQ(sessionId->EmeId(), "session123");
}

void DrmCallbackImplTest::SetUpTestCase(void) {}

void DrmCallbackImplTest::TearDownTestCase(void) {}

void DrmCallbackImplTest::SetUp(void)
{
    mockCallback_ = std::make_shared<DrmCallbackAdapterMock>();
    g_callback = std::make_shared<DrmCallbackImpl>(mockCallback_);
}

void DrmCallbackImplTest::TearDown(void)
{
    g_callback = nullptr;
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_002.
 * @tc.desc: test of DrmCallbackImpl :: OnSessionMessage
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_002, TestSize.Level1)
{
    std::string sessionId = "sessionId";
    int32_t type = 1;
    std::vector<uint8_t> message = { 0x01, 0x02 };

    EXPECT_CALL(*mockCallback_, OnSessionMessage(sessionId, type, message)).Times(1);
    g_callback->OnSessionMessage(sessionId, type, message);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnSessionMessage(sessionId, type, message);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_003.
 * @tc.desc: test of DrmCallbackImpl :: OnProvisionRequest
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_003, TestSize.Level1)
{
    std::string defaultUrl = "http://example.com";
    std::string requestData = "requestData";

    EXPECT_CALL(*mockCallback_, OnProvisionRequest(defaultUrl, requestData)).Times(1);
    g_callback->OnProvisionRequest(defaultUrl, requestData);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnProvisionRequest(defaultUrl, requestData);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_004.
 * @tc.desc: test of DrmCallbackImpl :: OnProvisioningComplete
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_004, TestSize.Level1)
{
    bool success = true;

    EXPECT_CALL(*mockCallback_, OnProvisioningComplete(success)).Times(1);
    g_callback->OnProvisioningComplete(success);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnProvisioningComplete(success);
}
/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_005.
 * @tc.desc: test of DrmCallbackImpl :: OnMediaKeySessionReady
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_005, TestSize.Level1)
{
    void* session = reinterpret_cast<void*>(0x1234);

    EXPECT_CALL(*mockCallback_, OnMediaKeySessionReady(session)).Times(1);
    g_callback->OnMediaKeySessionReady(session);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnMediaKeySessionReady(session);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_006.
 * @tc.desc: test of DrmCallbackImpl :: OnPromiseRejected
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_006, TestSize.Level1)
{
    uint32_t promiseId = 1;
    std::string errorMessage = "error";

    EXPECT_CALL(*mockCallback_, OnPromiseRejected(promiseId, errorMessage)).Times(1);
    g_callback->OnPromiseRejected(promiseId, errorMessage);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnPromiseRejected(promiseId, errorMessage);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_007.
 * @tc.desc: test of DrmCallbackImpl :: OnPromiseResolved
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_007, TestSize.Level1)
{
    uint32_t promiseId = 1;

    EXPECT_CALL(*mockCallback_, OnPromiseResolved(promiseId)).Times(1);
    g_callback->OnPromiseResolved(promiseId);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnPromiseResolved(promiseId);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_008.
 * @tc.desc: test of DrmCallbackImpl :: OnPromiseResolvedWithSession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_008, TestSize.Level1)
{
    uint32_t promiseId = 1;
    std::string sessionId = "sessionId";

    EXPECT_CALL(*mockCallback_, OnPromiseResolvedWithSession(promiseId, sessionId)).Times(1);
    g_callback->OnPromiseResolvedWithSession(promiseId, sessionId);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnPromiseResolvedWithSession(promiseId, sessionId);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_009.
 * @tc.desc: test of DrmCallbackImpl :: OnSessionClosed
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_009, TestSize.Level1)
{
    std::string testSessionId = "sessionId";

    EXPECT_CALL(*mockCallback_, OnSessionClosed(testSessionId)).Times(1);
    g_callback->OnSessionClosed(testSessionId);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnSessionClosed(testSessionId);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_010.
 * @tc.desc: test of DrmCallbackImpl :: OnSessionKeysChange
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_010, TestSize.Level1)
{
    std::string testSessionId = "sessionId";
    std::vector<std::string> dummyKeyId;
    std::vector<uint32_t> dummyStatus;
    dummyKeyId.push_back("");
    dummyStatus.push_back(static_cast<uint32_t>(KeyStatus::KEY_STATUS_INTERNAL_ERROR));
    bool hasAdditionalUsableKey = false;
    bool isKeyRelease = true;

    EXPECT_CALL(*mockCallback_,
        OnSessionKeysChange(testSessionId, dummyKeyId, dummyStatus, hasAdditionalUsableKey, isKeyRelease))
        .Times(1);
    g_callback->OnSessionKeysChange(testSessionId, dummyKeyId, dummyStatus, hasAdditionalUsableKey, isKeyRelease);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnSessionKeysChange(testSessionId, dummyKeyId, dummyStatus, hasAdditionalUsableKey, isKeyRelease);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_011.
 * @tc.desc: test of DrmCallbackImpl :: OnSessionExpirationUpdate
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_011, TestSize.Level1)
{
    std::string testSessionId = "sessionId";
    uint64_t expirationTime = 1234567890;
    EXPECT_CALL(*mockCallback_, OnSessionExpirationUpdate(testSessionId, expirationTime)).Times(1);
    g_callback->OnSessionExpirationUpdate(testSessionId, expirationTime);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnSessionExpirationUpdate(testSessionId, expirationTime);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_012.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageProvisioned
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_012, TestSize.Level1)
{
    EXPECT_CALL(*mockCallback_, OnStorageProvisioned()).Times(1);
    g_callback->OnStorageProvisioned();
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnStorageProvisioned();
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_013.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageSaveInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_013, TestSize.Level1)
{
    std::vector<uint8_t> ketSetId = { 0x01, 0x02 };
    std::string mimeType = "video/mp4";
    std::string sessionId = "sessionId";
    int32_t keyType = 1;

    EXPECT_CALL(*mockCallback_, OnStorageSaveInfo(ketSetId, mimeType, sessionId, keyType)).Times(1);
    g_callback->OnStorageSaveInfo(ketSetId, mimeType, sessionId, keyType);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnStorageSaveInfo(ketSetId, mimeType, sessionId, keyType);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_014.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageLoadInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_014, TestSize.Level1)
{
    std::string sessionId = "sessionId";

    EXPECT_CALL(*mockCallback_, OnStorageLoadInfo(sessionId)).Times(1);
    g_callback->OnStorageLoadInfo(sessionId);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnStorageLoadInfo(sessionId);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_015.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageLoadInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_015, TestSize.Level1)
{
    std::string sessionId = "sessionId";

    EXPECT_CALL(*mockCallback_, OnStorageClearInfoForKeyRelease(sessionId)).Times(1);
    g_callback->OnStorageClearInfoForKeyRelease(sessionId);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnStorageClearInfoForKeyRelease(sessionId);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_016.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageLoadInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_016, TestSize.Level1)
{
    std::string sessionId = "sessionId";

    EXPECT_CALL(*mockCallback_, OnStorageClearInfoForLoadFail(sessionId)).Times(1);
    g_callback->OnStorageClearInfoForLoadFail(sessionId);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnStorageClearInfoForLoadFail(sessionId);
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_017.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageLoadInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_017, TestSize.Level1)
{
    std::string emeId = "emeId";
    bool isRelease = true;
    g_callback->UpdateEmeId(emeId, isRelease);
    EXPECT_EQ(emeId, g_callback->EmeId());
    EXPECT_EQ(isRelease, g_callback->IsRelease());
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_018.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageLoadInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_018, TestSize.Level1)
{
    std::string mimeType = "video/mp4";
    int32_t type = 0;

    g_callback->UpdateMimeType(mimeType, type);
    EXPECT_EQ(mimeType, g_callback->MimeType());
    EXPECT_EQ(type, g_callback->Type());
}

/**
 * @tc.name: DrmCallbackImplTest_DrmCallbackImpl_019.
 * @tc.desc: test of DrmCallbackImpl :: OnStorageLoadInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmCallbackImplTest, DrmCallbackImplTest_DrmCallbackImpl_019, TestSize.Level1)
{
    EXPECT_CALL(*mockCallback_, OnMediaLicenseReady(true)).Times(1);
    g_callback->OnMediaLicenseReady(true);
    EXPECT_NE(g_callback->callbackAdapter_, nullptr);
    g_callback->callbackAdapter_ = nullptr;
    g_callback->OnMediaLicenseReady(true);
}

void DrmAdapterImplTest::SetUpTestCase(void) {}

void DrmAdapterImplTest::TearDownTestCase(void) {}

void DrmAdapterImplTest::SetUp(void)
{
    g_adapter = std::make_shared<DrmAdapterImpl>();
    mockCallback_ = std::make_shared<NiceMock<DrmCallbackAdapterMock>>();
}

void DrmAdapterImplTest::TearDown(void)
{
    g_adapter = nullptr;
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_001.
 * @tc.desc: test of DrmAdapterImpl :: IsSupported
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_001, TestSize.Level1)
{
    EXPECT_FALSE(g_adapter->IsSupported(""));
    GetKeySystemName();
    if (g_isSupportDrm) {
        EXPECT_TRUE(g_adapter->IsSupported(GetKeySystemName()));
    } else {
        EXPECT_FALSE(g_adapter->IsSupported(GetKeySystemName()));
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_002.
 * @tc.desc: test of DrmAdapterImpl :: IsSupported2
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_002, TestSize.Level1)
{
    EXPECT_FALSE(g_adapter->IsSupported2("", ""));
    EXPECT_FALSE(g_adapter->IsSupported2("example", ""));

    bool isSupported = g_adapter->IsSupported2(GetKeySystemName(), "video/mp4");
    if (g_isSupportDrm) {
        EXPECT_EQ(isSupported, true);
    } else {
        EXPECT_EQ(isSupported, false);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_003.
 * @tc.desc: test of DrmAdapterImpl :: IsSupported3
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_003, TestSize.Level1)
{
    EXPECT_FALSE(g_adapter->IsSupported3("", "", CONTENT_PROTECTION_LEVEL_HW_CRYPTO));
    EXPECT_FALSE(g_adapter->IsSupported3("example", "", CONTENT_PROTECTION_LEVEL_HW_CRYPTO));
    EXPECT_FALSE(g_adapter->IsSupported3("example", "video/mp4", CONTENT_PROTECTION_LEVEL_HW_CRYPTO));
    bool isSupported = g_adapter->IsSupported3(GetKeySystemName(), "video/mp4", CONTENT_PROTECTION_LEVEL_HW_CRYPTO);
    if (g_isSupportDrm) {
        EXPECT_EQ(isSupported, true);
    } else {
        EXPECT_EQ(isSupported, false);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_004.
 * @tc.desc: test of DrmAdapterImpl :: CreateKeySystem
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_004, TestSize.Level1)
{
    g_adapter->RegistDrmCallback(mockCallback_);

    auto result = g_adapter->CreateKeySystem("", "", SECURITY_LEVEL_3);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    result = g_adapter->CreateKeySystem("com.test.drm", "", SECURITY_LEVEL_3);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    result = g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        int32_t certStatus = -1;
        g_adapter->GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
            EXPECT_NE(g_adapter->GetSecurityLevel(), -1);
            g_adapter->ReleaseMediaKeySession();
            g_adapter->ReleaseMediaKeySystem();

            result = g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_1);
            EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
            EXPECT_NE(g_adapter->GetSecurityLevel(), -1);
            g_adapter->ReleaseMediaKeySession();
            g_adapter->ReleaseMediaKeySystem();
        } else {
            EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
        }

        EXPECT_EQ(g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_UNKNOWN),
            static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();

        EXPECT_EQ(g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", -1),
            static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_005.
 * @tc.desc: test of DrmAdapterImpl :: ReleaseMediaKeySystem
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_005, TestSize.Level1)
{
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        g_adapter->ReleaseMediaKeySession();
        int32_t result = g_adapter->ReleaseMediaKeySystem();
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

        result = g_adapter->ReleaseMediaKeySystem();
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    } else {
        int32_t result = g_adapter->ReleaseMediaKeySystem();
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_006.
 * @tc.desc: test of DrmAdapterImpl :: CreateMediaKeySession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_006, TestSize.Level1)
{
    g_adapter->RegistDrmCallback(mockCallback_);
    EXPECT_EQ(g_adapter->CreateMediaKeySession(), -1);

    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        int32_t certStatus = -1;
        g_adapter->GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            EXPECT_EQ(g_adapter->CreateMediaKeySession(), 0);
            g_adapter->ReleaseMediaKeySession();
            g_adapter->ReleaseMediaKeySystem();
        } else {
            EXPECT_NE(g_adapter->CreateMediaKeySession(), 0);
        }
    } else {
        EXPECT_NE(g_adapter->CreateMediaKeySession(), 0);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_007.
 * @tc.desc: test of DrmAdapterImpl :: ReleaseMediaKeySession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_007, TestSize.Level1)
{
    int32_t result = g_adapter->ReleaseMediaKeySession();
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        int32_t certStatus = -1;
        g_adapter->GetCertificateStatus(certStatus);
        result = g_adapter->ReleaseMediaKeySession();
        if (certStatus == 0) {
            EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        } else {
            EXPECT_NE(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        }
        g_adapter->ReleaseMediaKeySystem();
    } else {
        result = g_adapter->ReleaseMediaKeySession();
        EXPECT_NE(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_008.
 * @tc.desc: test of DrmAdapterImpl :: ReleaseMediaKeySession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_008, TestSize.Level1)
{
    g_adapter->callback_ = nullptr;
    int32_t result = g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    int32_t certStatus = -1;
    g_adapter->GetCertificateStatus(certStatus);
    if (certStatus == 0) {
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
    } else {
        EXPECT_NE(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
    }
    g_adapter->mediaKeySessionCallbackMap_.clear();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_009.
 * @tc.desc: test of DrmAdapterImpl :: SetConfigurationString
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_009, TestSize.Level1)
{
    int32_t result = g_adapter->SetConfigurationString("", "");
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    result = g_adapter->SetConfigurationString("version", "");
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    result = g_adapter->SetConfigurationString("version", "2.0");
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_010.
 * @tc.desc: test of DrmAdapterImpl :: GetConfigurationString
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_010, TestSize.Level1)
{
    char value[256];
    int32_t valueLen = sizeof(value);
    int32_t result;

    result = g_adapter->GetConfigurationString("", value, valueLen);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    result = g_adapter->GetConfigurationString("version", nullptr, valueLen);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    result = g_adapter->GetConfigurationString("version", value, valueLen);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    if (g_isSupportDrm) {
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        g_adapter->SetConfigurationString("version", "2.0");

        result = g_adapter->GetConfigurationString("testVersion", value, valueLen);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

        result = g_adapter->GetConfigurationString("version", value, valueLen);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_011.
 * @tc.desc: test of DrmAdapterImpl :: SetConfigurationByteArray
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_011, TestSize.Level1)
{
    uint8_t description[4] = { 0x00, 0x00, 0x00, 0x00 };
    int32_t valueLen = sizeof(description);

    EXPECT_EQ(g_adapter->SetConfigurationByteArray("", description, valueLen),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    EXPECT_EQ(g_adapter->SetConfigurationByteArray("description", nullptr, valueLen),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    EXPECT_EQ(g_adapter->SetConfigurationByteArray("description", description, valueLen),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_012.
 * @tc.desc: test of DrmAdapterImpl :: GetConfigurationByteArray
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_012, TestSize.Level1)
{
    uint8_t description[4] = { 0x00, 0x00, 0x00, 0x00 };
    int32_t descriptionValueLen = 32;

    EXPECT_EQ(g_adapter->GetConfigurationByteArray("", description, &descriptionValueLen),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    EXPECT_EQ(g_adapter->GetConfigurationByteArray("description", nullptr, &descriptionValueLen),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    EXPECT_EQ(g_adapter->GetConfigurationByteArray("description", description, &descriptionValueLen),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    EXPECT_EQ(g_adapter->GetConfigurationByteArray("description", description, nullptr),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    if (g_isSupportDrm) {
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);

        unsigned char configByteArray[128];
        int32_t byteArrayLen = 128;
        g_adapter->SetConfigurationByteArray("deviceUniqueId", description, descriptionValueLen);
        auto result = g_adapter->GetConfigurationByteArray("deviceUniqueId", configByteArray, &byteArrayLen);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        result = g_adapter->GetConfigurationByteArray("testArrayConfig", configByteArray, &byteArrayLen);
        EXPECT_NE(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_013.
 * @tc.desc: test of DrmAdapterImpl :: GetMaxContentProtectionLevel
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_013, TestSize.Level1)
{
    int32_t level = 0;

    EXPECT_EQ(g_adapter->GetMaxContentProtectionLevel(level), static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    if (g_isSupportDrm) {
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        EXPECT_EQ(g_adapter->GetMaxContentProtectionLevel(level), static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        EXPECT_NE(level, CONTENT_PROTECTION_LEVEL_UNKNOWN);
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_014.
 * @tc.desc: test of DrmAdapterImpl :: ProcessKeySystemResponse
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_014, TestSize.Level1)
{
    std::string KeySystemResponse = "response";
    EXPECT_EQ(g_adapter->ProcessKeySystemResponse("", false), static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    EXPECT_EQ(g_adapter->ProcessKeySystemResponse(KeySystemResponse, false),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    g_adapter->RegistDrmCallback(mockCallback_);
    EXPECT_EQ(g_adapter->ProcessKeySystemResponse(KeySystemResponse, false),
        static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_015.
 * @tc.desc: test of DrmAdapterImpl :: GenerateMediaKeyRequest
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_015, TestSize.Level1)
{
    std::string emeId = "invalidEmeId";
    int32_t type = 1;
    int32_t initDataLen = 128;
    std::vector<uint8_t> initData;
    initData.resize(128);
    uint32_t optionsCount = 1;
    int32_t result = g_adapter->GenerateMediaKeyRequest(emeId, type, initDataLen, initData, "video/avc", optionsCount);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    g_adapter->RegistDrmCallback(mockCallback_);
    result = g_adapter->GenerateMediaKeyRequest(emeId, type, initDataLen, initData, "video/avc", optionsCount);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_017.
 * @tc.desc: test of DrmAdapterImpl :: ClearMediaKeys
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_017, TestSize.Level1)
{
    EXPECT_EQ(g_adapter->ClearMediaKeys(), static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));

    if (g_isSupportDrm) {
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        int32_t certStatus = -1;
        g_adapter->GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            EXPECT_EQ(g_adapter->ClearMediaKeys(), static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        } else {
            EXPECT_NE(g_adapter->ClearMediaKeys(), static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_019.
 * @tc.desc: test of DrmAdapterImpl :: GetCertificateStatus
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_019, TestSize.Level1)
{
    int32_t certStatus;
    int32_t result = g_adapter->GetCertificateStatus(certStatus);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    if (g_isSupportDrm) {
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        result = g_adapter->GetCertificateStatus(certStatus);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_020.
 * @tc.desc: test of DrmAdapterImpl :: PutSessionInfo & GetSessionInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_020, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0); // Assume this constructor exists
    std::string mimeType = "video/mp4";
    int32_t sessionType = 1; // Assume valid session type
    auto sessionInfo = g_adapter->GetSessionInfo(nullptr);
    EXPECT_EQ(sessionInfo, nullptr);
    sessionInfo = g_adapter->GetSessionInfo(validSessionId);
    EXPECT_EQ(sessionInfo, nullptr);
    g_adapter->PutSessionInfo(validSessionId, mimeType, sessionType);

    sessionInfo = g_adapter->GetSessionInfo(validSessionId);
    ASSERT_NE(sessionInfo, nullptr);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_022.
 * @tc.desc: test of DrmAdapterImpl :: RegistDrmCallback
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_022, TestSize.Level1)
{
    int32_t result = g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);

    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK)); // Check that the return value is as expected
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_023.
 * @tc.desc: test of DrmAdapterImpl :: GetSessionIdByEmeId
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_023, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    std::string mimeType = "video/mp4";
    int32_t sessionType = 1;

    g_adapter->PutSessionInfo(validSessionId, mimeType, sessionType);
    auto sessionId = g_adapter->GetSessionIdByEmeId(validSessionId->EmeId());

    EXPECT_NE(sessionId, nullptr);
    EXPECT_EQ(sessionId->EmeId(), validSessionId->EmeId());

    sessionId = g_adapter->GetSessionIdByEmeId("invalid_eme_id");
    EXPECT_EQ(sessionId, nullptr);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_024.
 * @tc.desc: test of DrmAdapterImpl :: RemoveSessionInfo
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_024, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    std::string mimeType = "video/mp4";
    int32_t sessionType = 1;

    g_adapter->PutSessionInfo(validSessionId, mimeType, sessionType);
    auto sessionInfo = g_adapter->GetSessionInfo(validSessionId);
    ASSERT_NE(sessionInfo, nullptr);

    g_adapter->RemoveSessionInfo(validSessionId);
    sessionInfo = g_adapter->GetSessionInfo(validSessionId);
    EXPECT_EQ(sessionInfo, nullptr);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_026.
 * @tc.desc: test of DrmAdapterImpl :: SetKeyType
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_026, TestSize.Level1)
{
    std::vector<uint8_t> keySetId = { 0x01, 0x02, 0x03 };
    auto sessionId = std::make_shared<SessionId>("sessionId", keySetId.data(), keySetId.size());
    auto sessionInfo = std::make_shared<SessionInfo>(
        sessionId, "mimeType", static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    g_adapter->SetKeyType(nullptr, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->PutSessionInfo(sessionId, "mimeType", static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    EXPECT_CALL(*mockCallback_, OnStorageSaveInfo(_, _, _, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE)))
        .Times(1);
    g_adapter->SetKeyType(sessionId, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    g_adapter->RemoveSessionInfo(sessionId);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_027.
 * @tc.desc: test of DrmAdapterImpl :: SetKeySetId
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_027, TestSize.Level1)
{
    auto sessionId = std::make_shared<SessionId>("sessionId", nullptr, 0);
    unsigned char mediaKeyId[] = { 0x01, 0x02, 0x03 };
    g_adapter->SetKeySetId(sessionId, mediaKeyId, sizeof(mediaKeyId));
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->PutSessionInfo(sessionId, "mimeType", static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    EXPECT_CALL(*mockCallback_, OnStorageSaveInfo(_, _, _, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE)))
        .Times(1);
    g_adapter->SetKeySetId(sessionId, mediaKeyId, sizeof(mediaKeyId));
    g_adapter->RemoveSessionInfo(sessionId);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_028.
 * @tc.desc: test of DrmAdapterImpl :: LoadSessionWithLoadedStorage
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_028, TestSize.Level1)
{
    auto sessionId = std::make_shared<SessionId>("sessionId", nullptr, 0);
    EXPECT_CALL(*mockCallback_, OnPromiseResolvedWithSession(_, "sessionId")).Times(0);
    g_adapter->LoadSessionWithLoadedStorage(sessionId, 1);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_029.
 * @tc.desc: test of DrmAdapterImpl :: LoadSessionWithLoadedStorage
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_029, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    std::string mimeType = "video/mp4";
    uint32_t promiseId = 1;
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));
    EXPECT_CALL(*mockCallback_, OnPromiseResolvedWithSession(_, _)).Times(1);
    g_adapter->LoadSessionWithLoadedStorage(validSessionId, promiseId);
    g_adapter->RemoveSessionInfo(validSessionId);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_030.
 * @tc.desc: test of DrmAdapterImpl :: LoadSessionWithLoadedStorage
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_030, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0); // Assume this constructor exists
    std::string mimeType = "video/mp4";
    uint32_t promiseId = 1; // Example promiseId
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_ONLINE));

    EXPECT_CALL(*mockCallback_, OnPromiseResolvedWithSession(_, "sessionId")).Times(0);
    g_adapter->LoadSessionWithLoadedStorage(validSessionId, promiseId);
    g_adapter->RemoveSessionInfo(validSessionId);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_031.
 * @tc.desc: test of DrmAdapterImpl :: LoadSessionWithLoadedStorage
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_031, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    std::string mimeType = "video/mp4";
    uint32_t promiseId = 1;
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
    g_adapter->LoadSessionWithLoadedStorage(validSessionId, promiseId);
    EXPECT_CALL(*mockCallback_, OnPromiseResolvedWithSession(_, "sessionId")).Times(0);
    g_adapter->LoadSessionWithLoadedStorage(validSessionId, promiseId);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_032.
 * @tc.desc: test of DrmAdapterImpl :: LoadSessionWithLoadedStorage
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_032, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    std::string mimeType = "video/mp4";
    uint32_t promiseId = 1; // Example promiseId
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->RegistDrmCallback(mockCallback_);
    int32_t certStatus = -1;
    g_adapter->GetCertificateStatus(certStatus);
    if (certStatus == 0) {
        g_adapter->PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_OFFLINE));
        g_adapter->LoadSessionWithLoadedStorage(validSessionId, promiseId);
        EXPECT_CALL(*mockCallback_, OnStorageClearInfoForLoadFail(_)).Times(1);
        g_adapter->LoadSessionWithLoadedStorage(validSessionId, promiseId);
        EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
    } else {
        EXPECT_EQ(g_adapter->drmKeySessoin_, nullptr);
    }
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_033.
 * @tc.desc: test of DrmAdapterImpl :: HandleKeyUpdatedCallback
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_033, TestSize.Level1)
{
    uint32_t promiseId = 2;
    EXPECT_CALL(*mockCallback_, OnPromiseResolved(_)).Times(1);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->HandleKeyUpdatedCallback(promiseId, true);

    EXPECT_CALL(*mockCallback_, OnPromiseRejected(_, _)).Times(1);

    g_adapter->HandleKeyUpdatedCallback(promiseId, false);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_035.
 * @tc.desc: test of DrmAdapterImpl :: ProcessKeySystemResponse
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_035, TestSize.Level1)
{
    std::string response = "valid_response";
    int32_t result = g_adapter->ProcessKeySystemResponse(response, true);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    result = g_adapter->ProcessKeySystemResponse(response, false);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_039.
 * @tc.desc: test of DrmAdapterImpl :: SystemCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_039, TestSize.Level1)
{
    DRM_EventType eventType = EVENT_DRM_BASE;
    uint8_t info[] = { 0 };
    int32_t infoLen = sizeof(info);
    char extra[] = "extra";

    EXPECT_CALL(*mockCallback_, OnProvisionRequest(testing::_, testing::_)).Times(0);

    MediaKeySystem* drmKeySystem = nullptr;
    auto result = g_adapter->SystemCallBackWithObj(drmKeySystem, eventType, info, infoLen, extra);
    EXPECT_EQ(result, DRM_ERR_INVALID_VAL);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_040.
 * @tc.desc: test of DrmAdapterImpl :: SystemCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_040, TestSize.Level1)
{
    uint8_t info[10] = { 0 };
    char extra[10] = "extra";
    DRM_EventType eventType = EVENT_DRM_BASE;
    int32_t infoLen = sizeof(info);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);

    Drm_ErrCode ret = g_adapter->SystemCallBackWithObj(g_adapter->drmKeySystem_, eventType, info, infoLen, extra);
    EXPECT_EQ(ret, DRM_ERR_INVALID_VAL);

    g_adapter->mediaKeySystemCallbackMap_.clear();
    ret = g_adapter->SystemCallBackWithObj(g_adapter->drmKeySystem_, eventType, info, infoLen, extra);
    EXPECT_EQ(ret, DRM_ERR_INVALID_VAL);

    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_041.
 * @tc.desc: test of DrmAdapterImpl :: SessoinEventCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_041, TestSize.Level1)
{
    DRM_EventType eventType = EVENT_DRM_BASE;
    uint8_t info[] = { 0 };
    int32_t infoLen = sizeof(info);
    char extra[] = "extra";
    MediaKeySession* mediaKeySession = nullptr;

    Drm_ErrCode result = g_adapter->SessoinEventCallBackWithObj(mediaKeySession, eventType, info, infoLen, extra);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_042.
 * @tc.desc: test of DrmAdapterImpl :: SessoinKeyChangeCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_042, TestSize.Level1)
{
    bool newKeysAvailable = true;
    DRM_KeysInfo keysInfo;
    MediaKeySession* mediaKeySession = nullptr;

    Drm_ErrCode result = g_adapter->SessoinKeyChangeCallBackWithObj(mediaKeySession, &keysInfo, newKeysAvailable);

    EXPECT_EQ(result, DRM_ERR_INVALID_VAL);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_043.
 * @tc.desc: test of DrmAdapterImpl :: GetSecurityLevel
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_043, TestSize.Level1)
{
    EXPECT_EQ(g_adapter->GetSecurityLevel(), -1);
    if (g_isSupportDrm) {
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        int32_t certStatus = -1;
        g_adapter->GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            EXPECT_NE(g_adapter->GetSecurityLevel(), -1);
        } else {
            EXPECT_EQ(g_adapter->GetSecurityLevel(), -1);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_044.
 * @tc.desc: test of DrmAdapterImpl :: RequireSecureDecoderModule
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_044, TestSize.Level1)
{
    std::string mimeType = "video/avc";
    bool status = false;
    int32_t result = g_adapter->RequireSecureDecoderModule(mimeType, status);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    result = g_adapter->RequireSecureDecoderModule("", status);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_ERROR));
    if (g_isSupportDrm) {
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        result = g_adapter->RequireSecureDecoderModule(mimeType, status);
        int32_t certStatus = -1;
        g_adapter->GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        } else {
            EXPECT_NE(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_045.
 * @tc.desc: test of DrmAdapterImpl :: GetUUID
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_045, TestSize.Level1)
{
    if (g_isSupportDrm) {
        std::string name = GetKeySystemName();
        auto result = g_adapter->GetUUID(name);
        EXPECT_EQ(result.empty(), false);

        result = g_adapter->GetUUID("com.test.drm");
        EXPECT_NE(result.empty(), false);
    } else {
        std::string name = GetKeySystemName();
        auto result = g_adapter->GetUUID(name);
        EXPECT_NE(result.empty(), false);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_046.
 * @tc.desc: test of DrmAdapterImpl :: StorageProvisionedResult
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_046, TestSize.Level1)
{
    g_adapter->StorageProvisionedResult(false);
    EXPECT_EQ(g_adapter->drmKeySessoin_, nullptr);

    if (g_isSupportDrm) {
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->contentProtectionLevel_ = CONTENT_PROTECTION_LEVEL_SW_CRYPTO;

        int32_t certStatus = -1;
        int32_t result = g_adapter->GetCertificateStatus(certStatus);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        if (certStatus == 0) {
            g_adapter->ReleaseMediaKeySession();
            g_adapter->StorageProvisionedResult(true);
            EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_047.
 * @tc.desc: test of DrmAdapterImpl :: StorageSaveInfoResult
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_047, TestSize.Level1)
{
    g_adapter->StorageSaveInfoResult(true, 1);
    g_adapter->RegistDrmCallback(mockCallback_);
    EXPECT_CALL(*mockCallback_, OnPromiseResolved(_)).Times(1);
    g_adapter->StorageSaveInfoResult(true, 1);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_048.
 * @tc.desc: test of DrmAdapterImpl :: StorageSaveInfoResult
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_048, TestSize.Level1)
{
    g_adapter->StorageSaveInfoResult(false, 1);
    g_adapter->RegistDrmCallback(mockCallback_);
    EXPECT_CALL(*mockCallback_, OnPromiseRejected(_, _)).Times(1);
    g_adapter->StorageSaveInfoResult(false, 1);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_055.
 * @tc.desc: test of DrmAdapterImpl :: CloseSession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_055, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0);
    uint32_t promiseId = 1;
    std::string emeId = "test_eme_id";
    g_adapter->RegistDrmCallback(mockCallback_);
    int32_t result = g_adapter->CloseSession(promiseId, emeId);
    EXPECT_EQ(result, -1);
    if (g_isSupportDrm) {
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        result = g_adapter->CloseSession(promiseId, emeId);
        EXPECT_EQ(result, -1);
        std::string mimeType = "video/mp4";
        int32_t sessionType = 1; // Assume valid session type
        g_adapter->PutSessionInfo(validSessionId, mimeType, sessionType);
        result = g_adapter->CloseSession(promiseId, emeId);
        EXPECT_EQ(result, 0);
        g_adapter->RemoveSessionInfo(validSessionId);
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_056.
 * @tc.desc: test of DrmAdapterImpl :: RemoveSession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_056, TestSize.Level1)
{
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0); // Assume this constructor exists
    std::string mimeType = "video/mp4";
    uint32_t promiseId = 1;
    std::string emeId = "test_eme_id";
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->PutSessionInfo(validSessionId, mimeType, MEDIA_KEY_TYPE_ONLINE);
    int32_t result = g_adapter->RemoveSession(promiseId, "invalid_eme_id");
    EXPECT_EQ(result, -1);

    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    g_adapter->PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));
    result = g_adapter->RemoveSession(promiseId, emeId);
    EXPECT_EQ(result, 0);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_057.
 * @tc.desc: test of DrmAdapterImpl :: LoadSession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_057, TestSize.Level1)
{
    uint32_t promiseId = 1;
    std::string sessionId = "sessionId";
    int32_t result = g_adapter->LoadSession(promiseId, sessionId);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);

    result = g_adapter->LoadSession(promiseId, sessionId);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_058.
 * @tc.desc: test of DrmAdapterImpl :: UpdateSession
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_058, TestSize.Level1)
{
    uint32_t promiseId = 1;
    std::string emeId = "test_eme_id";
    std::vector<uint8_t> response = { 0x01, 0x02, 0x03 };                         // Example response data
    auto validSessionId = std::make_shared<SessionId>("test_eme_id", nullptr, 0); // Assume this constructor exists
    std::string mimeType = "video/mp4";
    int32_t result = g_adapter->UpdateSession(promiseId, emeId, response);
    EXPECT_EQ(result, -1);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);

    g_adapter->PutSessionInfo(validSessionId, mimeType, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));
    result = g_adapter->UpdateSession(promiseId, emeId, response);
    EXPECT_EQ(result, -1);
    g_adapter->RemoveSessionInfo(validSessionId);
    g_adapter->ReleaseMediaKeySession();
    g_adapter->ReleaseMediaKeySystem();
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_059.
 * @tc.desc: test of DrmAdapterImpl::OnSessionExpirationUpdate
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_059, TestSize.Level1)
{
    g_adapter->callback_ = nullptr;
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);
        g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, nullptr, 0);
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
        g_adapter->RegistDrmCallback(mockCallback_);
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);
        int32_t certStatus = -1;
        int32_t result = g_adapter->GetCertificateStatus(certStatus);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        if (certStatus == 0) {
            EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
            EXPECT_CALL(*mockCallback_, OnSessionExpirationUpdate(_, _)).Times(1);
            std::string infoString = "123456ms";
            uint32_t infoSize = infoString.size();
            uint8_t info[32] = { 0 };
            errno_t ret = memcpy_s(info, infoSize, infoString.data(), infoSize);
            EXPECT_EQ(ret, EOK);
            g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, info, infoSize);
        } else {
            EXPECT_EQ(g_adapter->drmKeySessoin_, nullptr);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_060.
 * @tc.desc: test of DrmAdapterImpl::GetKeyRequest
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_060, TestSize.Level1)
{
    uint8_t info[10] = { 0 };
    int32_t infoLen = sizeof(info);
    g_adapter->GetKeyRequest(nullptr, info, infoLen);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);

        int32_t certStatus = -1;
        int32_t result = g_adapter->GetCertificateStatus(certStatus);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

        if (certStatus == 0 && g_isWisePlay) {
            EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
            g_adapter->GetKeyRequest(g_adapter->drmKeySessoin_, info, infoLen);

            g_adapter->callback_ = nullptr;
            g_adapter->GetKeyRequest(g_adapter->drmKeySessoin_, info, infoLen);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_061.
 * @tc.desc: test of DrmAdapterImpl :: SessoinEventCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_061, TestSize.Level1)
{
    DRM_EventType eventType = EVENT_KEY_REQUIRED;
    uint8_t info[] = { 0 };
    int32_t infoLen = sizeof(info);
    char extra[] = "extra";
    MediaKeySession* mediaKeySession = nullptr;

    Drm_ErrCode result = g_adapter->SessoinEventCallBackWithObj(mediaKeySession, eventType, info, infoLen, extra);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

    eventType = EVENT_EXPIRATION_UPDATE;
    result = g_adapter->SessoinEventCallBackWithObj(mediaKeySession, eventType, info, infoLen, extra);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

    eventType = EVENT_PROVISION_REQUIRED;
    result = g_adapter->SessoinEventCallBackWithObj(mediaKeySession, eventType, info, infoLen, extra);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

    eventType = EVENT_KEY_EXPIRED;
    result = g_adapter->SessoinEventCallBackWithObj(mediaKeySession, eventType, info, infoLen, extra);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

    eventType = EVENT_VENDOR_DEFINED;
    result = g_adapter->SessoinEventCallBackWithObj(mediaKeySession, eventType, info, infoLen, extra);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

    result =
        g_adapter->SessoinEventCallBackWithObj(mediaKeySession, static_cast<DRM_EventType>(0), info, infoLen, extra);
    EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_062.
 * @tc.desc: test of DrmAdapterImpl :: SessoinKeyChangeCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_062, TestSize.Level1)
{
    bool newKeysAvailable = true;
    DRM_KeysInfo keysInfo;
    keysInfo.keysInfoCount = 0;

    Drm_ErrCode result =
        g_adapter->SessoinKeyChangeCallBackWithObj(g_adapter->drmKeySessoin_, nullptr, newKeysAvailable);
    EXPECT_EQ(result, DRM_ERR_INVALID_VAL);

    if (g_isSupportDrm) {
        g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);

        int32_t certStatus = -1;
        int32_t certResult = g_adapter->GetCertificateStatus(certStatus);
        EXPECT_EQ(certResult, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

        if (certStatus == 0) {
            EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
            g_adapter->RegistDrmCallback(mockCallback_);
            result = g_adapter->SessoinKeyChangeCallBackWithObj(g_adapter->drmKeySessoin_, &keysInfo, newKeysAvailable);
            EXPECT_EQ(result, DRM_ERR_OK);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_063.
 * @tc.desc: test of DrmAdapterImpl::StorageLoadInfoResult
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_063, TestSize.Level1)
{
    std::vector<uint8_t> keySetId;
    keySetId.clear();
    g_adapter->callback_ = nullptr;
    g_adapter->StorageLoadInfoResult("", keySetId, "", 0);

    g_adapter->RegistDrmCallback(mockCallback_);
    EXPECT_CALL(*mockCallback_, OnPromiseResolvedWithSession(_, _)).Times(1);
    g_adapter->StorageLoadInfoResult("", keySetId, "", 0);
    EXPECT_NE(g_adapter->callback_, nullptr);
    g_adapter->callback_ = nullptr;
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_064.
 * @tc.desc: test of DrmAdapterImpl::ClearPersistentSessionInfoFroKeyRelease
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_064, TestSize.Level1)
{
    g_adapter->ClearPersistentSessionInfoFroKeyRelease(nullptr);

    auto sessionId = SessionId::CreateSessionId("testSessionId");
    g_adapter->RegistDrmCallback(mockCallback_);
    EXPECT_CALL(*mockCallback_, OnStorageClearInfoForKeyRelease(_)).Times(1);
    g_adapter->ClearPersistentSessionInfoFroKeyRelease(sessionId);
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_065.
 * @tc.desc: test of DrmAdapterImpl :: SystemCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_065, TestSize.Level1)
{
    uint8_t info[10] = { 0 };
    char extra[10] = "extra";
    DRM_EventType eventType = EVENT_DRM_BASE;
    int32_t infoLen = sizeof(info);
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);
        auto ret = g_adapter->SystemCallBackWithObj(g_adapter->drmKeySystem_, eventType, info, infoLen, extra);
        EXPECT_EQ(ret, DRM_ERR_OK);
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_066.
 * @tc.desc: test of DrmAdapterImpl :: SessoinKeyChangeCallBackWithObj
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_066, TestSize.Level1)
{
    bool newKeysAvailable = true;
    DRM_KeysInfo keysInfo;
    keysInfo.keysInfoCount = 2;
    (void)memset_s(keysInfo.keyId[0], MAX_KEY_ID_LEN, 0x00, MAX_KEY_ID_LEN);
    (void)memset_s(keysInfo.keyId[1], MAX_KEY_ID_LEN, 0x00, MAX_KEY_ID_LEN);
    std::string keyStatus = "USABLE";
    uint32_t statusSize = keyStatus.size();
    (void)memset_s(keysInfo.statusValue[0], MAX_KEY_STATUS_VALUE_LEN, 0x00, MAX_KEY_STATUS_VALUE_LEN);
    errno_t ret = memcpy_s(keysInfo.statusValue[0], statusSize, keyStatus.data(), statusSize);
    EXPECT_EQ(ret, EOK);

    std::string keyStatus2 = "XXXX";
    uint32_t statusSize2 = keyStatus2.size();
    (void)memset_s(keysInfo.statusValue[1], MAX_KEY_STATUS_VALUE_LEN, 0x00, MAX_KEY_STATUS_VALUE_LEN);
    ret = memcpy_s(keysInfo.statusValue[1], statusSize2, keyStatus2.data(), statusSize2);
    EXPECT_EQ(ret, EOK);

    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);

        int32_t certStatus = -1;
        int32_t certResult = g_adapter->GetCertificateStatus(certStatus);
        EXPECT_EQ(certResult, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));

        if (certStatus == 0) {
            EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
            auto result =
                g_adapter->SessoinKeyChangeCallBackWithObj(g_adapter->drmKeySessoin_, &keysInfo, newKeysAvailable);
            EXPECT_EQ(result, DRM_ERR_OK);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_067.
 * @tc.desc: test of DrmAdapterImpl :: StorageSaveInfoResult
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_067, TestSize.Level1)
{
    g_adapter->callback_ = nullptr;
    g_adapter->StorageSaveInfoResult(true, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));
    EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    EXPECT_EQ(g_adapter->drmKeySessoin_, nullptr);

    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);
        g_adapter->releaseEmeId_ = "testId_067";
        g_adapter->StorageSaveInfoResult(true, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));

        g_adapter->RegistDrmCallback(mockCallback_);
        EXPECT_NE(g_adapter->callback_, nullptr);

        int32_t certStatus = -1;
        g_adapter->GetCertificateStatus(certStatus);
        if (certStatus == 0) {
            EXPECT_CALL(*mockCallback_, OnPromiseRejected(_, _)).Times(1);
            g_adapter->StorageSaveInfoResult(true, static_cast<int32_t>(MediaKeyType::MEDIA_KEY_TYPE_RELEASE));
            EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
        } else {
            EXPECT_EQ(g_adapter->drmKeySessoin_, nullptr);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    }
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_068.
 * @tc.desc: test of DrmAdapterImpl::UpdateSessionResult
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_068, TestSize.Level1)
{
    g_adapter->UpdateSessionResult(false, nullptr, nullptr, 0);
    auto sessionId = SessionId::CreateSessionId("tempSessionId068");
    g_adapter->UpdateSessionResult(false, sessionId, nullptr, 0);

    std::string mimeType = "video/mp4";
    int32_t sessionType = 1;

    g_adapter->callback_ = nullptr;
    g_adapter->PutSessionInfo(sessionId, mimeType, sessionType);

    g_adapter->UpdateSessionResult(false, sessionId, nullptr, 0);
    g_adapter->RegistDrmCallback(mockCallback_);
    EXPECT_CALL(*mockCallback_, OnPromiseResolved(_)).Times(1);
    g_adapter->UpdateSessionResult(false, sessionId, nullptr, 0);
    EXPECT_NE(g_adapter->callback_, nullptr);
    EXPECT_EQ(g_adapter->callback_->EmeId(), "tempSessionId068");
}

/**
 * @tc.name: DrmAdapterImplTest_DrmAdapterImpl_069.
 * @tc.desc: test of DrmAdapterImpl::OnSessionExpirationUpdate
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrmAdapterImplTest, DrmAdapterImplTest_DrmAdapterImpl_069, TestSize.Level1)
{
    g_adapter->RegistDrmCallback(mockCallback_);
    g_adapter->CreateKeySystem(GetKeySystemName(), "origin_id", SECURITY_LEVEL_3);
    if (g_isSupportDrm) {
        EXPECT_NE(g_adapter->drmKeySystem_, nullptr);
        int32_t certStatus = -1;
        int32_t result = g_adapter->GetCertificateStatus(certStatus);
        EXPECT_EQ(result, static_cast<int32_t>(DrmResult::DRM_RESULT_OK));
        if (certStatus == 0) {
            EXPECT_NE(g_adapter->drmKeySessoin_, nullptr);
            EXPECT_CALL(*mockCallback_, OnSessionExpirationUpdate(_, _)).Times(6);
            std::string infoString = "123456ms";
            uint8_t info[32] = { 0 };
            errno_t ret = memcpy_s(info, infoString.size(), infoString.data(), infoString.size());
            EXPECT_EQ(ret, EOK);
            g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, info, 0);
            g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, nullptr, 0);
            infoString = "12ms";
            (void)memset_s(info, 32, 0x00, 32);
            ret = memcpy_s(info, infoString.size(), infoString.data(), infoString.size());
            EXPECT_EQ(ret, EOK);
            g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, info, infoString.size());
            infoString = "1";
            (void)memset_s(info, 32, 0x00, 32);
            ret = memcpy_s(info, infoString.size(), infoString.data(), infoString.size());
            EXPECT_EQ(ret, EOK);
            g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, info, infoString.size());
            infoString = "abcdms";
            (void)memset_s(info, 32, 0x00, 32);
            ret = memcpy_s(info, infoString.size(), infoString.data(), infoString.size());
            EXPECT_EQ(ret, EOK);
            g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, info, infoString.size());
            infoString = "123456";
            (void)memset_s(info, 32, 0x00, 32);
            ret = memcpy_s(info, infoString.size(), infoString.data(), infoString.size());
            EXPECT_EQ(ret, EOK);
            g_adapter->OnSessionExpirationUpdate(g_adapter->drmKeySessoin_, info, infoString.size());
        } else {
            EXPECT_EQ(g_adapter->drmKeySessoin_, nullptr);
        }
        g_adapter->ReleaseMediaKeySession();
        g_adapter->ReleaseMediaKeySystem();
    } else {
        EXPECT_EQ(g_adapter->drmKeySystem_, nullptr);
    }
}
} // namespace OHOS::NWeb

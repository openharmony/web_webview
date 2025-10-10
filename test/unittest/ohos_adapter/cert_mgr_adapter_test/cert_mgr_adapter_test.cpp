/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "cert_manager_api.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "securec.h"
#include "syspara/parameters.h"

#define private public
#include "cert_mgr_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
namespace {
constexpr uint32_t DEFAULT_SIGNATURE_LEN = 1024;
uint32_t TEST_FAILURE = -1;
uint32_t TEST_OK = 0;
uint32_t g_cerSize = 0;
uint32_t g_certSum = 0;
uint32_t g_userCertSum = 0;
uint32_t g_appMaxSize = 0;
}

class MockCmApi {
public:
    MOCK_METHOD(int32_t, CmGetUserCertInfo, (const struct CmBlob*, uint32_t, struct CertInfo*));
    MOCK_METHOD(int32_t, CmGetCertInfo, (const struct CmBlob*, uint32_t, struct CertInfo*));
    MOCK_METHOD(int32_t, CmGetUserCertList, (uint32_t, struct CertList*));
    MOCK_METHOD(int32_t, CmGetCertList, (uint32_t, struct CertList*));
    MOCK_METHOD(int32_t, CmGetAppCert, (const struct CmBlob*, uint32_t, struct Credential*));
    MOCK_METHOD(int32_t, CmInit, (const struct CmBlob*, const struct CmSignatureSpec*, struct CmBlob*));
    MOCK_METHOD(int32_t, CmUpdate, (const struct CmBlob*, const struct CmBlob*));
    MOCK_METHOD(int32_t, CmFinish, (const struct CmBlob*, const struct CmBlob*, struct CmBlob*));
    MOCK_METHOD(int32_t, CmAbort, (const struct CmBlob*));
};

static MockCmApi* g_mockCmApi = nullptr;

#ifdef __cplusplus
extern "C" {
#endif

int32_t CmGetUserCertInfo(const struct CmBlob* certUri, uint32_t store, struct CertInfo* certificateInfo)
{
    return g_mockCmApi->CmGetUserCertInfo(certUri, store, certificateInfo);
}

int32_t CmGetCertInfo(const struct CmBlob* certUri, uint32_t store, struct CertInfo* certificateInfo)
{
    return g_mockCmApi->CmGetCertInfo(certUri, store, certificateInfo);
}

int32_t CmGetUserCertList(uint32_t store, struct CertList* certificateList)
{
    return g_mockCmApi->CmGetUserCertList(store, certificateList);
}

int32_t CmGetCertList(uint32_t store, struct CertList* certificateList)
{
    return g_mockCmApi->CmGetCertList(store, certificateList);
}

int32_t CmGetAppCert(const struct CmBlob* keyUri, const uint32_t store, struct Credential* certificate)
{
    return g_mockCmApi->CmGetAppCert(keyUri, store, certificate);
}

int32_t CmInit(const struct CmBlob* authUri, const struct CmSignatureSpec* spec, struct CmBlob* handle)
{
    return g_mockCmApi->CmInit(authUri, spec, handle);
}

int32_t CmUpdate(const struct CmBlob* handle, const struct CmBlob* inData)
{
    return g_mockCmApi->CmUpdate(handle, inData);
}

int32_t CmFinish(const struct CmBlob* handle, const struct CmBlob* inData, struct CmBlob* outData)
{
    return g_mockCmApi->CmFinish(handle, inData, outData);
}

int32_t CmAbort(const struct CmBlob* handle)
{
    return g_mockCmApi->CmAbort(handle);
}

#ifdef __cplusplus
}
#endif
class CertMgrAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::unique_ptr<MockCmApi> mockCmApi_;
};

void CertMgrAdapterTest::SetUpTestCase(void)
{
    const char **perms = new const char *[2]; // 2 permissions
    perms[0] = "ohos.permission.ACCESS_CERT_MANAGER_INTERNAL"; // system_basic
    perms[1] = "ohos.permission.ACCESS_CERT_MANAGER"; // normal
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "TestCertManager",
        .aplStr = "system_basic",
    };

    auto tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;
    *perms = nullptr;
}

void CertMgrAdapterTest::TearDownTestCase(void)
{
}

void CertMgrAdapterTest::SetUp(void)
{
    mockCmApi_ = std::make_unique<MockCmApi>();
    g_mockCmApi = mockCmApi_.get();
}

void CertMgrAdapterTest::TearDown(void)
{
    g_mockCmApi = nullptr;
    mockCmApi_.reset();
}

/**
 * @tc.name: CertMgrAdapterTest_GetCertMaxSize_001
 * @tc.desc: GetCertMaxSize.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetCertMaxSize_001, TestSize.Level1)
{
    EXPECT_CALL(*mockCmApi_, CmGetCertList(_, _)).Times(2).WillOnce(Return(0)).WillOnce(Return(-1));
    CertManagerAdapterImpl adapter;
    g_cerSize = adapter.GetCertMaxSize();
    EXPECT_NE(g_cerSize, TEST_FAILURE);
    g_appMaxSize = adapter.GetAppCertMaxSize();
    EXPECT_NE(g_appMaxSize, TEST_FAILURE);
    g_certSum = adapter.GetSytemRootCertSum();
    EXPECT_EQ(g_certSum, MAX_COUNT_CERTIFICATE);
    g_certSum = adapter.GetSytemRootCertSum();
    EXPECT_EQ(g_certSum, TEST_OK);
}

/**
 * @tc.name: CertMgrAdapterTest_GetAppCert_002
 * @tc.desc: GetAppCert.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetAppCert_002, TestSize.Level1)
{
    CertManagerAdapterImpl adapter;
    uint8_t* certData = static_cast<uint8_t *>(malloc(g_cerSize));
    EXPECT_NE(certData, nullptr);

    EXPECT_CALL(*mockCmApi_, CmGetCertList(_, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmGetCertInfo(_, _, _)).Times(1).WillOnce(Return(0));
    int32_t result = adapter.GetSytemRootCertData(0, certData);
    EXPECT_EQ(result, CM_SUCCESS);

    EXPECT_CALL(*mockCmApi_, CmGetCertList(_, _)).Times(1).WillOnce(Return(-1));
    result = adapter.GetSytemRootCertData(0, certData);
    EXPECT_EQ(result, CM_FAILURE);

    EXPECT_CALL(*mockCmApi_, CmGetCertList(_, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmGetCertInfo(_, _, _)).Times(1).WillOnce(Return(-1));
    result = adapter.GetSytemRootCertData(0, certData);
    EXPECT_EQ(result, CM_FAILURE);

    free(certData);
    certData = nullptr;
}

/**
 * @tc.name: CertMgrAdapterTest_GetUserRootCertSum_003
 * @tc.desc: GetUserRootCertSum.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetUserRootCertSum_003, TestSize.Level1)
{
    EXPECT_CALL(*mockCmApi_, CmGetUserCertList(_, _)).Times(2).WillOnce(Return(-1)).WillOnce(Return(0));
    CertManagerAdapterImpl adapter;
    g_userCertSum = adapter.GetUserRootCertSum();
    EXPECT_EQ(g_userCertSum, TEST_OK);
    g_userCertSum = adapter.GetUserRootCertSum();
    EXPECT_EQ(g_userCertSum, MAX_COUNT_CERTIFICATE);
}

/**
 * @tc.name: CertMgrAdapterTest_GetUserRootCertData_004
 * @tc.desc: GetUserRootCertData.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetUserRootCertData_004, TestSize.Level1)
{
    CertManagerAdapterImpl adapter;
    uint8_t* userData = static_cast<uint8_t *>(malloc(g_cerSize));
    EXPECT_NE(userData, nullptr);

    EXPECT_CALL(*mockCmApi_, CmGetUserCertList(_, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmGetUserCertInfo(_, _, _)).Times(1).WillOnce(Return(0));
    int32_t result = adapter.GetUserRootCertData(g_userCertSum, userData);
    EXPECT_EQ(result, CM_SUCCESS);

    EXPECT_CALL(*mockCmApi_, CmGetUserCertList(_, _)).Times(1).WillOnce(Return(-1));
    result = adapter.GetUserRootCertData(g_userCertSum, userData);
    EXPECT_EQ(result, CM_FAILURE);

    EXPECT_CALL(*mockCmApi_, CmGetUserCertList(_, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmGetUserCertInfo(_, _, _)).Times(1).WillOnce(Return(-1));
    result = adapter.GetUserRootCertData(g_userCertSum, userData);
    EXPECT_EQ(result, CM_FAILURE);
    free(userData);
    userData = nullptr;
}

/**
 * @tc.name: CertMgrAdapterTest_GetAppCert_005
 * @tc.desc: GetAppCert.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetAppCert_005, TestSize.Level1)
{
    CertManagerAdapterImpl adapter;
    uint8_t uriData[] = "oh:t=ak;o=inner020;u=0;a=0";
    uint32_t len;
    uint8_t* certData = static_cast<uint8_t *>(malloc(g_appMaxSize));
    EXPECT_NE(certData, nullptr);
    EXPECT_CALL(*mockCmApi_, CmGetAppCert(_, _, _)).Times(1).WillOnce(Return(0));
    int32_t result = adapter.GetAppCert(uriData, certData, &len);
    EXPECT_EQ(result, CM_SUCCESS);

    EXPECT_CALL(*mockCmApi_, CmGetAppCert(_, _, _)).Times(2).WillOnce(Return(-1)).WillOnce(Return(0));
    result = adapter.GetAppCert(uriData, certData, &len);
    EXPECT_EQ(result, CM_SUCCESS);

    EXPECT_CALL(*mockCmApi_, CmGetAppCert(_, _, _))
        .Times(3)
        .WillOnce(Return(-1))
        .WillOnce(Return(-1))
        .WillOnce(Return(0));
    result = adapter.GetAppCert(uriData, certData, &len);
    EXPECT_EQ(result, CM_SUCCESS);

    EXPECT_CALL(*mockCmApi_, CmGetAppCert(_, _, _))
        .Times(3)
        .WillOnce(Return(-1))
        .WillOnce(Return(-1))
        .WillOnce(Return(-1));
    result = adapter.GetAppCert(uriData, certData, &len);
    EXPECT_EQ(result, CM_FAILURE);
    free(certData);
    certData = nullptr;
}

/**
 * @tc.name: CertMgrAdapterTest_Sign_006
 * @tc.desc: Sign.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_Sign_006, TestSize.Level1)
{
    CertManagerAdapterImpl adapter;
    uint8_t uriData[] = "oh:t=ak;o=inner020;u=0;a=0";
    const uint8_t messageData[] = "this is message for private credential sign and verify";
    uint8_t signData[DEFAULT_SIGNATURE_LEN] = {0};

    EXPECT_CALL(*mockCmApi_, CmInit(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmUpdate(_, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmFinish(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmAbort(_)).Times(1).WillOnce(Return(0));
    int32_t result = adapter.Sign(uriData, messageData, sizeof(messageData), signData, sizeof(signData));
    EXPECT_EQ(result, CM_SUCCESS);

    EXPECT_CALL(*mockCmApi_, CmInit(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmUpdate(_, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmFinish(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmAbort(_)).Times(1).WillOnce(Return(-1));
    result = adapter.Sign(uriData, messageData, sizeof(messageData), signData, sizeof(signData));
    EXPECT_EQ(result, CM_FAILURE);

    EXPECT_CALL(*mockCmApi_, CmInit(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmUpdate(_, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmFinish(_, _, _)).Times(1).WillOnce(Return(-1));
    result = adapter.Sign(uriData, messageData, sizeof(messageData), signData, sizeof(signData));
    EXPECT_EQ(result, CM_FAILURE);

    EXPECT_CALL(*mockCmApi_, CmInit(_, _, _)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(*mockCmApi_, CmUpdate(_, _)).Times(1).WillOnce(Return(-1));
    result = adapter.Sign(uriData, messageData, sizeof(messageData), signData, sizeof(signData));
    EXPECT_EQ(result, CM_FAILURE);

    EXPECT_CALL(*mockCmApi_, CmInit(_, _, _)).Times(1).WillOnce(Return(-1));
    result = adapter.Sign(uriData, messageData, sizeof(messageData), signData, sizeof(signData));
    EXPECT_EQ(result, CM_FAILURE);

    uint32_t signDataLen = 0;
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0401);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0501);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0601);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0403);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0503);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0603);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0805);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, &signDataLen, 0x0806);
    EXPECT_EQ(result, -1);
    result = adapter.SignV2(uriData, nullptr, 0, signData, nullptr, 0x0806);
    EXPECT_EQ(result, -1);
    signDataLen = 1000;
    result = adapter.SignV2(uriData, nullptr, 0, nullptr, &signDataLen, 0x0806);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: CertMgrAdapterTest_InitCertList_007
 * @tc.desc: InitCertList.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_InitCertList_007, TestSize.Level1)
{
    CertManagerAdapterImpl adapter;
    int32_t result = adapter.InitCertInfo(nullptr);
    EXPECT_NE(result, 0);
    adapter.FreeCMBlobData(nullptr);
    struct CmBlob *blob = static_cast<CmBlob *>(malloc(g_appMaxSize));
    EXPECT_NE(blob, nullptr);
    blob->data = nullptr;
    adapter.FreeCMBlobData(blob);
    adapter.FreeCertList(nullptr);
    struct CertList *certList = static_cast<CertList *>(malloc(g_appMaxSize));
    EXPECT_NE(certList, nullptr);
    certList->certAbstract = nullptr;
    adapter.FreeCertList(certList);

    char uri[] = "webtest";
    struct CertInfo certInfo;
    unsigned int len = sizeof(struct CertInfo);
    (void)memset_s(&certInfo, len, 0, len);
    EXPECT_CALL(*mockCmApi_, CmGetUserCertInfo(_, _, _)).Times(1).WillOnce(Return(0));
    result = adapter.GetCertInfo(uri, &certInfo, CM_USER_TRUSTED_STORE);
    EXPECT_EQ(result, CM_SUCCESS);

    EXPECT_CALL(*mockCmApi_, CmGetCertInfo(_, _, _)).Times(1).WillOnce(Return(0));
    result = adapter.GetCertInfo(uri, &certInfo, CM_SYSTEM_TRUSTED_STORE);
    EXPECT_EQ(result, CM_SUCCESS);

    result = adapter.GetCertInfo(uri, &certInfo, CM_CREDENTIAL_STORE);
    EXPECT_EQ(result, CM_FAILURE);

    EXPECT_CALL(*mockCmApi_, CmGetUserCertInfo(_, _, _)).Times(1).WillOnce(Return(-1));
    result = adapter.GetCertInfo(uri, &certInfo, CM_USER_TRUSTED_STORE);
    EXPECT_EQ(result, TEST_FAILURE);

    EXPECT_CALL(*mockCmApi_, CmGetCertInfo(_, _, _)).Times(1).WillOnce(Return(-1));
    result = adapter.GetCertInfo(uri, &certInfo, CM_SYSTEM_TRUSTED_STORE);
    EXPECT_EQ(result, TEST_FAILURE);

    char subjectName[] = "test";
    uint8_t certData[] = "inner020";
    result = adapter.GetCertDataBySubject(subjectName, certData, CM_CREDENTIAL_STORE);
    EXPECT_NE(result, 0);
    EXPECT_CALL(*mockCmApi_, CmGetUserCertList(_, _)).Times(1).WillOnce(Return(0));
    result = adapter.GetCertDataBySubject(subjectName, certData, CM_USER_TRUSTED_STORE);
    EXPECT_NE(result, 0);
    EXPECT_CALL(*mockCmApi_, CmGetCertList(_, _)).Times(1).WillOnce(Return(0));
    result = adapter.GetCertDataBySubject(subjectName, certData, CM_SYSTEM_TRUSTED_STORE);
    EXPECT_NE(result, 0);
}
} // namespace OHOS
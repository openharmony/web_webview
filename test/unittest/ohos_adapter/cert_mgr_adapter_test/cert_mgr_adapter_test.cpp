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

class CertMgrAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
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
{}

void CertMgrAdapterTest::SetUp(void)
{}

void CertMgrAdapterTest::TearDown(void)
{}

/**
 * @tc.name: CertMgrAdapterTest_GetTrustAnchorsForHostName_001
 * @tc.desc: GetCertMaxSize.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetTrustAnchorsForHostName_001, TestSize.Level1)
{
    std::string hostname = "";
    std::vector<std::string> certs;
    bool getNameValue = adapter.GetTrustAnchorsForHostName(hostname, certs);
    EXPECT_TRUE(getNameValue);
    getNameValue = adapter.GetTrustAnchorsForHostName("hostnametest", certs);
    EXPECT_TRUE(getNameValue);
}

/**
 * @tc.name: CertMgrAdapterTest_GetPinSetForHostName_002
 * @tc.desc: GetPinSetForHostName.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetPinSetForHostName_002, TestSize.Level1)
{
    CertManagerAdapterImpl adapter;
    const std::string hostname = "testweb";
    std::vector<std::string> pins;
    bool result = adapter.GetPinSetForHostName(hostname, pins);
    EXPECT_TRUE(result);
}
} // namespace OHOS
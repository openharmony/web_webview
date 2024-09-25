/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <testing/gtest/include/gtest/gtest.h>
#include <testing/gmock/include/gmock/gmock.h>

#include <ohos_adapter_helper.h>
#define private public
#include <cert_mgr_adapter_impl.h>

using namespace OHOS;

namespace OHOS::NWeb {

class CertMgrAdapterTest : public testing::Test {
protected:
    CertMgrAdapterTest() {}

    ~CertMgrAdapterTest() override {}

    void SetUp() override {}

    void TearDown() override {}
};

/**
 * @tc.name: CertMgrAdapterTest_GetTrustAnchorsForHostName_001
 * @tc.desc: GetTrustAnchorsForHostName.
 * @tc.type: FUNC
 */
TEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetTrustAnchorsForHostName_001)
{
    std::unique_ptr<CertManagerAdapter> adapter = OhosAdapterHelper::GetInstance().GetRootCertDataAdapter();
    std::string hostname = "";
    std::vector<std::string> certs;
    bool getNameValue = adapter->GetTrustAnchorsForHostName(hostname, certs);
    EXPECT_TRUE(getNameValue);
    getNameValue = adapter->GetTrustAnchorsForHostName("hostnametest", certs);
    EXPECT_TRUE(getNameValue);
}

/**
 * @tc.name: CertMgrAdapterTest_GetPinSetForHostName_002
 * @tc.desc: GetPinSetForHostName.
 * @tc.type: FUNC
 */
TEST_F(CertMgrAdapterTest, CertMgrAdapterTest_GetPinSetForHostName_002)
{
    std::unique_ptr<CertManagerAdapter> adapter = OhosAdapterHelper::GetInstance().GetRootCertDataAdapter();
    const std::string hostname = "testweb";
    std::vector<std::string> pins;
    bool result = adapter->GetPinSetForHostName(hostname, pins);
    EXPECT_TRUE(result);
}
} // namespace OHOS
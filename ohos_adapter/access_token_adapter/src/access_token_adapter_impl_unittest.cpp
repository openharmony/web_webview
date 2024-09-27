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

#include <testing/gtest/include/gtest/gtest.h>
#include <accesstoken/ability_access_control.h>

#include "access_token_adapter_impl.h"
#include "ohos_adapter_helper.h"

using namespace testing;

namespace OHOS::NWeb {

class AccessTokenAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AccessTokenAdapterImplTest::SetUpTestCase(void) {}

void AccessTokenAdapterImplTest::TearDownTestCase(void) {}

void AccessTokenAdapterImplTest::SetUp(void) {}

void AccessTokenAdapterImplTest::TearDown(void) {}

/**
 * @tc.name: AccessTokenAdapterImplTest_AccessTokenAdapterImpl_001.
 * @tc.desc: AccessToken adapter unittest.
 * @tc.type: FUNC.
 */
TEST_F(AccessTokenAdapterImplTest, AccessTokenAdapterImplTest_AccessTokenAdapterImpl_001)
{
    std::string permissionName = "ohos.permission.INTERNET";
    bool isPermissionGranted = OH_AT_CheckSelfPermission(permissionName.c_str());
    EXPECT_FALSE(isPermissionGranted);
}
} // namespace OHOS::NWeb

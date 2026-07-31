/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "test_mock_webview_controller.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS {
namespace NWeb {
class NapiWebviewControllerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NapiWebviewControllerTest::SetUpTestCase(void)
{
}

void NapiWebviewControllerTest::TearDownTestCase(void)
{
}

void NapiWebviewControllerTest::SetUp(void)
{
}

void NapiWebviewControllerTest::TearDown(void)
{
}

/**
 * @tc.name: NapiWebviewControllerTest_GetControllerId_001
 * @tc.desc: Verify controller id auto-increments across different constructors.
 * @tc.type: FUNC
 */
HWTEST_F(NapiWebviewControllerTest, NapiWebviewControllerTest_GetControllerId_001, TestSize.Level1)
{
    WebviewController controller1;
    WebviewController controller2(123);
    WebviewController controller3("test_web_tag");

    uint64_t id1 = controller1.GetControllerId();
    uint64_t id2 = controller2.GetControllerId();
    uint64_t id3 = controller3.GetControllerId();

    EXPECT_NE(id1, 0u);
    EXPECT_EQ(id2, id1 + 1);
    EXPECT_EQ(id3, id2 + 1);
}

/**
 * @tc.name: NapiWebviewControllerTest_GetControllerId_002
 * @tc.desc: Verify default constructor assigns a non-zero and unique controller id.
 * @tc.type: FUNC
 */
HWTEST_F(NapiWebviewControllerTest, NapiWebviewControllerTest_GetControllerId_002, TestSize.Level1)
{
    WebviewController controller1;
    WebviewController controller2;

    uint64_t id1 = controller1.GetControllerId();
    uint64_t id2 = controller2.GetControllerId();

    EXPECT_NE(id1, 0u);
    EXPECT_NE(id2, 0u);
    EXPECT_NE(id1, id2);
    EXPECT_EQ(id2, id1 + 1);
}
} // namespace NWeb
} // namespace OHOS

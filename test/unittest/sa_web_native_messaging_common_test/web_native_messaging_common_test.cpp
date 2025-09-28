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

#include <gtest/gtest.h>

#define private public
#include "base/web/webview/sa/web_native_messaging/common/web_native_messaging_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb::NativeMessageError {

class WebNativeMessagingCommonTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void WebNativeMessagingCommonTest::SetUpTestCase(void) {}

void WebNativeMessagingCommonTest::TearDownTestCase(void) {}

void WebNativeMessagingCommonTest::SetUp(void) {}

void WebNativeMessagingCommonTest::TearDown(void) {}

/**
 * @tc.name  : OHWebNativeMessagingCommonTest_IsNativeMessagingErr_01
 * @tc.desc  : Test IsNativeMessagingErr
 */
HWTEST_F(WebNativeMessagingCommonTest,
         OHWebNativeMessagingCommonTest_IsNativeMessagingErr_01,
         TestSize.Level1)
{
    EXPECT_TRUE(IsNativeMessagingErr(-500));
    EXPECT_FALSE(IsNativeMessagingErr(-1000));
    EXPECT_TRUE(IsNativeMessagingErr(0));
    EXPECT_FALSE(IsNativeMessagingErr(1));
    EXPECT_FALSE(IsNativeMessagingErr(-1001));
}

/**
 * @tc.name  : OHWebNativeMessagingCommonTest_NativeCodeToJsCode_01
 * @tc.desc  : Test NativeCodeToJsCode
 */
HWTEST_F(WebNativeMessagingCommonTest,
         OHWebNativeMessagingCommonTest_NativeCodeToJsCode_01,
         TestSize.Level1)
{
    std::string errorMsg;

    int32_t ret = ConnectNativeRet::PERMISSION_CHECK_ERROR;
    int32_t jsCode = NativeCodeToJsCode(ret, errorMsg);
    EXPECT_EQ(jsCode, 201);
    EXPECT_EQ(errorMsg, "Permission denied.");

    ret = ConnectNativeRet::CONNECTION_NOT_EXIST;
    jsCode = NativeCodeToJsCode(ret, errorMsg);
    EXPECT_EQ(jsCode, 16000011);
    EXPECT_EQ(errorMsg, "This connection is not exist.");

    ret = -100;
    jsCode = NativeCodeToJsCode(ret, errorMsg);
    EXPECT_EQ(jsCode, 16000050);
    EXPECT_EQ(errorMsg, "Internal error, code is -100");
}

} // namespace OHOS::NWeb::NativeMessageError
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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "web_extension_connection_callback.h"

using namespace testing;
using namespace OHOS::NWeb;

class MockIExtensionConnectionCallback : public IExtensionConnectionCallback {
public:
    MOCK_METHOD(void, OnExtensionConnect, (ConnectionNativeInfo & info), (override));
    MOCK_METHOD(void, OnExtensionDisconnect, (ConnectionNativeInfo & info), (override));
    MOCK_METHOD(void, OnExtensionFailed, (int32_t connectionId, int32_t errorNum), (override));
};

class WebExtensionConnectionCallbackTest : public Test {
protected:
    void SetUp() override
    {
        mockCallback_ = std::make_shared<MockIExtensionConnectionCallback>();
        connectionCallback_ = std::make_unique<WebExtensionConnectionCallback>(mockCallback_);
        connectionCallback_->connectionId_ = TEST_CONNECTION_ID;
    }

    void TearDown() override
    {
        mockCallback_ = nullptr;
        connectionCallback_ = nullptr;
    }

    const int32_t TEST_CONNECTION_ID = 123;
    const int32_t TEST_ERROR_NUM = 456;
    std::shared_ptr<MockIExtensionConnectionCallback> mockCallback_;
    std::unique_ptr<WebExtensionConnectionCallback> connectionCallback_;
    ConnectionNativeInfo testInfo_;
};

/**
 * @tc.name: WebExtensionConnectionCallbackTest_OnConnect_001
 * @tc.desc: Verify OnConnect correctly calls the callback
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackTest, OnConnect_ShouldCallCallback)
{
    EXPECT_CALL(*mockCallback_, OnExtensionConnect(_)).Times(1);
    connectionCallback_->OnConnect(testInfo_);
}

/**
 * @tc.name: WebExtensionConnectionCallbackTest_OnDisconnect_001
 * @tc.desc: Verify OnDisconnect correctly calls the callback
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackTest, OnDisconnect_ShouldCallCallback)
{
    EXPECT_CALL(*mockCallback_, OnExtensionDisconnect(_)).Times(1);
    connectionCallback_->OnDisconnect(testInfo_);
}

/**
 * @tc.name: WebExtensionConnectionCallbackTest_OnFailed_001
 * @tc.desc: Verify OnFailed calls callback with correct parameters
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackTest, OnFailed_ShouldCallCallbackWithParams)
{
    EXPECT_CALL(*mockCallback_, OnExtensionFailed(TEST_CONNECTION_ID, TEST_ERROR_NUM)).Times(1);
    connectionCallback_->OnFailed(TEST_ERROR_NUM);
}

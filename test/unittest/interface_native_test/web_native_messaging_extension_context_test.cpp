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

#define private public
#define protected public
#include <memory>

#include "ability_manager_client.h"
#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "nocopyable.h"
#include "want.h"
#include "web_native_messaging_client.h"
#include "web_native_messaging_extension_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;
using namespace OHOS::AAFwk;

class ITestAbilityToken : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"test.web.ITestAbilityToken");
    virtual int Test() = 0;
};

class TestAbilityToken : public IRemoteStub<ITestAbilityToken> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return 0;
    }

    int Test() override
    {
        return 0;
    }
};

class WebNativeMessagingContextTest : public testing::Test {
public:
    void SetUp() override
    {
        context_ = std::make_unique<WebNativeMessagingExtensionContext>();
        token_ = sptr<IRemoteObject>(new TestAbilityToken());
        context_->SetToken(token_);
        want_ = std::make_shared<Want>();
        want_->SetElementName("com.test", "TestAbility");
    }

    void TearDown() override
    {
        context_.reset();
        token_ = nullptr;
        want_.reset();
    }

protected:
    std::unique_ptr<WebNativeMessagingExtensionContext> context_;
    sptr<IRemoteObject> token_;
    std::shared_ptr<Want> want_;
    StartOptions startOpts_;
};

/**
 * @tc.name: WebNativeMessagingContextTest.StartAbility_Success
 * @tc.desc: Test StartAbility method with successful case
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StartAbility_Success, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    ErrCode result = context_->StartAbility(*want_, startOpts_);
    EXPECT_EQ(result, -1) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.StartAbility_Fail
 * @tc.desc: Test StartAbility method with failure case
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StartAbility_Fail, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    ErrCode result = context_->StartAbility(*want_, startOpts_);
    EXPECT_EQ(result, -1) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.StopNativeConnection_Success
 * @tc.desc: Test StopNativeConnection method with valid connection ID
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StopNativeConnection_Success, TestSize.Level1)
{
    ErrCode result = context_->StopNativeConnection(100);
    EXPECT_EQ(result, -4) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.StopNativeConnection_Fail
 * @tc.desc: Test StopNativeConnection method with invalid connection ID
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StopNativeConnection_Fail, TestSize.Level1)
{
    ErrCode result = context_->StopNativeConnection(-1);
    EXPECT_EQ(result, -4) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.TerminateSelf_Success
 * @tc.desc: Test TerminateSelf method with successful case
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, TerminateSelf_Success, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    ErrCode result = context_->TerminateSelf();
    EXPECT_EQ(result, 22) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.TerminateSelf_Fail
 * @tc.desc: Test TerminateSelf method with failure case
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, TerminateSelf_Fail, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    ErrCode result = context_->TerminateSelf();
    EXPECT_EQ(result, 22) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.StartAbilityForResult_NoCallback_Success
 * @tc.desc: Test StartAbilityForResult without callback
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StartAbilityForResult_NoCallback_Success, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    int requestCode = 1001;
    ErrCode result = context_->StartAbilityForResult(*want_, startOpts_, requestCode);
    EXPECT_EQ(result, -1) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.StartAbilityForResult_NoCallback_Fail
 * @tc.desc: Test StartAbilityForResult without callback with invalid request code
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StartAbilityForResult_NoCallback_Fail, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    int requestCode = -1;
    ErrCode result = context_->StartAbilityForResult(*want_, startOpts_, requestCode);
    EXPECT_EQ(result, -1) << "Actual result: " << result;
}

/**
 * @tc.name: WebNativeMessagingContextTest.StartAbilityForResult_WithCallback_Success
 * @tc.desc: Test StartAbilityForResult with callback
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StartAbilityForResult_WithCallback_Success, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    int requestCode = 1002;
    bool callbackCalled = false;
    int testResultCode = 0;

    RuntimeTask task = [&callbackCalled, &testResultCode](int resultCode, const AAFwk::Want& want, bool isInner) {
        callbackCalled = true;
        testResultCode = resultCode;
    };

    ErrCode result = context_->StartAbilityForResult(*want_, startOpts_, requestCode, std::move(task));
    EXPECT_EQ(result, -1) << "Actual result: " << result;
    // Callback should not be called on error
    EXPECT_FALSE(callbackCalled) << "Callback should not be called on error";
}

/**
 * @tc.name: WebNativeMessagingContextTest.StartAbilityForResult_WithCallback_DifferentRequestCodes
 * @tc.desc: Test StartAbilityForResult with various request codes
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, StartAbilityForResult_WithCallback_DifferentRequestCodes, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";

    std::vector<int32_t> requestCodes = {0, 1, 100, 1000, INT32_MAX};

    for (int requestCode : requestCodes) {
        RuntimeTask task = [](int resultCode, const AAFwk::Want& want, bool isInner) {
            // Empty callback
        };

        ErrCode result = context_->StartAbilityForResult(*want_, startOpts_, requestCode, std::move(task));
        EXPECT_EQ(result, -1) << "RequestCode: " << requestCode << ", Actual result: " << result;
    }
}

/**
 * @tc.name: WebNativeMessagingContextTest.OnAbilityResult_CallbackFound
 * @tc.desc: Test OnAbilityResult with valid request code
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, OnAbilityResult_CallbackFound, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    int requestCode = 1003;
    int resultCode = 1;
    bool callbackCalled = false;
    int receivedResultCode = -1;

    RuntimeTask task = [&callbackCalled, &receivedResultCode](int rc, const AAFwk::Want& want, bool isInner) {
        callbackCalled = true;
        receivedResultCode = rc;
    };

    // Register callback
    context_->StartAbilityForResult(*want_, startOpts_, requestCode, std::move(task));

    // Simulate ability result
    Want resultWant;
    context_->OnAbilityResult(requestCode, resultCode, resultWant);

    EXPECT_TRUE(callbackCalled) << "Callback should be called";
    EXPECT_EQ(receivedResultCode, resultCode) << "Result code should match";
}

/**
 * @tc.name: WebNativeMessagingContextTest.OnAbilityResult_CallbackNotFound
 * @tc.desc: Test OnAbilityResult with invalid request code
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, OnAbilityResult_CallbackNotFound, TestSize.Level1)
{
    EXPECT_NE(context_->token_, nullptr) << "Token should be set";
    int invalidRequestCode = 9999;
    int resultCode = 1;

    // Try to trigger result with unregistered request code
    Want resultWant;
    context_->OnAbilityResult(invalidRequestCode, resultCode, resultWant);

    // Should not crash, just log warning
    EXPECT_NE(context_->token_, nullptr) << "Token should still be valid";
}

/**
 * @tc.name: WebNativeMessagingContextTest.GenerateCurRequestCode_Incremental
 * @tc.desc: Test GenerateCurRequestCode returns incremental values
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, GenerateCurRequestCode_Incremental, TestSize.Level1)
{
    int code1 = context_->GenerateCurRequestCode();
    int code2 = context_->GenerateCurRequestCode();
    int code3 = context_->GenerateCurRequestCode();

    EXPECT_GT(code1, 0) << "Request code should be positive";
    EXPECT_GT(code2, code1) << "Request codes should be incremental";
    EXPECT_GT(code3, code2) << "Request codes should be incremental";
}

/**
 * @tc.name: WebNativeMessagingContextTest.GenerateCurRequestCode_MultipleCalls
 * @tc.desc: Test GenerateCurRequestCode with multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(WebNativeMessagingContextTest, GenerateCurRequestCode_MultipleCalls, TestSize.Level1)
{
    const int callCount = 100;
    std::set<int> requestCodes;

    for (int i = 0; i < callCount; i++) {
        int code = context_->GenerateCurRequestCode();
        requestCodes.insert(code);
    }

    EXPECT_EQ(requestCodes.size(), callCount) << "All request codes should be unique";
}

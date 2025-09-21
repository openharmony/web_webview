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
#include "iremote_object.h"
#include "system_ability_definition.h"
#include "web_native_messaging_client.h"
#include "web_native_messaging_load_callback.h"

using namespace testing;
using namespace OHOS;
using namespace OHOS::NWeb;

class MockRemoteObject : public IRemoteObject {
public:
    MockRemoteObject() : IRemoteObject(u"mock_remote_object") {}
    ~MockRemoteObject() override = default;
    int32_t GetObjectRefCount() override
    {
        return 1;
    }
    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return 0;
    }
    int Dump(int fd, const std::vector<std::u16string>& args) override
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return true;
    }
};

class MockWebNativeMessagingClient : public WebNativeMessagingClient {
public:
    MOCK_METHOD(void, OnLoadSystemAbilitySuccess, (const sptr<IRemoteObject>& remoteObject), ());
    MOCK_METHOD(void, OnLoadSystemAbilityFail, (), ());

    static void SetInstance(MockWebNativeMessagingClient* instance)
    {
        mockInstance_ = instance;
    }

    static WebNativeMessagingClient& GetInstance()
    {
        return *mockInstance_;
    }

private:
    static inline MockWebNativeMessagingClient* mockInstance_ = nullptr;
};

class WebNativeMessagingLoadCallbackTest : public Test {
protected:
    void SetUp() override
    {
        callback_ = std::make_unique<WebNativeMessagingLoadCallback>();
        mockRemote_ = new MockRemoteObject();
        mockClient_ = new MockWebNativeMessagingClient();
        MockWebNativeMessagingClient::SetInstance(mockClient_);
    }

    void TearDown() override
    {
        callback_ = nullptr;
        mockRemote_ = nullptr;
        delete mockClient_;
        MockWebNativeMessagingClient::SetInstance(nullptr);
    }

    std::unique_ptr<WebNativeMessagingLoadCallback> callback_;
    sptr<MockRemoteObject> mockRemote_;
    MockWebNativeMessagingClient* mockClient_ = nullptr;
    const int32_t VALID_SA_ID = SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID;
    const int32_t INVALID_SA_ID = 9999;
};

TEST_F(WebNativeMessagingLoadCallbackTest, OnLoadSuccess_InvalidSaId)
{
    EXPECT_CALL(*mockClient_, OnLoadSystemAbilitySuccess(_)).Times(0);
    callback_->OnLoadSystemAbilitySuccess(INVALID_SA_ID, mockRemote_);
}

TEST_F(WebNativeMessagingLoadCallbackTest, OnLoadSuccess_NullRemote)
{
    EXPECT_CALL(*mockClient_, OnLoadSystemAbilitySuccess(_)).Times(0);
    callback_->OnLoadSystemAbilitySuccess(VALID_SA_ID, nullptr);
}

TEST_F(WebNativeMessagingLoadCallbackTest, OnLoadFail_InvalidSaId)
{
    EXPECT_CALL(*mockClient_, OnLoadSystemAbilityFail()).Times(0);
    callback_->OnLoadSystemAbilityFail(INVALID_SA_ID);
}

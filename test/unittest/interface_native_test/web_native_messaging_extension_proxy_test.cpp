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
#include <gmock/gmock.h>

#define private public
#include "base/web/webview/interfaces/kits/napi/web_native_messaging_extension/extension_client/include/web_native_messaging_extension_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {

class IRemoteObjectMocker : public IRemoteObject {
public:
    IRemoteObjectMocker() : IRemoteObject {u"IRemoteObjectMocker"}
    {
    }

    ~IRemoteObjectMocker()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
    {
        reply.WriteInt32(errorCode_);
        return 0;
    }

    bool IsProxyObject() const
    {
        return true;
    }

    bool CheckObjectLegality() const
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface()
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string>& args)
    {
        return 0;
    }
protected:
    int errorCode_ = 200;
};

class WebNativeMessagingExtensionProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    IRemoteObjectMocker* mockRemote;
    std::shared_ptr<WebNativeMessagingExtensionProxy> proxy;
protected:
    int validFd_ = 1;
};

void WebNativeMessagingExtensionProxyTest::SetUpTestCase(void) {}

void WebNativeMessagingExtensionProxyTest::TearDownTestCase(void) {}

void WebNativeMessagingExtensionProxyTest::SetUp(void)
{
    sptr<IRemoteObject> mockRemote = new IRemoteObjectMocker();
    proxy = std::make_shared<WebNativeMessagingExtensionProxy>(mockRemote);
}

void WebNativeMessagingExtensionProxyTest::TearDown(void) {}

/**
 * @tc.name  : OHWebNativeMessagingExtensionProxyTest_ConnectNative
 * @tc.desc  : Test ConnectNative
 */
HWTEST_F(WebNativeMessagingExtensionProxyTest,
         OHWebNativeMessagingExtensionProxyTest_ConnectNative,
         TestSize.Level1)
{
    WNMEConnectionInfo connectionInfo;
    connectionInfo.connectionId = 1001;
    connectionInfo.bundleName = "com.test.app";
    connectionInfo.extensionOrigin = "https://test.com";
    connectionInfo.fdRead = validFd_;
    connectionInfo.fdWrite = validFd_;

    int32_t expectedErrorCode = 200;

    int32_t result = proxy->ConnectNative(connectionInfo);
    EXPECT_EQ(result, expectedErrorCode);
}

/**
 * @tc.name  : OHWebNativeMessagingExtensionProxyTest_DisconnectNative
 * @tc.desc  : Test DisconnectNative
 */
HWTEST_F(WebNativeMessagingExtensionProxyTest,
         OHWebNativeMessagingExtensionProxyTest_DisconnectNative,
         TestSize.Level1)
{
    WNMEConnectionInfo connectionInfo;
    connectionInfo.connectionId = 1001;
    connectionInfo.bundleName = "com.test.app";
    connectionInfo.extensionOrigin = "https://test.com";
    connectionInfo.fdRead = validFd_;
    connectionInfo.fdWrite = validFd_;

    int32_t expectedErrorCode = 200;

    int32_t result = proxy->DisconnectNative(connectionInfo);
    EXPECT_EQ(result, expectedErrorCode);
}

} // namespace NWeb
} // namespace OHOS

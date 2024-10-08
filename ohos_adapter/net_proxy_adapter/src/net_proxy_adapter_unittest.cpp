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

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include <cstring>

#define private public
#include "net_proxy_adapter_impl.h"

using namespace testing;

namespace OHOS::NWeb {
class NetProxyAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NetProxyAdapterTest::SetUpTestCase(void) {}

void NetProxyAdapterTest::TearDownTestCase(void) {}

void NetProxyAdapterTest::SetUp(void) {}

void NetProxyAdapterTest::TearDown(void) {}

class MockNetProxyEventCallbackAdapter : public NetProxyEventCallbackAdapter {
    public:
        MockNetProxyEventCallbackAdapter() = default;
        void Changed(const std::string& host, const uint16_t& port, const std::string& pacUrl,
                     const std::vector<std::string>& exclusionList) {}
};

/**
 * @tc.name: NetProxyAdapterTest_OnReceiveEvent_001.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 */
TEST_F(NetProxyAdapterTest, NetProxyAdapterTest_OnReceiveEvent_001)
{
    const char *events[] = {
        COMMON_EVENT_HTTP_PROXY_CHANGE,
    };

    int count = sizeof(events) / sizeof(events[0]);
    CommonEvent_SubscribeInfo *commonEventSubscribeInfo = OH_CommonEvent_CreateSubscribeInfo(events, count);
    EXPECT_NE(commonEventSubscribeInfo, nullptr);
    CommonEvent_Subscriber *commonEventSubscriber = OH_CommonEvent_CreateSubscriber(commonEventSubscribeInfo,
        NetProxyAdapterImpl::OnReceiveEvent);
    EXPECT_NE(commonEventSubscriber, nullptr);
    OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo);
    OH_CommonEvent_DestroySubscriber(commonEventSubscriber);
}

/**
 * @tc.name: NetProxyAdapterTest_RegNetProxyEvent_002.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 */
TEST_F(NetProxyAdapterTest, NetProxyAdapterTest_RegNetProxyEvent_002)
{
    NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(nullptr);
    EXPECT_EQ(NetProxyAdapterImpl::GetInstance().cb_, nullptr);
    bool result = NetProxyAdapterImpl::GetInstance().StartListen();
    EXPECT_FALSE(result);

    std::shared_ptr<NetProxyEventCallbackAdapter> eventCallback =
        std::make_shared<MockNetProxyEventCallbackAdapter>();
    NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(eventCallback);
    EXPECT_NE(NetProxyAdapterImpl::GetInstance().cb_, nullptr);
    result = NetProxyAdapterImpl::GetInstance().StartListen();
    EXPECT_TRUE(result);

    NetConn_HttpProxy httpProxy;
    uint16_t validPort = 8080;
    httpProxy.port = validPort;
    std::string hostStr = "192.168.xxx.xxx";
    std::strncpy(httpProxy.host, hostStr.c_str(), sizeof(httpProxy.host) - 1);
    httpProxy.host[sizeof(httpProxy.host) - 1] = '\0';
    int32_t res = OH_NetConn_SetAppHttpProxy(&httpProxy);
    EXPECT_EQ(res, 0);

    std::string host;
    uint16_t port;
    std::string pacUrl;
    std::string exclusion;
    NetProxyAdapterImpl::GetInstance().GetProperty(host, port, pacUrl, exclusion);
    EXPECT_EQ(port, validPort);
    EXPECT_FALSE(host.empty());
    EXPECT_TRUE(pacUrl.empty());
    EXPECT_TRUE(exclusion.empty());
}

/**
 * @tc.name: NetProxyAdapterTest_GetProperty_003.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 */
TEST_F(NetProxyAdapterTest, NetProxyAdapterTest_GetProperty_003)
{
    std::string host;
    uint16_t port;
    std::string pacUrl;
    std::string exclusion;
    NetProxyAdapterImpl::GetInstance().GetProperty(host, port, pacUrl, exclusion);
    EXPECT_NE(NetProxyAdapterImpl::GetInstance().commonEventSubscriber_, nullptr);
    NetProxyAdapterImpl::GetInstance().StopListen();
    NetProxyAdapterImpl::GetInstance().commonEventSubscriber_ = nullptr;
    NetProxyAdapterImpl::GetInstance().StopListen();
}

/**
 * @tc.name: NetProxyAdapterTest_Encode_004.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 */
TEST_F(NetProxyAdapterTest, NetProxyAdapterTest_Encode_004)
{
    std::string source = "";
    std::string  result = Base64::Encode(source);
    EXPECT_TRUE(result.empty());
    result = Base64::Decode(source);
    EXPECT_TRUE(result.empty());
    source = "++webtest";
    Base64::Decode(source);
    source = "webtest++++++";
    Base64::Decode(source);
    result = Base64::Encode(source);
    EXPECT_FALSE(result.empty());
}
} // namespace OHOS::NWeb

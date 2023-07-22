/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "accesstoken_kit.h"

#define private public
#include "net_proxy_adapter_impl.h"

#include "net_conn_client.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AAFwk;
using namespace OHOS::NetManagerStandard;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::EventFwk;

namespace OHOS::EventFwk {
namespace {
bool g_subscribeCommonEventRet = true;
bool g_unSubscribeCommonEventRet = true;
}
bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_subscribeCommonEventRet;
}

bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_unSubscribeCommonEventRet;
}
}

namespace OHOS::NetManagerStandard {
class NetConnClientMock : public NetConnClient {
public:
    MOCK_METHOD1(GetGlobalHttpProxy, int32_t(HttpProxy &));
    MOCK_METHOD1(GetDefaultHttpProxy, int32_t(HttpProxy &));
};
NetConnClientMock *g_mock = nullptr;
NetConnClient &NetConnClient::GetInstance()
{
    if (!g_mock) {
        g_mock = new NetConnClientMock();
    }
    return *g_mock;
}
}

namespace OHOS::NWeb {
namespace {
using Security::AccessToken::AccessTokenID;
using namespace Security::AccessToken;

HapInfoParams testInfoParms = {
    .userID = 1,
    .bundleName = "web_conn_manager_test",
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};

PermissionDef testPermDef = {
    .permissionName = "ohos.permission.GET_NETWORK_INFO",
    .bundleName = "web_conn_manager_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "Test web connect maneger",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionDef testInternalPermDef = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .bundleName = "web_conn_manager_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test web connect manager internal",
    .descriptionId = 1,
};

PermissionDef testInternetPermDef = {
    .permissionName = "ohos.permission.INTERNET",
    .bundleName = "web_conn_manager_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test web connect manager internet",
    .descriptionId = 1,
};

PermissionStateFull testState = {
    .grantFlags = {2},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.GET_NETWORK_INFO",
    .resDeviceID = {"local"},
};

PermissionStateFull testInternalState = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .isGeneral = true,
    .resDeviceID = {"local"},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .grantFlags = {2},
};

PermissionStateFull testInternetState = {
    .permissionName = "ohos.permission.INTERNET",
    .isGeneral = true,
    .resDeviceID = {"local"},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .grantFlags = {2},
};

HapPolicyParams testPolicyPrams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = {testPermDef, testInternalPermDef, testInternetPermDef},
    .permStateList = {testState, testInternalState, testInternetState},
};
} // namespace

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

class AccessToken {
public:
    AccessToken() : currentID_(GetSelfTokenID())
    {
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParms, testPolicyPrams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(tokenIdEx.tokenIDEx);
    }
    ~AccessToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }

private:
    AccessTokenID currentID_;
    AccessTokenID accessID_ = 0;
};

/**
 * @tc.name: NetProxyAdapterTest_OnReceiveEvent_001.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NetProxyAdapterTest, NetProxyAdapterTest_OnReceiveEvent_001, TestSize.Level1)
{
    EventFwk::MatchingSkills skill = EventFwk::MatchingSkills();
    EventFwk::CommonEventSubscribeInfo info(skill);
    NetProxyEventCallback eventCallback =
        [](std::string& host, uint16_t& port, const std::string& pacUrl,
        const std::vector<std::string>& exclusionList) {};
    NetProxyEventSubscriber criber(info, eventCallback);
    EXPECT_NE(criber.eventCallback_, nullptr);
    EventFwk::CommonEventData data;

    AccessToken token;
    HttpProxy httpProxy = {"tes.web.com", 0, {}};
    int32_t ret = NetConnClient::GetInstance().SetGlobalHttpProxy(httpProxy);
    EXPECT_EQ(ret, 0);
    criber.OnReceiveEvent(data);
    Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_HTTP_PROXY_CHANGE);
    data.SetWant(want);
    EXPECT_CALL(*g_mock, GetGlobalHttpProxy(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(NetManagerStandard::NET_CONN_SUCCESS));
    criber.OnReceiveEvent(data);
    EXPECT_CALL(*g_mock, GetGlobalHttpProxy(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(NetManagerStandard::NET_CONN_ERR_INPUT_NULL_PTR));
    criber.OnReceiveEvent(data);
    std::string host;
    uint16_t port;
    std::string pacUrl;
    std::string exclusion;
    EXPECT_CALL(*g_mock, GetGlobalHttpProxy(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(NetManagerStandard::NET_CONN_SUCCESS));
    NetProxyAdapterImpl::GetInstance().GetProperty(host, port, pacUrl, exclusion);
    EXPECT_CALL(*g_mock, GetGlobalHttpProxy(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(NetManagerStandard::NET_CONN_ERR_INPUT_NULL_PTR));
    NetProxyAdapterImpl::GetInstance().GetProperty(host, port, pacUrl, exclusion);
}

/**
 * @tc.name: NetProxyAdapterTest_RegNetProxyEvent_002.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NetProxyAdapterTest, NetProxyAdapterTest_RegNetProxyEvent_002, TestSize.Level1)
{
    NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(nullptr);
    EXPECT_EQ(NetProxyAdapterImpl::GetInstance().cb_, nullptr);
    bool result = NetProxyAdapterImpl::GetInstance().StartListen();
    EXPECT_FALSE(result);
    NetProxyEventCallback eventCallback =
        [](std::string& host, uint16_t& port, const std::string& pacUrl,
        const std::vector<std::string>& exclusionList) {};
    NetProxyAdapterImpl::GetInstance().RegNetProxyEvent(std::move(eventCallback));
    EXPECT_NE(NetProxyAdapterImpl::GetInstance().cb_, nullptr);
    result = NetProxyAdapterImpl::GetInstance().StartListen();
    EXPECT_TRUE(result);
    g_subscribeCommonEventRet = false;
    result = NetProxyAdapterImpl::GetInstance().StartListen();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NetProxyAdapterTest_GetProperty_003.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NetProxyAdapterTest, NetProxyAdapterTest_GetProperty_003, TestSize.Level1)
{
    std::string host;
    uint16_t port;
    std::string pacUrl;
    std::string exclusion;
    NetProxyAdapterImpl::GetInstance().GetProperty(host, port, pacUrl, exclusion);
    EXPECT_NE(NetProxyAdapterImpl::GetInstance().commonEventSubscriber_, nullptr);
    g_unSubscribeCommonEventRet = false;
    NetProxyAdapterImpl::GetInstance().StopListen();
    g_unSubscribeCommonEventRet = true;
    NetProxyAdapterImpl::GetInstance().StopListen();
    NetProxyAdapterImpl::GetInstance().commonEventSubscriber_ = nullptr;
    NetProxyAdapterImpl::GetInstance().StopListen();
}

/**
 * @tc.name: NetProxyAdapterTest_Encode_004.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NetProxyAdapterTest, NetProxyAdapterTest_Encode_004, TestSize.Level1)
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
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>

#define private public
#include "battery_mgr_client_adapter_impl.h"
#undef private

#include "battery_srv_client.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace EventFwk;
using namespace OHOS::EventFwk;
using namespace PowerMgr;

namespace OHOS {
namespace {
bool g_subscribeCommonEventRet = true;
bool g_unSubscribeCommonEventRet = true;
BatteryPluggedType g_pluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
} // namespace

namespace EventFwk {
bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_subscribeCommonEventRet;
}

bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_unSubscribeCommonEventRet;
}
}
namespace PowerMgr {
BatteryPluggedType BatterySrvClient::GetPluggedType()
{
    return g_pluggedType;
}
}
namespace NWeb {
namespace {
std::shared_ptr<NWeb::NWebBatteryEventSubscriber> g_batter;
std::shared_ptr<NWeb::BatteryMgrClientAdapterImpl> g_batterImpl;
} // namespace
class BatteryMgrAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BatteryMgrAdapterTest::SetUpTestCase(void)
{
    bool result = true;
    CommonEventSubscribeInfo subscribe;
    
    auto callback = [] (WebBatteryInfo& info) {};
    BatteryEventCallback eventCallback = callback;
    g_batter = std::make_shared<NWebBatteryEventSubscriber>(subscribe, eventCallback);
    if (g_batter == nullptr) {
        result = false;
    }
    EXPECT_TRUE(result);
    g_batterImpl = std::make_shared<BatteryMgrClientAdapterImpl>();
    if (g_batter == nullptr) {
        result = false;
    }
    EXPECT_TRUE(result);
}

void BatteryMgrAdapterTest::TearDownTestCase(void)
{}

void BatteryMgrAdapterTest::SetUp(void)
{}

void BatteryMgrAdapterTest::TearDown(void)
{}

/**
 * @tc.name: BatteryAdapter_OnReceiveEvent_001.
 * @tc.desc: Test the OnReceiveEvent.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BatteryMgrAdapterTest, BatteryAdapter_OnReceiveEvent_001, TestSize.Level1)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data(want);
    std::string key = PowerMgr::BatteryInfo::COMMON_EVENT_KEY_PLUGGED_TYPE;
    g_batter->OnReceiveEvent(data);

    want.SetParam(key, static_cast<int>(BatteryPluggedType::PLUGGED_TYPE_NONE));
    g_batter->OnReceiveEvent(data);
    want.SetParam(key, static_cast<int>(BatteryPluggedType::PLUGGED_TYPE_BUTT));
    g_batter->OnReceiveEvent(data);
}

/**
 * @tc.name: BatteryAdapter_RegBatteryEvent_002.
 * @tc.desc: Test the RegBatteryEvent.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BatteryMgrAdapterTest, BatteryAdapter_RegBatteryEvent_002, TestSize.Level1)
{
    auto cb = [] (WebBatteryInfo& info) {};
    BatteryEventCallback eventCallback = cb;
    g_batterImpl->RegBatteryEvent(std::move(eventCallback));
}

/**
 * @tc.name: BatteryAdapter_StartListen_003.
 * @tc.desc: Test the StartListen.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BatteryMgrAdapterTest, BatteryAdapter_StartListen_003, TestSize.Level1)
{
    bool result = g_batterImpl->StartListen();
    EXPECT_TRUE(result);
    g_subscribeCommonEventRet = false;
    result = g_batterImpl->StartListen();
    EXPECT_FALSE(result);
    g_subscribeCommonEventRet = true;
}

/**
 * @tc.name: BatteryAdapter_StopListen_004.
 * @tc.desc: Test the StopListen.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BatteryMgrAdapterTest, BatteryAdapter_StopListen_004, TestSize.Level1)
{
    g_unSubscribeCommonEventRet = false;
    g_batterImpl->StopListen();
    g_unSubscribeCommonEventRet = true;
    g_batterImpl->StopListen();
}

/**
 * @tc.name: BatteryAdapter_RequestBatteryInfo_005.
 * @tc.desc: Test the RequestBatteryInfo.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BatteryMgrAdapterTest, BatteryAdapter_RequestBatteryInfo_005, TestSize.Level1)
{
    bool result = true;
    g_pluggedType = BatteryPluggedType::PLUGGED_TYPE_AC;
    std::unique_ptr<WebBatteryInfo> battery = g_batterImpl->RequestBatteryInfo();
    if (battery == nullptr) {
        result = false;
    }
    EXPECT_TRUE(result);
    g_pluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
    battery = g_batterImpl->RequestBatteryInfo();
    if (battery == nullptr) {
        result = false;
    }
    EXPECT_TRUE(result);
    g_pluggedType = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    battery = g_batterImpl->RequestBatteryInfo();
    if (battery == nullptr) {
        result = false;
    }
    EXPECT_TRUE(result);
}

/**
 * @tc.name: BatteryAdapter_WebBatteryInfoImpl_006.
 * @tc.desc: Test the WebBatteryInfoImpl.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BatteryMgrAdapterTest, BatteryAdapter_WebBatteryInfoImpl_006, TestSize.Level1)
{
    double level = 1;
    bool isCharging = true;
    int disChargingTime = 1;
    int chargingTime = 1;
    WebBatteryInfoImpl info(level, isCharging, disChargingTime, chargingTime);
    EXPECT_FLOAT_EQ(info.GetLevel(), level);
    EXPECT_TRUE(info.IsCharging());
    EXPECT_EQ(info.DisChargingTime(), disChargingTime);
    EXPECT_EQ(info.ChargingTime(), chargingTime);
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_LOW);
    CommonEventData data(want);
    g_batter->OnReceiveEvent(data);
    g_batterImpl->commonEventSubscriber_ = nullptr;
    g_batterImpl->StopListen();
}
}
}
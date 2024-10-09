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

#define private public
#include "battery_mgr_client_adapter_impl.h"
#include <BasicServicesKit/ohbattery_info.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#undef private

using namespace testing;

struct CParameters {
    int8_t valueType;
    char *key;
    void *value;
    int64_t size;
};

struct CArrParameters {
    CParameters *head;
    int64_t size;
};

struct CommonEvent_RcvData {
    char *event;
    char *bundleName;
    int32_t code;
    char *data;
    CArrParameters* parameters;
};

namespace OHOS::NWeb {
std::shared_ptr<BatteryMgrClientAdapterImpl> g_batterImpl;
double g_level = 0.0;
bool g_isCharging = false;
CommonEvent_RcvData rcvData;
CArrParameters params;
CParameters* paramsArray;

void UpdateRcvData(int* capacity, int* plugType)
{
    paramsArray[0].valueType = 0;
    paramsArray[0].key = strdup(COMMON_EVENT_KEY_CAPACITY);
    paramsArray[0].value = static_cast<void *>(capacity);
    paramsArray[0].size = sizeof(*capacity);
    paramsArray[1].valueType = 0;
    paramsArray[1].key = strdup(COMMON_EVENT_KEY_PLUGGED_TYPE);
    paramsArray[1].value = static_cast<void *>(plugType);
    paramsArray[1].size = sizeof(*plugType);

    params.head = paramsArray;
    rcvData = {
        .event = strdup(COMMON_EVENT_BATTERY_CHANGED),
        .parameters = &params
    };
}

class MockWebBatteryEventCallback : public WebBatteryEventCallback {
public:
    MockWebBatteryEventCallback() = default;
    void BatteryInfoChanged(std::shared_ptr<WebBatteryInfo> info) {
        g_level = info->GetLevel();
        g_isCharging = info->IsCharging();
    }
};

class BatteryMgrAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BatteryMgrAdapterTest::SetUpTestCase(void)
{
    int size = 2;
    paramsArray = new CParameters[size];
    params.size = size;
}

void BatteryMgrAdapterTest::TearDownTestCase(void)
{
    delete[] paramsArray;
    paramsArray = nullptr;
}

void BatteryMgrAdapterTest::SetUp(void)
{
    g_batterImpl = std::make_shared<BatteryMgrClientAdapterImpl>();
    EXPECT_NE(g_batterImpl, nullptr);
}

void BatteryMgrAdapterTest::TearDown(void)
{
    g_batterImpl = nullptr;
}

/**
 * @tc.name: BatteryAdapter_OnReceiveEvent_001.
 * @tc.desc: Test the OnReceiveEvent.
 * @tc.type: FUNC
 */
TEST_F(BatteryMgrAdapterTest, BatteryAdapter_OnReceiveEvent_001)
{
    EXPECT_NE(g_batterImpl, nullptr);
    std::shared_ptr<MockWebBatteryEventCallback> eventCallback = std::make_shared<MockWebBatteryEventCallback>();
    g_batterImpl->RegBatteryEvent(std::move(eventCallback));
    EXPECT_TRUE(g_batterImpl->StartListen());
    int size = 5;
    int capacityArr[] = {20, 30, 40, 50, 60};
    int plugTypeArr[] = {
        PLUGGED_TYPE_NONE,
        PLUGGED_TYPE_AC,
        PLUGGED_TYPE_USB,
        PLUGGED_TYPE_WIRELESS,
        PLUGGED_TYPE_BUTT
    };
    int isChargingArr[] = {false, true, true, true, false};
    for (int i = 0; i < size; i++) {
        UpdateRcvData(&capacityArr[i], &plugTypeArr[i]);
        const CommonEvent_RcvData* rcvDataPtr = &rcvData;
        g_batterImpl->OnBatteryEvent(rcvDataPtr);
        EXPECT_TRUE((g_level - capacityArr[i] / 100.f) < 1e-9);
        EXPECT_TRUE(g_isCharging == isChargingArr[i]);
    }
}

/**
 * @tc.name: BatteryAdapter_RegBatteryEvent_002.
 * @tc.desc: Test the RegBatteryEvent.
 * @tc.type: FUNC
 */
TEST_F(BatteryMgrAdapterTest, BatteryAdapter_RegBatteryEvent_002)
{
    EXPECT_NE(g_batterImpl, nullptr);
    EXPECT_EQ(g_batterImpl->cb_, nullptr);
    g_batterImpl->RegBatteryEvent(nullptr);
    EXPECT_EQ(g_batterImpl->cb_, nullptr);
    std::shared_ptr<WebBatteryEventCallback> eventCallback = std::make_shared<MockWebBatteryEventCallback>();
    g_batterImpl->RegBatteryEvent(std::move(eventCallback));
    EXPECT_NE(g_batterImpl->cb_, nullptr);
}

/**
 * @tc.name: BatteryAdapter_StartListen_003.
 * @tc.desc: Test the StartListen.
 * @tc.type: FUNC
 */
TEST_F(BatteryMgrAdapterTest, BatteryAdapter_StartListen_003)
{
    EXPECT_NE(g_batterImpl, nullptr);
    EXPECT_TRUE(g_batterImpl->StartListen());
    EXPECT_NE(g_batterImpl->commonEventSubscriber_, nullptr);
    EXPECT_NE(g_batterImpl->commonEventSubscriberInfo_, nullptr);
}

/**
 * @tc.name: BatteryAdapter_StopListen_004.
 * @tc.desc: Test the StopListen.
 * @tc.type: FUNC
 */
TEST_F(BatteryMgrAdapterTest, BatteryAdapter_StopListen_004)
{
    EXPECT_NE(g_batterImpl, nullptr);
    g_batterImpl->StopListen();
    EXPECT_EQ(g_batterImpl->commonEventSubscriber_, nullptr);
    EXPECT_EQ(g_batterImpl->commonEventSubscriberInfo_, nullptr);
}

/**
 * @tc.name: BatteryAdapter_RequestBatteryInfo_005.
 * @tc.desc: Test the RequestBatteryInfo.
 * @tc.type: FUNC
 */
TEST_F(BatteryMgrAdapterTest, BatteryAdapter_RequestBatteryInfo_005)
{
    EXPECT_NE(g_batterImpl, nullptr);
    std::shared_ptr<WebBatteryInfo> battery = g_batterImpl->RequestBatteryInfo();
    EXPECT_NE(battery, nullptr);
}

/**
 * @tc.name: BatteryAdapter_WebBatteryInfoImpl_006.
 * @tc.desc: Test the WebBatteryInfoImpl.
 * @tc.type: FUNC
 */
TEST_F(BatteryMgrAdapterTest, BatteryAdapter_WebBatteryInfoImpl_006)
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
}
}
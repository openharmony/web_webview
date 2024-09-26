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

#include <cstring>
#include <cstdint>
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "date_time_format_adapter_impl.h"
#include <BasicServicesKit/oh_commonevent_support.h>

using namespace testing;

namespace OHOS {
namespace {
bool g_testTimeZoneChangedCallBack = false;
}

namespace NWeb {

class DateTimeAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DateTimeAdapterImplTest::SetUpTestCase(void)
{}

void DateTimeAdapterImplTest::TearDownTestCase(void)
{}

void DateTimeAdapterImplTest::SetUp(void)
{}

void DateTimeAdapterImplTest::TearDown(void)
{}

class MockTimezoneEventCallbackAdapter : public TimezoneEventCallbackAdapter {
public:
    MockTimezoneEventCallbackAdapter() = default;
    void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info) {
        g_testTimeZoneChangedCallBack = true;
    }
};

struct TestCommonEvent_RcvData {
    const char *event;
};

/**
 * @tc.name: DateTimeAdapterImplTest_NWebTimeZoneEventSubscriber_001
 * @tc.desc: NWebTimeZoneEventSubscriber.
 * @tc.type: FUNC
 */
TEST_F(DateTimeAdapterImplTest, DateTimeAdapterImplTest_NWebTimeZoneEventSubscriber_001)
{
    g_testTimeZoneChangedCallBack = false;
    TestCommonEvent_RcvData data = {0};
    data.event = COMMON_EVENT_TIME_CHANGED;
    DateTimeFormatAdapterImpl::DateTimeFormatReceiveCallback(reinterpret_cast<CommonEvent_RcvData *>(&data));
    EXPECT_FALSE(g_testTimeZoneChangedCallBack);

    auto adapter = std::make_shared<DateTimeFormatAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<MockTimezoneEventCallbackAdapter>();
    adapter->RegTimezoneEvent(std::move(cb));

    data.event = COMMON_EVENT_TIME_CHANGED;
    DateTimeFormatAdapterImpl::DateTimeFormatReceiveCallback(reinterpret_cast<CommonEvent_RcvData *>(&data));
    EXPECT_FALSE(g_testTimeZoneChangedCallBack);

    EXPECT_TRUE(adapter->StartListen());

    data.event = COMMON_EVENT_TIMEZONE_CHANGED;
    DateTimeFormatAdapterImpl::DateTimeFormatReceiveCallback(reinterpret_cast<CommonEvent_RcvData *>(&data));
    EXPECT_TRUE(g_testTimeZoneChangedCallBack);

    g_testTimeZoneChangedCallBack = false;
    adapter->StopListen();
    data.event = COMMON_EVENT_TIMEZONE_CHANGED;
    DateTimeFormatAdapterImpl::DateTimeFormatReceiveCallback(reinterpret_cast<CommonEvent_RcvData *>(&data));
    EXPECT_FALSE(g_testTimeZoneChangedCallBack);

    g_testTimeZoneChangedCallBack = false;
}

/**
 * @tc.name: DateTimeAdapterImplTest_DateTimeFormatAdapterImpl_002
 * @tc.desc: DateTimeFormatAdapterImpl.
 * @tc.type: FUNC
 */
TEST_F(DateTimeAdapterImplTest, DateTimeAdapterImplTest_DateTimeFormatAdapterImpl_002)
{
    auto adapter = std::make_shared<DateTimeFormatAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<MockTimezoneEventCallbackAdapter>();
    adapter->RegTimezoneEvent(std::move(cb));
    bool result = adapter->StartListen();
    EXPECT_TRUE(result);
    result = adapter->StartListen();
    EXPECT_TRUE(result);
    adapter->StopListen();
    adapter->StopListen();
    adapter->StopListen();
    auto timeStr = adapter->GetTimezone();
    EXPECT_FALSE(timeStr.empty());
}
}
} // namespace NWeb

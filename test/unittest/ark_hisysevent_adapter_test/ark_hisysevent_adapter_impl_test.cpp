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
#include "base/web/webview/ohos_interface/ohos_glue/ohos_adapter/bridge/webview/ark_hisysevent_adapter_impl.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <tuple>
#include <string>
#include <vector>

#include "base/web/webview/ohos_interface/include/ohos_adapter/hisysevent_adapter.h"

using namespace testing;
using namespace OHOS::ArkWeb;
using namespace OHOS::NWeb;

class MockHiSysEventAdapter : public HiSysEventAdapter {
public:
    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string,const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const int64_t, const std::string, const int,
    const std::string, const std::vector<uint16_t>, const std::string, const int>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int , const std::string, const int,
    const std::string, const int64_t, const std::string, const int>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const uint32_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>&)), (override));
};

class ArkHiSysEventAdapterImplTest : public testing::Test {
public:
    void SetUp() override {
        mockAdapter = std::make_unique<MockHiSysEventAdapter>();
        adapterImpl = std::make_unique<ArkHiSysEventAdapterImpl>(*mockAdapter);
    }

    void TearDown() override {}

    std::unique_ptr<MockHiSysEventAdapter> mockAdapter;
    std::unique_ptr<ArkHiSysEventAdapterImpl> adapterImpl;
};

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    uint32_t type = 1;
    ArkWebString key1;
    key1.size = 6;
    key1.value = const_cast<char*>("key1");
    key1.ark_web_mem_free_func = nullptr;
    ArkWebString value1;
    value1.size = 6;
    value1.value = const_cast<char*>("value1");
    value1.ark_web_mem_free_func = nullptr;

    const std::string event1 = "event1";
    const std::tuple<const std::string, const std::string> keyValue("key1", "value1");
    MockHiSysEventAdapter::EventType eventType = MockHiSysEventAdapter::STATISTIC;
    EXPECT_CALL(*mockAdapter, Write(event1, eventType, keyValue))
        .WillOnce(testing::Return(0));

    int result = adapterImpl->Write(eventName, type, key1, value1);

    EXPECT_EQ(result, 0);
}
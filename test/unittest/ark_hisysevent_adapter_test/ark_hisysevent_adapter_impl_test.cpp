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

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey1) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey2) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    value.value = const_cast<char*>("value2");
    const ArkWebString value2 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey3) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    value.value = const_cast<char*>("value2");
    const ArkWebString value2 =value;
    value.value = const_cast<char*>("value3");
    const ArkWebString value3 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey4) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    value.value = const_cast<char*>("value2");
    const ArkWebString value2 =value;
    value.value = const_cast<char*>("value3");
    const ArkWebString value3 =value;
    value.value = const_cast<char*>("value4");
    const ArkWebString value4 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3, key4, value4);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey5) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;
    key.value = const_cast<char*>("key5");
    const ArkWebString key5 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    value.value = const_cast<char*>("value2");
    const ArkWebString value2 =value;
    value.value = const_cast<char*>("value3");
    const ArkWebString value3 =value;
    value.value = const_cast<char*>("value4");
    const ArkWebString value4 =value;
    value.value = const_cast<char*>("value5");
    const ArkWebString value5 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3, key4, value4, key5, value5);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey6) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;
    key.value = const_cast<char*>("key5");
    const ArkWebString key5 = key;
    key.value = const_cast<char*>("key6");
    const ArkWebString key6 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    value.value = const_cast<char*>("value2");
    const ArkWebString value2 =value;
    value.value = const_cast<char*>("value3");
    const ArkWebString value3 =value;
    value.value = const_cast<char*>("value4");
    const ArkWebString value4 =value;
    value.value = const_cast<char*>("value5");
    const ArkWebString value5 =value;
    value.value = const_cast<char*>("value6");
    const ArkWebString value6 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3, key4, value4, key5, value5, key6, value6);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey7) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;

    const int64_t value1 = 1;
    const int value2 = 2;
    const ArkWebUint16Vector value3 = ark_web_uint16_vector_default;
    const int value4 = 4;

    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const int64_t, const std::string, const int,
            const std::string, const std::vector<uint16_t>, const std::string, const int>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3, key4, value4);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey8) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;
    key.value = const_cast<char*>("key5");
    const ArkWebString key5 = key;
    key.value = const_cast<char*>("key6");
    const ArkWebString key6 = key;

    const int64_t value1 = 1;
    const int64_t value2 = 2;
    const int value3 = 3;
    const int value4 = 4;
    const int64_t value5 = 5;
    const int value6 = 6;

    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
            const std::string, const int , const std::string, const int,
            const std::string, const int64_t, const std::string, const int>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3, key4, value4, key5, value5, key6, value6);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey9) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;
    key.value = const_cast<char*>("key5");
    const ArkWebString key5 = key;
    key.value = const_cast<char*>("key6");
    const ArkWebString key6 = key;
    key.value = const_cast<char*>("key7");
    const ArkWebString key7 = key;
    key.value = const_cast<char*>("key8");
    const ArkWebString key8 = key;
    key.value = const_cast<char*>("key9");
    const ArkWebString key9 = key;
    key.value = const_cast<char*>("key10");
    const ArkWebString key10 = key;
    key.value = const_cast<char*>("key11");
    const ArkWebString key11 = key;
    key.value = const_cast<char*>("key12");
    const ArkWebString key12 = key;
    key.value = const_cast<char*>("key13");
    const ArkWebString key13 = key;
    key.value = const_cast<char*>("key14");
    const ArkWebString key14 = key;
    key.value = const_cast<char*>("key15");
    const ArkWebString key15 = key;
    key.value = const_cast<char*>("key16");
    const ArkWebString key16 = key;
    key.value = const_cast<char*>("key17");
    const ArkWebString key17 = key;
    key.value = const_cast<char*>("key18");
    const ArkWebString key18 = key;
    key.value = const_cast<char*>("key19");
    const ArkWebString key19 = key;
    key.value = const_cast<char*>("key20");
    const ArkWebString key20 = key;
    key.value = const_cast<char*>("key21");
    const ArkWebString key21 = key;
    key.value = const_cast<char*>("key22");
    const ArkWebString key22 = key;
    key.value = const_cast<char*>("key23");
    const ArkWebString key23 = key;

    const int64_t value1 = 1;
    const int64_t value2 = 2;
    const int64_t value3 = 3;
    const int64_t value4 = 4;
    const int64_t value5 = 5;
    const int64_t value6 = 6;
    const int64_t value7 = 7;
    const int64_t value8 = 8;
    const int64_t value9 = 9;
    const int64_t value10 = 10;
    const int64_t value11 = 11;
    const int64_t value12 = 12;
    const int64_t value13 = 13;
    const int64_t value14 = 14;
    const int64_t value15 = 15;
    const int64_t value16 = 16;
    const int64_t value17 = 17;
    const int64_t value18 = 18;
    const int64_t value19 = 19;
    const int64_t value20 = 20;
    const int64_t value21 = 21;
    const int64_t value22 = 22;
    const int64_t value23 = 23;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
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
            const std::string, const int64_t>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, 
            key1, value1, key2, value2, key3, value3, key4, value4, 
            key5, value5, key6, value6, key7, value7, key8, value8,
            key9, value9, key10, value10, key11, value11, key12, value12,
            key13, value13, key14, value14, key15, value15, key16, value16,
            key17, value17, key18, value18, key19, value19, key20, value20,
            key21, value21, key22, value22, key23, value23);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey10) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;
    key.value = const_cast<char*>("key5");
    const ArkWebString key5 = key;
    key.value = const_cast<char*>("key6");
    const ArkWebString key6 = key;
    key.value = const_cast<char*>("key7");
    const ArkWebString key7 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    value.value = const_cast<char*>("value2");
    const ArkWebString value2 =value;
    value.value = const_cast<char*>("value3");
    const ArkWebString value3 =value;
    value.value = const_cast<char*>("value4");
    const ArkWebString value4 =value;
    value.value = const_cast<char*>("value5");
    const ArkWebString value5 =value;
    value.value = const_cast<char*>("value6");
    const ArkWebString value6 =value;
    value.value = const_cast<char*>("value7");
    const ArkWebString value7 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3, key4, value4, key5, value5, key6, value6, key7, value7);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey11) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;
    key.value = const_cast<char*>("key3");
    const ArkWebString key3 = key;
    key.value = const_cast<char*>("key4");
    const ArkWebString key4 = key;
    key.value = const_cast<char*>("key5");
    const ArkWebString key5 = key;
    key.value = const_cast<char*>("key6");
    const ArkWebString key6 = key;
    key.value = const_cast<char*>("key7");
    const ArkWebString key7 = key;
    key.value = const_cast<char*>("key8");
    const ArkWebString key8 = key;
    ArkWebString value;
    value.size = 6;
    value.value = const_cast<char*>("value1");
    value.ark_web_mem_free_func = nullptr;
    const ArkWebString value1 =value;
    value.value = const_cast<char*>("value2");
    const ArkWebString value2 =value;
    value.value = const_cast<char*>("value3");
    const ArkWebString value3 =value;
    value.value = const_cast<char*>("value4");
    const ArkWebString value4 =value;
    value.value = const_cast<char*>("value5");
    const ArkWebString value5 =value;
    value.value = const_cast<char*>("value6");
    const ArkWebString value6 =value;
    value.value = const_cast<char*>("value7");
    const ArkWebString value7 =value;
    value.value = const_cast<char*>("value8");
    const ArkWebString value8 =value;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2, key3, value3, key4, value4, key5, value5, key6, value6, key7, value7, key8, value8);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write_WithOneKey12) {
    ArkWebString eventName;
    eventName.size = 6;
    eventName.value = const_cast<char*>("event1");
    eventName.ark_web_mem_free_func = nullptr;
    const ArkWebString eventName1 = eventName;
    uint32_t type = 1;
    ArkWebString key;
    key.size = 6;
    key.value = const_cast<char*>("key1");
    key.ark_web_mem_free_func = nullptr;
    const ArkWebString key1 = key;
    key.value = const_cast<char*>("key2");
    const ArkWebString key2 = key;

    const uint32_t value1 = 1;
    const uint64_t value2 = 2;

    EXPECT_CALL(*mockAdapter, Write(
        testing::_, 
        testing::_, 
        testing::A<const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName1, type, key1, value1, key2, value2);
    EXPECT_EQ(result, 0);
}
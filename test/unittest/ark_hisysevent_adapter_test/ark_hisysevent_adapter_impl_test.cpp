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
#include "ark_hisysevent_adapter_impl.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <tuple>
#include <string>
#include <vector>

#include "hisysevent_adapter.h"

using namespace testing;
using namespace OHOS::ArkWeb;
using namespace OHOS::NWeb;

class MockHiSysEventAdapter : public HiSysEventAdapter
{
public:
    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string,
    const std::string, const std::string>&)), (override));

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const std::string, const std::string, const std::string,
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
    const std::string, const int, const std::string, const int,
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

    MOCK_METHOD(int, Write, (const std::string&, EventType,
    (const std::tuple<const std::string, const int32_t, const std::string, const std::string,
    const std::string, const std::string, const std::string, const std::string,
    const std::string, const int32_t>&)), (override));
};

class ArkHiSysEventAdapterImplTest : public testing::Test
{
public:
    void SetUp() override
    {
        mockAdapter = std::make_unique<MockHiSysEventAdapter>();
        adapterImpl = std::make_unique<ArkHiSysEventAdapterImpl>(*mockAdapter);
    }

    void TearDown() override {}

    std::unique_ptr<MockHiSysEventAdapter> mockAdapter;
    std::unique_ptr<ArkHiSysEventAdapterImpl> adapterImpl;
};

TEST_F(ArkHiSysEventAdapterImplTest, Write1)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write2)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write3)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString value3 = ArkWebStringClassToStruct("value3");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write4)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString value3 = ArkWebStringClassToStruct("value3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString value4 = ArkWebStringClassToStruct("value4");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3, key4, value4);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write5)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString value3 = ArkWebStringClassToStruct("value3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString value4 = ArkWebStringClassToStruct("value4");
    ArkWebString key5 = ArkWebStringClassToStruct("key5");
    ArkWebString value5 = ArkWebStringClassToStruct("value5");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3,
        key4, value4, key5, value5);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write6)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString value3 = ArkWebStringClassToStruct("value3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString value4 = ArkWebStringClassToStruct("value4");
    ArkWebString key5 = ArkWebStringClassToStruct("key5");
    ArkWebString value5 = ArkWebStringClassToStruct("value5");
    ArkWebString key6 = ArkWebStringClassToStruct("key6");
    ArkWebString value6 = ArkWebStringClassToStruct("value6");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3,
        key4, value4, key5, value5, key6, value6);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write7)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
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
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3, key4, value4);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write8)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString key5 = ArkWebStringClassToStruct("key5");
    ArkWebString key6 = ArkWebStringClassToStruct("key6");
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
            const std::string, const int, const std::string, const int,
            const std::string, const int64_t, const std::string, const int>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3,
        key4, value4, key5, value5, key6, value6);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write9)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString key5 = ArkWebStringClassToStruct("key5");
    ArkWebString key6 = ArkWebStringClassToStruct("key6");
    ArkWebString key7 = ArkWebStringClassToStruct("key7");
    ArkWebString key8 = ArkWebStringClassToStruct("key8");
    ArkWebString key9 = ArkWebStringClassToStruct("key9");
    ArkWebString key10 = ArkWebStringClassToStruct("key10");
    ArkWebString key11 = ArkWebStringClassToStruct("key11");
    ArkWebString key12 = ArkWebStringClassToStruct("key12");
    ArkWebString key13 = ArkWebStringClassToStruct("key13");
    ArkWebString key14 = ArkWebStringClassToStruct("key14");
    ArkWebString key15 = ArkWebStringClassToStruct("key15");
    ArkWebString key16 = ArkWebStringClassToStruct("key16");
    ArkWebString key17 = ArkWebStringClassToStruct("key17");
    ArkWebString key18 = ArkWebStringClassToStruct("key18");
    ArkWebString key19 = ArkWebStringClassToStruct("key19");
    ArkWebString key20 = ArkWebStringClassToStruct("key20");
    ArkWebString key21 = ArkWebStringClassToStruct("key21");
    ArkWebString key22 = ArkWebStringClassToStruct("key22");
    ArkWebString key23 = ArkWebStringClassToStruct("key23");
    const int64_t value1 = 1;
    const int64_t value2 = 2;
    const int32_t value3 = 3;
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
    int result = adapterImpl->Write(eventName, type,
        key1, value1, key2, value2, key3, value3, key4, value4,
        key5, value5, key6, value6, key7, value7, key8, value8,
        key9, value9, key10, value10, key11, value11, key12, value12,
        key13, value13, key14, value14, key15, value15, key16, value16,
        key17, value17, key18, value18, key19, value19, key20, value20,
        key21, value21, key22, value22, key23, value23);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write10)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString value3 = ArkWebStringClassToStruct("value3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString value4 = ArkWebStringClassToStruct("value4");
    ArkWebString key5 = ArkWebStringClassToStruct("key5");
    ArkWebString value5 = ArkWebStringClassToStruct("value5");
    ArkWebString key6 = ArkWebStringClassToStruct("key6");
    ArkWebString value6 = ArkWebStringClassToStruct("value6");
    ArkWebString key7 = ArkWebStringClassToStruct("key7");
    ArkWebString value7 = ArkWebStringClassToStruct("value7");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3,
        key4, value4, key5, value5, key6, value6, key7, value7);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write11)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString value1 = ArkWebStringClassToStruct("value1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString value3 = ArkWebStringClassToStruct("value3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString value4 = ArkWebStringClassToStruct("value4");
    ArkWebString key5 = ArkWebStringClassToStruct("key5");
    ArkWebString value5 = ArkWebStringClassToStruct("value5");
    ArkWebString key6 = ArkWebStringClassToStruct("key6");
    ArkWebString value6 = ArkWebStringClassToStruct("value6");
    ArkWebString key7 = ArkWebStringClassToStruct("key7");
    ArkWebString value7 = ArkWebStringClassToStruct("value7");
    ArkWebString key8 = ArkWebStringClassToStruct("key8");
    ArkWebString value8 = ArkWebStringClassToStruct("value8");
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3,
        key4, value4, key5, value5, key6, value6, key7, value7, key8, value8);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write12)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    const uint32_t value1 = 1;
    const uint64_t value2 = 2;

    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2);
    EXPECT_EQ(result, 0);
}

TEST_F(ArkHiSysEventAdapterImplTest, Write13)
{
    ArkWebString eventName = ArkWebStringClassToStruct("testEvent");
    uint32_t type = 1;
    ArkWebString key1 = ArkWebStringClassToStruct("key1");
    int32_t value1 = 1;
    ArkWebString key2 = ArkWebStringClassToStruct("key2");
    ArkWebString value2 = ArkWebStringClassToStruct("value2");
    ArkWebString key3 = ArkWebStringClassToStruct("key3");
    ArkWebString value3 = ArkWebStringClassToStruct("value3");
    ArkWebString key4 = ArkWebStringClassToStruct("key4");
    ArkWebString value4 = ArkWebStringClassToStruct("value4");
    ArkWebString key5 = ArkWebStringClassToStruct("key5");
    int32_t value5 = 5;
    EXPECT_CALL(*mockAdapter, Write(
        testing::_,
        testing::_,
        testing::A<const std::tuple<const std::string, const int32_t, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const int32_t>&>()))
        .WillOnce(testing::Return(0));
    int result = adapterImpl->Write(eventName, type, key1, value1, key2, value2, key3, value3,
        key4, value4, key5, value5);
    EXPECT_EQ(result, 0);
}
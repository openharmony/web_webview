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

#include <gtest/gtest.h>
#include "nweb_value.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::NWeb {
class NWebValueTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NWebValueTest::SetUpTestCase(void)
{}

void NWebValueTest::TearDownTestCase(void)
{}

void NWebValueTest::SetUp(void)
{}

void NWebValueTest::TearDown(void)
{}

/**
 * @tc.name: NWebValue_BOOLEAN_001
 * @tc.desc: Test set and get boolean/type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebValueTest, NWebValue_BOOLEAN_001, TestSize.Level1)
{
    std::shared_ptr<NWebValue> nwebValue = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    NWebValue::Type type = nwebValue->GetType();
    EXPECT_EQ(NWebValue::Type::NONE, type);

    nwebValue->SetType(NWebValue::Type::BOOLEAN);
    nwebValue->SetBoolean(true);

    bool value = nwebValue->GetBoolean();
    type = nwebValue->GetType();
    EXPECT_EQ(NWebValue::Type::BOOLEAN, type);
    EXPECT_TRUE(value);

    nwebValue->SetBoolean(false);
    value = nwebValue->GetBoolean();
    EXPECT_FALSE(value);
}

/**
 * @tc.name: NWebValue_String_001
 * @tc.desc: Test set and get string.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebValueTest, NWebValue_String_002, TestSize.Level1)
{
    const std::string testData = "Hello,String";
    std::shared_ptr<NWebValue> nwebValue = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    nwebValue->SetType(NWebValue::Type::STRING);
    nwebValue->SetString(testData);

    std::string actual = nwebValue->GetString();
    EXPECT_STREQ(testData.c_str(), actual.c_str());
}

/**
 * @tc.name: NWebValue_INT_003
 * @tc.desc: Test set and get integer.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebValueTest, NWebValue_INT_003, TestSize.Level1)
{
    int testData = 12345;
    std::shared_ptr<NWebValue> nwebValue = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    nwebValue->SetType(NWebValue::Type::INTEGER);
    nwebValue->SetInt(testData);

    int actual = nwebValue->GetInt();
    EXPECT_EQ(testData, actual);
}

/**
 * @tc.name: NWebValue_JsonString_004
 * @tc.desc: Test set and get json string.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebValueTest, NWebValue_JsonString_004, TestSize.Level1)
{
    std::shared_ptr<NWebValue> nwebValue = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    nwebValue->SetType(NWebValue::Type::STRING);

    const std::string testData = "{\"name\":\"zhangsan\",\"city\":\"hangzhou\"}";
    nwebValue->SetJsonString(testData);

    std::string actual = nwebValue->GetJsonString();
    EXPECT_STREQ(testData.c_str(), actual.c_str());
}

/**
 * @tc.name: NWebValue_Double_005
 * @tc.desc: Test set and get double.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebValueTest, NWebValue_Double_005, TestSize.Level1)
{
    std::shared_ptr<NWebValue> nwebValue = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    nwebValue->SetType(NWebValue::Type::DOUBLE);

    double testData = 1.234567;
    nwebValue->SetDouble(testData);

    double actual = nwebValue->GetDouble();
    EXPECT_DOUBLE_EQ(testData, actual);
    nwebValue->IsNone();
    nwebValue->IsBoolean();
    nwebValue->IsString();
    nwebValue->IsDouble();
    nwebValue->IsINTEGER();
    nwebValue->IsList();
    nwebValue->IsDictionary();
    nwebValue->IsBinary();
    nwebValue->GetListValueSize();
    nwebValue->GetListValue();
    NWebValue value;
    nwebValue->AddListValue(value);
    nwebValue->deleteListValue();
    nwebValue->GetDictionaryValueSize();
    nwebValue->GetDictionaryValueKeys();
    std::string key;
    nwebValue->HasDictionaryValueKey(key);
    nwebValue->GetDictionaryValue();
    nwebValue->AddDictionaryValue("test", value);
    nwebValue->DeleteDictionaryValue(key);
    nwebValue->GetBinaryValueSize();
    nwebValue->GetBinaryValue();
    nwebValue->validateType(NWebValue::Type::NONE);
}
} // namespace OHOS::NWeb
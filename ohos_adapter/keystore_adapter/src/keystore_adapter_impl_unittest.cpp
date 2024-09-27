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

#include <testing/gmock/include/gmock/gmock.h>
#include <testing/gtest/include/gtest/gtest.h>


#define private public
#include "keystore_adapter_impl.h"

using namespace testing;

namespace OHOS {
namespace NWeb {

class KeystoreAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void KeystoreAdapterImplTest::SetUpTestCase(void)
{}

void KeystoreAdapterImplTest::TearDownTestCase(void)
{}

void KeystoreAdapterImplTest::SetUp(void)
{}

void KeystoreAdapterImplTest::TearDown(void)
{}

/**
 * @tc.name: KeystoreAdapterImplTest_InitParamSet_001
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
TEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_InitParamSet_001)
{
    struct OH_Huks_ParamSet *paramSet;
    struct OH_Huks_Param decryptParams[] = {
        {
            .tag = OH_HUKS_TAG_ALGORITHM,
            .uint32Param = OH_HUKS_ALG_AES
        }, {
            .tag = OH_HUKS_TAG_PURPOSE,
            .uint32Param = OH_HUKS_KEY_PURPOSE_DECRYPT
        }, {
            .tag = OH_HUKS_TAG_KEY_SIZE,
            .uint32Param = OH_HUKS_AES_KEY_SIZE_256
        }, {
            .tag = OH_HUKS_TAG_PADDING,
            .uint32Param = OH_HUKS_PADDING_NONE
        }, {
            .tag = OH_HUKS_TAG_BLOCK_MODE,
            .uint32Param = OH_HUKS_MODE_CBC
        }
    };

    int32_t result = KeystoreAdapterImpl::GetInstance().InitParamSet(nullptr, decryptParams,
        sizeof(decryptParams) / sizeof(OH_Huks_Param));
    EXPECT_NE(result, 0);
    result = KeystoreAdapterImpl::GetInstance().InitParamSet(nullptr, nullptr,
        sizeof(decryptParams) / sizeof(OH_Huks_Param));
    EXPECT_NE(result, 0);
    result = KeystoreAdapterImpl::GetInstance().InitParamSet(&paramSet, nullptr,
        sizeof(decryptParams) / sizeof(OH_Huks_Param));
    EXPECT_NE(result, 0);
    result = KeystoreAdapterImpl::GetInstance().InitParamSet(&paramSet, decryptParams,
        sizeof(decryptParams) / sizeof(OH_Huks_Param));
    EXPECT_EQ(result, 0);
    std::string alias = "test";
    std::string plainData = "web_test";
    std::string encryptString = KeystoreAdapterImpl::GetInstance().EncryptKey(alias, plainData);
    EXPECT_TRUE(encryptString.empty());
    std::string DecryptString = KeystoreAdapterImpl::GetInstance().DecryptKey(alias, encryptString);
    EXPECT_TRUE(DecryptString.empty());
}

/**
 * @tc.name: KeystoreAdapterImplTest_EncryptKey_002
 * @tc.desc: Encrypt and Decrypt
 * @tc.type: FUNC
 */
TEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_EncryptKey_002)
{
    std::string alias = "test";
    std::string plainData = "web_test";
    std::string encryptString = KeystoreAdapterImpl::GetInstance().EncryptKey(alias, plainData);
    EXPECT_TRUE(encryptString.empty());
    std::string DecryptString = KeystoreAdapterImpl::GetInstance().DecryptKey(alias, encryptString);
    EXPECT_TRUE(DecryptString.empty());
    encryptString = KeystoreAdapterImpl::GetInstance().EncryptKey(alias, "");
    EXPECT_TRUE(encryptString.empty());
    DecryptString = KeystoreAdapterImpl::GetInstance().DecryptKey(alias, "test");
    EXPECT_TRUE(DecryptString.empty());

    std::string nullAlias = "";
    std::string nullPlainData = "";
    std::string nullEncrypt = KeystoreAdapterImpl::GetInstance().EncryptKey(nullAlias, nullPlainData);
    EXPECT_TRUE(nullEncrypt.empty());
    std::string nullDecrypt = KeystoreAdapterImpl::GetInstance().DecryptKey(nullAlias, nullPlainData);
    EXPECT_TRUE(nullDecrypt.empty());

    std::string long_str_alias(2048U, 'a');
    nullEncrypt = KeystoreAdapterImpl::GetInstance().EncryptKey(long_str_alias, "test");
    EXPECT_TRUE(nullEncrypt.empty());
    nullDecrypt = KeystoreAdapterImpl::GetInstance().DecryptKey(long_str_alias, "test");
    EXPECT_TRUE(nullDecrypt.empty());
}
} // namespace NWeb
} // namespace OHOS
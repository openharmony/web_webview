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

#include <cstdint>
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "keystore_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {

namespace {}

class KeystoreAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void KeystoreAdapterImplTest::SetUpTestCase(void) {}

void KeystoreAdapterImplTest::TearDownTestCase(void) {}

void KeystoreAdapterImplTest::SetUp(void) {}

void KeystoreAdapterImplTest::TearDown(void) {}

/**
 * @tc.name: KeystoreAdapterImplTest_InitParamSet_001
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_InitParamSet_001, TestSize.Level1)
{
    struct HksParamSet* paramSet;
    struct HksParam decryptParams[] = { { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC } };

    int32_t result = KeystoreAdapterImpl::GetInstance().InitParamSet(
        nullptr, decryptParams, sizeof(decryptParams) / sizeof(HksParam));
    EXPECT_NE(result, 0);
    result =
        KeystoreAdapterImpl::GetInstance().InitParamSet(nullptr, nullptr, sizeof(decryptParams) / sizeof(HksParam));
    EXPECT_NE(result, 0);
    result =
        KeystoreAdapterImpl::GetInstance().InitParamSet(&paramSet, nullptr, sizeof(decryptParams) / sizeof(HksParam));
    EXPECT_NE(result, 0);
    result = KeystoreAdapterImpl::GetInstance().InitParamSet(
        &paramSet, decryptParams, sizeof(decryptParams) / sizeof(HksParam));
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: KeystoreAdapterImplTest_EncryptKey_002
 * @tc.desc: Encrypt and Decrypt
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_EncryptKey_002, TestSize.Level1)
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
    EXPECT_TRUE(DecryptString.empty());

    std::string long_str_alias(2048, 'a');
    nullEncrypt = KeystoreAdapterImpl::GetInstance().EncryptKey(long_str_alias, "test");
    EXPECT_TRUE(nullEncrypt.empty());
    nullDecrypt = KeystoreAdapterImpl::GetInstance().DecryptKey(long_str_alias, "test");
    EXPECT_TRUE(DecryptString.empty());

    std::string keyAlias(16, 'a');
    std::string keyPlain(32, 0x1c);
    const int prefixSize = 3 + 16; // len("V10") + IV size
    std::string result = KeystoreAdapterImpl::GetInstance().EncryptKey(keyAlias, keyPlain);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.length(), keyPlain.length() + prefixSize);
    std::string plainText = KeystoreAdapterImpl::GetInstance().DecryptKey(keyAlias, result);
    EXPECT_EQ(keyPlain, plainText);
}

/**
 * @tc.name: KeystoreAdapterImplTest_DecryptKey_001
 * @tc.desc: Encrypt and Decrypt
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_DecryptKey_001, TestSize.Level1)
{
    const int prefixSize = 3 + 16;                    // len("V10") + IV size
    std::string absentKeyAlias = "absent_alias_test"; // Do not use absent key to encrypt anything!
    std::string legacyAlias = "legacy_alias";

    // Legacy mode testing.
    std::string plainData = "legacy_key_test_16_bytes_aligned";
    std::string encryptString = KeystoreAdapterImpl::GetInstance().EncryptKey(legacyAlias, plainData);
    EXPECT_FALSE(encryptString.empty());
    EXPECT_EQ(encryptString.length(), plainData.length() + prefixSize);
    std::string plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(legacyAlias, encryptString);
    EXPECT_FALSE(plainString.empty());
    EXPECT_EQ(plainString, plainData);
    encryptString = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";
    plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(legacyAlias, encryptString);
    EXPECT_FALSE(plainString.empty());
    EXPECT_EQ(plainString.length(), encryptString.length()); // in/out data was 16-bytes aligned
    encryptString = "V10abcdefghijklmnopABCDEFGHIJKLMNOPQRSTUVWXYZ123456";
    plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(legacyAlias, encryptString);
    EXPECT_FALSE(plainString.empty());
    EXPECT_EQ(plainString.length(), encryptString.length() - prefixSize);

    // Absent key testing.
    encryptString = "fake_web_test";
    plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(absentKeyAlias, encryptString);
    EXPECT_TRUE(plainString.empty());
    encryptString = "V10abcdefghijklmnopABCDEFGHIJKLMNOPQRSTUVWXYZ123456";
    plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(absentKeyAlias, encryptString);
    EXPECT_TRUE(plainString.empty());

    std::string nullAlias = "";
    encryptString = "";
    plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(nullAlias, encryptString);
    EXPECT_TRUE(plainString.empty());

    std::string long_str_alias(2048, 'a');
    plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(long_str_alias, "test");
    EXPECT_TRUE(plainString.empty());

    std::string keyAlias(16, 'a');
    std::string keyPlain(32, 'x');
    std::string result = KeystoreAdapterImpl::GetInstance().EncryptKey(keyAlias, keyPlain);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.length(), keyPlain.length() + prefixSize);
    plainString = KeystoreAdapterImpl::GetInstance().DecryptKey(keyAlias, result);
    EXPECT_EQ(keyPlain, plainString);
}

/**
 * @tc.name: KeystoreAdapterImplTest_AssetQuery_002
 * @tc.desc: Query asset
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_AssetQuery_003, TestSize.Level1)
{
    std::string alias = "test";
    std::string encryptString = KeystoreAdapterImpl::GetInstance().AssetQuery(alias);
    EXPECT_TRUE(encryptString.empty());

    std::string nullAlias = "";
    std::string nullEncrypt = KeystoreAdapterImpl::GetInstance().AssetQuery(nullAlias);
    EXPECT_TRUE(nullEncrypt.empty());
}
} // namespace NWeb
} // namespace OHOS
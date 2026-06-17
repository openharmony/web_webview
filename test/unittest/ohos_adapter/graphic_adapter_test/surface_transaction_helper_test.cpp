/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#define private public
#include "surface_transaction_helper.h"
#undef private

using testing::ext::TestSize;
using testing::_;

namespace OHOS::NWeb {

namespace {
constexpr uint64_t TEST_SRC_ID = 100;
constexpr uint64_t TEST_SEQ_NUM = 200;
}

class SurfaceTransactionCallBackHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    
protected:
    SurfaceTransactionCallBackHelper* helper_;
};

void SurfaceTransactionCallBackHelperTest::SetUpTestCase(void)
{}

void SurfaceTransactionCallBackHelperTest::TearDownTestCase(void)
{}

void SurfaceTransactionCallBackHelperTest::SetUp(void)
{
    helper_ = &SurfaceTransactionCallBackHelper::GetInstance();
}

void SurfaceTransactionCallBackHelperTest::TearDown(void)
{
    helper_->UnRegisterCallBack(TEST_SRC_ID);
    helper_ = nullptr;
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_GetInstance_001.
 * @tc.desc: Test GetInstance returns the same instance.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_GetInstance_001, TestSize.Level1)
{
    auto& instance1 = SurfaceTransactionCallBackHelper::GetInstance();
    auto& instance2 = SurfaceTransactionCallBackHelper::GetInstance();
    
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_RegisterCallBack_NewSrcId_002.
 * @tc.desc: Test RegisterCallBack when srcId does not exist.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, RegisterCallBack_NewSrcId_002, TestSize.Level1)
{
    auto callback = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback);
    
    EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID) != helper_->callbackMap_.end());
    EXPECT_TRUE(helper_->callbackMap_[TEST_SRC_ID].find(TEST_SEQ_NUM) != helper_->callbackMap_[TEST_SRC_ID].end());
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_RegisterCallBack_ExistingSrcId_003.
 * @tc.desc: Test RegisterCallBack when srcId already exists.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, RegisterCallBack_ExistingSrcId_003, TestSize.Level1)
{
    auto callback1 = [](uint64_t timestamp) {};
    auto callback2 = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback1);
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM + 1, callback2);
    
    EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID) != helper_->callbackMap_.end());
    EXPECT_TRUE(helper_->callbackMap_[TEST_SRC_ID].find(TEST_SEQ_NUM) != helper_->callbackMap_[TEST_SRC_ID].end());
    EXPECT_TRUE(helper_->callbackMap_[TEST_SRC_ID].find(TEST_SEQ_NUM + 1) != helper_->callbackMap_[TEST_SRC_ID].end());
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_RegisterCallBack_OverwriteSeq_004.
 * @tc.desc: Test RegisterCallBack overwrites existing seq.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, RegisterCallBack_OverwriteSeq_004, TestSize.Level1)
{
    auto callback1 = [](uint64_t timestamp) {};
    auto callback2 = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback1);
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback2);
    
    EXPECT_TRUE(helper_->callbackMap_[TEST_SRC_ID].find(TEST_SEQ_NUM) != helper_->callbackMap_[TEST_SRC_ID].end());
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_UnRegisterCallBack_005.
 * @tc.desc: Test UnRegisterCallBack removes srcId.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_UnRegisterCallBack_005, TestSize.Level1)
{
    auto callback = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback);
    EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID) != helper_->callbackMap_.end());
    
    helper_->UnRegisterCallBack(TEST_SRC_ID);
    EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID) == helper_->callbackMap_.end());
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_UnRegisterCallBack_NotExist_006.
 * @tc.desc: Test UnRegisterCallBack when srcId does not exist.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, UnRegisterCallBack_NotExist_006, TestSize.Level1)
{
    helper_->UnRegisterCallBack(TEST_SRC_ID + 1000);
    EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID + 1000) == helper_->callbackMap_.end());
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_GetCallBack_SrcIdNotExist_007.
 * @tc.desc: Test GetCallBack when srcId does not exist.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, GetCallBack_SrcIdNotExist_007, TestSize.Level1)
{
    auto result = helper_->GetCallBack(TEST_SRC_ID + 1000, TEST_SEQ_NUM);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_GetCallBack_SeqNotExist_008.
 * @tc.desc: Test GetCallBack when seq does not exist.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, GetCallBack_SeqNotExist_008, TestSize.Level1)
{
    auto callback = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback);
    
    auto result = helper_->GetCallBack(TEST_SRC_ID, TEST_SEQ_NUM + 1000);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_GetCallBack_Success_009.
 * @tc.desc: Test GetCallBack success.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, GetCallBack_Success_009, TestSize.Level1)
{
    auto callback = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback);
    
    auto result = helper_->GetCallBack(TEST_SRC_ID, TEST_SEQ_NUM);
    EXPECT_NE(result, nullptr);
    
    EXPECT_TRUE(helper_->callbackMap_[TEST_SRC_ID].find(TEST_SEQ_NUM) == helper_->callbackMap_[TEST_SRC_ID].end());
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_GetCallBack_RemoveAfterGet_010.
 * @tc.desc: Test GetCallBack removes the callback after retrieving.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_GetCallBack_RemoveAfterGet_010,
    TestSize.Level1)
{
    auto callback = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback);
    
    auto result1 = helper_->GetCallBack(TEST_SRC_ID, TEST_SEQ_NUM);
    EXPECT_NE(result1, nullptr);
    
    auto result2 = helper_->GetCallBack(TEST_SRC_ID, TEST_SEQ_NUM);
    EXPECT_EQ(result2, nullptr);
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_MultiSrcId_011.
 * @tc.desc: Test multiple srcId registration and retrieval.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_MultiSrcId_011, TestSize.Level1)
{
    auto callback1 = [](uint64_t timestamp) {};
    auto callback2 = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback1);
    helper_->RegisterCallBack(TEST_SRC_ID + 100, TEST_SEQ_NUM + 100, callback2);
    
    auto result1 = helper_->GetCallBack(TEST_SRC_ID, TEST_SEQ_NUM);
    EXPECT_NE(result1, nullptr);
    
    auto result2 = helper_->GetCallBack(TEST_SRC_ID + 100, TEST_SEQ_NUM + 100);
    EXPECT_NE(result2, nullptr);
    
    helper_->UnRegisterCallBack(TEST_SRC_ID);
    helper_->UnRegisterCallBack(TEST_SRC_ID + 100);
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_DfxInfo_012.
 * @tc.desc: Test DfxInfo method.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_DfxInfo_012, TestSize.Level1)
{
    auto callback = [](uint64_t timestamp) {};
    
    helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM, callback);
    EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID) != helper_->callbackMap_.end());
    
    helper_->DfxInfo();
    EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID) != helper_->callbackMap_.end());
    
    helper_->UnRegisterCallBack(TEST_SRC_ID);
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_ThreadSafety_013.
 * @tc.desc: Test thread safety with concurrent RegisterCallBack.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_ThreadSafety_013, TestSize.Level1)
{
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([this, i]() {
            auto callback = [](uint64_t timestamp) {};
            helper_->RegisterCallBack(TEST_SRC_ID + i, TEST_SEQ_NUM + i, callback);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    for (int i = 0; i < 10; i++) {
        EXPECT_TRUE(helper_->callbackMap_.find(TEST_SRC_ID + i) != helper_->callbackMap_.end());
        helper_->UnRegisterCallBack(TEST_SRC_ID + i);
    }
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_ThreadSafetyGet_014.
 * @tc.desc: Test thread safety with concurrent GetCallBack.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_ThreadSafetyGet_014, TestSize.Level1)
{
    auto callback = [](uint64_t timestamp) {};
    
    for (int i = 0; i < 10; i++) {
        helper_->RegisterCallBack(TEST_SRC_ID, TEST_SEQ_NUM + i, callback);
    }
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([this, i, &successCount]() {
            auto result = helper_->GetCallBack(TEST_SRC_ID, TEST_SEQ_NUM + i);
            if (result != nullptr) {
                successCount++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount.load(), 10);
    helper_->UnRegisterCallBack(TEST_SRC_ID);
}

/**
 * @tc.name: SurfaceTransactionCallBackHelper_EmptyCallbackMap_015.
 * @tc.desc: Test behavior when callbackMap is empty.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionCallBackHelperTest, SurfaceTransactionCallBackHelper_EmptyCallbackMap_015, TestSize.Level1)
{
    helper_->UnRegisterCallBack(TEST_SRC_ID);
    
    EXPECT_TRUE(helper_->callbackMap_.empty());
    
    auto result = helper_->GetCallBack(TEST_SRC_ID, TEST_SEQ_NUM);
    EXPECT_EQ(result, nullptr);
}

}
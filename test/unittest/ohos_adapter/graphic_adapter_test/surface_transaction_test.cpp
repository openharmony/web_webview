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
#include "surface_transaction.h"
#include "surface_control.h"
#include "oh_surface_control.h"
#undef private

using testing::ext::TestSize;

namespace OHOS::NWeb {
class SurfaceTransactionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    SurfaceTransaction* transaction_;
};

void SurfaceTransactionTest::SetUpTestCase(void)
{}

void SurfaceTransactionTest::TearDownTestCase(void)
{}

void SurfaceTransactionTest::SetUp(void)
{
    transaction_ = new SurfaceTransaction(nullptr);
}

void SurfaceTransactionTest::TearDown(void)
{
    if (transaction_) {
        delete transaction_;
        transaction_ = nullptr;
    }
}

HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Constructor_001, TestSize.Level1)
{
    auto txn = new SurfaceTransaction(nullptr);
    EXPECT_NE(txn, nullptr);
    delete txn;
}
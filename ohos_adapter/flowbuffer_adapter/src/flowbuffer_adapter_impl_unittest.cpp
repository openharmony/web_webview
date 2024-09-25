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

#include <testing/gtest/include/gtest/gtest.h>
#include <testing/gmock/include/gmock/gmock.h>
#include <sys/mman.h>

#include "ohos_adapter_helper.h"

#define private public
#include "flowbuffer_adapter_impl.h"

using namespace testing;

namespace OHOS {
namespace NWeb {
class FlowbufferAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FlowbufferAdapterImplTest::SetUpTestCase() {}

void FlowbufferAdapterImplTest::TearDownTestCase() {}

void FlowbufferAdapterImplTest::SetUp() {}

void FlowbufferAdapterImplTest::TearDown() {}

/**
 * @tc.name: FlowbufferAdapterImplTest_001.
 * @tc.desc: test FlowbufferAdapterImpl StartPerformanceBoost.
 * @tc.type: FUNC.
 */
TEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_001)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    EXPECT_TRUE(flowbufferAdapter != nullptr);
    flowbufferAdapter->StartPerformanceBoost();
}

/**
 * @tc.name: FlowbufferAdapterImplTest_002.
 * @tc.desc: test FlowbufferAdapterImpl CreateAshmem.
 * @tc.type: FUNC.
 */
TEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_002)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    EXPECT_TRUE(flowbufferAdapter != nullptr);
    int fd;
    size_t scriptLength = 10;
    auto ashmem = flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    EXPECT_TRUE(ashmem != nullptr);
    ashmem = flowbufferAdapter->CreateAshmem(-1, PROT_READ | PROT_WRITE, fd);
    EXPECT_EQ(ashmem, nullptr);
}

/**
 * @tc.name: FlowbufferAdapterImplTest_002.
 * @tc.desc: test FlowbufferAdapterImpl CreateAshmemWithFd.
 * @tc.type: FUNC.
 */
TEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_003)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    EXPECT_TRUE(flowbufferAdapter != nullptr);
    int fd;
    size_t scriptLength = 10;
    flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    auto ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, scriptLength, PROT_READ);
    EXPECT_TRUE(ashmem != nullptr);
    ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, INT_MAX, PROT_READ);
    EXPECT_EQ(ashmem, nullptr);
    ashmem = flowbufferAdapter->CreateAshmemWithFd(-1, scriptLength, PROT_READ);
    EXPECT_EQ(ashmem, nullptr);
}
} // namespace NWeb
} // namespace OHOS

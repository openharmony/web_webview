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

#include <gtest/gtest.h>
#include <sys/mman.h>

#include "ohos_adapter_helper.h"

#define private public
#include "flowbuffer_adapter_impl.h"
#include <chrono>
#include <thread>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {
namespace {
    const int64_t PERFORMANCE_PERIOD_MS = 300;
}

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
 * @tc.require:
 */
HWTEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_001, TestSize.Level1)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    EXPECT_TRUE(flowbufferAdapter != nullptr);
    flowbufferAdapter->StartPerformanceBoost();
}

/**
 * @tc.name: FlowbufferAdapterImplTest_002.
 * @tc.desc: test FlowbufferAdapterImpl CreateAshmem.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_002, TestSize.Level1)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    EXPECT_TRUE(flowbufferAdapter != nullptr);
    int fd;
    size_t scriptLength = 10;
    auto ashmem = flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    EXPECT_TRUE(ashmem != nullptr);
}

/**
 * @tc.name: FlowbufferAdapterImplTest_002.
 * @tc.desc: test FlowbufferAdapterImpl CreateAshmemWithFd.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_003, TestSize.Level1)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    EXPECT_TRUE(flowbufferAdapter != nullptr);
    int fd;
    size_t scriptLength = 10;
    flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    auto ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, scriptLength, PROT_READ);
    EXPECT_TRUE(ashmem != nullptr);
}

/**
 * @tc.name: FlowbufferAdapterImplTest_004.
 * @tc.desc: test FlowbufferAdapterImpl NeedReportScene.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_004, TestSize.Level1)
{
    auto flowbufferAdapter = std::make_shared<FlowbufferAdapterImpl>();
    bool needReportScene  = flowbufferAdapter->NeedReportScene();
    EXPECT_FALSE(needReportScene);
    std::this_thread::sleep_for(std::chrono::milliseconds(PERFORMANCE_PERIOD_MS));
    needReportScene = flowbufferAdapter->NeedReportScene();
    EXPECT_TRUE(needReportScene);
}

/**
 * @tc.name: FlowbufferAdapterImplTest_005.
 * @tc.desc: test FlowbufferAdapterImpl StartPerformanceBoost.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_005, TestSize.Level1)
{
    auto flowbufferAdapter = std::make_shared<FlowbufferAdapterImpl>();
    bool needReportScene  = flowbufferAdapter->NeedReportScene();
    flowbufferAdapter->StartPerformanceBoost();
    std::this_thread::sleep_for(std::chrono::milliseconds(PERFORMANCE_PERIOD_MS));
    flowbufferAdapter->StartPerformanceBoost();
    EXPECT_FALSE(needReportScene);
}

/**
 * @tc.name: FlowbufferAdapterImplTest_006.
 * @tc.desc: test FlowbufferAdapterImpl CreateAshmem.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_006, TestSize.Level1)
{
    int fd = -1;
    size_t scriptLength = 10;
    auto flowbufferAdapter = std::make_shared<FlowbufferAdapterImpl>();
    flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    fd = 1;
    flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    scriptLength = 1024;
    auto ashmem = flowbufferAdapter->CreateAshmem(scriptLength, PROT_READ | PROT_WRITE, fd);
    EXPECT_TRUE(ashmem != nullptr);
}

/**
 * @tc.name: FlowbufferAdapterImplTest_007.
 * @tc.desc: test FlowbufferAdapterImpl CreateAshmemWithFd.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(FlowbufferAdapterImplTest, FlowbufferAdapterImplTest_007, TestSize.Level1)
{
    int fd = -1;
    size_t scriptLength = 10;
    auto flowbufferAdapter = std::make_shared<FlowbufferAdapterImpl>();
    flowbufferAdapter->CreateAshmemWithFd(fd, scriptLength, PROT_READ);
    fd = 1;
    flowbufferAdapter->CreateAshmemWithFd(fd, scriptLength, PROT_READ);
    scriptLength = 1024;
    auto ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, scriptLength, PROT_READ);
    EXPECT_TRUE(ashmem == nullptr);
}
} // namespace NWeb
} // namespace OHOS

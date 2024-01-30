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

#include <cstring>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "res_sched_client_adapter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {
namespace {
}

class ResSchedAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ResSchedAdapterImplTest::SetUpTestCase(void)
{}

void ResSchedAdapterImplTest::TearDownTestCase(void)
{}

void ResSchedAdapterImplTest::SetUp(void)
{}

void ResSchedAdapterImplTest::TearDown(void)
{}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportKeyThread_001
 * @tc.desc: ReportKeyThread.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportKeyThread_001, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    bool result = resAdapter->ReportKeyThread(static_cast<ResSchedStatusAdapter>(-1),
        1, 1, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_FALSE(result);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 1, static_cast<ResSchedRoleAdapter>(-1));
    EXPECT_FALSE(result);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 1, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);

    resAdapter->ReportWindowId(1, 1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(1, -1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(-1, -1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(-1, 1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportWindowStatus_002
 * @tc.desc: ReportWindowStatus.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportWindowStatus_002, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    bool result = resAdapter->ReportWindowStatus(static_cast<ResSchedStatusAdapter>(-1), 1, 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 1, 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportWindowStatus(static_cast<ResSchedStatusAdapter>(-1), 1, 1, 1);
    EXPECT_FALSE(result);

    result = resAdapter->ReportScene(static_cast<ResSchedStatusAdapter>(-1), ResSchedSceneAdapter::CLICK);
    EXPECT_FALSE(result);
    result = resAdapter->ReportScene(static_cast<ResSchedStatusAdapter>(-1), ResSchedSceneAdapter::CLICK, 0);
    EXPECT_FALSE(result);
    result = resAdapter->ReportScene(ResSchedStatusAdapter::THREAD_CREATED, ResSchedSceneAdapter::CLICK, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportScene(ResSchedStatusAdapter::THREAD_CREATED, static_cast<ResSchedSceneAdapter>(-1), -1);
    EXPECT_FALSE(result);

    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 0, 1, 1);
    EXPECT_FALSE(result);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 1);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 1, 1, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 1, 1, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 1, 0, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 0, 1, 1);
    EXPECT_FALSE(result);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT, 1);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportAudioData_003
 * @tc.desc: ReportAudioData.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportAudioData_003, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    bool result = resAdapter->ReportAudioData(static_cast<ResSchedStatusAdapter>(-1), 1, 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportAudioData(ResSchedStatusAdapter::AUDIO_STATUS_START, 1, 1);
    EXPECT_TRUE(result);
}
}
} // namespace NWeb

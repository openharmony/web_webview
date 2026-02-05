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
    resAdapter->ReportWindowId(1, 1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 1, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
        1, 1, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(2, 2);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        0, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportKeyThread_002
 * @tc.desc: ReportKeyThread.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportKeyThread_002, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 1);
    bool result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT, 1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
        1, 0, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(1, 1);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
        2, 2, ResSchedRoleAdapter::IMPORTANT_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(1, 2);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
        1, 1, ResSchedRoleAdapter::NORMAL_AUDIO);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(1, 2);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
        0, 0, ResSchedRoleAdapter::USER_INTERACT);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(1, 2);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
        1, 1, ResSchedRoleAdapter::NORMAL_DISPLAY);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(1, 2);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
        1, 1, ResSchedRoleAdapter::IMPORTANT_DISPLAY);
    EXPECT_TRUE(result);
    resAdapter->ReportWindowId(1, 2);
    result = resAdapter->ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
        1, 1, ResSchedRoleAdapter::IMAGE_DECODE);
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
    EXPECT_FALSE(result);
    result = resAdapter->ReportScene(ResSchedStatusAdapter::THREAD_CREATED, static_cast<ResSchedSceneAdapter>(-1), -1);
    EXPECT_FALSE(result);
    resAdapter->ReportScene(ResSchedStatusAdapter::THREAD_CREATED, ResSchedSceneAdapter::SLIDE, -1);

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
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 1, 1, 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 1, 1, 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 1, 0, 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::THREAD_CREATED, 0, 1, 1);
    EXPECT_FALSE(result);
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
    result = resAdapter->ReportAudioData(ResSchedStatusAdapter::AUDIO_STATUS_STOP, 1, 1);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportRenderProcessStatus_004
 * @tc.desc: ReportRenderProcessStatus.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportRenderProcessStatus_004, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    resAdapter->ReportRenderProcessStatus(static_cast<ResSchedStatusAdapter>(-1), 1);
    resAdapter->ReportRenderProcessStatus(static_cast<ResSchedStatusAdapter>(2), 0);
    resAdapter->ReportRenderProcessStatus(static_cast<ResSchedStatusAdapter>(2), 0);
    resAdapter->ReportRenderProcessStatus(static_cast<ResSchedStatusAdapter>(3), 0);
    resAdapter->ReportRenderProcessStatus(static_cast<ResSchedStatusAdapter>(3), 1);
}

/*
 * @tc.name: ResSchedAdapterImplTest_ReportScreenCapture_005
 * @tc.desc: ReportScreenCapture.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportScreenCapture_003, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    bool result = resAdapter->ReportScreenCapture(static_cast<ResSchedStatusAdapter>(-1), 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportScreenCapture(ResSchedStatusAdapter::AUDIO_STATUS_START, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportScreenCapture(ResSchedStatusAdapter::AUDIO_STATUS_STOP, 1);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportVideoPlaying_006
 * @tc.desc: ReportVideoPlaying.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportVideoPlaying_003, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    bool result = resAdapter->ReportVideoPlaying(static_cast<ResSchedStatusAdapter>(-1), 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportVideoPlaying(ResSchedStatusAdapter::AUDIO_STATUS_START, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportVideoPlaying(ResSchedStatusAdapter::AUDIO_STATUS_STOP, 1);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportProcessInUse_007
 * @tc.desc: ReportProcessInUse.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportProcessInUse_007, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    resAdapter->ReportSiteIsolationMode(true);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 1);
    bool result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_ACTIVE, 1, 1, 1);
    EXPECT_TRUE(result);
    resAdapter->ReportProcessInUse(1);

    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_ACTIVE, 1, 1, 1);
    EXPECT_TRUE(result);
    resAdapter->ReportProcessInUse(1);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT, 1);

    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 1);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 2);
    resAdapter->ReportProcessInUse(1);
    resAdapter->ReportProcessInUse(2);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_ACTIVE, 1, 1, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_INACTIVE, 1, 1, 2);
    EXPECT_TRUE(result);

    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 3);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 4);
    resAdapter->ReportProcessInUse(3);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_INACTIVE, 3, 3, 3);
    EXPECT_TRUE(result);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_INACTIVE, 3, 3, 4);
    EXPECT_TRUE(result);

    resAdapter->ReportSiteIsolationMode(true);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, -1);
    resAdapter->ReportWindowId(1, -1);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_ACTIVE, 1, 1, -1);
    EXPECT_TRUE(result);
    resAdapter->ReportProcessInUse(1);

    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_ACTIVE, 1, 1, -1);
    EXPECT_TRUE(result);
    resAdapter->ReportProcessInUse(1);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT, -1);

    resAdapter->ReportSiteIsolationMode(false);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_ENTER, 1);
    resAdapter->ReportWindowId(1, 1);
    result = resAdapter->ReportWindowStatus(ResSchedStatusAdapter::WEB_ACTIVE, 1, 1, 1);
    EXPECT_TRUE(result);
    resAdapter->ReportProcessInUse(1);
    resAdapter->ReportNWebInit(ResSchedStatusAdapter::WEB_SCENE_EXIT, 1);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportSceneInternal_009
 * @tc.desc: ReportSceneInternal.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportSceneInternal_009, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    bool result = resAdapter->ReportScene(ResSchedStatusAdapter::WEB_ACTIVE, ResSchedSceneAdapter::SLIDE, -1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportScene(ResSchedStatusAdapter::WEB_ACTIVE, ResSchedSceneAdapter::SLIDE, -1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportScene(ResSchedStatusAdapter::WEB_ACTIVE, ResSchedSceneAdapter::VISIBLE, -1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportScene(ResSchedStatusAdapter::WEB_ACTIVE, ResSchedSceneAdapter::LOAD_URL, -1);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: ResSchedAdapterImplTest_ReportSubwindowCall_010
 * @tc.desc: ReportSubwindowCall.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ResSchedAdapterImplTest, ResSchedAdapterImplTest_ReportSubwindowCall_010, TestSize.Level1)
{
    auto resAdapter = std::make_shared<ResSchedClientAdapter>();
    EXPECT_NE(resAdapter, nullptr);
    bool result = resAdapter->ReportSubwindowCall(static_cast<ResSchedStatusAdapter>(-1), 1, 1);
    EXPECT_FALSE(result);
    result = resAdapter->ReportSubwindowCall(ResSchedStatusAdapter::WEB_SUBWIN_CALL_START, 1, 1);
    EXPECT_TRUE(result);
    result = resAdapter->ReportSubwindowCall(ResSchedStatusAdapter::WEB_SUBWIN_CALL_STOP, 1, 1);
    EXPECT_TRUE(result);
}
}
} // namespace NWeb

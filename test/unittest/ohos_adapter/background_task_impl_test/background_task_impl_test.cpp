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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "background_task_adapter.h"
#include "background_task_mgr_helper.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::BackgroundTaskMgr;

namespace OHOS::BackgroundTaskMgr {
namespace {
ErrCode g_errCode = 0;
} // namespace
ErrCode BackgroundTaskMgrHelper::RequestBackgroundRunningForInner(const ContinuousTaskParamForInner &taskParam)
{
    return g_errCode;
}
}

namespace OHOS::NWeb {
class BackgroundTaskImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BackgroundTaskImplTest::SetUpTestCase(void)
{}

void BackgroundTaskImplTest::TearDownTestCase(void)
{}

void BackgroundTaskImplTest::SetUp(void)
{}

void BackgroundTaskImplTest::TearDown(void)
{}

/**
 * @tc.name: BackgroundTaskImplTest_BackgroundTaskAdapter_001
 * @tc.desc: BackgroundTaskAdapter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundTaskImplTest, BackgroundTaskImplTest_BackgroundTaskAdapter_001, TestSize.Level1)
{
    auto taskAdapter = std::make_shared<BackgroundTaskAdapter>();
    EXPECT_NE(taskAdapter, nullptr);
    bool result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::DATA_TRANSFER);
    EXPECT_TRUE(result);
    g_errCode = -1;
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::AUDIO_PLAYBACK);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::AUDIO_RECORDING);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::LOCATION);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::BLUETOOTH_INTERACTION);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::MULTI_DEVICE_CONNECTION);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::WIFI_INTERACTION);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::VOIP);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, BackgroundModeAdapter::TASK_KEEPING);
    EXPECT_FALSE(result);
    result = taskAdapter->RequestBackgroundRunning(true, static_cast<BackgroundModeAdapter>(0));
    EXPECT_FALSE(result);
}
} // namespace OHOS
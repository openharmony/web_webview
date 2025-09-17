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
#include "background_task_impl.h"

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
class BackgroundStateChangeCallbackAdapterMock : public BackgroundStateChangeCallbackAdapter {
public:
    BackgroundStateChangeCallbackAdapterMock() = default;
    void NotifyApplicationForeground() {}
    void NotifyApplicationBackground() {}
};

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
    g_errCode = 0;
    bool result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::DATA_TRANSFER);
    EXPECT_TRUE(result);
    g_errCode = -1;
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::AUDIO_PLAYBACK);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::AUDIO_RECORDING);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::LOCATION);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::BLUETOOTH_INTERACTION);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::MULTI_DEVICE_CONNECTION);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::WIFI_INTERACTION);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::VOIP);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, BackgroundModeAdapter::TASK_KEEPING);
    EXPECT_FALSE(result);
    result = BackgroundTaskAdapter::RequestBackgroundRunning(true, static_cast<BackgroundModeAdapter>(0));
    EXPECT_FALSE(result);
}

/**
 * @tc.name: BackgroundTaskImplTest_BackgroundTaskAdapter_002
 * @tc.desc: BackgroundTaskAdapter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundTaskImplTest, BackgroundTaskImplTest_BackgroundTaskAdapter_002, TestSize.Level1)
{
    std::shared_ptr<BackgroundStateChangeCallbackAdapter> callback =
        std::make_shared<BackgroundStateChangeCallbackAdapterMock>();
    std::shared_ptr<BackgroundTaskAdapterImpl> adapter =
        std::make_shared<BackgroundTaskAdapterImpl>();
    EXPECT_NE(adapter, nullptr);
    adapter->RegisterBackgroundTaskPolicyCallback(callback);
}

/**
 * @tc.name: BackgroundTaskImplTest_BackgroundTaskAdapter_003
 * @tc.desc: BackgroundTaskAdapter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BackgroundTaskImplTest, BackgroundTaskImplTest_BackgroundTaskAdapter_003, TestSize.Level1)
{
    std::shared_ptr<BackgroundStateChangeCallbackAdapter> callback =
        std::make_shared<BackgroundStateChangeCallbackAdapterMock>();
    std::shared_ptr<BackgroundTaskAdapterImpl> adapter =
        std::make_shared<BackgroundTaskAdapterImpl>();
    EXPECT_NE(adapter, nullptr);

    g_errCode = 0;
    bool result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::DATA_TRANSFER);
    EXPECT_TRUE(result);
    g_errCode = -1;
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::AUDIO_PLAYBACK);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::AUDIO_RECORDING);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::LOCATION);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::BLUETOOTH_INTERACTION);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::MULTI_DEVICE_CONNECTION);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::WIFI_INTERACTION);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::VOIP);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, BackgroundModeAdapter::TASK_KEEPING);
    EXPECT_FALSE(result);
    result = adapter->RequestBackgroundTaskRunning(true, static_cast<BackgroundModeAdapter>(0));
    EXPECT_FALSE(result);
}
} // namespace OHOS
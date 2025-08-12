/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "vsync_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
static void OnVsyncCallback()
{}

class GraphicAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void GraphicAdapterTest::SetUpTestCase(void)
{}

void GraphicAdapterTest::TearDownTestCase(void)
{}

void GraphicAdapterTest::SetUp(void)
{}

void GraphicAdapterTest::TearDown(void)
{}

void MockNWebVSyncCb(int64_t, void*)
{}

class MockEventRunner {
public:
    MockEventRunner() = default;
    ~MockEventRunner() = default;

    static std::shared_ptr<MockEventRunner> Create(const std::string &threadName)
    {
        return std::make_shared<MockEventRunner>();
    }

    uint64_t GetKernelThreadId() {
        return 1;
    }
};

class MockEventHandler : public OHOS::AppExecFwk::EventHandler {
public:
    explicit MockEventHandler(const std::shared_ptr<MockEventRunner> &runner = nullptr);
    std::shared_ptr<MockEventRunner> &GetEventRunner()
    {
        return eventRunner_;
    }
private:
    std::shared_ptr<MockEventRunner> eventRunner_;
};

MockEventHandler::MockEventHandler(const std::shared_ptr<MockEventRunner> &runner) : eventRunner_(runner)
{}

class MockVSyncAdapterImpl : public VSyncAdapterImpl {
public:
    MockVSyncAdapterImpl() = default;
    ~MockVSyncAdapterImpl() override = default;
    static std::shared_ptr<MockVSyncAdapterImpl> GetInstance() {
        static std::shared_ptr<MockVSyncAdapterImpl> instance = std::make_shared<MockVSyncAdapterImpl>();
        return instance;
    }
};

/**
 * @tc.name: GraphicAdapterTest_RequestVsync_001
 * @tc.desc: RequestVsync.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GraphicAdapterTest, GraphicAdapterTest_RequestVsync_001, TestSize.Level1)
{
    VSyncAdapterImpl &adapter = VSyncAdapterImpl::GetInstance();
    adapter.Init();
    adapter.Init();

    VSyncAdapterImpl vsyncAdapter;
    void* client = nullptr;
    adapter.OnVsync(1, client);
    client = &vsyncAdapter;
    adapter.OnVsync(1, client);
    adapter.VsyncCallbackInner(0);
    adapter.VsyncCallbackInner(1);
    int64_t period = adapter.GetVSyncPeriod();
    EXPECT_EQ(period, 0);
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFramePreferredRate(60);
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFrameRateLinkerEnable(false);
    adapter.SetFramePreferredRate(0);
    adapter.frameRateLinker_ = nullptr;
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFramePreferredRate(60);
    adapter.vsyncHandler_ = nullptr;
    adapter.SetOnVsyncCallback(OnVsyncCallback);
    adapter.SetOnVsyncEndCallback(OnVsyncCallback);
    adapter.OnVsync(1, client);
    adapter.SetIsGPUProcess(false);
}

/**
 * @tc.name: GraphicAdapterTest_RequestVsync_002
 * @tc.desc: RequestVsync.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GraphicAdapterTest, GraphicAdapterTest_RequestVsync_002, TestSize.Level1)
{
    VSyncAdapterImpl &adapter = VSyncAdapterImpl::GetInstance();
    adapter.SetScene("", 1);
    adapter.hasReportedKeyThread_ = true;
    adapter.hasRequestedVsync_ = true;
    EXPECT_EQ(VSyncErrorCode::SUCCESS, adapter.RequestVsync(nullptr, nullptr));
    adapter.hasRequestedVsync_ = false;
    EXPECT_EQ(VSyncErrorCode::SUCCESS, adapter.RequestVsync(nullptr, nullptr));

    auto mockAdapter = MockVSyncAdapterImpl::GetInstance();
    mockAdapter->hasReportedKeyThread_ = false;
    mockAdapter->SetIsGPUProcess(false);
    EXPECT_EQ(VSyncErrorCode::SUCCESS, mockAdapter->RequestVsync(nullptr, nullptr));

    mockAdapter->hasReportedKeyThread_ = false;
    mockAdapter->SetIsGPUProcess(true);
    EXPECT_EQ(VSyncErrorCode::SUCCESS, mockAdapter->RequestVsync(nullptr, nullptr));
}

/**
 * @tc.name: GraphicAdapterTest_SetDVSyncSwitch_001
 * @tc.desc: SetDVSyncSwitch.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(GraphicAdapterTest, GraphicAdapterTest_SetDVSyncSwitch_001, TestSize.Level1)
{
    VSyncAdapterImpl &adapter = VSyncAdapterImpl::GetInstance();
    bool dvsyncSwitch = false;
    adapter.SetDVSyncSwitch(dvsyncSwitch);
    dvsyncSwitch = true;
    adapter.SetDVSyncSwitch(dvsyncSwitch);
}
} // namespace NWeb

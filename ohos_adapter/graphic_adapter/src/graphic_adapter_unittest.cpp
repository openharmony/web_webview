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

#include <cstring>
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#define private public
#include "vsync_adapter_impl.h"
#undef private

using namespace OHOS;

namespace OHOS::NWeb {
static void OnVsyncCallback()
{}

class GraphicAdapterTest : public testing::Test {
public:
    GraphicAdapterTest() {}

    ~GraphicAdapterTest() override {}
    
    void SetUp() override {}

    void TearDown() override {}
};

void MockNWebVSyncCb(int64_t, void*)
{}

/**
 * @tc.name: GraphicAdapterTest_RequestVsync_001
 * @tc.desc: RequestVsync.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(GraphicAdapterTest, GraphicAdapterTest_RequestVsync_001)
{
    VSyncAdapterImpl &adapter = VSyncAdapterImpl::GetInstance();
    adapter.Init();
    adapter.Init();

    VSyncAdapterImpl vsyncAdapter;
    void* client = nullptr;
    adapter.OnVsync(1, client);
    client = &vsyncAdapter;
    adapter.OnVsync(1, client);
    adapter.VsyncCallbackInner(1);
    int64_t period = adapter.GetVSyncPeriod();
    EXPECT_EQ(period, 0);
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFramePreferredRate(60);
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFrameRateLinkerEnable(false);
    adapter.SetFramePreferredRate(0);
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFrameRateLinkerEnable(true);
    adapter.SetFramePreferredRate(60);
    adapter.SetOnVsyncCallback(OnVsyncCallback);
    adapter.SetOnVsyncEndCallback(OnVsyncCallback);
    adapter.OnVsync(1, client);
    adapter.SetIsGPUProcess(false);
}
} // namespace NWeb

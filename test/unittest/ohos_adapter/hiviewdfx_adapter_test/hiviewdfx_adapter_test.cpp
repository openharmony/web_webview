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

#include "gtest/gtest.h"
#include "hilog_adapter.h"
#include "ohos_adapter_helper.h"
#include "hitrace_adapter_impl.h"

#include "parameters.h"
using namespace OHOS;

using testing::ext::TestSize;

namespace OHOS::NWeb {
/**
 * @tc.name: NormalScene.
 * @tc.desc: test normal scene of HiViewDFXAdapter.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(HiViewDFXAdapterTest, NormalScene, TestSize.Level1)
{
    HiLogAdapter::PrintLog(LogLevelAdapter::INFO, "nwebTest", "nweb hilogAdapter PrintLog Test");

    HiLogAdapter::PrintConsoleLog(LogLevelAdapter::INFO, "ARKWEB-CONSOLE", "nwe console log test");

    int ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, { "testkey1", "0"});
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, { "testkey1", "0", "testkey2", "0"});
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, { "testkey1", "0", "testkey2", "0", "testkey3", "0" });
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write("testEvent",
        HiSysEventAdapter::EventType::STATISTIC,
        { "testkey1", "0", "testkey2", "testvalue2", "testkey3", "0", "testkey4", "testvalue4" });
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write("testEvent",
        HiSysEventAdapter::EventType::STATISTIC,
        { "testkey1", "0", "testkey2", "0", "testkey3", "0", "testkey4", "0", "testkey5", "0.0" });
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write("testEvent",
        HiSysEventAdapter::EventType::STATISTIC,
        { "testkey1", "0", "testkey2", "0", "testkey3", "0", "testkey4", "0", "testkey5", "0.0", "testkey6", "0.0" });
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write("testEvent",
        HiSysEventAdapter::EventType::STATISTIC,
        { "testkey1", "0", "testkey2", "0", "testkey3", "0", "testkey4", "0", "testkey5", "0.0", "testkey6", "0.0",
          "testkey7", "0.0" });
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write("testEvent",
        HiSysEventAdapter::EventType::STATISTIC,
        { "testkey1", "0", "testkey2", "0", "testkey3", "0", "testkey4", "0", "testkey5", "0.0", "testkey6", "0.0",
          "testkey7", "0.0", "testkey8", "0.0" });
    EXPECT_EQ(ret, 0);

    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartTrace("test");
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartAsyncTrace("test", 0);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountTrace("test", 1);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishAsyncTrace("test", 0);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishTrace();
    HiTraceAdapterImpl::GetInstance().IsHiTraceEnable();
}

/**
 * @tc.name: NormalScene_01.
 * @tc.desc: test normal scene of HiViewDFXAdapter.
 * @tc.type: FUNC 01.
 * @tc.require:
 */
HWTEST(HiViewDFXAdapterTest, NormalScene_01, TestSize.Level1)
{
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace("test");
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountOHOSTrace("test", 1);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
    uint64_t val = (1ULL << 24);
    std::string value = std::to_string(val);
    OHOS::system::SetParameter("debug.hitrace.tags.enableflags", value);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace("test");
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountOHOSTrace("test", 1);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
    val = (1ULL << 39);
    value = std::to_string(val);
    OHOS::system::SetParameter("debug.hitrace.tags.enableflags", value);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace("test");
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountOHOSTrace("test", 1);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
    HiTraceAdapterImpl::GetInstance().IsACETraceEnable();
}
} // namespace OHOS::NWeb

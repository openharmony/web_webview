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
#include "application_context.h"
#include "arkweb_utils.h"
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
    uint64_t val = 0;
    std::string value = std::to_string(val);
    OHOS::system::SetParameter("debug.hitrace.tags.enableflags", value);
    auto ret = HiTraceAdapterImpl::GetInstance().IsACETraceEnable();
    EXPECT_EQ(ret, 0);

    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().StartOHOSTrace("test");
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().CountOHOSTrace("test", 1);
    OhosAdapterHelper::GetInstance().GetHiTraceAdapterInstance().FinishOHOSTrace();
    val = (1ULL << 24);
    value = std::to_string(val);
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

/**
 * @tc.name: NormalScene_02.
 * @tc.desc: test normal scene of HiViewDFXAdapter.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(HiViewDFXAdapterTest, NormalScene_02, TestSize.Level1)
{
    const std::tuple<const std::string, const int64_t, const std::string, const int,
        const std::string, const std::vector<uint16_t>, const std::string,
        const int> data ("testkey1", 0, "testkey2", 0, "testkey3", {0}, "testkey4", 0);
    int ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, data);

    const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int, const std::string, const int,
    const std::string, const int64_t, const std::string, const int> data1 (
        "testkey1", 0, "testkey2", 0, "testkey3", 0, "testkey4", 0, "testkey5", 0, "testkey6", 0);
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, data1);

    const std::tuple<const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const uint32_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t> data2 (
    "testkey1", 0, "testkey2", 0, "testkey3", 0, "testkey4", 0, "testkey5", 0, "testkey6", 0,
    "testkey7", 0, "testkey8", 0, "testkey9", 0, "testkey10", 0, "testkey11", 0, "testkey12", 0,
    "testkey13", 0, "testkey14", 0, "testkey15", 0, "testkey16", 0, "testkey17", 0, "testkey18", 0,
    "testkey19", 0, "testkey20", 0, "testkey21", 0, "testkey22", 0, "testkey23", 0);
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, data2);

    const std::tuple<const std::string, const uint32_t,
        const std::string, const uint64_t> data3 ("testkey1", 0, "testkey2", 0);
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write("testEvent",
        HiSysEventAdapter::EventType::STATISTIC, data3);
}

/**
 * @tc.name: NormalScene_03.
 * @tc.desc: test normal scene of HiViewDFXAdapter.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(HiViewDFXAdapterTest, NormalScene_03, TestSize.Level1)
{
    auto contextImpl = std::make_shared<AbilityRuntime::ContextImpl>();
    auto applicationContext = AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<AppExecFwk::ApplicationInfo> info = std::make_shared<AppExecFwk::ApplicationInfo>();
    std::string bundleName = "NormalScene_03";
    info->bundleName = bundleName;
    contextImpl->SetApplicationInfo(info);
    applicationContext->AttachContextImpl(contextImpl);
    std::shared_ptr<AppExecFwk::ApplicationInfo> ans = AbilityRuntime::ApplicationContext
        ::GetInstance()->GetApplicationInfo();
    EXPECT_NE(ans, nullptr);

    const std::tuple<const std::string, const int64_t, const std::string, const int,
        const std::string, const std::vector<uint16_t>, const std::string,
        const int> data ("testkey1", 0, "testkey2", 0, "testkey3", {0}, "testkey4", 0);
    int ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, data);

    const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int, const std::string, const int,
    const std::string, const int64_t, const std::string, const int> data1 (
        "testkey1", 0, "testkey2", 0, "testkey3", 0, "testkey4", 0, "testkey5", 0, "testkey6", 0);
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, data1);

    const std::tuple<const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const uint32_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t> data2 (
    "testkey1", 0, "testkey2", 0, "testkey3", 0, "testkey4", 0, "testkey5", 0, "testkey6", 0,
    "testkey7", 0, "testkey8", 0, "testkey9", 0, "testkey10", 0, "testkey11", 0, "testkey12", 0,
    "testkey13", 0, "testkey14", 0, "testkey15", 0, "testkey16", 0, "testkey17", 0, "testkey18", 0,
    "testkey19", 0, "testkey20", 0, "testkey21", 0, "testkey22", 0, "testkey23", 0);
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::STATISTIC, data2);

    const std::tuple<const std::string, const uint32_t,
        const std::string, const uint64_t> data3 ("testkey1", 0, "testkey2", 0);
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write("testEvent",
        HiSysEventAdapter::EventType::STATISTIC, data3);

    applicationContext->AttachContextImpl(nullptr);
    ans = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    EXPECT_EQ(ans, nullptr);
}

/**
 * @tc.name: NormalScene_04.
 * @tc.desc: test normal scene of HiViewDFXAdapter.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(HiViewDFXAdapterTest, NormalScene_04, TestSize.Level1)
{
    auto contextImpl = std::make_shared<AbilityRuntime::ContextImpl>();
    auto applicationContext = AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<AppExecFwk::ApplicationInfo> info = std::make_shared<AppExecFwk::ApplicationInfo>();
    std::string bundleName = "NormalScene_04";
    info->bundleName = bundleName;
    contextImpl->SetApplicationInfo(info);
    applicationContext->AttachContextImpl(contextImpl);
    std::shared_ptr<AppExecFwk::ApplicationInfo> ans = AbilityRuntime::ApplicationContext
        ::GetInstance()->GetApplicationInfo();
    EXPECT_NE(ans, nullptr);

    const std::string input = "NAVIGATION_ID" + std::to_string(0) + "NAVIGATION_START" + std::to_string(0) +
        "REDIRECT_COUNT" + std::to_string(0) + "INPUT_TIME" + std::to_string(0) +
        "FIRST_PAINT" + std::to_string(0) + "FIRST_CONTENTFUL_PAINT" + std::to_string(0) +
        "FIRST_MEANINGFUL_PAINT" + std::to_string(0) + "IS_PAINT_DONE" + std::to_string(0);
    const std::tuple<const std::string, const std::string> data (input, "");
    int ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "FIRST_MEANINGFUL_PAINT_DONE", HiSysEventAdapter::EventType::STATISTIC, data);
    EXPECT_EQ(ret, 0);

    const std::string input1 = "NAVIGATION_ID" + std::to_string(0) + "NAVIGATION_START" + std::to_string(0) +
        "REDIRECT_COUNT" + std::to_string(0) + "REDIRECT_START" + std::to_string(0) +
        "REDIRECT_END" + std::to_string(0) + "FETCH_START" + std::to_string(0) +
        "WORKER_START" + std::to_string(0) + "DOMAIN_LOOKUP_START" + std::to_string(0) +
        "DOMAIN_LOOKUP_END" + std::to_string(0) + "CONNECT_START" + std::to_string(0) +
        "SECURE_CONNECT_START" + std::to_string(0) + "CONNECT_END" + std::to_string(0) +
        "REQUEST_START" + std::to_string(0) + "RESPONSE_START" + std::to_string(0) +
        "RESPONSE_END" + std::to_string(0) + "DOM_INTERACTIVE" + std::to_string(0) +
        "DOM_CONTENT_LOADED_EVENT_START" + std::to_string(0) + "DOM_CONTENT_LOADED_EVENT_END" + std::to_string(0) +
        "LOAD_EVENT_START" + std::to_string(0) + "LOAD_EVENT_END" + std::to_string(0) +
        "FIRST_PAINT" + std::to_string(0) + "FIRST_CONTENTFUL_PAINT" + std::to_string(0) +
        "LARGEST_CONTENTFUL_PAINT" + std::to_string(0) + "RENDER_INIT_BLOCK" + std::to_string(0);
    const std::tuple<const std::string, const std::string> data1 (input1, "");
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "PAGE_LOAD_TIME", HiSysEventAdapter::EventType::STATISTIC, data1);
    EXPECT_EQ(ret, 0);

    applicationContext->AttachContextImpl(nullptr);
    ans = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    EXPECT_EQ(ans, nullptr);
}

/**
 * @tc.name: NormalScene_05.
 * @tc.desc: test normal scene of HiViewDFXAdapter.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(HiViewDFXAdapterTest, NormalScene_05, TestSize.Level1)
{
    auto contextImpl = std::make_shared<AbilityRuntime::ContextImpl>();
    auto applicationContext = AbilityRuntime::ApplicationContext::GetInstance();
    std::shared_ptr<AppExecFwk::ApplicationInfo> info = std::make_shared<AppExecFwk::ApplicationInfo>();
    std::string bundleName = "NormalScene_05";
    info->bundleName = bundleName;
    contextImpl->SetApplicationInfo(info);
    applicationContext->AttachContextImpl(contextImpl);
    std::shared_ptr<AppExecFwk::ApplicationInfo> ans = AbilityRuntime::ApplicationContext
        ::GetInstance()->GetApplicationInfo();
    EXPECT_NE(ans, nullptr);

    const std::string input = "NAVIGATION_ID" + std::to_string(0) + "NAVIGATION_START" + std::to_string(LLONG_MAX) +
        "REDIRECT_COUNT" + std::to_string(0) + "INPUT_TIME" + std::to_string(0) +
        "FIRST_PAINT" + std::to_string(0) + "FIRST_CONTENTFUL_PAINT" + std::to_string(0) +
        "FIRST_MEANINGFUL_PAINT" + std::to_string(0) + "IS_PAINT_DONEA" + std::to_string(0);
    const std::tuple<const std::string, const std::string> data (input, "");
    int ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "FIRST_MEANINGFUL_PAINT_DONE", HiSysEventAdapter::EventType::STATISTIC, data);
    EXPECT_EQ(ret, 0);

    applicationContext->AttachContextImpl(nullptr);
    ans = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    EXPECT_EQ(ans, nullptr);
}

/**
* @tc.name: NormalScene.
* @tc.desc: test normal scene of HiViewDFXAdapter.
* @tc.type: FUNC.
* @tc.require:
*/
HWTEST(HiViewDFXAdapterTest, NormalScene_06, TestSize.Level1)
{
    OHOS::ArkWeb::SetBundleNameInner("test");
    int ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::BEHAVIOR, { "testkey1", "0"});
    EXPECT_EQ(ret, 0);

    OHOS::ArkWeb::SetApiVersionInner("test");
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::BEHAVIOR, { "testkey1", "0"});
    EXPECT_EQ(ret, 0);

    OHOS::ArkWeb::SetAppVersionInner("test");
    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::BEHAVIOR, { "testkey1", "0"});
    EXPECT_EQ(ret, 0);

    ret = OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(
        "testEvent", HiSysEventAdapter::EventType::BEHAVIOR, { "testkey1", "0"});
    EXPECT_EQ(ret, 0);
}
} // namespace OHOS::NWeb

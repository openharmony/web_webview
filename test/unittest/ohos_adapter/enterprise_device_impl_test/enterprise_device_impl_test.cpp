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
#include "enterprise_device_management_adapter_impl.h"
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
#include "browser_proxy.h"
#endif
#include "common_event_data.h"

using namespace testing;
using namespace testing::ext;
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
using namespace OHOS::EDM;
#endif
using namespace OHOS::EventFwk;

namespace OHOS {
namespace {
bool g_subscribeCommonEventRet = true;
bool g_unSubscribeCommonEventRet = true;
using Want = OHOS::AAFwk::Want;
} //namespace
namespace EventFwk {
bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_subscribeCommonEventRet;
}
bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_unSubscribeCommonEventRet;
}
}

#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
namespace EDM {
namespace {
std::shared_ptr<BrowserProxy> g_browserProxy = nullptr;
} // namespace
std::shared_ptr<BrowserProxy> BrowserProxy::GetBrowserProxy()
{
    return g_browserProxy;
}
}
#endif

namespace NWeb {
class EnterpriseDeviceImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EnterpriseDeviceImplTest::SetUpTestCase(void)
{}

void EnterpriseDeviceImplTest::TearDownTestCase(void)
{}

void EnterpriseDeviceImplTest::SetUp(void)
{}

void EnterpriseDeviceImplTest::TearDown(void)
{}

class MockEdmPolicyChangedEventCallbackAdapter : public EdmPolicyChangedEventCallbackAdapter {
public:
    MockEdmPolicyChangedEventCallbackAdapter() = default;
    void Changed() {}
};

/**
 * @tc.name: EnterpriseDeviceImplTest_BackgroundTaskAdapter_001
 * @tc.desc: BackgroundTaskAdapter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(EnterpriseDeviceImplTest, EnterpriseDeviceImplTest_BackgroundTaskAdapter_001, TestSize.Level0)
{
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
    std::string policies = "web_test";
    int32_t result = EnterpriseDeviceManagementAdapterImpl::GetInstance().GetPolicies(policies);
    EXPECT_EQ(result, -1);
    g_browserProxy = std::make_shared<BrowserProxy>();
    result = EnterpriseDeviceManagementAdapterImpl::GetInstance().GetPolicies(policies);
    EXPECT_EQ(result, 0);

    bool start = EnterpriseDeviceManagementAdapterImpl::GetInstance().StartObservePolicyChange();
    EXPECT_TRUE(start);
    g_subscribeCommonEventRet = false;
    start = EnterpriseDeviceManagementAdapterImpl::GetInstance().StartObservePolicyChange();
    EXPECT_FALSE(start);

    bool stop = EnterpriseDeviceManagementAdapterImpl::GetInstance().StopObservePolicyChange();
    EXPECT_TRUE(stop);
    g_unSubscribeCommonEventRet = false;
    stop = EnterpriseDeviceManagementAdapterImpl::GetInstance().StopObservePolicyChange();
    EXPECT_FALSE(stop);
    EnterpriseDeviceManagementAdapterImpl::GetInstance().commonEventSubscriber_ = nullptr;
    stop = EnterpriseDeviceManagementAdapterImpl::GetInstance().StopObservePolicyChange();
    EXPECT_TRUE(stop);
#else
    std::string policies = "web_test";
    int32_t result = EnterpriseDeviceManagementAdapterImpl::GetInstance().GetPolicies(policies);
    EXPECT_EQ(result, -1);
#endif
}

/**
 * @tc.name: EnterpriseDeviceImplTest_OnReceiveEvent_002
 * @tc.desc: OnReceiveEvent.
 * @tc.type: FUNC
 * @tc.require:
 */
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
HWTEST_F(EnterpriseDeviceImplTest, EnterpriseDeviceImplTest_OnReceiveEvent_002, TestSize.Level2)
{
    Want want;
    want.SetAction("web_test");
    OHOS::EventFwk::CommonEventData data(want);
    OHOS::EventFwk::CommonEventSubscribeInfo in;
    std::shared_ptr<EdmPolicyChangedEventCallbackAdapter> cb =
        std::make_shared<MockEdmPolicyChangedEventCallbackAdapter>();
    std::shared_ptr<NWebEdmEventSubscriber> result = std::make_shared<NWebEdmEventSubscriber>(in, cb);
    EXPECT_NE(result, nullptr);
    result->OnReceiveEvent(data);
    Want setwant;
    setwant.SetAction("com.ohos.edm.browserpolicychanged");
    OHOS::EventFwk::CommonEventData test(setwant);
    result->OnReceiveEvent(test);
}
#endif
}
} // namespace OHOS

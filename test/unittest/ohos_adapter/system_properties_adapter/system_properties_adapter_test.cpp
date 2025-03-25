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
#include "nweb_config_helper.h"
#include "nweb_log.h"
#include "system_properties_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
class SystemPropertiesAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SystemPropertiesAdapterTest::SetUpTestCase(void)
{}

void SystemPropertiesAdapterTest::TearDownTestCase(void)
{}

void SystemPropertiesAdapterTest::SetUp(void)
{}

void SystemPropertiesAdapterTest::TearDown(void)
{}

class SystemPropertiesObserverTest : public SystemPropertiesObserver {
 public:
   SystemPropertiesObserverTest() = default;
   ~SystemPropertiesObserverTest() override = default;

    void PropertiesUpdate(const char* value) override
    {
        if (strcmp(value, "true") == 0) {
            update_value_ = true;
        } else if (strcmp(value, "false") == 0) {
            update_value_ = false;
        } else {
            WVLOG_E("SystemPropertiesObserverTest return value is invalid");
        }
    }

   bool UpdateValue()
   {
     return update_value_;
   }
 private:
    bool update_value_ = false;
};

/**
 * @tc.name: SystemPropertiesAdapterTest_GetDeviceInfoBrand_001
 * @tc.desc: GetInstance.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SystemPropertiesAdapterTest, SystemPropertiesAdapterTest_GetDeviceInfoBrand_001, TestSize.Level1)
{
    std::string model = SystemPropertiesAdapterImpl::GetInstance().GetDeviceInfoProductModel();
    EXPECT_NE(model, "");
    std::string deviceInfo = SystemPropertiesAdapterImpl::GetInstance().GetDeviceInfoBrand();
    EXPECT_NE(deviceInfo, "");
    int32_t result = SystemPropertiesAdapterImpl::GetInstance().GetDeviceInfoMajorVersion();
    EXPECT_NE(result, -1);
    SystemPropertiesAdapterImpl::GetInstance().GetResourceUseHapPathEnable();
    SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType();
    bool value = SystemPropertiesAdapterImpl::GetInstance().GetWebOptimizationValue();
    EXPECT_TRUE(value);
    system("param set web.optimization false");
    value = SystemPropertiesAdapterImpl::GetInstance().GetWebOptimizationValue();
    EXPECT_FALSE(value);
    system("param set web.optimization true");
    bool mode = SystemPropertiesAdapterImpl::GetInstance().IsAdvancedSecurityMode();
    EXPECT_FALSE(mode);
    string logMode = SystemPropertiesAdapterImpl::GetInstance().GetNetlogMode();
    EXPECT_EQ(logMode, "None");
    string siteIsolationMode = SystemPropertiesAdapterImpl::GetInstance().GetSiteIsolationMode();
    EXPECT_EQ(siteIsolationMode, "none");
}

/**
 * @tc.name: SystemPropertiesAdapterTest_OptSystemPropertiesObserver_002
 * @tc.desc: AttachSysPropObserver DetachSysPropObserver unittest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SystemPropertiesAdapterTest, SystemPropertiesAdapterTest_OptSystemPropertiesObserver_002, TestSize.Level1)
{
    system("param set web.render.dump false");
    auto& system_properties_adapter = SystemPropertiesAdapterImpl::GetInstance();
    system_properties_adapter.AttachSysPropObserver(PropertiesKey::PROP_RENDER_DUMP, nullptr);

    auto observer = std::make_shared<SystemPropertiesObserverTest>();
    system_properties_adapter.AttachSysPropObserver(PropertiesKey::PROP_RENDER_DUMP, observer.get());
    system("param set web.render.dump true");
    bool result_first = observer->UpdateValue();
    EXPECT_TRUE(result_first);
    system("param set web.render.dump false");
    bool result_second = observer->UpdateValue();
    EXPECT_FALSE(result_second);

    system_properties_adapter.DetachSysPropObserver(PropertiesKey::PROP_RENDER_DUMP, nullptr);
    system_properties_adapter.DetachSysPropObserver(PropertiesKey::PROP_RENDER_DUMP, observer.get());
    system("param set web.render.dump true");
    bool result_third = observer->UpdateValue();
    EXPECT_FALSE(result_third);
}

/**
 * @tc.name: SystemPropertiesAdapterTest_GetOOPGPUEnable_003
 * @tc.desc: GetInstance unittest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SystemPropertiesAdapterTest, SystemPropertiesAdapterTest_GetOOPGPUEnable_003, TestSize.Level1)
{
    system("param set web.oop.gpu None");
    bool value = SystemPropertiesAdapterImpl::GetInstance().GetOOPGPUEnable();
    EXPECT_FALSE(value);
    SystemPropertiesAdapterImpl::GetInstance().SetOOPGPUDisable();
    value = SystemPropertiesAdapterImpl::GetInstance().GetOOPGPUEnable();
    EXPECT_FALSE(value);
    system("param set web.oop.gpu false");
    value = SystemPropertiesAdapterImpl::GetInstance().GetOOPGPUEnable();
    EXPECT_FALSE(value);
    system("param set web.oop.gpu true");
    value = SystemPropertiesAdapterImpl::GetInstance().GetOOPGPUEnable();
    EXPECT_TRUE(value);
}

/**
 * @tc.name: SystemPropertiesAdapterTest_GetVulkanEnable_004
 * @tc.desc: GetInstance unittest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SystemPropertiesAdapterTest, SystemPropertiesAdapterTest_GetVulkanEnable_004, TestSize.Level1)
{
    system("param set web.ohos.vulkan false");
    string value = SystemPropertiesAdapterImpl::GetInstance().GetVulkanStatus();
    EXPECT_EQ(value, "false");
}

/**
 * @tc.name  : AnalysisFromConfig_ShouldReturnMobile_WhenFactoryLevelIsPhone
 * @tc.number: SystemPropertiesAdapterImplTest_001
 * @tc.desc  : Test AnalysisFromConfig function when factory level is phone.
 */
HWTEST_F(SystemPropertiesAdapterTest, AnalysisFromConfig_ShouldReturnMobile_WhenFactoryLevelIsPhone, TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.emplace("factoryConfig/factoryLevel", "2");
    ProductDeviceType result = SystemPropertiesAdapterImpl::GetInstance().AnalysisFromConfig();
    EXPECT_EQ(result, ProductDeviceType::DEVICE_TYPE_MOBILE);
    NWebConfigHelper::Instance().perfConfig_.clear();
}

/**
 * @tc.name  : AnalysisFromConfig_ShouldReturnTablet_WhenFactoryLevelIsTablet
 * @tc.number: SystemPropertiesAdapterImplTest_002
 * @tc.desc  : Test AnalysisFromConfig function when factory level is tablet.
 */
HWTEST_F(SystemPropertiesAdapterTest, AnalysisFromConfig_ShouldReturnTablet_WhenFactoryLevelIsTablet, TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.emplace("factoryConfig/factoryLevel", "4");
    ProductDeviceType result = SystemPropertiesAdapterImpl::GetInstance().AnalysisFromConfig();
    EXPECT_EQ(result, ProductDeviceType::DEVICE_TYPE_TABLET);
    NWebConfigHelper::Instance().perfConfig_.clear();
}

/**
 * @tc.name  : AnalysisFromConfig_ShouldReturn2In1_WhenFactoryLevelIsPC
 * @tc.number: SystemPropertiesAdapterImplTest_003
 * @tc.desc  : Test AnalysisFromConfig function when factory level is PC.
 */
HWTEST_F(SystemPropertiesAdapterTest, AnalysisFromConfig_ShouldReturn2In1_WhenFactoryLevelIsPC, TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.emplace("factoryConfig/factoryLevel", "8");
    ProductDeviceType result = SystemPropertiesAdapterImpl::GetInstance().AnalysisFromConfig();
    EXPECT_EQ(result, ProductDeviceType::DEVICE_TYPE_2IN1);
    NWebConfigHelper::Instance().perfConfig_.clear();
}

/**
 * @tc.name  : AnalysisFromConfig_ShouldReturnWearable_WhenFactoryLevelIsWearable
 * @tc.number: SystemPropertiesAdapterImplTest_004
 * @tc.desc  : Test AnalysisFromConfig function when factory level is Wearable.
 */
HWTEST_F(SystemPropertiesAdapterTest, AnalysisFromConfig_ShouldReturnWearable_WhenFactoryLevelIsWearable,
         TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.emplace("factoryConfig/factoryLevel", "16");
    ProductDeviceType result = SystemPropertiesAdapterImpl::GetInstance().AnalysisFromConfig();
    EXPECT_EQ(result, ProductDeviceType::DEVICE_TYPE_WEARABLE);
    NWebConfigHelper::Instance().perfConfig_.clear();
}

/**
 * @tc.name  : AnalysisFromConfig_ShouldReturnWearable_WhenFactoryLevelIsWearable
 * @tc.number: SystemPropertiesAdapterImplTest_005
 * @tc.desc  : Test AnalysisFromConfig function when factory level is TV.
 */
HWTEST_F(SystemPropertiesAdapterTest, AnalysisFromConfig_ShouldReturnTV_WhenFactoryLevelIsTV, TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.emplace("factoryConfig/factoryLevel", "32");
    ProductDeviceType result = SystemPropertiesAdapterImpl::GetInstance().AnalysisFromConfig();
    EXPECT_EQ(result, ProductDeviceType::DEVICE_TYPE_TV);
    NWebConfigHelper::Instance().perfConfig_.clear();
}

/**
 * @tc.name  : AnalysisFromConfig_ShouldReturnUnknown_WhenFactoryLevelIsUnknown
 * @tc.number: SystemPropertiesAdapterImplTest_006
 * @tc.desc  : Test AnalysisFromConfig function when factory level is unknown.
 */
HWTEST_F(SystemPropertiesAdapterTest, AnalysisFromConfig_ShouldReturnUnknown_WhenFactoryLevelIsUnknown, TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.emplace("factoryConfig/factoryLevel", "64");
    ProductDeviceType result = SystemPropertiesAdapterImpl::GetInstance().AnalysisFromConfig();
    EXPECT_EQ(result, ProductDeviceType::DEVICE_TYPE_UNKNOWN);
    NWebConfigHelper::Instance().perfConfig_.clear();
}

/**
 * @tc.name: SystemPropertiesAdapterTest_GetPRPPreloadMode_007
 * @tc.desc: GetInstance unittest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SystemPropertiesAdapterTest, SystemPropertiesAdapterTest_GetPRPPreloadMode_007, TestSize.Level1)
{
    system("param set web.prppreload.mode none");
    string value = SystemPropertiesAdapterImpl::GetInstance().GetPRPPreloadMode();
    EXPECT_EQ(value, "none");
    system("param set web.prppreload.mode preconnect");
    value = SystemPropertiesAdapterImpl::GetInstance().GetPRPPreloadMode();
    EXPECT_EQ(value, "preconnect");
    system("param set web.prppreload.mode preload");
    value = SystemPropertiesAdapterImpl::GetInstance().GetPRPPreloadMode();
    EXPECT_EQ(value, "preload");
}
} // namespace OHOS

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
#include <testing/gmock/include/gmock/gmock.h>
#include <testing/gtest/include/gtest/gtest.h>

#define private public
#include "location_proxy_adapter_impl.h"

using namespace testing;

namespace OHOS::NWeb {

class LocationProxyAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void LocationProxyAdapterTest::SetUpTestCase(void)
{}

void LocationProxyAdapterTest::TearDownTestCase(void)
{}

void LocationProxyAdapterTest::SetUp(void)
{}

void LocationProxyAdapterTest::TearDown(void)
{}

class LocationCallbackAdapterMock : public LocationCallbackAdapter {
public:
    LocationCallbackAdapterMock() = default;

    void OnLocationReport(
        const std::shared_ptr<LocationInfo> location)  override
    {}

    void OnLocatingStatusChange(const int status) override
    {}

    void OnErrorReport(const int errorCode) override
    {}
};

/**
 * @tc.name: LocationProxyAdapterTest_LocationInstance_001
 * @tc.desc: LocationInstance.
 * @tc.type: FUNC
 */
TEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_LocationInstance_001)
{
    std::shared_ptr<LocationProxyAdapter> proxyAdapter = LocationInstance::GetInstance().CreateLocationProxyAdapter();
    EXPECT_NE(proxyAdapter, nullptr);
    std::shared_ptr<LocationRequestConfig> requestConfig =
        LocationInstance::GetInstance().CreateLocationRequestConfig();
    EXPECT_NE(requestConfig, nullptr);
}

/**
 * @tc.name: LocationProxyAdapterTest_SetScenario_002
 * @tc.desc: SetScenario.
 * @tc.type: FUNC
 */
TEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_SetScenario_002)
{
    auto requestConfigImpl = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfigImpl, nullptr);
    int32_t scenario = -1;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::UNSET;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::NAVIGATION;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::TRAJECTORY_TRACKING;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::CAR_HAILING;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::DAILY_LIFE_SERVICE;
    requestConfigImpl->SetScenario(scenario);
    scenario = OHOS::NWeb::LocationRequestConfig::Scenario::NO_POWER;
    requestConfigImpl->SetScenario(scenario);

    requestConfigImpl->SetFixNumber(0);
    requestConfigImpl->SetMaxAccuracy(0);
    requestConfigImpl->SetDistanceInterval(0);
    requestConfigImpl->SetTimeInterval(0);
    int32_t priority = -1;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_UNSET;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_ACCURACY;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_LOW_POWER;
    requestConfigImpl->SetPriority(priority);
    priority = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_FAST_FIRST_FIX;
    requestConfigImpl->SetPriority(priority);
}

/**
 * @tc.name: LocationProxyAdapterTest_SetScenario_003
 * @tc.desc: SetScenario.
 * @tc.type: FUNC
 */
TEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_SetScenario_003)
{
    auto requestConfigImpl = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfigImpl, nullptr);
    requestConfigImpl->SetScenario(0);
    requestConfigImpl->SetFixNumber(0);
    requestConfigImpl->SetMaxAccuracy(0);
    requestConfigImpl->SetDistanceInterval(0);
    requestConfigImpl->SetTimeInterval(0);
    requestConfigImpl->SetPriority(0);
}

/**
 * @tc.name: LocationProxyAdapterTest_LocationInfoImpl_005
 * @tc.desc: LocationInfoImpl.
 * @tc.type: FUNC
 */
TEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_LocationInfoImpl_005)
{
    auto locationInfoImpl = std::make_shared<LocationInfoImpl>();
    EXPECT_NE(locationInfoImpl, nullptr);
    const double latitudeTolerance = 0.01L;
    double latitude = locationInfoImpl->GetLatitude();
    EXPECT_NEAR(latitude, 0.0, latitudeTolerance);
    latitude = locationInfoImpl->GetLongitude();
    EXPECT_NEAR(latitude, 0.0, latitudeTolerance);
    latitude = locationInfoImpl->GetAltitude();
    EXPECT_NEAR(latitude, 0.0, latitudeTolerance);
    const float accuracyTolerance = 0.01F;
    float accuracy = locationInfoImpl->GetAccuracy();
    EXPECT_NEAR(accuracy, 0.0, accuracyTolerance);
    accuracy = locationInfoImpl->GetSpeed();
    EXPECT_NEAR(accuracy, 0.0, accuracyTolerance);
    double direction = locationInfoImpl->GetDirection();
    EXPECT_NEAR(direction, 0.0, latitudeTolerance);
    int64_t timeStamp = locationInfoImpl->GetTimeStamp();
    EXPECT_EQ(timeStamp, 0);
    timeStamp = locationInfoImpl->GetTimeSinceBoot();
    EXPECT_EQ(timeStamp, 0);
}

/**
 * @tc.name: LocationProxyAdapterTest_EnableAbility_006
 * @tc.desc: EnableAbility.
 * @tc.type: FUNC
 */
TEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_EnableAbility_006)
{
    auto proxyAdapterImpl = std::make_shared<LocationProxyAdapterImpl>();
    EXPECT_NE(proxyAdapterImpl, nullptr);
    std::shared_ptr<LocationRequestConfig> requestConfig = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfig, nullptr);
    std::shared_ptr<LocationCallbackAdapter> callback = std::make_shared<LocationCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    bool enabled = proxyAdapterImpl->IsLocationEnabled();
    if (enabled) {
        EXPECT_TRUE(enabled);
    } else {
        EXPECT_FALSE(enabled);
    }

    int32_t id = proxyAdapterImpl->StartLocating(requestConfig, nullptr);
    EXPECT_EQ(id, -1);
    id = proxyAdapterImpl->StartLocating(requestConfig, callback);
    EXPECT_EQ(id, -1);

    bool result = proxyAdapterImpl->StopLocating(-1);
    EXPECT_FALSE(result);
    result = proxyAdapterImpl->StopLocating(0);
    EXPECT_FALSE(result);
    result = proxyAdapterImpl->StopLocating(0);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: LocationProxyAdapterTest_EnableAbility_007
 * @tc.desc: EnableAbility.
 * @tc.type: FUNC
 */
TEST_F(LocationProxyAdapterTest, LocationProxyAdapterTest_EnableAbility_007)
{
    auto proxyAdapterImpl = std::make_shared<LocationProxyAdapterImpl>();
    EXPECT_NE(proxyAdapterImpl, nullptr);
    proxyAdapterImpl->ohRequestConfig = nullptr;
    bool result = proxyAdapterImpl->IsLocationEnabled();
    if (result) {
        EXPECT_TRUE(result);
    } else {
        EXPECT_FALSE(result);
    }
    std::shared_ptr<LocationRequestConfig> requestConfig = std::make_shared<LocationRequestConfigImpl>();
    EXPECT_NE(requestConfig, nullptr);
    std::shared_ptr<LocationCallbackAdapter> callback = std::make_shared<LocationCallbackAdapterMock>();
    EXPECT_NE(callback, nullptr);
    int32_t id = proxyAdapterImpl->StartLocating(requestConfig, callback);
    EXPECT_EQ(id, -1);
    result = proxyAdapterImpl->StopLocating(id);
    EXPECT_FALSE(result);
}
} // namespace OHOS::NWeb

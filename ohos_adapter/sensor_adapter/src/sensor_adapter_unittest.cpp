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

#include <testing/gmock/include/gmock/gmock.h>
#include <testing/gtest/include/gtest/gtest.h>

#define private public
#include "sensor_adapter.h"
#include "sensor_adapter_impl.h"

using namespace testing;

namespace OHOS::NWeb {
namespace {
std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> g_sensoCallback;
std::shared_ptr<OHOS::NWeb::SensorAdapterImpl> g_sensorAdapter;
}
constexpr double NANOSECONDS_IN_SECOND = 1000000000.0;
constexpr double DEFAULT_SAMPLE_PERIOD = 200000000.0;
constexpr int32_t SENSOR_TYPE_ID_NOT_EXSIT = 1;
constexpr int32_t SENSOR_TYPE_ID_EXSIT = 2;

class SensorCallbackImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class SensorAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class SensorCallbackAdapterMock : public SensorCallbackAdapter {
public:
    ~SensorCallbackAdapterMock() = default;
    void UpdateOhosSensorData(double timestamp,
        double value1, double value2, double value3, double value4) {}
};

void SensorCallbackImplTest::SetUpTestCase(void) {}

void SensorCallbackImplTest::TearDownTestCase(void) {}

void SensorCallbackImplTest::SetUp(void)
{
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    g_sensoCallback = std::make_shared<SensorCallbackImpl>(callbackAdapter);
    ASSERT_NE(g_sensoCallback, nullptr);
}

void SensorCallbackImplTest::TearDown(void)
{
    g_sensoCallback = nullptr;
}

void SensorAdapterImplTest::SetUpTestCase(void) {}

void SensorAdapterImplTest::TearDownTestCase(void) {}

void SensorAdapterImplTest::SetUp(void)
{
    g_sensorAdapter = std::make_shared<SensorAdapterImpl>();
    ASSERT_NE(g_sensorAdapter, nullptr);
}

void SensorAdapterImplTest::TearDown(void)
{
    g_sensorAdapter = nullptr;
}

/**
 * @tc.name: SensorAdapterImplTest_SensorCallbackImpl_001.
 * @tc.desc: test of SensorCallbackImpl :: UpdateOhosSensorData()
 * @tc.type: FUNC.
 */
TEST_F(SensorCallbackImplTest, SensorAdapterImplTest_SensorCallbackImplTest_001)
{
    double temp = 1.1;
    g_sensoCallback->callbackAdapter_ = nullptr;
    g_sensoCallback->UpdateOhosSensorData(temp, temp, temp, temp, temp);
    EXPECT_EQ(g_sensoCallback->callbackAdapter_, nullptr);

    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    g_sensoCallback->callbackAdapter_ = callbackAdapter;
    g_sensoCallback->UpdateOhosSensorData(temp, temp, temp, temp, temp);
    EXPECT_NE(g_sensoCallback->callbackAdapter_, nullptr);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_001.
 * @tc.desc: test of SensorAdapterImpl :: IsOhosSensorSupported()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_001)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    auto number = g_sensorAdapter->IsOhosSensorSupported(sensorTypeId);
    EXPECT_EQ(number, SENSOR_ERROR);

    sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    number = g_sensorAdapter->IsOhosSensorSupported(sensorTypeId);
    EXPECT_EQ(number, SENSOR_SUCCESS);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_002.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorReportingMode()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_002)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    int32_t temp1 = SENSOR_DATA_REPORT_CONTINUOUS;
    auto mode = g_sensorAdapter->GetOhosSensorReportingMode(sensorTypeId);
    EXPECT_EQ(mode, temp1);

    sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    temp1 = -1;
    mode = g_sensorAdapter->GetOhosSensorReportingMode(sensorTypeId);
    EXPECT_EQ(mode, temp1);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_003.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorDefaultSupportedFrequency()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_003)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    double temp1 = 0.0;
    auto mode = g_sensorAdapter->GetOhosSensorDefaultSupportedFrequency(sensorTypeId);
    EXPECT_EQ(mode, temp1);

    sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    temp1 = NANOSECONDS_IN_SECOND / DEFAULT_SAMPLE_PERIOD;
    mode = g_sensorAdapter->GetOhosSensorDefaultSupportedFrequency(sensorTypeId);
    EXPECT_EQ(mode, temp1);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_004.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorMinSupportedFrequency()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_004)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    double temp1 = 0.0;
    auto mode = g_sensorAdapter->GetOhosSensorMinSupportedFrequency(sensorTypeId);
    EXPECT_EQ(mode, temp1);

    sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    mode = g_sensorAdapter->GetOhosSensorMinSupportedFrequency(sensorTypeId);
    EXPECT_NE(mode, temp1);    
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_005.
 * @tc.desc: test of SensorAdapterImpl :: GetOhosSensorMaxSupportedFrequency()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_005)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    double temp1 = 0.0;
    auto mode = g_sensorAdapter->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
    EXPECT_EQ(mode, temp1);

    sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    mode = g_sensorAdapter->GetOhosSensorMaxSupportedFrequency(sensorTypeId);
    EXPECT_NE(mode, temp1);    
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_006.
 * @tc.desc: test of SensorAdapterImpl :: SubscribeOhosSensor()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_006)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    int64_t samplingInterval = 0;
    auto status = g_sensorAdapter->SubscribeOhosSensor(sensorTypeId, samplingInterval);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    samplingInterval = 1;
    status = g_sensorAdapter->SubscribeOhosSensor(sensorTypeId, samplingInterval);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    status = g_sensorAdapter->SubscribeOhosSensor(sensorTypeId, samplingInterval);
    EXPECT_EQ(status, SENSOR_SUCCESS);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_007.
 * @tc.desc: test of SensorAdapterImpl :: RegistOhosSensorCallback()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_007)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    auto callbackAdapter = std::make_shared<SensorCallbackAdapterMock>();
    auto status = g_sensorAdapter->RegistOhosSensorCallback(sensorTypeId, callbackAdapter);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    status = g_sensorAdapter->RegistOhosSensorCallback(sensorTypeId, callbackAdapter);
    EXPECT_EQ(status, SENSOR_SUCCESS);
}

/**
 * @tc.name: SensorAdapterImplTest_SensorAdapterImpl_008.
 * @tc.desc: test of SensorAdapterImpl :: UnsubscribeOhosSensor()
 * @tc.type: FUNC.
 */
TEST_F(SensorAdapterImplTest, SensorAdapterImplTest_SensorAdapterImpl_008)
{
    int32_t sensorTypeId = SENSOR_TYPE_ID_NOT_EXSIT;
    auto status = g_sensorAdapter->UnsubscribeOhosSensor(sensorTypeId);
    EXPECT_EQ(status, SENSOR_PARAMETER_ERROR);

    sensorTypeId = SENSOR_TYPE_ID_EXSIT;
    status = g_sensorAdapter->UnsubscribeOhosSensor(sensorTypeId);
    EXPECT_EQ(status, SENSOR_SUCCESS);
}
} // namespace OHOS::NWeb
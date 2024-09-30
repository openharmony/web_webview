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

#ifndef SENSOR_ADAPTER_IMPL_H
#define SENSOR_ADAPTER_IMPL_H

#include "sensor_adapter.h"
#include "sensor_agent.h"

#include <mutex>

namespace OHOS::NWeb {

class SensorCallbackImpl {
public:
    SensorCallbackImpl(std::shared_ptr<SensorCallbackAdapter> callbackAdapter);
    ~SensorCallbackImpl() = default;

    void UpdateOhosSensorData(double timestamp, 
        double value1, double value2, double value3, double value4);
private:
    std::shared_ptr<SensorCallbackAdapter> callbackAdapter_;
};

class SensorAdapterImpl : public SensorAdapter {
public:
    SensorAdapterImpl() = default;
    ~SensorAdapterImpl() = default;

    int32_t IsOhosSensorSupported(int32_t sensorTypeId) override;
    int32_t GetOhosSensorReportingMode(int32_t sensorTypeId) override;
    double GetOhosSensorDefaultSupportedFrequency(int32_t sensorTypeId) override;
    double GetOhosSensorMinSupportedFrequency(int32_t sensorTypeId) override;
    double GetOhosSensorMaxSupportedFrequency(int32_t sensorTypeId) override;
    int32_t SubscribeOhosSensor(int32_t sensorTypeId, int64_t samplingInterval) override;
    int32_t RegistOhosSensorCallback(int32_t sensorTypeId,
        std::shared_ptr<SensorCallbackAdapter> callbackAdapter) override;
    int32_t UnsubscribeOhosSensor(int32_t sensorTypeId) override;

private:
    static void OhosSensorCallback(SensorEvent* event);
    static std::unordered_map<int32_t, std::shared_ptr<SensorCallbackImpl>> sensorCallbackMap;
    static std::mutex sensorCallbackMapMutex_;

    static void handleAccelerometerData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);
    static void handleLinearAccelerometerData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);
    static void handleGravityData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);
    static void handleCyroscopeData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);
    static void handleMagnetometerData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);
    static void handleOrientationData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);
    static void handleRotationVectorData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);
    static void handleGameRotationVectorData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
        SensorEvent* event);

    SensorUser mSensorUser{};
};

}
#endif // SENSOR_ADAPTER_IMPL_H
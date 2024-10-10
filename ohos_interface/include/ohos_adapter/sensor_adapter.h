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

#ifndef SENSOR_ADAPTER_H
#define SENSOR_ADAPTER_H

#include <string>


namespace OHOS::NWeb {

enum {
    SENSOR_ERROR = -1,
    SENSOR_SUCCESS = 0,
    SENSOR_PERMISSION_DENIED = 201, // Use this error code when permission is denied.
    SENSOR_PARAMETER_ERROR = 401, // Use this error code when the input parameter type or range does not match.
    SENSOR_SERVICE_EXCEPTION = 14500101, // Use this error code when the service is exception.
    SENSOR_NO_SUPPORT = 14500102, // Use this error code when the sensor is not supported by the device.
    SENSOR_NON_SYSTEM_API = 202 // Permission check failed. A non-system application uses the system API.
};

enum {
    SENSOR_DATA_REPORT_ON_CHANGE = 0,
    SENSOR_DATA_REPORT_CONTINUOUS = 1
};

class SensorCallbackAdapter {
public:
    virtual ~SensorCallbackAdapter() = default;

    virtual void UpdateOhosSensorData(double timestamp,
        double value1, double value2, double value3, double value4) = 0;
};

class SensorAdapter {
public:
    SensorAdapter() = default;
    virtual ~SensorAdapter() = default;

    virtual int32_t IsOhosSensorSupported(int32_t sensorTypeId) = 0;
    virtual int32_t GetOhosSensorReportingMode(int32_t sensorTypeId) = 0;
    virtual double GetOhosSensorDefaultSupportedFrequency(int32_t sensorTypeId) = 0;
    virtual double GetOhosSensorMinSupportedFrequency(int32_t sensorTypeId) = 0;
    virtual double GetOhosSensorMaxSupportedFrequency(int32_t sensorTypeId) = 0;
    virtual int32_t SubscribeOhosSensor(int32_t sensorTypeId, int64_t samplingInterval) = 0;
    virtual int32_t RegistOhosSensorCallback(int32_t sensorTypeId,
        std::shared_ptr<SensorCallbackAdapter> callbackAdapter) = 0;
    virtual int32_t UnsubscribeOhosSensor(int32_t sensorTypeId) = 0;
};

}

#endif // SENSOR_ADAPTER_H
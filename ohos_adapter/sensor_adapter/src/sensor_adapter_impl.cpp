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

#include "sensor_adapter_impl.h"

#include <securec.h>
#include <map>

#include "nweb_log.h"

namespace OHOS::NWeb {

std::unordered_map<int32_t, std::shared_ptr<SensorCallbackImpl>> SensorAdapterImpl::sensorCallbackMap;
std::mutex SensorAdapterImpl::sensorCallbackMapMutex_;
constexpr double NANOSECONDS_IN_SECOND = 1000000000.0;
constexpr double DEFAULT_SAMPLE_PERIOD = 200000000.0;

SensorTypeId SensorTypeToOhosSensorType(int sensorTypeId)
{
    SensorTypeId ohosSensorTypeId = SENSOR_TYPE_ID_NONE;
    const static std::map<int32_t, SensorTypeId> TO_OHOS_SENSOR_TYPE_MAP = {
        {2  /* ACCELEROMETER                     */, SENSOR_TYPE_ID_ACCELEROMETER },
        {3  /* LINEAR_ACCELERATION               */, SENSOR_TYPE_ID_LINEAR_ACCELERATION },
        {4  /* GRAVITY                           */, SENSOR_TYPE_ID_GRAVITY },
        {5  /* GYROSCOPE                         */, SENSOR_TYPE_ID_GYROSCOPE },
        {6  /* MAGNETOMETER                      */, SENSOR_TYPE_ID_MAGNETIC_FIELD },
        {8  /* ABSOLUTE_ORIENTATION_EULER_ANGLES}*/, SENSOR_TYPE_ID_ORIENTATION },
        {9  /* ABSOLUTE_ORIENTATION_QUATERNION}  */, SENSOR_TYPE_ID_ROTATION_VECTOR },
        {11 /* RELATIVE_ORIENTATION_QUATERNION}  */, SENSOR_TYPE_ID_GAME_ROTATION_VECTOR }
    };
    auto checkIter = TO_OHOS_SENSOR_TYPE_MAP.find(sensorTypeId);
    if (checkIter != TO_OHOS_SENSOR_TYPE_MAP.end()) {
        ohosSensorTypeId = checkIter->second;
    }
    return ohosSensorTypeId;
}

std::string SensorTypeToSensorUserName(int sensorTypeId)
{
    const static std::map<int32_t, std::string> TO_OHOS_SENSOR_USER_NAME_MAP = {
        {2  /* ACCELEROMETER                     */, "OhosAccelerometerService" },
        {3  /* LINEAR_ACCELERATION               */, "OhosLinearAccelerometerService" },
        {4  /* GRAVITY                           */, "OhosGravityService" },
        {5  /* GYROSCOPE                         */, "OhosCyroscopeService" },
        {6  /* MAGNETOMETER                      */, "OhosMagnetometerService" },
        {8  /* ABSOLUTE_ORIENTATION_EULER_ANGLES}*/, "OhosOrientationService" },
        {9  /* ABSOLUTE_ORIENTATION_QUATERNION}  */, "OhosRotationVectorService"},
        {11 /* RELATIVE_ORIENTATION_QUATERNION}  */, "OhosGameRotationVectorService" }
    };
    std::string userName = "OhosSensorService";
    auto checkIter = TO_OHOS_SENSOR_USER_NAME_MAP.find(sensorTypeId);
    if (checkIter != TO_OHOS_SENSOR_USER_NAME_MAP.end()) {
        userName = checkIter->second;
    }
    return userName;
}

SensorCallbackImpl::SensorCallbackImpl(
    std::shared_ptr<SensorCallbackAdapter> callbackAdapter)
    : callbackAdapter_(callbackAdapter)
{}

void SensorCallbackImpl::UpdateOhosSensorData(double timestamp,
    double value1, double value2, double value3, double value4)
{
    if (callbackAdapter_) {
        callbackAdapter_->UpdateOhosSensorData(timestamp, value1, value2, value3, value4);
    }
}

int32_t SensorAdapterImpl::IsOhosSensorSupported(int32_t sensorTypeId)
{
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    if (ohosSensorTypeId != SENSOR_TYPE_ID_NONE) {
        SensorInfo* sensorInfo = nullptr;
        int32_t count;
        int ret = GetAllSensors(&sensorInfo, &count);
        if (ret != SENSOR_SUCCESS || sensorInfo == nullptr || count < 0) {
            WVLOG_E("IsOhosSensorSupported Error, ret = %{public}d, count = %{public}d.", ret, count);
            return SENSOR_ERROR;
        }

        for (int i = 0; i < count; i++) {
            if (sensorInfo[i].sensorId == ohosSensorTypeId) {
                WVLOG_I("IsOhosSensorSupported SUCCESS, sensorTypeId = %{public}d.", sensorTypeId);
                return SENSOR_SUCCESS;
            }
        }
    }
    WVLOG_E("IsOhosSensorSupported Error, sensorTypeId = %{public}d is invalid.", sensorTypeId);
    return SENSOR_ERROR;
}

int32_t SensorAdapterImpl::GetOhosSensorReportingMode(int32_t sensorTypeId)
{
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    int32_t reportingMode = -1;
    switch (ohosSensorTypeId) {
        case SENSOR_TYPE_ID_ACCELEROMETER:
        case SENSOR_TYPE_ID_GRAVITY:
        case SENSOR_TYPE_ID_LINEAR_ACCELERATION:
        case SENSOR_TYPE_ID_GYROSCOPE:
        case SENSOR_TYPE_ID_MAGNETIC_FIELD:
        case SENSOR_TYPE_ID_ORIENTATION:
        case SENSOR_TYPE_ID_ROTATION_VECTOR:
        case SENSOR_TYPE_ID_GAME_ROTATION_VECTOR:
            reportingMode = SENSOR_DATA_REPORT_CONTINUOUS;
            break;
        default:
            break;
    }
    return reportingMode;
}

double SensorAdapterImpl::GetOhosSensorDefaultSupportedFrequency(int32_t sensorTypeId)
{
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    double defaultFrequency = 0.0;
    if (ohosSensorTypeId != SENSOR_TYPE_ID_NONE) {
        defaultFrequency = NANOSECONDS_IN_SECOND / DEFAULT_SAMPLE_PERIOD;
    }
    WVLOG_I("GetOhosSensorDefaultSupportedFrequency sensorTypeId: %{public}d, defaultFrequency: %{public}f",
        sensorTypeId, defaultFrequency);
    return defaultFrequency;
}

double SensorAdapterImpl::GetOhosSensorMinSupportedFrequency(int32_t sensorTypeId)
{
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    double minFrequency = 0.0;
    if (ohosSensorTypeId == SENSOR_TYPE_ID_NONE) {
        WVLOG_E("GetOhosSensorMinSupportedFrequency Error, sensorTypeId = %{public}d is invalid.", sensorTypeId);
        return minFrequency;
    }
    SensorInfo* sensorInfo = nullptr;
    int32_t count;
    int ret = GetAllSensors(&sensorInfo, &count);
    if (ret != SENSOR_SUCCESS || sensorInfo == nullptr || count < 0) {
        WVLOG_E("GetOhosSensorMinSupportedFrequency Error, ret = %{public}d, count = %{public}d.", ret, count);
        return minFrequency;
    }
    for (int i = 0; i < count; i++) {
        if (sensorInfo[i].sensorId == ohosSensorTypeId) {
            int64_t maxSamplePeriod = sensorInfo[i].maxSamplePeriod;
            if (maxSamplePeriod > 0) {
                minFrequency = NANOSECONDS_IN_SECOND / static_cast<double>(maxSamplePeriod);
            }
            break;
        }
    }
    WVLOG_I("GetOhosSensorMinSupportedFrequency sensorTypeId: %{public}d, minFrequency: %{public}f",
        sensorTypeId, minFrequency);
    return minFrequency;
}

double SensorAdapterImpl::GetOhosSensorMaxSupportedFrequency(int32_t sensorTypeId)
{
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    double maxFrequency = 0.0;
    if (ohosSensorTypeId == SENSOR_TYPE_ID_NONE) {
        WVLOG_E("GetOhosSensorMaxSupportedFrequency Error, sensorTypeId = %{public}d is invalid.", sensorTypeId);
        return maxFrequency;
    }
    SensorInfo* sensorInfo = nullptr;
    int32_t count;
    int ret = GetAllSensors(&sensorInfo, &count);
    if (ret != SENSOR_SUCCESS || sensorInfo == nullptr || count < 0) {
        WVLOG_E("GetOhosSensorMaxSupportedFrequency Error, ret = %{public}d, count = %{public}d.", ret, count);
        return maxFrequency;
    }
    for (int i = 0; i < count; i++) {
        if (sensorInfo[i].sensorId == ohosSensorTypeId) {
            int64_t minSamplePeriod = sensorInfo[i].minSamplePeriod;
            if (minSamplePeriod > 0) {
                maxFrequency = NANOSECONDS_IN_SECOND / static_cast<double>(minSamplePeriod);
            }
            break;
        }
    }
    WVLOG_I("GetOhosSensorMaxSupportedFrequency sensorTypeId: %{public}d, maxFrequency: %{public}f",
        sensorTypeId, maxFrequency);
    return maxFrequency;
}

void SensorAdapterImpl::handleAccelerometerData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleAccelerometerData Error.");
        return;
    }
    AccelData* data = reinterpret_cast<AccelData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->x, data->y, data->z, 0.0f);
    }
}

void SensorAdapterImpl::handleLinearAccelerometerData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleLinearAccelerometerData Error.");
        return;
    }
    LinearAccelData* data = reinterpret_cast<LinearAccelData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->x, data->y, data->z, 0.0f);
    }
}

void SensorAdapterImpl::handleGravityData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleGravityData Error.");
        return;
    }
    GravityData* data = reinterpret_cast<GravityData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->x, data->y, data->z, 0.0f);
    }
}

void SensorAdapterImpl::handleCyroscopeData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleCyroscopeData Error.");
        return;
    }
    GyroscopeData* data = reinterpret_cast<GyroscopeData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->x, data->y, data->z, 0.0f);
    }
}

void SensorAdapterImpl::handleMagnetometerData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleMagnetometerData Error.");
        return;
    }
    MagneticFieldData* data = reinterpret_cast<MagneticFieldData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->x, data->y, data->z, 0.0f);
    }
}

void SensorAdapterImpl::handleOrientationData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleOrientationData Error.");
        return;
    }
    OrientationData* data = reinterpret_cast<OrientationData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->beta, data->gamma, data->alpha, 0.0f);
    }
}

void SensorAdapterImpl::handleRotationVectorData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleRotationVectorData Error.");
        return;
    }
    RotationVectorData* data = reinterpret_cast<RotationVectorData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->x, data->y, data->z, data->w);
    }
}

void SensorAdapterImpl::handleGameRotationVectorData(std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback,
    SensorEvent* event)
{
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("handleGameRotationVectorData Error.");
        return;
    }
    GameRotationVectorData* data = reinterpret_cast<GameRotationVectorData*>(event->data);
    if (data != nullptr) {
        callback->UpdateOhosSensorData(event->timestamp, data->x, data->y, data->z, data->w);
    }
}

void SensorAdapterImpl::OhosSensorCallback(SensorEvent* event)
{
    if (event == nullptr) {
        WVLOG_E("SensorEvent Error.");
        return;
    }
    std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(sensorCallbackMapMutex_);
        auto findIter = sensorCallbackMap.find(event->sensorTypeId);
        if (findIter != sensorCallbackMap.end()) {
            callback = findIter->second;
        }
    }
    if (callback == nullptr) {
        WVLOG_E("OhosSensorCallback Error.");
        return;
    }
    switch (event->sensorTypeId) {
        case SENSOR_TYPE_ID_ACCELEROMETER:
            handleAccelerometerData(callback, event);
            break;

        case SENSOR_TYPE_ID_GRAVITY:
            handleGravityData(callback, event);
            break;

        case SENSOR_TYPE_ID_LINEAR_ACCELERATION:
            handleLinearAccelerometerData(callback, event);
            break;

        case SENSOR_TYPE_ID_GYROSCOPE:
            handleCyroscopeData(callback, event);
            break;

        case SENSOR_TYPE_ID_MAGNETIC_FIELD:
            handleMagnetometerData(callback, event);
            break;
        case SENSOR_TYPE_ID_ORIENTATION:
            handleOrientationData(callback, event);
            break;
        case SENSOR_TYPE_ID_ROTATION_VECTOR:
            handleRotationVectorData(callback, event);
            break;
        case SENSOR_TYPE_ID_GAME_ROTATION_VECTOR:
            handleGameRotationVectorData(callback, event);
            break;

        default:
            break;
    }
}

int32_t SensorAdapterImpl::SubscribeOhosSensor(int32_t sensorTypeId, int64_t samplingInterval)
{
    WVLOG_I("SubscribeOhosSensor sensorTypeId: %{public}d, samplingInterval: %{public}ld",
        sensorTypeId, samplingInterval);
    WVLOG_I("SubscribeOhosSensor sensorTypeId: %{public}d", sensorTypeId);
    if (samplingInterval <= 0) {
        WVLOG_E("SubscribeOhosSensor error, samplingInterval is invalid.");
        return SENSOR_PARAMETER_ERROR;
    }
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    if (ohosSensorTypeId == SENSOR_TYPE_ID_NONE) {
        WVLOG_E("SubscribeOhosSensor error, sensorTypeId is invalid.");
        return SENSOR_PARAMETER_ERROR;
    }

    std::string userName = SensorTypeToSensorUserName(sensorTypeId);
    int cpyret = strcpy_s(mSensorUser.name, sizeof(mSensorUser.name), userName.c_str());
    if (cpyret != 0) {
        WVLOG_E("SubscribeOhosSensor error, call strcpy_s ret = %{public}d.", cpyret);
    }
    mSensorUser.userData = nullptr;
    mSensorUser.callback = &OhosSensorCallback;
    int32_t ret = SENSOR_SUCCESS;
    ret = SubscribeSensor(ohosSensorTypeId, &mSensorUser);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call SubscribeSensor ret = %{public}d.", ret);
        return ret;
    }
    ret = SetBatch(ohosSensorTypeId, &mSensorUser, samplingInterval, samplingInterval);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call SetBatch ret = %{public}d.", ret);
        return ret;
    }
    ret = ActivateSensor(ohosSensorTypeId, &mSensorUser);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call ActivateSensor ret = %{public}d.", ret);
        return ret;
    }
    ret = SetMode(ohosSensorTypeId, &mSensorUser, SENSOR_REALTIME_MODE);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call SetMode ret = %{public}d.", ret);
        return ret;
    }
    return SENSOR_SUCCESS;
}

int32_t SensorAdapterImpl::RegistOhosSensorCallback(int32_t sensorTypeId,
    std::shared_ptr<SensorCallbackAdapter> callbackAdapter)
{
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    if (ohosSensorTypeId != SENSOR_TYPE_ID_NONE) {
        auto callback = std::make_shared<SensorCallbackImpl>(callbackAdapter);
        std::lock_guard<std::mutex> lock(sensorCallbackMapMutex_);
        sensorCallbackMap[ohosSensorTypeId] = callback;
        return SENSOR_SUCCESS;
    }
    WVLOG_E("RegistOhosSensorCallback error, sensorTypeId is invalid.");
    return SENSOR_PARAMETER_ERROR;
}

int32_t SensorAdapterImpl::UnsubscribeOhosSensor(int32_t sensorTypeId)
{
    WVLOG_I("UnsubscribeOhosSensor sensorTypeId: %{public}d.", sensorTypeId);
    int32_t ohosSensorTypeId = SensorTypeToOhosSensorType(sensorTypeId);
    if (ohosSensorTypeId != SENSOR_TYPE_ID_NONE) {
        int32_t ret = DeactivateSensor(ohosSensorTypeId, &mSensorUser);
        if (ret != SENSOR_SUCCESS) {
            WVLOG_E("UnsubscribeOhosSensor error, call DeactivateSensor ret = %{public}d.", ret);
            return ret;
        }
        ret = UnsubscribeSensor(ohosSensorTypeId, &mSensorUser);
        if (ret != SENSOR_SUCCESS) {
            WVLOG_E("UnsubscribeOhosSensor error, call UnsubscribeSensor ret = %{public}d.", ret);
            return ret;
        }
        std::lock_guard<std::mutex> lock(sensorCallbackMapMutex_);
        sensorCallbackMap.erase(ohosSensorTypeId);
        return SENSOR_SUCCESS;
    }
    WVLOG_E("UnsubscribeOhosSensor error, sensorTypeId is invalid.");
    return SENSOR_PARAMETER_ERROR;
}
} // namespace OHOS::NWeb
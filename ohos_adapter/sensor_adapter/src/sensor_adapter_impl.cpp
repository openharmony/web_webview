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
#include <map>
#include <mutex>

#include "nweb_log.h"

namespace OHOS::NWeb {

typedef enum Event_DataLen {
    DATA_LEN_3 = 3,
    DATA_LEN_4 = 4,
} Event_DataLen;

typedef enum Data_Index {
    IDX_0,
    IDX_1,
    IDX_2,
    IDX_3,
} Data_Index;

typedef struct SensorSubscriber {
    Sensor_Subscriber *subscriber;
    Sensor_SubscriptionId *id;
    Sensor_SubscriptionAttribute *attr;
} SensorSubscriber;
std::unordered_map<Sensor_Type, std::shared_ptr<SensorSubscriber>> sensorSubscriberMap;
std::unordered_map<Sensor_Type, std::shared_ptr<SensorCallbackImpl>> SensorAdapterImpl::sensorCallbackMap;
std::mutex callbackMutex;
constexpr double NANOSECONDS_IN_SECOND = 1000000000.0;
constexpr double DEFAULT_SAMPLE_PERIOD = 200000000.0;

int32_t SensorTypeToOhosSensorType(int sensorTypeId, Sensor_Type *ohosSensorTypeId)
{
    const static std::map<int32_t, Sensor_Type> TO_OHOS_SENSOR_TYPE_MAP = {
        {2  /* ACCELEROMETER                     */, SENSOR_TYPE_ACCELEROMETER },
        {3  /* LINEAR_ACCELERATION               */, SENSOR_TYPE_LINEAR_ACCELERATION },
        {4  /* GRAVITY                           */, SENSOR_TYPE_GRAVITY },
        {5  /* GYROSCOPE                         */, SENSOR_TYPE_GYROSCOPE },
        {6  /* MAGNETOMETER                      */, SENSOR_TYPE_MAGNETIC_FIELD },
        {8  /* ABSOLUTE_ORIENTATION_EULER_ANGLES}*/, SENSOR_TYPE_ORIENTATION },
        {9  /* ABSOLUTE_ORIENTATION_QUATERNION}  */, SENSOR_TYPE_ROTATION_VECTOR },
        {11 /* RELATIVE_ORIENTATION_QUATERNION}  */, SENSOR_TYPE_GAME_ROTATION_VECTOR }
    };
    auto checkIter = TO_OHOS_SENSOR_TYPE_MAP.find(sensorTypeId);
    if (checkIter != TO_OHOS_SENSOR_TYPE_MAP.end()) {
        *ohosSensorTypeId = checkIter->second;
        return SENSOR_SUCCESS;
    }
    WVLOG_E("Sensor type '%{public}d' is not supported", sensorTypeId);
    return SENSOR_NO_SUPPORT;
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
    Sensor_Type ohosSensorTypeId;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret == SENSOR_SUCCESS) {
        uint32_t count = 0;
        ret = OH_Sensor_GetInfos(nullptr, &count);
        if (ret != SENSOR_SUCCESS) {
            WVLOG_E("IsOhosSensorSupported Error, ret = %{public}d, count = %{public}d.", ret, count);
            return SENSOR_ERROR;
        }
        Sensor_Info **sensorInfo = OH_Sensor_CreateInfos(count);
        ret = OH_Sensor_GetInfos(sensorInfo, &count);
        if (ret != SENSOR_SUCCESS || sensorInfo == nullptr || count < 0) {
            OH_Sensor_DestroyInfos(sensorInfo, count);
            WVLOG_E("IsOhosSensorSupported Error, ret = %{public}d, count = %{public}d.", ret, count);
            return SENSOR_ERROR;
        }
        for (uint32_t i = 0; i < count; i++) {
            Sensor_Type type;
            ret = OH_SensorInfo_GetType(sensorInfo[i], &type);
            if (ret != SENSOR_SUCCESS) {
                OH_Sensor_DestroyInfos(sensorInfo, count);
                WVLOG_E("IsOhosSensorSupported Error, GetType ret = %{public}d", ret);
                return SENSOR_ERROR;
            }
            if (type == ohosSensorTypeId) {
                OH_Sensor_DestroyInfos(sensorInfo, count);
                WVLOG_I("IsOhosSensorSupported SUCCESS, sensorTypeId = %{public}d.", sensorTypeId);
                return SENSOR_SUCCESS;
            }
        }
        OH_Sensor_DestroyInfos(sensorInfo, count);
    }
    WVLOG_E("IsOhosSensorSupported Error, sensorTypeId = %{public}d is invalid.", sensorTypeId);
    return SENSOR_ERROR;
}

int32_t SensorAdapterImpl::GetOhosSensorReportingMode(int32_t sensorTypeId)
{
    Sensor_Type ohosSensorTypeId;
    int32_t reportingMode = -1;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret != SENSOR_SUCCESS) {
        return reportingMode;
    }
    switch (ohosSensorTypeId) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_GRAVITY:
        case SENSOR_TYPE_LINEAR_ACCELERATION:
        case SENSOR_TYPE_GYROSCOPE:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_ORIENTATION:
        case SENSOR_TYPE_ROTATION_VECTOR:
        case SENSOR_TYPE_GAME_ROTATION_VECTOR:
            reportingMode = SENSOR_DATA_REPORT_CONTINUOUS;
            break;
        default:
            break;
    }
    return reportingMode;
}

double SensorAdapterImpl::GetOhosSensorDefaultSupportedFrequency(int32_t sensorTypeId)
{
    Sensor_Type ohosSensorTypeId;
    double defaultFrequency = 0.0;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret == SENSOR_SUCCESS) {
        defaultFrequency = NANOSECONDS_IN_SECOND / DEFAULT_SAMPLE_PERIOD;
    }
    WVLOG_I("GetOhosSensorDefaultSupportedFrequency sensorTypeId: %{public}d, defaultFrequency: %{public}f",
                sensorTypeId, defaultFrequency);
    return defaultFrequency;
}

double SensorAdapterImpl::GetOhosSensorMinSupportedFrequency(int32_t sensorTypeId)
{
    Sensor_Type ohosSensorTypeId;
    double minFrequency = 0.0;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("GetOhosSensorMinSupportedFrequency Error, sensorTypeId = %{public}d is invalid.", sensorTypeId);
        return minFrequency;
    }
    uint32_t count;
    ret = OH_Sensor_GetInfos(nullptr, &count);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("GetOhosSensorMinSupportedFrequency Error, ret = %{public}d, count = %{public}d.", ret, count);
        return SENSOR_ERROR;
    }
    Sensor_Info **sensorInfo = OH_Sensor_CreateInfos(count);
    ret = OH_Sensor_GetInfos(sensorInfo, &count);
    if (ret != SENSOR_SUCCESS || sensorInfo == nullptr || count < 0) {
        OH_Sensor_DestroyInfos(sensorInfo, count);
        WVLOG_E("GetOhosSensorMinSupportedFrequency Error, ret = %{public}d, count = %{public}d.", ret, count);
        return minFrequency;
    }
    for (uint32_t i = 0; i < count; i++) {
        Sensor_Type type;
        ret = OH_SensorInfo_GetType(sensorInfo[i], &type);
        if (ret != SENSOR_SUCCESS) {
            OH_Sensor_DestroyInfos(sensorInfo, count);
            WVLOG_E("GetOhosSensorMinSupportedFrequency Error, GetType ret = %{public}d", ret);
            return minFrequency;
        }
        if (type == ohosSensorTypeId) {
            int64_t maxSamplePeriod;
            ret = OH_SensorInfo_GetMaxSamplingInterval(sensorInfo[i], &maxSamplePeriod);
            if (ret != SENSOR_SUCCESS) {
                OH_Sensor_DestroyInfos(sensorInfo, count);
                WVLOG_E("GetOhosSensorMinSupportedFrequency Error, GetMaxSamplePeriod ret = %{public}d", ret);
                return minFrequency;
            }
            if (maxSamplePeriod > 0) {
                minFrequency = NANOSECONDS_IN_SECOND / static_cast<double>(maxSamplePeriod);
            }
            break;
        }
    }
    OH_Sensor_DestroyInfos(sensorInfo, count);
    WVLOG_I("GetOhosSensorMinSupportedFrequency sensorTypeId: %{public}d, minFrequency: %{public}f",
                sensorTypeId, minFrequency);
    return minFrequency;
}

double SensorAdapterImpl::GetOhosSensorMaxSupportedFrequency(int32_t sensorTypeId)
{
    Sensor_Type ohosSensorTypeId;
    double maxFrequency = 0.0;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("GetOhosSensorMaxSupportedFrequency Error, sensorTypeId = %{public}d is invalid.", sensorTypeId);
        return maxFrequency;
    }
    uint32_t count;
    ret = OH_Sensor_GetInfos(nullptr, &count);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("GetOhosSensorMaxSupportedFrequency Error, ret = %{public}d, count = %{public}d.", ret, count);
        return SENSOR_ERROR;
    }
    Sensor_Info **sensorInfo = OH_Sensor_CreateInfos(count);
    ret = OH_Sensor_GetInfos(sensorInfo, &count);
    if (ret != SENSOR_SUCCESS || sensorInfo == nullptr || count < 0) {
        OH_Sensor_DestroyInfos(sensorInfo, count);
        WVLOG_E("GetOhosSensorMaxSupportedFrequency Error, ret = %{public}d, count = %{public}d.", ret, count);
        return maxFrequency;
    }
    for (uint32_t i = 0; i < count; i++) {
        Sensor_Type type;
        ret = OH_SensorInfo_GetType(sensorInfo[i], &type);
        if (ret != SENSOR_SUCCESS) {
            OH_Sensor_DestroyInfos(sensorInfo, count);
            WVLOG_E("GetOhosSensorMaxSupportedFrequency Error, GetType ret = %{public}d", ret);
            return maxFrequency;
        }
        if (type == ohosSensorTypeId) {
            int64_t minSamplePeriod;
            ret = OH_SensorInfo_GetMinSamplingInterval(sensorInfo[i], &minSamplePeriod);
            if (ret != SENSOR_SUCCESS) {
                OH_Sensor_DestroyInfos(sensorInfo, count);
                WVLOG_E("GetOhosSensorMaxSupportedFrequency Error, GetminSamplePeriod ret = %{public}d", ret);
                return maxFrequency;
            }
            if (minSamplePeriod > 0) {
                maxFrequency = NANOSECONDS_IN_SECOND / static_cast<double>(minSamplePeriod);
            }
            break;
        }
    }
    OH_Sensor_DestroyInfos(sensorInfo, count);
    WVLOG_I("GetOhosSensorMaxSupportedFrequency sensorTypeId: %{public}d, maxFrequency: %{public}f",
                sensorTypeId, maxFrequency);
    return maxFrequency;
}

void SensorAdapterImpl::OhosSensorCallback(Sensor_Event* event)
{
    std::shared_ptr<OHOS::NWeb::SensorCallbackImpl> callback = nullptr;
    Sensor_Type type;
    int32_t ret = OH_SensorEvent_GetType(event, &type);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("OhosSensorCallback error, GetType ret = %{public}d.", ret);
        return;
    }
    {
        std::lock_guard<std::mutex> lock(callbackMutex);
        auto findIter = sensorCallbackMap.find(type);
        if (findIter != sensorCallbackMap.end()) {
            callback = findIter->second;
        }
    }
    if ((event == nullptr) || (callback == nullptr)) {
        WVLOG_E("OhosSensorCallback Error.");
        return;
    }
    float *data;
    uint32_t length;
    ret = OH_SensorEvent_GetData(event, &data, &length);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("OhosSensorCallback Error, GetData ret=%{public}d.", ret);
        return;
    }
    int64_t timestamp;
    ret = OH_SensorEvent_GetTimestamp(event, &timestamp);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("OhosSensorCallback Error, GetTimestamp ret=%{public}d.", ret);
        return;
    }
    switch (length) {
        case DATA_LEN_3:
            callback->UpdateOhosSensorData(timestamp, data[IDX_0], data[IDX_1], data[IDX_2], 0.0f);
            break;
        case DATA_LEN_4:
            callback->UpdateOhosSensorData(timestamp, data[IDX_0], data[IDX_1], data[IDX_2], data[IDX_3]);
            break;
        default:
            WVLOG_W("OhosSensorCallback warning, Unknown Length=%{public}d.", length);
            break;
    }
}

int32_t SensorAdapterImpl::SubscribeOhosSensor(int32_t sensorTypeId, int64_t samplingInterval)
{
    WVLOG_I("SubscribeOhosSensor sensorTypeId: %{public}d", sensorTypeId);
    if (samplingInterval <= 0) {
        WVLOG_E("SubscribeOhosSensor error, samplingInterval is invalid.");
        return SENSOR_PARAMETER_ERROR;
    }
    Sensor_Type ohosSensorTypeId;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, sensorTypeId is invalid.");
        return SENSOR_PARAMETER_ERROR;
    }

    std::string userName = SensorTypeToSensorUserName(sensorTypeId);
    Sensor_Subscriber *subscriber = OH_Sensor_CreateSubscriber();
    ret = OH_SensorSubscriber_SetCallback(subscriber, OhosSensorCallback);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call SubscribeSensor ret = %{public}d.", ret);
        return ret;
    }
    Sensor_SubscriptionId *id = OH_Sensor_CreateSubscriptionId();
    ret = OH_SensorSubscriptionId_SetType(id, ohosSensorTypeId);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call Set type ret = %{public}d.", ret);
        return ret;
    }
    Sensor_SubscriptionAttribute *attr = OH_Sensor_CreateSubscriptionAttribute();
    ret = OH_SensorSubscriptionAttribute_SetSamplingInterval(attr, samplingInterval);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call set samplingInterval ret = %{public}d.", ret);
        return ret;
    }
    Sensor_Result oh_ret = OH_Sensor_Subscribe(id, attr, subscriber);
    if (ret != SENSOR_SUCCESS) {
        WVLOG_E("SubscribeOhosSensor error, call Subscribe ret = %{public}d.", oh_ret);
        return oh_ret;
    }
    auto sensorSubscriber = std::make_shared<SensorSubscriber>();
    sensorSubscriber->subscriber = subscriber;
    sensorSubscriber->id = id;
    sensorSubscriber->attr = attr;
    sensorSubscriberMap[ohosSensorTypeId] = sensorSubscriber;
    WVLOG_I("SubscribeOhosSensor sensorTypeId: %{public}d, sampingInterval:%{public}ld",
        sensorTypeId, samplingInterval);
    return SENSOR_SUCCESS;
}

int32_t SensorAdapterImpl::RegistOhosSensorCallback(int32_t sensorTypeId,
    std::shared_ptr<SensorCallbackAdapter> callbackAdapter)
{
    Sensor_Type ohosSensorTypeId;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret == SENSOR_SUCCESS) {
        auto callback = std::make_shared<SensorCallbackImpl>(callbackAdapter);
        {
            std::lock_guard<std::mutex> lock(callbackMutex);
            sensorCallbackMap[ohosSensorTypeId] = callback;
        }
        return SENSOR_SUCCESS;
    }
    WVLOG_E("RegistOhosSensorCallback error, sensorTypeId is invalid.");
    return SENSOR_PARAMETER_ERROR;
}

int32_t UnsubscribeOhosSensorInternal(Sensor_Type ohosSensorTypeId)
{
    auto findIter = sensorSubscriberMap.find(ohosSensorTypeId);
    if (findIter == sensorSubscriberMap.end()) {
        WVLOG_E("UnsubscribeOhosSensor error, map not contain type %{public}d.", ohosSensorTypeId);
        return SENSOR_PARAMETER_ERROR;
    }
    auto sensorSubscriber = findIter->second;
    Sensor_Result oh_ret = OH_Sensor_Unsubscribe(sensorSubscriber->id, sensorSubscriber->subscriber);
    if (oh_ret != SENSOR_SUCCESS) {
        WVLOG_E("UnsubscribeOhosSensor error, call unsubscribe ret = %{public}d.", oh_ret);
        return oh_ret;
    }
    OH_Sensor_DestroySubscriber(sensorSubscriber->subscriber);
    OH_Sensor_DestroySubscriptionId(sensorSubscriber->id);
    OH_Sensor_DestroySubscriptionAttribute(sensorSubscriber->attr);
    sensorSubscriberMap.erase(ohosSensorTypeId);
    return SENSOR_SUCCESS;
}

int32_t SensorAdapterImpl::UnsubscribeOhosSensor(int32_t sensorTypeId)
{
    WVLOG_I("UnsubscribeOhosSensor sensorTypeId: %{public}d.", sensorTypeId);
    Sensor_Type ohosSensorTypeId;
    int32_t ret = SensorTypeToOhosSensorType(sensorTypeId, &ohosSensorTypeId);
    if (ret == SENSOR_SUCCESS) {
        auto oh_ret = UnsubscribeOhosSensorInternal(ohosSensorTypeId);
        WVLOG_I("UnsubscribeOhosSensor sensorTypeId: %{public}d, result: %{public}d", sensorTypeId, oh_ret);
        {
            std::lock_guard<std::mutex> lock(callbackMutex);
            sensorCallbackMap.erase(ohosSensorTypeId);
        }
        return oh_ret;
    }
    WVLOG_E("UnsubscribeOhosSensor error, sensorTypeId is invalid.");
    return SENSOR_PARAMETER_ERROR;
}
} // namespace OHOS::NWeb
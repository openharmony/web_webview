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

#ifndef LOCATION_ADAPTER_H
#define LOCATION_ADAPTER_H

#include <cstdint>
#include <memory>
#include <sys/types.h>

namespace OHOS::NWeb {
class LocationRequestConfig {
public:
    enum Priority {
        PRIORITY_UNSET = 0,
        PRIORITY_ACCURACY,
        PRIORITY_LOW_POWER,
        PRIORITY_FAST_FIRST_FIX,
    };

    enum Scenario {
        UNSET = 0,
        NAVIGATION,
        TRAJECTORY_TRACKING,
        CAR_HAILING,
        DAILY_LIFE_SERVICE,
        NO_POWER,
    };

    LocationRequestConfig() = default;

    virtual ~LocationRequestConfig() = default;

    virtual void SetScenario(int32_t scenario) = 0;

    virtual void SetFixNumber(int32_t number) = 0;

    virtual void SetMaxAccuracy(int32_t maxAccuary) = 0;

    virtual void SetDistanceInterval(int32_t disInterval) = 0;

    virtual void SetTimeInterval(int32_t timeInterval) = 0;

    virtual void SetPriority(int32_t priority) = 0;
};

class LocationInfo {
public:
    LocationInfo() = default;

    virtual ~LocationInfo() = default;

    virtual double GetLatitude() const = 0;

    virtual double GetLongitude() const = 0;

    virtual double GetAltitude() const = 0;

    virtual float GetAccuracy() const = 0;

    virtual float GetSpeed() const = 0;

    virtual double GetDirection() const = 0;

    virtual int64_t GetTimeStamp() const = 0;

    virtual int64_t GetTimeSinceBoot() const = 0;

    virtual std::string GetAdditions() const = 0;
};

class LocationCallbackAdapter {
public:
    LocationCallbackAdapter() = default;

    virtual ~LocationCallbackAdapter() = default;

    virtual void OnLocationReport(
        const std::unique_ptr<LocationInfo>& location) = 0;

    virtual void OnLocatingStatusChange(const int status) = 0;

    virtual void OnErrorReport(const int errorCode) = 0;
};

class LocationProxyAdapter {
public:
    LocationProxyAdapter() = default;

    virtual ~LocationProxyAdapter() = default;

    virtual bool StartLocating(
        std::unique_ptr<LocationRequestConfig>& requestConfig,
        std::shared_ptr<LocationCallbackAdapter> callback) = 0;

    virtual bool StopLocating(std::shared_ptr<LocationCallbackAdapter> callback) = 0;

    virtual bool EnableAbility(bool isEnabled) = 0;

    virtual bool IsLocationEnabled() = 0;
};

class LocationInstance {
public:
    static LocationInstance& GetInstance();

    std::unique_ptr<LocationProxyAdapter> CreateLocationProxyAdapter();
    
    std::unique_ptr<LocationRequestConfig> CreateLocationRequestConfig();
};
}

#endif
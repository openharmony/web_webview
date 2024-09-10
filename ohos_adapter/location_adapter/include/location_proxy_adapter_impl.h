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

#ifndef LOCATION_PROXY_ADAPTER_IMPL_H
#define LOCATION_PROXY_ADAPTER_IMPL_H

#include <string>
#include <LocationKit/oh_location_type.h>

#include "location_adapter.h"

namespace OHOS::NWeb {
class LocationRequestConfigImpl : public LocationRequestConfig {
public:
    LocationRequestConfigImpl();
    virtual ~LocationRequestConfigImpl() = default;

    void SetScenario(int32_t scenario) override;
    void SetFixNumber(int32_t number) override;
    void SetMaxAccuracy(int32_t maxAccuary) override;
    void SetDistanceInterval(int32_t disInterval) override;
    void SetTimeInterval(int32_t timeInterval) override;
    void SetPriority(int32_t priority) override;
    int32_t GetScenario();
    int32_t GetTimeInterval();
    Location_PowerConsumptionScene GetPriority();

private:
    int32_t scenario_;
    int32_t timeInterval_;
    Location_PowerConsumptionScene priority_;
};

class LocationInfoImpl : public LocationInfo {
public:
    LocationInfoImpl();
    virtual ~LocationInfoImpl() = default;

    double GetLatitude() override;
    double GetLongitude() override;
    double GetAltitude() override;
    float GetAccuracy() override;
    float GetSpeed() override;
    double GetDirection() override;
    int64_t GetTimeStamp() override;
    int64_t GetTimeSinceBoot() override;
    std::vector<std::string> GetAdditions() override;

    void SetAdditions(char* additions);
    void SetBasicInfo(const Location_BasicInfo* basicInfo);

private:
    std::vector<std::string> additions_;
    Location_BasicInfo basicInfo_;
};

class LocationProxyAdapterImpl : public LocationProxyAdapter {
public:
    LocationProxyAdapterImpl();
    virtual ~LocationProxyAdapterImpl() = default;

    int32_t StartLocating(
        std::shared_ptr<LocationRequestConfig> requestConfig,
        std::shared_ptr<LocationCallbackAdapter> callback) override;
    bool StopLocating(int32_t callbackId) override;
    bool EnableAbility(bool isEnabled) override;
    bool IsLocationEnabled() override;

private:
    Location_RequestConfig* ohRequestConfig;
    std::shared_ptr<LocationCallbackAdapter> ohCallback_;
};
}

#endif

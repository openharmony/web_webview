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

#include <map>

#include "i_locator_callback.h"
#include "location.h"
#include "request_config.h"

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
    std::unique_ptr<OHOS::Location::RequestConfig>& GetConfig();
private:
    std::unique_ptr<OHOS::Location::RequestConfig> config_;
};

class LocationInfoImpl : public LocationInfo {
public:
    LocationInfoImpl() = delete;
    explicit LocationInfoImpl(std::unique_ptr<OHOS::Location::Location>& location);
    virtual ~LocationInfoImpl() = default;
    
    double GetLatitude() const override;
    double GetLongitude() const override;
    double GetAltitude() const override;
    float GetAccuracy() const override;
    float GetSpeed() const override;
    double GetDirection() const override;
    int64_t GetTimeStamp() const override;
    int64_t GetTimeSinceBoot() const override;
    std::string GetAdditions() const override;
    std::unique_ptr<OHOS::Location::Location>& GetLocation();
private:
    std::unique_ptr<OHOS::Location::Location> location_;
};

using LocatorCallbackMap =
    std::map<LocationCallbackAdapter*, sptr<OHOS::Location::ILocatorCallback>>;
using IsEnableLocationFuncType = bool(*)(bool& isEnabled);
using EnableAbilityFuncType = bool(*)(bool enable);
using StartLocatingFuncType = bool(*)(
    std::unique_ptr<OHOS::Location::RequestConfig>& requestConfig,
    OHOS::sptr<OHOS::Location::ILocatorCallback>& callback);
using StopLocatingFuncType = bool(*)(
    OHOS::sptr<OHOS::Location::ILocatorCallback>& callback);
class LocationProxyAdapterImpl : public LocationProxyAdapter {
public:
    LocationProxyAdapterImpl();
    virtual ~LocationProxyAdapterImpl() = default;

    bool StartLocating(
        std::unique_ptr<LocationRequestConfig>& requestConfig,
        std::shared_ptr<LocationCallbackAdapter> callback) override;
    bool StopLocating(std::shared_ptr<LocationCallbackAdapter> callback) override;
    bool EnableAbility(bool isEnabled) override;
    bool IsLocationEnabled() override;
private:
    static void* wrapperHandle_;
    static IsEnableLocationFuncType isEnableLocationFunc_;
    static EnableAbilityFuncType enableAbilityFunc_;
    static StartLocatingFuncType startLocatingFunc_;
    static StopLocatingFuncType stopLocatingFunc_;
    LocatorCallbackMap reg_;
};
}

#endif
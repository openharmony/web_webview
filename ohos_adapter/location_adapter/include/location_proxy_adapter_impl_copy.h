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

#if defined(NWEB_LOCATION_ENABLE)
#include "i_locator_callback.h"
#include "location.h"
#include "request_config.h"
#endif

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
#if defined(NWEB_LOCATION_ENABLE)
    std::unique_ptr<OHOS::Location::RequestConfig>& GetConfig();
private:
    std::unique_ptr<OHOS::Location::RequestConfig> config_;
#endif
};

class LocationInfoImpl : public LocationInfo {
public:
#if defined(NWEB_LOCATION_ENABLE)
    LocationInfoImpl() = delete;
    explicit LocationInfoImpl(std::unique_ptr<OHOS::Location::Location>& location);
#endif
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
#if defined(NWEB_LOCATION_ENABLE)
    std::unique_ptr<OHOS::Location::Location>& GetLocation();
private:
    std::unique_ptr<OHOS::Location::Location> location_;
#endif
};

#if defined(NWEB_LOCATION_ENABLE)
using LocatorCallbackMap =
    std::map<int32_t, sptr<OHOS::Location::ILocatorCallback>>;
using IsEnableLocationFuncType = bool(*)(bool& isEnabled);
using EnableAbilityFuncType = bool(*)(bool enable);
using StartLocatingFuncType = bool(*)(
    std::unique_ptr<OHOS::Location::RequestConfig>& requestConfig,
    OHOS::sptr<OHOS::Location::ILocatorCallback>& callback);
using StopLocatingFuncType = bool(*)(
    OHOS::sptr<OHOS::Location::ILocatorCallback>& callback);
#endif

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
#if defined(NWEB_LOCATION_ENABLE)
private:
    static void* wrapperHandle_;
    static IsEnableLocationFuncType isEnableLocationFunc_;
    static EnableAbilityFuncType enableAbilityFunc_;
    static StartLocatingFuncType startLocatingFunc_;
    static StopLocatingFuncType stopLocatingFunc_;
    LocatorCallbackMap reg_;
#endif
};
}

#endif

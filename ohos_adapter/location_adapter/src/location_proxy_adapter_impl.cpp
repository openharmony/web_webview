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
#include <dlfcn.h>
#include "location_proxy_adapter_impl.h"

#include "common_utils.h"
#include "location_adapter.h"
#include "location_callback_adapter_impl.h"
#include "nweb_log.h"

using namespace OHOS::Location;
namespace {
#ifdef __aarch64__
const std::string NWEB_WRAPPER_SO_PATH = "/system/lib64/libnweb_ohos_wrapper.z.so"; 
#else
const std::string NWEB_WRAPPER_SO_PATH = "/system/lib/libnweb_ohos_wrapper.z.so"; 
#endif
int32_t ConvertScenario(int32_t scenario)
{
    int32_t ret = OHOS::NWeb::LocationRequestConfig::Scenario::UNSET;
    switch (scenario) {
        case OHOS::NWeb::LocationRequestConfig::Scenario::UNSET:
            ret = OHOS::Location::SCENE_UNSET;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::NAVIGATION:
            ret = OHOS::Location::SCENE_NAVIGATION;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::TRAJECTORY_TRACKING:
            ret = OHOS::Location::SCENE_TRAJECTORY_TRACKING;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::CAR_HAILING:
            ret = OHOS::Location::SCENE_CAR_HAILING;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::DAILY_LIFE_SERVICE:
            ret = OHOS::Location::SCENE_DAILY_LIFE_SERVICE;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::NO_POWER:
            ret = OHOS::Location::SCENE_NO_POWER;
            break;
        default:
            break;
    }
    return ret;
}

int32_t ConvertPriority(int32_t priority)
{
    int32_t ret = OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_UNSET;
    switch (priority) {
        case OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_UNSET:
            ret = OHOS::Location::PRIORITY_UNSET;
            break;
        case OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_ACCURACY:
            ret = OHOS::Location::PRIORITY_ACCURACY;
            break;
        case OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_LOW_POWER:
            ret = OHOS::Location::PRIORITY_LOW_POWER;
            break;
        case OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_FAST_FIRST_FIX:
            ret = OHOS::Location::PRIORITY_FAST_FIRST_FIX;
            break;
        default:
            break;
    }
    return ret;
}
}

namespace OHOS::NWeb {
LocationInstance& LocationInstance::GetInstance()
{
    static LocationInstance instance;
    return instance;
}

std::unique_ptr<LocationProxyAdapter> LocationInstance::CreateLocationProxyAdapter()
{
    return std::make_unique<LocationProxyAdapterImpl>();
}

std::unique_ptr<LocationRequestConfig> LocationInstance::CreateLocationRequestConfig()
{
    return std::make_unique<LocationRequestConfigImpl>();
}

LocationRequestConfigImpl::LocationRequestConfigImpl()
{
    config_ = std::make_unique<OHOS::Location::RequestConfig>();
}

void LocationRequestConfigImpl::SetScenario(int32_t scenario)
{
    if (config_ == nullptr) {
        return;
    }
    config_->SetScenario(ConvertScenario(scenario));
}

void LocationRequestConfigImpl::SetFixNumber(int32_t number)
{
    if (config_ == nullptr) {
        return;
    }
    config_->SetFixNumber(number);
}

void LocationRequestConfigImpl::SetMaxAccuracy(int32_t maxAccuary)
{
    if (config_ == nullptr) {
        return;
    }
    config_->SetMaxAccuracy(maxAccuary);
}

void LocationRequestConfigImpl::SetDistanceInterval(int32_t disInterval)
{
    if (config_ == nullptr) {
        return;
    }
    config_->SetDistanceInterval(disInterval);
}

void LocationRequestConfigImpl::SetTimeInterval(int32_t timeInterval)
{
    if (config_ == nullptr) {
        return;
    }
    config_->SetTimeInterval(timeInterval);
}

void LocationRequestConfigImpl::SetPriority(int32_t priority)
{
    if (config_ == nullptr) {
        return;
    }
    config_->SetPriority(ConvertPriority(priority));
}

std::unique_ptr<OHOS::Location::RequestConfig>& LocationRequestConfigImpl::GetConfig()
{
    return config_;
}

LocationInfoImpl::LocationInfoImpl(std::unique_ptr<OHOS::Location::Location>& location)
    : location_(std::move(location)) {}

double LocationInfoImpl::GetLatitude() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetLatitude();
}

double LocationInfoImpl::GetLongitude() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetLongitude();
}

double LocationInfoImpl::GetAltitude() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetAltitude();
}

float LocationInfoImpl::GetAccuracy() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetAccuracy();
}

float LocationInfoImpl::GetSpeed() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetSpeed();
}

double LocationInfoImpl::GetDirection() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetDirection();
}

int64_t LocationInfoImpl::GetTimeStamp() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetTimeStamp();
}

int64_t LocationInfoImpl::GetTimeSinceBoot() const
{
    if (location_ == nullptr) {
        return 0;
    }
    return location_->GetTimeSinceBoot();
}

std::unique_ptr<OHOS::Location::Location>& LocationInfoImpl::GetLocation()
{
    return location_;
}

void* LocationProxyAdapterImpl::wrapperHandle_;
IsEnableLocationFuncType LocationProxyAdapterImpl::isEnableLocationFunc_;
EnableAbilityFuncType LocationProxyAdapterImpl::enableAbilityFunc_;
StartLocatingFuncType LocationProxyAdapterImpl::startLocatingFunc_;
StopLocatingFuncType LocationProxyAdapterImpl::stopLocatingFunc_;

LocationProxyAdapterImpl::LocationProxyAdapterImpl()
{
    if (!wrapperHandle_) {
        wrapperHandle_ = dlopen(NWEB_WRAPPER_SO_PATH.c_str(), RTLD_NOW);
    }
    if (wrapperHandle_) {
        if (!isEnableLocationFunc_) {
            isEnableLocationFunc_ = reinterpret_cast<IsEnableLocationFuncType>(
                dlsym(wrapperHandle_, "IsLocationEnable"));
        }
        if (!enableAbilityFunc_) {
            enableAbilityFunc_ = reinterpret_cast<EnableAbilityFuncType>(
                dlsym(wrapperHandle_, "EnableAbility"));
        }
        if (!startLocatingFunc_) {
            startLocatingFunc_ = reinterpret_cast<StartLocatingFuncType>(
                dlsym(wrapperHandle_, "StartLocating"));
        }
        if (!stopLocatingFunc_) {
            stopLocatingFunc_ = reinterpret_cast<StopLocatingFuncType>(
                dlsym(wrapperHandle_, "StopLocating"));
        }
    }
}

bool LocationProxyAdapterImpl::StartLocating(
    std::unique_ptr<LocationRequestConfig>& requestConfig,
    std::shared_ptr<LocationCallbackAdapter> callback)
{
    if (!startLocatingFunc_ || !callback) {
        WVLOG_E("get Locator::GetInstance() failed or callback is nullptr");
        return false;
    }
    sptr<OHOS::Location::ILocatorCallback> iCallback =
        sptr<OHOS::Location::ILocatorCallback>(new LocationCallbackImpl(callback));
    LocatorCallbackMap::iterator iter = reg_.find(callback.get());
    if (iter != reg_.end()) {
        WVLOG_E("StartLocating failed, callback already in map");
        return false;
    }
    bool ret = startLocatingFunc_(
        reinterpret_cast<LocationRequestConfigImpl*>(requestConfig.get())->GetConfig(),
        iCallback);
    if (!ret) {
        WVLOG_E("StartLocating failed, errcode:%{public}d", ret);
        return false;
    }

    reg_.emplace(std::make_pair(callback.get(), iCallback));
    return true;
}

bool LocationProxyAdapterImpl::StopLocating(
    std::shared_ptr<LocationCallbackAdapter> callback)
{
    if (!stopLocatingFunc_ || !callback) {
        WVLOG_E("get Locator::GetInstance() failed or callback is nullptr");
        return false;
    }
    LocatorCallbackMap::iterator iter = reg_.find(callback.get());
    if (iter == reg_.end()) {
        WVLOG_E("StopLocating failed due to reg_ not find iCallback");
        return false;
    }
    bool ret = stopLocatingFunc_(iter->second);
    reg_.erase(iter);
    return ret;
}

bool LocationProxyAdapterImpl::EnableAbility(bool isEnabled)
{
    if (!enableAbilityFunc_) {
        WVLOG_E("get Locator::GetInstance() failed");
        return false;
    }
    return enableAbilityFunc_(isEnabled);
}

bool LocationProxyAdapterImpl::IsLocationEnabled()
{
    if (!isEnableLocationFunc_) {
        WVLOG_E("get Locator::GetInstance() failed");
        return false;
    }
    bool isEnabled = false;
    return isEnableLocationFunc_(isEnabled) ? isEnabled : false;
}
}
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

#include "location_proxy_adapter_impl.h"

#include <LocationKit/oh_location.h>

#include "json/json.h"
#include "location_adapter.h"
#include "nweb_log.h"

namespace {
int32_t ConvertScenario(int32_t scenario)
{
    int32_t ret = LOCATION_USE_SCENE_DAILY_LIFE_SERVICE;
    switch (scenario) {
        case OHOS::NWeb::LocationRequestConfig::Scenario::NAVIGATION:
            ret = LOCATION_USE_SCENE_NAVIGATION;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::TRAJECTORY_TRACKING:
            ret = LOCATION_USE_SCENE_SPORT;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::CAR_HAILING:
            ret = LOCATION_USE_SCENE_TRANSPORT;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::DAILY_LIFE_SERVICE:
            ret = LOCATION_USE_SCENE_DAILY_LIFE_SERVICE;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::NO_POWER:
            ret = LOCATION_NO_POWER_CONSUMPTION;
            break;
        case OHOS::NWeb::LocationRequestConfig::Scenario::UNSET:
            ret = scenario;
            break;
        default:
            break;
    }
    return ret;
}

Location_PowerConsumptionScene ConvertPriority(int32_t priority)
{
    Location_PowerConsumptionScene ret = LOCATION_LOW_POWER_CONSUMPTION;
    switch (priority) {
        case OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_ACCURACY:
            ret = LOCATION_HIGH_POWER_CONSUMPTION;
            break;
        case OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_LOW_POWER:
            ret = LOCATION_LOW_POWER_CONSUMPTION;
            break;
        case OHOS::NWeb::LocationRequestConfig::Priority::PRIORITY_FAST_FIRST_FIX:
            ret = LOCATION_HIGH_POWER_CONSUMPTION;
            break;
        default:
            break;
    }
    return ret;
}
} // namespace

namespace OHOS::NWeb {
#define MAX_ADDITION_LEN 1024

static std::vector<std::weak_ptr<LocationCallbackAdapter>> g_enableUserVec;
std::mutex g_mutexEnableUserVec;

void AddUserData(std::shared_ptr<LocationCallbackAdapter> callbackPtr)
{
    std::lock_guard<std::mutex> lock(g_mutexEnableUserVec);
    g_enableUserVec.push_back(std::weak_ptr<LocationCallbackAdapter>(callbackPtr));
}

std::shared_ptr<LocationCallbackAdapter> getSharedPtrByUserData(void* userData)
{
    std::lock_guard<std::mutex> lock(g_mutexEnableUserVec);
    auto it = g_enableUserVec.cbegin();
    while (it != g_enableUserVec.cend()) {
        if (auto lockedPtr = it->lock()) {
            if (lockedPtr.get() == userData) {
                return lockedPtr;
            }
        } else {
            it = g_enableUserVec.erase(it);
            continue;
        }
        ++it;
    }
    return nullptr;
}

static void LocationCallback(Location_Info* location, void* userData)
{
    WVLOG_I("LocationCallback");
    Location_BasicInfo basicInfo = OH_LocationInfo_GetBasicInfo(location);
    char additions[MAX_ADDITION_LEN] = { 0 };
    OH_LocationInfo_GetAdditionalInfo(location, additions, MAX_ADDITION_LEN);
    auto locationInfoImpl = std::make_shared<LocationInfoImpl>();
    locationInfoImpl->SetBasicInfo(&basicInfo);
    locationInfoImpl->SetAdditions(additions);
    std::shared_ptr<LocationInfo> locationInfo = locationInfoImpl;

    auto locationCallbackAdapter = getSharedPtrByUserData(userData);
    if (locationCallbackAdapter == nullptr) {
        WVLOG_E("user data is invalid");
        return;
    }
    locationCallbackAdapter->OnLocationReport(locationInfo);
}

LocationRequestConfigImpl::LocationRequestConfigImpl()
    : scenario_(LOCATION_USE_SCENE_DAILY_LIFE_SERVICE), timeInterval_(0), priority_(LOCATION_LOW_POWER_CONSUMPTION)
{}

void LocationRequestConfigImpl::SetScenario(int32_t scenario)
{
    scenario_ = ConvertScenario(scenario);
    if (scenario_ == LOCATION_NO_POWER_CONSUMPTION) {
        scenario_ = OHOS::NWeb::LocationRequestConfig::Scenario::UNSET;
        priority_ = LOCATION_NO_POWER_CONSUMPTION;
    }
}

void LocationRequestConfigImpl::SetFixNumber(int32_t number)
{
    // invalid interface
    return;
}

void LocationRequestConfigImpl::SetMaxAccuracy(int32_t maxAccuary)
{
    // invalid interface
    return;
}

void LocationRequestConfigImpl::SetDistanceInterval(int32_t disInterval)
{
    // invalid interface
    return;
}

void LocationRequestConfigImpl::SetTimeInterval(int32_t timeInterval)
{
    timeInterval_ = timeInterval;
}

void LocationRequestConfigImpl::SetPriority(int32_t priority)
{
    if (priority_ != LOCATION_NO_POWER_CONSUMPTION) {
        priority_ = ConvertPriority(priority);
    }
}

int32_t LocationRequestConfigImpl::GetScenario()
{
    return scenario_;
}

int32_t LocationRequestConfigImpl::GetTimeInterval()
{
    return timeInterval_;
}

Location_PowerConsumptionScene LocationRequestConfigImpl::GetPriority()
{
    return priority_;
}

LocationInfoImpl::LocationInfoImpl() : basicInfo_({ 0 }) {}

double LocationInfoImpl::GetLatitude()
{
    return basicInfo_.latitude;
}

double LocationInfoImpl::GetLongitude()
{
    return basicInfo_.longitude;
}

double LocationInfoImpl::GetAltitude()
{
    return basicInfo_.altitude;
}

float LocationInfoImpl::GetAccuracy()
{
    return basicInfo_.accuracy;
}

float LocationInfoImpl::GetSpeed()
{
    return basicInfo_.speed;
}

double LocationInfoImpl::GetDirection()
{
    return basicInfo_.direction;
}

int64_t LocationInfoImpl::GetTimeStamp()
{
    return basicInfo_.timeForFix;
}

int64_t LocationInfoImpl::GetTimeSinceBoot()
{
    return basicInfo_.timeSinceBoot;
}

std::vector<std::string> LocationInfoImpl::GetAdditions()
{
    return additions_;
}

void LocationInfoImpl::SetBasicInfo(const Location_BasicInfo* basicInfo)
{
    basicInfo_ = *basicInfo;
}

void LocationInfoImpl::SetAdditions(char* additions)
{
    Json::Value root;
    Json::Reader reader;
    std::string additionsStr(additions);
    bool result = reader.parse(additionsStr, root);
    if (!result) {
        WVLOG_I("failed parse additions!");
        return;
    }
    for (const auto& member : root.getMemberNames()) {
        std::string value = root.get(member.c_str(), "").asString();
        std::string temp = member + ":" + value;
        WVLOG_I("SetAdditions, addition=%{public}s", temp.c_str());
        additions_.emplace_back(temp);
    }
}

LocationProxyAdapterImpl::LocationProxyAdapterImpl() : ohRequestConfig_(nullptr), ohCallback_(nullptr) {}

LocationProxyAdapterImpl::~LocationProxyAdapterImpl()
{
    if (ohRequestConfig_ != nullptr) {
        OH_Location_StopLocating(ohRequestConfig_);
        OH_Location_DestroyRequestConfig(ohRequestConfig_);
        ohRequestConfig_ = nullptr;
    }
}

int32_t LocationProxyAdapterImpl::StartLocating(
    std::shared_ptr<LocationRequestConfig> requestConfig, std::shared_ptr<LocationCallbackAdapter> callback)
{
    int32_t id = -1;
    if (!callback) {
        WVLOG_E("callback is nullptr");
        return id;
    }
    LocationRequestConfigImpl* requestConfigImpl = static_cast<LocationRequestConfigImpl*>(requestConfig.get());
    ohRequestConfig_ = OH_Location_CreateRequestConfig();
    if (requestConfigImpl->GetScenario() != OHOS::NWeb::LocationRequestConfig::Scenario::UNSET) {
        OH_LocationRequestConfig_SetUseScene(ohRequestConfig_, (Location_UseScene)requestConfigImpl->GetScenario());
    } else {
        OH_LocationRequestConfig_SetPowerConsumptionScene(ohRequestConfig_, requestConfigImpl->GetPriority());
    }
    OH_LocationRequestConfig_SetInterval(ohRequestConfig_, requestConfigImpl->GetTimeInterval());
    ohCallback_ = std::move(callback);
    AddUserData(ohCallback_);
    OH_LocationRequestConfig_SetCallback(ohRequestConfig_, LocationCallback, ohCallback_.get());
    Location_ResultCode errCode = OH_Location_StartLocating(ohRequestConfig_);
    if (errCode != LOCATION_SUCCESS) {
        WVLOG_E("StartLocating failed, errcode:%{public}d", errCode);
        return id;
    }

    WVLOG_I("LocationProxyAdapterImpl::StartLocating");
    return 0;
}

bool LocationProxyAdapterImpl::StopLocating(int32_t callbackId)
{
    if (callbackId < 0 || ohRequestConfig_ == nullptr) {
        WVLOG_E("callback is null");
        return false;
    }
    WVLOG_I("LocationProxyAdapterImpl::StopLocating");
    Location_ResultCode errCode = OH_Location_StopLocating(ohRequestConfig_);
    OH_Location_DestroyRequestConfig(ohRequestConfig_);
    ohRequestConfig_ = nullptr;
    return errCode == LOCATION_SUCCESS ? true : false;
}

bool LocationProxyAdapterImpl::EnableAbility(bool isEnabled)
{
    // invalid interface
    return isEnabled;
}

bool LocationProxyAdapterImpl::IsLocationEnabled()
{
    bool isEnabled = false;
    return OH_Location_IsLocatingEnabled(&isEnabled) == LOCATION_SUCCESS ? isEnabled : false;
}
} // namespace OHOS::NWeb

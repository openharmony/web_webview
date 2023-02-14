/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "battery_mgr_client_adapter_impl.h"
#include "nweb_log.h"
#include "battery_srv_client.h"

using namespace OHOS::NWeb;
using namespace OHOS::PowerMgr;
namespace OHOS::NWeb {
double WebBatteryInfoImpl::GetLevel()
{
    return level_;
}

bool WebBatteryInfoImpl::IsCharging()
{
    return isCharging_;
}

int WebBatteryInfoImpl::DisChargingTime()
{
    return disChargingTime_;
}

int WebBatteryInfoImpl::ChargingTime()
{
    return chargingTime_;
}

NWebBatteryEventSubscriber::NWebBatteryEventSubscriber(EventFwk::CommonEventSubscribeInfo& in, BatteryEventCallback& cb)
    : EventFwk::CommonEventSubscriber(in), eventCallback_(cb) {}

void NWebBatteryEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData &data)
{
    const std::string action = data.GetWant().GetAction();
    WVLOG_I("receive battery action: %{public}s", action.c_str());
    if (action != EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED) {
        return;
    }
    int32_t defaultInvalid = -1;
    std::string KEY_CAPACITY = PowerMgr::BatteryInfo::COMMON_EVENT_KEY_CAPACITY;
    std::string KEY_CHARGE_TYPE = PowerMgr::BatteryInfo::COMMON_EVENT_KEY_PLUGGED_TYPE;
    auto capacity = data.GetWant().GetIntParam(KEY_CAPACITY, defaultInvalid);
    auto isChangingType = data.GetWant().GetIntParam(KEY_CHARGE_TYPE, defaultInvalid);
    bool ischanging = true;
    if ((isChangingType == static_cast<int>(BatteryPluggedType::PLUGGED_TYPE_NONE)) ||
        (isChangingType == static_cast<int>(BatteryPluggedType::PLUGGED_TYPE_BUTT))) {
        ischanging = false;
    }
    const double changeFullLevel = 100.f;
    double changeLevel = capacity / changeFullLevel;
    WVLOG_I("receive capacity %{public}d  isChangingType %{public}d", capacity, isChangingType);
    WebBatteryInfoImpl batterinfo(changeLevel, ischanging, -1, -1);
    eventCallback_(batterinfo);
}

void BatteryMgrClientAdapterImpl::RegBatteryEvent(const BatteryEventCallback&& eventCallback)
{
    WVLOG_I("Reg Battery Event");
    cb = std::move(eventCallback);
}

bool BatteryMgrClientAdapterImpl::StartListen()
{
    WVLOG_I("start battery listen");
    EventFwk::MatchingSkills skill = EventFwk::MatchingSkills();
    skill.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    EventFwk::CommonEventSubscribeInfo info(skill);
    this->commonEventSubscriber_ = std::make_shared<NWebBatteryEventSubscriber>(info, this->cb);
    bool ret = EventFwk::CommonEventManager::SubscribeCommonEvent(this->commonEventSubscriber_);
    if (ret == false) {
        WVLOG_E("start battery listen fail");
        return false;
    } else {
        return true;
    }
}

void BatteryMgrClientAdapterImpl::StopListen()
{
    WVLOG_I("stop battery listen");
    if (this->commonEventSubscriber_ != nullptr) {
        bool result = EventFwk::CommonEventManager::UnSubscribeCommonEvent(this->commonEventSubscriber_);
        if (result) {
            this->commonEventSubscriber_ = nullptr;
        } else {
            WVLOG_E("stop battery listen fail");
        }
    }
}

std::unique_ptr<WebBatteryInfo> BatteryMgrClientAdapterImpl::RequestBatteryInfo()
{
    WVLOG_I("request batteryInfo");
    BatterySrvClient& battClient = BatterySrvClient::GetInstance();
    auto capacity = battClient.GetCapacity();
    auto isChangingType = battClient.GetPluggedType();
    bool ischanging = true;
    if ((isChangingType == BatteryPluggedType::PLUGGED_TYPE_NONE) ||
        (isChangingType == BatteryPluggedType::PLUGGED_TYPE_BUTT)) {
        ischanging = false;
    }
    const double changeFullLevel = 100.f;
    double changeLevel = capacity / changeFullLevel;
    return std::make_unique<WebBatteryInfoImpl>(changeLevel, ischanging, -1, -1);
}
}
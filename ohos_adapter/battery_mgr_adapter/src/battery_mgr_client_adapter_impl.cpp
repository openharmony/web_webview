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
#include <BasicServicesKit/ohbattery_info.h>
#include <BasicServicesKit/oh_commonevent_support.h>

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

std::set<std::shared_ptr<WebBatteryEventCallback>> BatteryMgrClientAdapterImpl::callbackSet_;
CommonEvent_SubscribeInfo* BatteryMgrClientAdapterImpl::commonEventSubscriberInfo_;
CommonEvent_Subscriber* BatteryMgrClientAdapterImpl::commonEventSubscriber_;
std::mutex BatteryMgrClientAdapterImpl::mutex_;


BatteryMgrClientAdapterImpl::~WebBatteryInfoImpl()
{
    StopListen();
}

void BatteryMgrClientAdapterImpl::OnBatteryEvent(const CommonEvent_RcvData *data)
{
    if (data == nullptr ||
        strcmp(OH_CommonEvent_GetEventFromRcvData(data), COMMON_EVENT_BATTERY_CHANGED)) {
        WVLOG_E("not battery event");
        return;
    }

    const CommonEvent_Parameters* para = OH_CommonEvent_GetParametersFromRcvData(data);
    if (para == nullptr) {
        WVLOG_E("failed to get patameters");
        return;
    }

    int capacity = 0;
    if (OH_CommonEvent_HasKeyInParameters(para, COMMON_EVENT_KEY_CAPACITY)) {
        capacity = OH_CommonEvent_GetIntFromParameters(para, COMMON_EVENT_KEY_CAPACITY, capacity);
    } else {
        WVLOG_E("failed to get capacity");
    }
    const double changeFullLevel = 100.f;
    double changeLevel = capacity / changeFullLevel;

    int plugType = 0;
    if (OH_CommonEvent_HasKeyInParameters(para, COMMON_EVENT_KEY_PLUGGED_TYPE)) {
        plugType = OH_CommonEvent_GetIntFromParameters(para, COMMON_EVENT_KEY_PLUGGED_TYPE, plugType);
    } else {
        WVLOG_E("failed to get plugType");
    }
    bool ischarging = true;
    if ((plugType == PLUGGED_TYPE_NONE) ||
        (plugType == PLUGGED_TYPE_BUTT)) {
        ischarging = false;
    }

    std::shared_ptr<WebBatteryInfoImpl> batterinfo =
        std::make_shared<WebBatteryInfoImpl>(changeLevel, ischarging, -1, -1);
    WVLOG_I("receive capacity %{public}d  plugType %{public}d", capacity, plugType);
    
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &eventCallback : callbackSet_) {
        eventCallback->BatteryInfoChanged(batterinfo);
    }
}

void BatteryMgrClientAdapterImpl::RegBatteryEvent(std::shared_ptr<WebBatteryEventCallback> eventCallback)
{
    if (!eventCallback) {
        WVLOG_E("WebBatteryEventCallback is nullptr.");
        return;
    }

    WVLOG_I("Reg Battery Event");
    cb_ = std::move(eventCallback);
}

bool BatteryMgrClientAdapterImpl::StartListen()
{
    WVLOG_I("start battery listen");
    std::lock_guard<std::mutex> lock(mutex_);
    if (commonEventSubscriber_ == nullptr) {
        const char* events[] = {
            COMMON_EVENT_BATTERY_CHANGED,
        };

        int count = sizeof(events) / sizeof(events[0]);
        commonEventSubscriberInfo_ = OH_CommonEvent_CreateSubscribeInfo(events, count);
        if (commonEventSubscriberInfo_ == NULL) {
            WVLOG_E("Create SubscribeInfo fail.");
            return false;
        }

        commonEventSubscriber_ = OH_CommonEvent_CreateSubscriber(commonEventSubscriberInfo_,
            OnBatteryEvent);
        if (commonEventSubscriber_ == NULL) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscriberInfo_);
            commonEventSubscriberInfo_ = nullptr;
            WVLOG_E("Create Subscriber fail.");
            return false;
        }

        CommonEvent_ErrCode ret = OH_CommonEvent_Subscribe(commonEventSubscriber_);
        if (ret != COMMONEVENT_ERR_OK) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscriberInfo_);
            OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
            WVLOG_E("Subscribe fail. ret: %{public}d", ret);
            return false;
        }
    }
    callbackSet_.insert(cb_);
    return true;
}

void BatteryMgrClientAdapterImpl::StopListen()
{
    WVLOG_I("stop battery listen");
    std::lock_guard<std::mutex> lock(mutex_);
    if (callbackSet_.find(cb_) == callbackSet_.end()) {
        return;
    }

    callbackSet_.erase(cb_);
    if (!callbackSet_.empty()) {
        return;
    }

    if (commonEventSubscriber_ == nullptr) {
        return;
    }
    
    CommonEvent_ErrCode ret = OH_CommonEvent_UnSubscribe(commonEventSubscriber_);
    if (ret != COMMONEVENT_ERR_OK) {
        WVLOG_E("UnSubscribe fail. ret: %{public}d", ret);
    }

    OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
    commonEventSubscriber_ = nullptr;

    OH_CommonEvent_DestroySubscribeInfo(commonEventSubscriberInfo_);
    commonEventSubscriberInfo_ = nullptr;
}

std::shared_ptr<WebBatteryInfo> BatteryMgrClientAdapterImpl::RequestBatteryInfo()
{
    WVLOG_I("request batteryInfo");
    int32_t capacity = OH_BatteryInfo_GetCapacity();
    BatteryInfo_BatteryPluggedType plugType = OH_BatteryInfo_GetPluggedType();
    bool ischarging = true;
    if ((plugType == PLUGGED_TYPE_NONE) ||
        (plugType == PLUGGED_TYPE_BUTT)) {
        ischarging = false;
    }
    const double changeFullLevel = 100.f;
    double changeLevel = capacity / changeFullLevel;
    return std::make_shared<WebBatteryInfoImpl>(changeLevel, ischarging, -1, -1);
}
}

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

#ifndef BATTERY_MGR_CLIENT_ADAPTER_IMPL_H
#define BATTERY_MGR_CLIENT_ADAPTER_IMPL_H

#include <mutex>

#include "battery_mgr_client_adapter.h"
#include "battery_info.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "matching_skills.h"
#include "want.h"
#include "common_event_subscriber.h"

namespace OHOS::NWeb {
class WebBatteryInfoImpl final : public WebBatteryInfo {
public:
    WebBatteryInfoImpl(double level, bool isCharging, int disChargingTime, int chargingTime)
        : level_(level), isCharging_(isCharging), disChargingTime_(disChargingTime), chargingTime_(chargingTime) {}

    ~WebBatteryInfoImpl() override = default;

    double GetLevel() override;

    bool IsCharging() override;

    int DisChargingTime() override;

    int ChargingTime() override;
private:
    double level_;
    bool isCharging_;
    int disChargingTime_;
    int chargingTime_;
};

class NWebBatteryEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    NWebBatteryEventSubscriber(EventFwk::CommonEventSubscribeInfo& in,  std::shared_ptr<WebBatteryEventCallback> cb);
    ~NWebBatteryEventSubscriber() override = default;
    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
private:
    std::shared_ptr<WebBatteryEventCallback> eventCallback_;
};

class BatteryMgrClientAdapterImpl : public BatteryMgrClientAdapter {
public:
    BatteryMgrClientAdapterImpl() = default;
    ~BatteryMgrClientAdapterImpl()
    { 
        StopListen();
    }

    void RegBatteryEvent(std::shared_ptr<WebBatteryEventCallback> eventCallback) override;
  
    bool StartListen() override;

    void StopListen() override;

    std::shared_ptr<WebBatteryInfo> RequestBatteryInfo() override;
private:
    std::shared_ptr<WebBatteryEventCallback> cb_ = nullptr;
    std::shared_ptr<EventFwk::CommonEventSubscriber> commonEventSubscriber_ = nullptr;
    std::mutex stop_mutex_;
};
}

#endif

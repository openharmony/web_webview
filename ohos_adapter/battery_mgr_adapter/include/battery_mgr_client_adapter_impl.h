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

#include <BasicServicesKit/oh_commonevent.h>
#include <mutex>
#include <set>
#include "battery_mgr_client_adapter.h"

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

class BatteryMgrClientAdapterImpl : public BatteryMgrClientAdapter {
public:
    BatteryMgrClientAdapterImpl() = default;
    ~BatteryMgrClientAdapterImpl() override;

    void RegBatteryEvent(std::shared_ptr<WebBatteryEventCallback> eventCallback) override;
  
    bool StartListen() override;

    void StopListen() override;

    std::shared_ptr<WebBatteryInfo> RequestBatteryInfo() override;

    static void OnBatteryEvent(const CommonEvent_RcvData *data);
private:
    std::shared_ptr<WebBatteryEventCallback> cb_ = nullptr;
    static std::set<std::shared_ptr<WebBatteryEventCallback>> callbackSet_;
    static CommonEvent_SubscribeInfo *commonEventSubscriberInfo_;
    static CommonEvent_Subscriber *commonEventSubscriber_;
    static std::mutex mutex_;
};
}

#endif

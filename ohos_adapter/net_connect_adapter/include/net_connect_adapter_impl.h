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

#ifndef NET_CONNECT_ADAPTER_IMPL_H
#define NET_CONNECT_ADAPTER_IMPL_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "i_net_conn_callback.h"
#include "net_connect_adapter.h"
#include "net_connect_callback_impl.h"
#include "net_conn_client.h"
#include "want.h"

namespace OHOS::NWeb {
using namespace OHOS::NetManagerStandard;

class NetVPNEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    NetVPNEventSubscriber(EventFwk::CommonEventSubscribeInfo& in, std::shared_ptr<VpnListener>);
    ~NetVPNEventSubscriber() override = default;
    void OnReceiveEvent(const EventFwk::CommonEventData& data) override;
 
private:
    std::shared_ptr<VpnListener> cb_ = nullptr;
};

class NetConnectAdapterImpl : public NetConnectAdapter {
public:
    NetConnectAdapterImpl() = default;

    ~NetConnectAdapterImpl() override = default;

    int32_t RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb) override;

    int32_t UnregisterNetConnCallback(int32_t id) override;

    int32_t GetDefaultNetConnect(NetConnectType &type, NetConnectSubtype &netConnectSubtype) override;

    std::vector<std::string> GetDnsServers() override;

    std::vector<std::string> GetDnsServersByNetId(int32_t netId) override;

    std::vector<std::string> GetDnsServersForVpn() override;
 
    void RegisterVpnListener(std::shared_ptr<VpnListener> cb) override;
 
    void UnRegisterVpnListener() override;

private:
    std::vector<std::string> GetDnsServersInternal(const NetHandle &netHandle);

    bool HasVpnTransport();

    std::unordered_map<int32_t, sptr<NetConnectCallbackImpl>> netConnCallbackMap_;

    std::shared_ptr<EventFwk::CommonEventSubscriber> commonEventSubscriber_ = nullptr;
};
}  // namespace OHOS::NWeb

#endif  // NET_CONNECT_ADAPTER_IMPL_H

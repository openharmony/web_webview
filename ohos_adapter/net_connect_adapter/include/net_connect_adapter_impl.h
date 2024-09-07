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

#include "net_connect_adapter.h"

#include <network/netmanager/net_connection.h>
#include <network/netmanager/net_connection_type.h>
#include <telephony/core_service/telephony_radio_type.h>

namespace OHOS::NWeb {
class NetConnectAdapterImpl : public NetConnectAdapter {
public:
    NetConnectAdapterImpl() = default;

    ~NetConnectAdapterImpl() override = default;

    int32_t RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb) override;

    int32_t UnregisterNetConnCallback(int32_t id) override;

    int32_t GetDefaultNetConnect(NetConnectType &type, NetConnectSubtype &netConnectSubtype) override;

    std::vector<std::string> GetDnsServers() override;

    std::vector<std::string> GetDnsServersByNetId(int32_t netId) override;

private:
    static std::unordered_map<int32_t, std::shared_ptr<NetConnCallback>> netConnCallbackMap_;

    std::vector<std::string> GetDnsServersInternal(NetConn_NetHandle &netHandle);

    static int32_t NetAvailable(std::shared_ptr<NetConnCallback> cb,
                                            NetConn_NetHandle *netHandle);

    static int32_t NetCapabilitiesChange(std::shared_ptr<NetConnCallback> cb,
                                            NetConn_NetHandle *netHandle,
                                            NetConn_NetCapabilities *netCapabilities);

    static int32_t NetConnectionPropertiesChange(std::shared_ptr<NetConnCallback> cb,
                                                    NetConn_NetHandle *netHandle,
                                                    NetConn_ConnectionProperties *connConnetionProperties);

    static void InitNetConnCallback(NetConn_NetConnCallback *netConnCallback);
};
}  // namespace OHOS::NWeb

#endif  // NET_CONNECT_ADAPTER_IMPL_H

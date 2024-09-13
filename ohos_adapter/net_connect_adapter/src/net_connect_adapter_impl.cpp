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

#include "net_connect_adapter_impl.h"

#include "net_connect_utils.h"
#include "net_capabilities_adapter_impl.h"
#include "net_connection_properties_adapter_impl.h"

#include "nweb_log.h"

#include <network/netmanager/net_connection.h>
#include <network/netmanager/net_connection_type.h>
#include <telephony/cellular_data/telephony_data.h>
#include <telephony/core_service/telephony_radio.h>
#include <telephony/core_service/telephony_radio_type.h>

namespace OHOS::NWeb {
std::unordered_map<int32_t, std::shared_ptr<NetConnCallback>> NetConnectAdapterImpl::netConnCallbackMap_ = {};
std::mutex NetConnectAdapterImpl::mutex_;

int32_t NetConnectAdapterImpl::NetAvailable(std::shared_ptr<NetConnCallback> cb, NetConn_NetHandle *netHandle)
{
    if (netHandle == nullptr) {
        WVLOG_E("NetConnCallback enter, net available, netHandle is nullptr.");
        return 0;
    }
    WVLOG_I("NetConnCallback enter, net available, net id = %{public}d.", netHandle->netId);
    if (cb != nullptr) {
        cb->NetAvailable();
    }
    return 0;
}

int32_t NetConnectAdapterImpl::NetCapabilitiesChange(std::shared_ptr<NetConnCallback> cb,
                                                     NetConn_NetHandle *netHandle,
                                                     NetConn_NetCapabilities * netCapabilities)
{
    if (netHandle == nullptr || netCapabilities == nullptr) {
        WVLOG_E("NetConnCallback enter, NetCapabilitiesChange, netHandle or netAllCap is nullptr.");
        return 0;
    }
    WVLOG_I("NetConnCallback enter, NetCapabilitiesChange, net id = %{public}d.", netHandle->netId);
    NetConnectSubtype subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    Telephony_RadioTechnology radioTech = Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN;
    for (auto bearerTypes : netCapabilities->bearerTypes) {
        if (bearerTypes == NETCONN_BEARER_CELLULAR) {
            int32_t slotId = OH_Telephony_GetDefaultCellularDataSlotId();
            if (slotId < 0) {
                WVLOG_E("get default soltId failed, ret = %{public}d.", slotId);
                slotId = 0;
            }
            Telephony_NetworkState networkState;
            Telephony_RadioResult radioRet = OH_Telephony_GetNetworkStateForSlot(slotId, &networkState);
            if (radioRet == TEL_RADIO_SUCCESS) {
                radioTech = networkState.cfgTech_;
                WVLOG_I("net radio tech = %{public}d.", static_cast<int32_t>(radioTech));
                subtype = NetConnectUtils::ConvertToConnectsubtype(radioTech);
            }
        }
        NetConnectType type = NetConnectUtils::ConvertToConnectType(bearerTypes, radioTech);
        WVLOG_I("net connect type = %{public}s.", NetConnectUtils::ConnectTypeToString(type).c_str());
        if (cb != nullptr) {
            auto capabilites = std::make_shared<NetCapabilitiesAdapterImpl>();
            capabilites->SetNetId(netHandle->netId);
            capabilites->SetConnectType(type);
            capabilites->SetConnectSubtype(subtype);
            return cb->OnNetCapabilitiesChanged(capabilites);
        }
    }

    return 0;
}

int32_t NetConnectAdapterImpl::NetConnectionPropertiesChange(std::shared_ptr<NetConnCallback> cb,
                                                             NetConn_NetHandle *netHandle,
                                                             NetConn_ConnectionProperties *connConnetionProperties)
{
    if (netHandle == nullptr || connConnetionProperties == nullptr) {
        WVLOG_E("NetConnCallback enter, NetConnectionPropertiesChange, netHandle or info is nullptr.");
        return 0;
    }
    WVLOG_I("NetConnCallback enter, NetConnectionPropertiesChange, net id = %{public}d.", netHandle->netId);
    if (cb != nullptr) {
        auto properties = std::make_shared<NetConnectionPropertiesAdapterImpl>();
        properties->SetNetId(netHandle->netId);
        return cb->OnNetConnectionPropertiesChanged(properties);
    }
    return 0;
}

int32_t NetConnectAdapterImpl::NetLost(std::shared_ptr<NetConnCallback> cb, NetConn_NetHandle *netHandle)
{
    if (netHandle == nullptr) {
        WVLOG_E("NetConnCallback enter, NetLost, netHandle is nullptr.");
        return 0;
    }
    WVLOG_I("NetConnCallback enter, NetLost, net id = %{public}d.", netHandle->netId);
    if (cb != nullptr) {
        cb->NetUnavailable();
    }
    return 0;
}

int32_t NetConnectAdapterImpl::NetUnavailable(std::shared_ptr<NetConnCallback> cb)
{
    WVLOG_I("NetConnCallback enter, NetUnavailable.");
    if (cb != nullptr) {
        cb->NetUnavailable();
    }
    return 0;
}

void NetConnectAdapterImpl::InitNetConnCallback(NetConn_NetConnCallback *netConnCallback)
{
    netConnCallback->onNetworkAvailable = [](NetConn_NetHandle *netHandle) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetAvailable(it->second, netHandle);
        }
    };
    netConnCallback->onNetCapabilitiesChange = [](NetConn_NetHandle *netHandle, NetConn_NetCapabilities *netCap) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetCapabilitiesChange(it->second, netHandle, netCap);
        }
    };
    netConnCallback->onConnetionProperties = [](NetConn_NetHandle *netHandle, NetConn_ConnectionProperties *connProp) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetConnectionPropertiesChange(it->second, netHandle, connProp);
        }
    };
    netConnCallback->onNetLost = [](NetConn_NetHandle *netHandle) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetLost(it->second, netHandle);
        }
    };
    netConnCallback->onNetUnavailable = [](void) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetUnavailable(it->second);
        }
    };
    netConnCallback->onNetBlockStatusChange = [](NetConn_NetHandle *netHandle, bool blocked) {
        WVLOG_I("NetConnCallback enter, NetBlockStatusChange, net id = %{public}d, blocked = %{public}d.",
            netHandle->netId, blocked);
    };
}

int32_t NetConnectAdapterImpl::RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb)
{
    if (cb == nullptr) {
        WVLOG_E("register NetConnCallback, cb is nullptr.");
        return -1;
    }

    uint32_t uid;
    NetConn_NetConnCallback netConnCallback;
    InitNetConnCallback(&netConnCallback);

    int32_t ret = OH_NetConn_RegisterDefaultNetConnCallback(&netConnCallback, &uid);
    if (ret != 0) {
        WVLOG_E("register NetConnCallback failed, ret = %{public}d.", ret);
        return -1;
    }

    int32_t id = static_cast<int32_t>(uid);
    netConnCallbackMap_.insert(std::make_pair(id, cb));
    WVLOG_I("register NetConnCallback success.");
    return id;
}

int32_t NetConnectAdapterImpl::UnregisterNetConnCallback(int32_t id)
{
    auto it = netConnCallbackMap_.find(id);
    if (it == netConnCallbackMap_.end()) {
        WVLOG_E("unregister NetConnCallback, not find the NetConnCallback.");
        return -1;
    }

    int32_t ret = OH_NetConn_UnregisterNetConnCallback(id);
    if (ret != 0) {
        WVLOG_E("unregister NetConnCallback failed, ret = %{public}d.", ret);
        return -1;
    }

    netConnCallbackMap_.erase(it);
    WVLOG_I("uregister NetConnCallback success.");
    return 0;
}

int32_t NetConnectAdapterImpl::GetDefaultNetConnect(NetConnectType &type, NetConnectSubtype &subtype)
{
    NetConn_NetHandle netHandle;
    int32_t ret = OH_NetConn_GetDefaultNet(&netHandle);
    if (ret != 0) {
        WVLOG_E("get default net failed, ret = %{public}d.", ret);
        return -1;
    }
    WVLOG_I("get default net success, net id = %{public}d.", netHandle.netId);

    NetConn_NetCapabilities netCapabilities;
    ret = OH_NetConn_GetNetCapabilities(&netHandle, &netCapabilities);
    if (ret != 0) {
        WVLOG_E("get default net capbilities failed, ret = %{public}d.", ret);
        return -1;
    }
    WVLOG_I("get default net capbilities success");

    subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    Telephony_RadioTechnology radioTech = Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN;
    auto bearerTypes = netCapabilities.bearerTypes[0];
    if (bearerTypes == NETCONN_BEARER_CELLULAR) {
        int32_t slotId = OH_Telephony_GetDefaultCellularDataSlotId();
        if (slotId < 0) {
            WVLOG_E("get default soltId failed, ret = %{public}d.", slotId);
            slotId = 0;
        }
        Telephony_NetworkState networkState;
        Telephony_RadioResult radioRet = OH_Telephony_GetNetworkStateForSlot(slotId, &networkState);
        if (radioRet == TEL_RADIO_SUCCESS) {
            radioTech = networkState.cfgTech_;
            WVLOG_I("net radio tech = %{public}d.", static_cast<int32_t>(radioTech));
            subtype = NetConnectUtils::ConvertToConnectsubtype(radioTech);
        }
    }
    type = NetConnectUtils::ConvertToConnectType(bearerTypes, radioTech);
    WVLOG_I("net connect type = %{public}s.", NetConnectUtils::ConnectTypeToString(type).c_str());
    return 0;
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServersInternal(NetConn_NetHandle &netHandle) {
    std::vector<std::string> servers;
    NetConn_ConnectionProperties connetionProperties;
    int32_t ret = OH_NetConn_GetConnectionProperties(&netHandle, &connetionProperties);
    if (ret != 0) {
        WVLOG_E("get net properties failed, ret = %{public}d.", ret);
        return servers;
    }
    WVLOG_D("get net properties for dns servers success, net id = %{public}d, ", netHandle.netId);

    for (const auto &dns : connetionProperties.dnsList) {
        servers.emplace_back(dns.address);
    }
    WVLOG_I("get dns servers success, net id = %{public}d, servers size = %{public}d.",
        netHandle.netId, static_cast<int32_t>(servers.size()));
    return servers;
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServers()
{
    NetConn_NetHandle netHandle;
    int32_t ret = OH_NetConn_GetDefaultNet(&netHandle);
    if (ret != 0) {
        WVLOG_E("get default net for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    return GetDnsServersInternal(netHandle);
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServersByNetId(int32_t netId)
{
    WVLOG_I("get dns servers by net id %{public}d.", netId);
    if (netId == -1) {
        return GetDnsServers();
    }

    NetConn_NetHandleList netHandleList;
    int32_t ret = OH_NetConn_GetAllNets(&netHandleList);
    if (ret != 0) {
        WVLOG_E("get all nets by net id for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    for (int i = 0; i < netHandleList.netHandleListSize; i++) {
        if (netHandleList.netHandles[i].netId == netId) {
            return GetDnsServersInternal(netHandleList.netHandles[i]);
        }
    }
    return std::vector<std::string>();
}
} // namespace OHOS::NWeb

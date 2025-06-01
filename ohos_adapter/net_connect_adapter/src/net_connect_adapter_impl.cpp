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

#include "cellular_data_client.h"
#include "core_service_client.h"
#include "net_connect_utils.h"
#include "net_link_info.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

namespace {
const int32_t DEFAULT_VALUE = -1;
const int32_t BEARER_VPN = 4;
}
 
NetVPNEventSubscriber::NetVPNEventSubscriber(
    EventFwk::CommonEventSubscribeInfo& in,
    std::shared_ptr<VpnListener> cb)
    : EventFwk::CommonEventSubscriber(in),
      cb_(cb)
{}
 
void NetVPNEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    const std::string action = data.GetWant().GetAction();
    if (action != EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE) {
        WVLOG_I("receive other action %{public}s", action.c_str());
        return;
    }
 
    int32_t code = data.GetCode();
    int32_t netType = data.GetWant().GetIntParam("NetType", DEFAULT_VALUE);
 
    WVLOG_I("receive COMMON_EVENT_CONNECTIVITY_CHANGE type: %{public}d code: %{public}d", netType, code);
    if (netType == DEFAULT_VALUE) {
        WVLOG_E("net vpn default net type");
        return;
    }
 
    if (netType != BEARER_VPN) {
        return;
    }
 
    if (!cb_) {
        WVLOG_E("net vpn listener is not set");
        return;
    }
 
    if (code == NetManagerStandard::NetConnState::NET_CONN_STATE_CONNECTED) {
       WVLOG_I("vpn is available");
       cb_->OnAvailable();
       return;
    }
 
    if (code == NetManagerStandard::NetConnState::NET_CONN_STATE_DISCONNECTED) {
        WVLOG_I("vpn is lost");
        cb_->OnLost();
        return;
    }
}
 
void NetConnectAdapterImpl::RegisterVpnListener(std::shared_ptr<VpnListener> cb)
{
    if (commonEventSubscriber_) {
      WVLOG_I("start vpn listen, common event subscriber has registered");
      return;
    }
 
    if (!cb)  {
      WVLOG_E("start vpn listen, register vpn listener failed cb is nullptr");
      return;
    }
 
    EventFwk::MatchingSkills skill = EventFwk::MatchingSkills();
    skill.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    EventFwk::CommonEventSubscribeInfo info(skill);
    info.SetPriority(1); //The higher the value, the higher the priority
    commonEventSubscriber_ = std::make_shared<NetVPNEventSubscriber>(info, cb);
    if (!commonEventSubscriber_) {
        WVLOG_E("start vpn listen, common event subscriber is null");
        return;
    }
 
    bool ret = EventFwk::CommonEventManager::SubscribeCommonEvent(commonEventSubscriber_);
    if (ret == false) {
        WVLOG_E("start vpn listen, subscribe common event failed");
    }
 
    if (HasVpnTransport()) {
        WVLOG_I("has vpn transport, vpn is available");
        cb->OnAvailable();
    }
}
 
bool NetConnectAdapterImpl::HasVpnTransport()
{
    std::list<sptr<NetManagerStandard::NetHandle>> netHandleList;
    int32_t ret = NetConnClient::GetInstance().GetAllNets(netHandleList);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get all nets failed, ret = %{public}d.", ret);
        return false;
    }
 
    for (sptr<NetManagerStandard::NetHandle> netHandle : netHandleList) {
        NetManagerStandard::NetAllCapabilities netAllCap;
        NetConnClient::GetInstance().GetNetCapabilities(*netHandle, netAllCap);
        if (netAllCap.bearerTypes_.count(NetManagerStandard::BEARER_VPN) > 0) {
            return true;
        }
    }
    return false;
}
 
void NetConnectAdapterImpl::UnRegisterVpnListener()
{
    if (commonEventSubscriber_) {
        commonEventSubscriber_ = nullptr;
    }
}

int32_t NetConnectAdapterImpl::RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb)
{
    static int32_t count = 0;
    if (cb == nullptr) {
        WVLOG_E("register NetConnCallback, cb is nullptr.");
        return -1;
    }

    sptr<NetConnectCallbackImpl> callbackImpl = new (std::nothrow) NetConnectCallbackImpl(cb);
    if (callbackImpl == nullptr) {
        WVLOG_E("new NetConnectCallbackImpl failed.");
        return -1;
    }

    int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(callbackImpl);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("register NetConnCallback failed, ret = %{public}d.", ret);
        return -1;
    }

    int32_t id = count++;
    if (count < 0) {
        count = 0;
    }

    netConnCallbackMap_.insert(std::make_pair(id, callbackImpl));
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

    int32_t ret = NetConnClient::GetInstance().UnregisterNetConnCallback(it->second);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("unregister NetConnCallback failed, ret = %{public}d.", ret);
        return -1;
    }

    netConnCallbackMap_.erase(it);
    WVLOG_I("uregister NetConnCallback success.");
    return 0;
}

int32_t NetConnectAdapterImpl::GetDefaultNetConnect(NetConnectType &type, NetConnectSubtype &subtype)
{
    NetHandle netHandle;
    int32_t ret = NetConnClient::GetInstance().GetDefaultNet(netHandle);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get default net failed, ret = %{public}d.", ret);
        return -1;
    }
    WVLOG_I("get default net success, net id = %{public}d.", netHandle.GetNetId());

    NetAllCapabilities netAllCap;
    ret = NetConnClient::GetInstance().GetNetCapabilities(netHandle, netAllCap);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get default net capbilities failed, ret = %{public}d.", ret);
        return -1;
    }
    WVLOG_I("get default net capbilities success");

    subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    RadioTech radioTech = RadioTech::RADIO_TECHNOLOGY_UNKNOWN;
    for (auto bearerTypes : netAllCap.bearerTypes_) {
        if (bearerTypes == BEARER_CELLULAR) {
            int32_t slotId = CellularDataClient::GetInstance().GetDefaultCellularDataSlotId();
            if (slotId < 0) {
                WVLOG_E("get default soltId failed, ret = %{public}d.", slotId);
                slotId = 0;
            }
            sptr<NetworkState> networkState = nullptr;
            CoreServiceClient::GetInstance().GetNetworkState(slotId, networkState);
            if (networkState != nullptr) {
                radioTech = networkState->GetPsRadioTech();
                WVLOG_I("net radio tech = %{public}d.", static_cast<int32_t>(radioTech));
                subtype = NetConnectUtils::ConvertToConnectsubtype(radioTech);
            }
        }
        type = NetConnectUtils::ConvertToConnectType(bearerTypes, radioTech);
        WVLOG_I("net connect type = %{public}s.", NetConnectUtils::ConnectTypeToString(type).c_str());
        break;
    }
    WVLOG_D("NetAllCapabilities dump, %{public}s.", netAllCap.ToString("").c_str());
    return 0;
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServersInternal(const NetHandle &netHandle) {
    std::vector<std::string> servers;
    NetLinkInfo info;
    int32_t ret = NetConnClient::GetInstance().GetConnectionProperties(netHandle, info);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get net properties failed, ret = %{public}d.", ret);
        return servers;
    }
    WVLOG_D("get net properties for dns servers success, net id = %{public}d, "
        "netinfo = %{public}s.", netHandle.GetNetId(), info.ToString(" ").c_str());

    for (const auto &dns : info.dnsList_) {
        servers.emplace_back(dns.address_);
    }
    WVLOG_I("get dns servers success, net id = %{public}d, servers size = %{public}d.",
        netHandle.GetNetId(), static_cast<int32_t>(servers.size()));
    return servers;
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServers()
{
    NetHandle netHandle;
    int32_t ret = NetConnClient::GetInstance().GetDefaultNet(netHandle);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get default net for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    return GetDnsServersInternal(netHandle);
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServersForVpn()
{
    std::list<sptr<NetManagerStandard::NetHandle>> netHandleList;
    int32_t ret = NetConnClient::GetInstance().GetAllNets(netHandleList);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get all nets by net id for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }
 
    for (sptr<NetManagerStandard::NetHandle> netHandle : netHandleList) {
        NetManagerStandard::NetAllCapabilities netAllCap;
        NetConnClient::GetInstance().GetNetCapabilities(*netHandle, netAllCap);
        if (netAllCap.bearerTypes_.count(NetManagerStandard::BEARER_VPN) > 0) {
            WVLOG_I("get dns for vpn");
            return GetDnsServersInternal(*netHandle);
        }
    }
    return std::vector<std::string>();
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServersByNetId(int32_t netId)
{
    WVLOG_I("get dns servers by net id %{public}d.", netId);
    if (netId == -1) {
        return GetDnsServers();
    }

    std::list<sptr<NetManagerStandard::NetHandle>> netHandleList;
    int32_t ret = NetConnClient::GetInstance().GetAllNets(netHandleList);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get all nets by net id for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    for (sptr<NetManagerStandard::NetHandle> netHandle : netHandleList) {
        if (netHandle->GetNetId() == netId) {
            return GetDnsServersInternal(*netHandle);
        }
    }
    return std::vector<std::string>();
}
} // namespace OHOS::NWeb

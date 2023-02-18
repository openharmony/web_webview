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
#include "nweb_log.h"

namespace OHOS::NWeb {
NetConnectCallbackImpl::NetConnectCallbackImpl(std::shared_ptr<NetConnCallback> cb) : cb_(cb) {}

int32_t NetConnectCallbackImpl::NetAvailable(sptr<NetHandle> &netHandle)
{
    if (netHandle == nullptr) {
        WVLOG_E("NetConnCallback enter, net available, netHandle is nullptr.");
        return 0;
    }

    WVLOG_I("NetConnCallback enter, net available, net id = %{public}d.", netHandle->GetNetId());
    if (cb_ != nullptr) {
        return cb_->NetAvailable();
    }

    return 0;
}

int32_t NetConnectCallbackImpl::NetCapabilitiesChange(sptr<NetHandle> &netHandle,
                                                      const sptr<NetAllCapabilities> &netAllCap)
{
    if (netHandle == nullptr || netAllCap == nullptr) {
        WVLOG_E("NetConnCallback enter, NetCapabilitiesChange, netHandle or netAllCap is nullptr.");
        return 0;
    }

    WVLOG_I("NetConnCallback enter, NetCapabilitiesChange, net id = %{public}d.", netHandle->GetNetId());
    WVLOG_D("NetAllCapabilities dump, %{public}s.", netAllCap->ToString("").c_str());
    NetConnectSubtype subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    RadioTech radioTech = RadioTech::RADIO_TECHNOLOGY_UNKNOWN;
    for (auto bearerTypes : netAllCap->bearerTypes_) {
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
                subtype = NetConnectUtils::ConvertToConnectsubtype(networkState->GetPsRadioTech());
            }
        }
        NetConnectType type = NetConnectUtils::ConvertToConnectType(bearerTypes, radioTech);
        WVLOG_I("net connect type = %{public}s.", NetConnectUtils::ConnectTypeToString(type).c_str());
        if (cb_ != nullptr) {
            return cb_->NetCapabilitiesChange(type, subtype);
        }
    }

    return 0;
}

int32_t NetConnectCallbackImpl::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info)
{
    if (netHandle == nullptr || info == nullptr) {
        WVLOG_E("NetConnCallback enter, NetCapabilitiesChange, netHandle or info is nullptr.");
        return 0;
    }

    WVLOG_I("NetConnCallback enter, NetConnectionPropertiesChange, net id = %{public}d.", netHandle->GetNetId());
    WVLOG_D("%{public}s.", info->ToString("").c_str());
    if (cb_ != nullptr) {
        return cb_->NetConnectionPropertiesChange();
    }

    return 0;
}

int32_t NetConnectCallbackImpl::NetLost(sptr<NetHandle> &netHandle)
{
    WVLOG_I("NetConnCallback enter, NetLost, net id = %{public}d.", netHandle->GetNetId());
    if (cb_ != nullptr) {
        return cb_->NetUnavailable();
    }
    return 0;
}

int32_t NetConnectCallbackImpl::NetUnavailable()
{
    WVLOG_I("NetConnCallback enter, NetUnavailable.");
    if (cb_ != nullptr) {
        return cb_->NetUnavailable();
    }
    return 0;
}

int32_t NetConnectCallbackImpl::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    WVLOG_I("NetConnCallback enter, NetBlockStatusChange, net id = %{public}d, blocked = %{public}d.",
        netHandle->GetNetId(), blocked);
    return 0;
}
}  // namespace OHOS::NWeb
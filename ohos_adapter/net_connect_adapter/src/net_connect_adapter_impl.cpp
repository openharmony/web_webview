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
int32_t NetConnectAdapterImpl::RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb)
{
    if (cb == nullptr) {
        WVLOG_E("register NetConnCallback, cb is nullptr.");
        return -1;
    }

    sptr<NetConnectCallbackImpl> callbackImpl = new (std::nothrow) NetConnectCallbackImpl(cb);
    if (callbackImpl == nullptr) {
        WVLOG_E("new NetConnectCallbackImpl failed.");
        return -1;
    }

    int32_t ret = DelayedSingleton<NetConnClient>::GetInstance()->RegisterNetConnCallback(callbackImpl);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("register NetConnCallback failed, ret = %{public}d.", ret);
        return -1;
    }

    netConnCallbackMap_.insert(std::make_pair(cb.get(), callbackImpl));
    WVLOG_I("register NetConnCallback success.");
    return 0;
}

int32_t NetConnectAdapterImpl::UnregisterNetConnCallback(std::shared_ptr<NetConnCallback> cb)
{
    if (cb == nullptr) {
        WVLOG_E("unregister NetConnCallback, cb is nullptr.");
        return -1;
    }

    auto it = netConnCallbackMap_.find(cb.get());
    if (it == netConnCallbackMap_.end()) {
        WVLOG_E("unregister NetConnCallback, not find the NetConnCallback.");
        return -1;
    }

    int32_t ret = DelayedSingleton<NetConnClient>::GetInstance()->UnregisterNetConnCallback(it->second);
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
    int32_t ret = DelayedSingleton<NetConnClient>::GetInstance()->GetDefaultNet(netHandle);
    if (ret != NETMANAGER_SUCCESS) {
        WVLOG_E("get default net failed, ret = %{public}d.", ret);
        return -1;
    }
    WVLOG_I("get default net success, net id = %{public}d.", netHandle.GetNetId());

    NetAllCapabilities netAllCap;
    ret = DelayedSingleton<NetConnClient>::GetInstance()->GetNetCapabilities(netHandle, netAllCap);
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
} // namespace OHOS::NWeb
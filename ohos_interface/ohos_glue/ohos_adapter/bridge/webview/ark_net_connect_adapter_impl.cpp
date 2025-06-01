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

#include "ohos_adapter/bridge/ark_net_connect_adapter_impl.h"

#include "ohos_adapter/bridge/ark_net_conn_callback_wrapper.h"
#include "ohos_adapter/bridge/ark_vpn_listener_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNetConnectAdapterImpl::ArkNetConnectAdapterImpl(std::shared_ptr<OHOS::NWeb::NetConnectAdapter> ref) : real_(ref) {}

int32_t ArkNetConnectAdapterImpl::RegisterNetConnCallback(ArkWebRefPtr<ArkNetConnCallback> cb)
{
    if (CHECK_REF_PTR_IS_NULL(cb)) {
        return real_->RegisterNetConnCallback(nullptr);
    }

    return real_->RegisterNetConnCallback(std::make_shared<ArkNetConnCallbackWrapper>(cb));
}

int32_t ArkNetConnectAdapterImpl::UnregisterNetConnCallback(int32_t id)
{
    return real_->UnregisterNetConnCallback(id);
}

int32_t ArkNetConnectAdapterImpl::GetDefaultNetConnect(uint32_t& type, uint32_t& netConnectSubtype)
{
    NWeb::NetConnectType nType;
    NWeb::NetConnectSubtype nSubtype;
    int32_t result = real_->GetDefaultNetConnect(nType, nSubtype);
    type = (uint32_t)nType;
    netConnectSubtype = (uint32_t)nSubtype;
    return result;
}

ArkWebStringVector ArkNetConnectAdapterImpl::GetDnsServers()
{
    std::vector<std::string> servers = real_->GetDnsServers();
    return ArkWebStringVectorClassToStruct(servers);
}

ArkWebStringVector ArkNetConnectAdapterImpl::GetDnsServersByNetId(int32_t netId)
{
    std::vector<std::string> servers = real_->GetDnsServersByNetId(netId);
    return ArkWebStringVectorClassToStruct(servers);
}

ArkWebStringVector ArkNetConnectAdapterImpl::GetDnsServersForVpn()
{
    std::vector<std::string> servers = real_->GetDnsServersForVpn();
    return ArkWebStringVectorClassToStruct(servers);
}
 
void ArkNetConnectAdapterImpl::RegisterVpnListener(ArkWebRefPtr<ArkVpnListener> cb)
{
    if (CHECK_REF_PTR_IS_NULL(cb)) {
        return real_->RegisterVpnListener(nullptr);
    }
 
    return real_->RegisterVpnListener(std::make_shared<ArkVpnListenerWrapper>(cb));
}
 
void ArkNetConnectAdapterImpl::UnRegisterVpnListener()
{
    real_->UnRegisterVpnListener();
}

} // namespace OHOS::ArkWeb

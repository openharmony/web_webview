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

#include "ohos_adapter/bridge/ark_net_connect_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_net_conn_callback_impl.h"
#include "ohos_adapter/bridge/ark_vpn_listener_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkNetConnectAdapterWrapper::ArkNetConnectAdapterWrapper(ArkWebRefPtr<ArkNetConnectAdapter> ref) : ctocpp_(ref) {}

int32_t ArkNetConnectAdapterWrapper::RegisterNetConnCallback(std::shared_ptr<OHOS::NWeb::NetConnCallback> cb)
{
    if (CHECK_SHARED_PTR_IS_NULL(cb)) {
        return ctocpp_->RegisterNetConnCallback(nullptr);
    }

    return ctocpp_->RegisterNetConnCallback(new ArkNetConnCallbackImpl(cb));
}

int32_t ArkNetConnectAdapterWrapper::UnregisterNetConnCallback(int32_t id)
{
    return ctocpp_->UnregisterNetConnCallback(id);
}

int32_t ArkNetConnectAdapterWrapper::GetDefaultNetConnect(
    OHOS::NWeb::NetConnectType& type, OHOS::NWeb::NetConnectSubtype& netConnectSubtype)
{
    uint32_t uType = 0;
    uint32_t uNetConnectSubtype = 0;
    int32_t result = ctocpp_->GetDefaultNetConnect(uType, uNetConnectSubtype);
    type = OHOS::NWeb::NetConnectType(uType);
    netConnectSubtype = OHOS::NWeb::NetConnectSubtype(uNetConnectSubtype);
    return result;
}

std::vector<std::string> ArkNetConnectAdapterWrapper::GetDnsServers()
{
    ArkWebStringVector ark_servers = ctocpp_->GetDnsServers();
    std::vector<std::string> servers = ArkWebStringVectorStructToClass(ark_servers);
    ArkWebStringVectorStructRelease(ark_servers);
    return servers;
}

std::vector<std::string> ArkNetConnectAdapterWrapper::GetDnsServersByNetId(int32_t net_id)
{
    ArkWebStringVector ark_servers = ctocpp_->GetDnsServersByNetId(net_id);
    std::vector<std::string> servers = ArkWebStringVectorStructToClass(ark_servers);
    ArkWebStringVectorStructRelease(ark_servers);
    return servers;
}

std::vector<std::string> ArkNetConnectAdapterWrapper::GetDnsServersForVpn()
{
    ArkWebStringVector ark_servers = ctocpp_->GetDnsServersForVpn();
    std::vector<std::string> servers = ArkWebStringVectorStructToClass(ark_servers);
    ArkWebStringVectorStructRelease(ark_servers);
    return servers;
}
 
void ArkNetConnectAdapterWrapper::RegisterVpnListener(std::shared_ptr<OHOS::NWeb::VpnListener> cb)
{
    if (CHECK_SHARED_PTR_IS_NULL(cb)) {
        return ctocpp_->RegisterVpnListener(nullptr);
    }
 
    return ctocpp_->RegisterVpnListener(new ArkVpnListenerImpl(cb));
}
 
void ArkNetConnectAdapterWrapper::UnRegisterVpnListener()
{
    ctocpp_->UnRegisterVpnListener();
}
} // namespace OHOS::ArkWeb

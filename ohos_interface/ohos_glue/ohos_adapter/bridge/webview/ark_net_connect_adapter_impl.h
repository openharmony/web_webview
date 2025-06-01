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

#ifndef ARK_NET_CONNECT_ADAPTER_IMPL_H
#define ARK_NET_CONNECT_ADAPTER_IMPL_H
#pragma once

#include "net_connect_adapter.h"
#include "ohos_adapter/include/ark_net_connect_adapter.h"

namespace OHOS::ArkWeb {

class ArkNetConnectAdapterImpl : public ArkNetConnectAdapter {
public:
    explicit ArkNetConnectAdapterImpl(std::shared_ptr<OHOS::NWeb::NetConnectAdapter>);

    int32_t RegisterNetConnCallback(ArkWebRefPtr<ArkNetConnCallback> cb) override;

    int32_t UnregisterNetConnCallback(int32_t id) override;

    int32_t GetDefaultNetConnect(uint32_t& type, uint32_t& netConnectSubtype) override;

    ArkWebStringVector GetDnsServers() override;

    ArkWebStringVector GetDnsServersByNetId(int32_t netId) override;

    ArkWebStringVector GetDnsServersForVpn() override;
 
    void RegisterVpnListener(ArkWebRefPtr<ArkVpnListener> cb) override;
 
    void UnRegisterVpnListener() override;

private:
    std::shared_ptr<OHOS::NWeb::NetConnectAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkNetConnectAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_CONNECT_ADAPTER_IMPL_H

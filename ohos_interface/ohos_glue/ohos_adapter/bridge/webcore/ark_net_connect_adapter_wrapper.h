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

#ifndef ARK_NET_CONNECT_ADAPTER_WRAPPER_H
#define ARK_NET_CONNECT_ADAPTER_WRAPPER_H
#pragma once

#include "net_connect_adapter.h"
#include "ohos_adapter/include/ark_net_connect_adapter.h"

namespace OHOS::ArkWeb {

class ArkNetConnectAdapterWrapper : public OHOS::NWeb::NetConnectAdapter {
public:
    explicit ArkNetConnectAdapterWrapper(ArkWebRefPtr<ArkNetConnectAdapter>);

    int32_t RegisterNetConnCallback(std::shared_ptr<OHOS::NWeb::NetConnCallback> cb) override;

    int32_t UnregisterNetConnCallback(int32_t id) override;

    int32_t GetDefaultNetConnect(
        OHOS::NWeb::NetConnectType& type, OHOS::NWeb::NetConnectSubtype& netConnectSubtype) override;

    std::vector<std::string> GetDnsServers() override;

    std::vector<std::string> GetDnsServersByNetId(int32_t net_id) override;

    std::vector<std::string> GetDnsServersForVpn() override;
 
    void RegisterVpnListener(std::shared_ptr<OHOS::NWeb::VpnListener> cb) override;
 
    void UnRegisterVpnListener() override;

private:
    ArkWebRefPtr<ArkNetConnectAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_CONNECT_ADAPTER_WRAPPER_H

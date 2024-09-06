/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NET_PROXY_ADAPTER_IMPL_H
#define NET_PROXY_ADAPTER_IMPL_H

#include <string>
#include <vector>

#include "net_proxy_adapter.h"

#include <BasicServicesKit/oh_commonevent.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <network/netmanager/net_connection_type.h>
#include <network/netmanager/net_connection.h>

namespace OHOS::NWeb {
class NetProxyAdapterImpl final : public NetProxyAdapter {
public:
    NetProxyAdapterImpl() = default;
    ~NetProxyAdapterImpl() override;

    static NetProxyAdapterImpl& GetInstance();

    void RegNetProxyEvent(std::shared_ptr<NetProxyEventCallbackAdapter> eventCallback) override;

    bool StartListen() override;

    void StopListen() override;

    void GetProperty(std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion) override;

    void Changed();

    static void OnReceiveEvent(const CommonEvent_RcvData *data);

    static void AppProxyChange(NetConn_HttpProxy *receiveHttpProxy);
private:
    static std::shared_ptr<NetProxyEventCallbackAdapter> cb_;
    static CommonEvent_SubscribeInfo *commonEventSubscribeInfo_;
    static CommonEvent_Subscriber *commonEventSubscriber_;
    bool listen_ = false;
    uint32_t appProxyCallbackId_ = 0;
    void StartListenAppProxy();
};

} // namespace OHOS::NWeb

#endif // NET_PROXY_ADAPTER_IMPL_H

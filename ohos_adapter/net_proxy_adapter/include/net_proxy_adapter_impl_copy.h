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

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "matching_skills.h"
#include "net_conn_client.h"
#include "net_proxy_adapter.h"
#include "want.h"

namespace OHOS::NWeb {
class NetProxyAdapterImpl final : public NetProxyAdapter {
public:
    NetProxyAdapterImpl() = default;
    ~NetProxyAdapterImpl() override = default;

    static NetProxyAdapterImpl& GetInstance();

    void RegNetProxyEvent(std::shared_ptr<NetProxyEventCallbackAdapter> eventCallback) override;

    bool StartListen() override;

    void StopListen() override;

    void GetProperty(std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion) override;

    void Changed();

private:
    bool listen_ = false;
    std::shared_ptr<NetProxyEventCallbackAdapter> cb_ = nullptr;
    uint32_t appProxyCallbackId_ = 0;
    void StartListenAppProxy();
    std::shared_ptr<EventFwk::CommonEventSubscriber> commonEventSubscriber_ = nullptr;
};

class NetProxyEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    NetProxyEventSubscriber(EventFwk::CommonEventSubscribeInfo& in, std::shared_ptr<NetProxyEventCallbackAdapter> cb);
    ~NetProxyEventSubscriber() override = default;
    void OnReceiveEvent(const EventFwk::CommonEventData& data) override;

private:
    std::shared_ptr<NetProxyEventCallbackAdapter> eventCallback_ = nullptr;
};

namespace Base64 {
std::string Encode(const std::string& source);
std::string Decode(const std::string& encoded);
} // namespace Base64

} // namespace OHOS::NWeb

#endif // NET_PROXY_ADAPTER_IMPL_H

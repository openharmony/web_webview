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

#include "net_proxy_adapter.h"

namespace OHOS::NWeb {
class NetProxyAdapterImpl final : public NetProxyAdapter {
public:
    NetProxyAdapterImpl() = default;
    ~NetProxyAdapterImpl() override = default;

    static NetProxyAdapterImpl& GetInstance() {
        static NetProxyAdapterImpl instance;
        return instance;
    }

    void RegNetProxyEvent(std::shared_ptr<NetProxyEventCallbackAdapter> eventCallback) override {}

    bool StartListen() override { return false; }

    void StopListen() override {}

    void GetProperty(std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion) override {}

    void Changed() {}
};

} // namespace OHOS::NWeb

#endif // NET_PROXY_ADAPTER_IMPL_H

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

#ifndef NET_PROXY_ADAPTER_H
#define NET_PROXY_ADAPTER_H

#include <memory>

namespace OHOS::NWeb {

class NetProxyEventCallbackAdapter {
public:
    NetProxyEventCallbackAdapter() = default;
    virtual ~NetProxyEventCallbackAdapter() = default;

    virtual void Changed(const std::string& host, const uint16_t& port, const std::string& pacUrl,
        const std::vector<std::string>& exclusionList) = 0;
};

class NetProxyAdapter {
public:
    NetProxyAdapter() = default;
    virtual ~NetProxyAdapter() = default;

    virtual void RegNetProxyEvent(std::shared_ptr<NetProxyEventCallbackAdapter> eventCallback) = 0;

    virtual bool StartListen() = 0;

    virtual void StopListen() = 0;

    virtual void GetProperty(std::string& host, uint16_t& port, std::string& pacUrl, std::string& exclusion) = 0;
};

} // namespace OHOS::NWeb

#endif // NET_PROXY_ADAPTER_H

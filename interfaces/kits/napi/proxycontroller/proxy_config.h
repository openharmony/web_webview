/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_PROXY_CONFIG_H
#define NWEB_PROXY_CONFIG_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "proxy_rule.h"

namespace OHOS {
namespace NWeb {
class ProxyConfig {
public:
    ProxyConfig() = default;
    ~ProxyConfig() = default;

    void InsertBypassRule(const std::string& bypassRule);
    void InsertDirectRule(int32_t proxySchemeFilter = static_cast<int32_t>(ProxySchemeFilter::MATCH_ALL_SCHEMES));
    void InsertProxyRule(const std::string& proxyUrl, int32_t proxySchemeFilter =
                         static_cast<int32_t>(ProxySchemeFilter::MATCH_ALL_SCHEMES));
    void BypassHostnamesWithoutPeriod();
    void ClearImplicitRules();
    void EnableReverseBypass(const bool enable);
    const std::vector<std::string>& GetBypassRules();
    const std::vector<ProxyRule>& GetProxyRules();
    bool IsReverseBypassEnabled();

private:
    std::vector<std::string> bypassRules_;
    std::vector<ProxyRule> proxyRules_;
    bool enbaleReverseBypass_;
};
} // namespace NWeb
} // namespace OHOS

#endif  // NWEB_PROXY_CONFIG_H
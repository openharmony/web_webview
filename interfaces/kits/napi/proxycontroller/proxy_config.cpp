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

#include "proxy_config.h"

#include <string>

#include "nweb_log.h"

namespace OHOS {
namespace NWeb {
const std::string BYPASS_HOST_NAMES = "<local>";
const std::string CLEAR_IMPLICIT_RULES = "<-loopback>";
const std::string DIRECT = "direct://";

void ProxyConfig::InsertBypassRule(const std::string& bypassRule)
{
    bypassRules_.push_back(bypassRule);
}

void ProxyConfig::InsertDirectRule(int32_t proxySchemeFilter)
{
    proxyRules_.push_back(ProxyRule(DIRECT, proxySchemeFilter));
}

void ProxyConfig::InsertProxyRule(const std::string& proxyUrl, int32_t proxySchemeFilter)
{
    proxyRules_.push_back(ProxyRule(proxyUrl, proxySchemeFilter));
}

void ProxyConfig::BypassHostnamesWithoutPeriod()
{
    InsertBypassRule(BYPASS_HOST_NAMES.c_str());
}

void ProxyConfig::ClearImplicitRules()
{
    InsertBypassRule(CLEAR_IMPLICIT_RULES.c_str());
}

void ProxyConfig::EnableReverseBypass(const bool enable)
{
    enbaleReverseBypass_ = enable;
}

const std::vector<std::string>& ProxyConfig::GetBypassRules()
{
    return bypassRules_;
}

const std::vector<ProxyRule>& ProxyConfig::GetProxyRules()
{
    return proxyRules_;
}

bool ProxyConfig::IsReverseBypassEnabled()
{
    return enbaleReverseBypass_;
}
} // namespace NWeb
} // namespace OHOS
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

#include "proxy_rule.h"

#include <string>
#include "nweb_log.h"

namespace OHOS {
namespace NWeb {

ProxyRule::ProxyRule(const std::string& url, int32_t filter)
    : url_(url), schemeFilter_(filter){}

const std::string& ProxyRule::GetUrl()
{
    return url_;
}

int32_t ProxyRule::GetSchemeFilter()
{
    return schemeFilter_;
}

ProxyRule::~ProxyRule()
{
    WVLOG_D("[PROXYCONTROLLER] ProxyRule::~ProxyRule()");
}
} // namespace NWeb
} // namespace OHOS
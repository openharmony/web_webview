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
 
#ifndef OHOS_NWEB_ANI_PROXY_RULE_H
#define OHOS_NWEB_ANI_PROXY_RULE_H
 
#include <string>
 
namespace OHOS {
namespace NWeb {
 
enum class ProxySchemeFilter : int32_t {
    MATCH_ALL_SCHEMES = 0,
    MATCH_HTTP = 1,
    MATCH_HTTPS = 2
};
 
class ProxyRule {
public:
    ProxyRule() = default;
    ProxyRule(const std::string& url, int32_t filter);
    ~ProxyRule();
 
    const std::string& GetUrl();
 
    int32_t GetSchemeFilter();
 
private:
    std::string url_;
    int32_t schemeFilter_;
};
} // namespace NWeb
} // namespace OHOS
 
#endif  // OHOS_NWEB_ANI_PROXY_RULE_H
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "net_conn_client_test_mock.h"

#include <map>

namespace OHOS {
namespace NetManagerStandard {

HttpProxy defaultHttpProxy_;
HttpProxy appHttpProxy_("", 0, {});
uint32_t appId_;
std::map<int, int> appMap;

int32_t NetConnClient::GetDefaultHttpProxy(HttpProxy& httpProxy)
{
    httpProxy = defaultHttpProxy_;
    return 0;
}

void NetConnClient::RegisterAppHttpProxyCallback(std::function<void(const HttpProxy &httpProxy)> callback,
                                                 uint32_t &callbackid)
{
    callbackid = appId_;
    appMap[callbackid] = callbackid;
    callback(appHttpProxy_);
}

void NetConnClient::UnregisterAppHttpProxyCallback(uint32_t callbackid)
{
    appMap.erase(callbackid);
}

void NetConnClientMock::SetDefaultHttpProxy(HttpProxy httpProxy)
{
    defaultHttpProxy_ = httpProxy;
}

void NetConnClientMock::SetAppId(uint32_t appId)
{
    appId_ = appId;
}

bool NetConnClientMock::IsAppExist(uint32_t appId)
{
    return appMap.find(appId) != appMap.end();
}

}

}
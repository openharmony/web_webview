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

#ifndef NET_CONNECT_CALLBACK_IMPL_H
#define NET_CONNECT_CALLBACK_IMPL_H

#include <cstdint>
#include <string>

#include "net_connect_adapter.h"
#include "net_conn_callback_stub.h"

namespace OHOS::NWeb {
using namespace OHOS::NetManagerStandard;
class NetConnectCallbackImpl : public NetConnCallbackStub {
public:
    explicit NetConnectCallbackImpl(std::shared_ptr<NetConnCallback> cb);

    ~NetConnectCallbackImpl() override = default;

    int32_t NetAvailable(sptr<NetHandle> &netHandle) override;
    int32_t NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap) override;
    int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info) override;
    int32_t NetLost(sptr<NetHandle> &netHandle) override;
    int32_t NetUnavailable() override;
    int32_t NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked) override;

private:
    std::shared_ptr<NetConnCallback> cb_ = nullptr;
};
}  // namespace OHOS::NWeb

#endif  // NET_CONNECT_CALLBACK_IMPL_H
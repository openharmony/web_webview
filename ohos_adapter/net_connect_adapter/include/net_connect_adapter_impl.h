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

#ifndef NET_CONNECT_ADAPTER_IMPL_H
#define NET_CONNECT_ADAPTER_IMPL_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "i_net_conn_callback.h"
#include "net_connect_adapter.h"
#include "net_connect_callback_impl.h"
#include "net_conn_client.h"

namespace OHOS::NWeb {
using namespace OHOS::NetManagerStandard;
class NetConnectAdapterImpl : public NetConnectAdapter {
public:
    NetConnectAdapterImpl() = default;

    ~NetConnectAdapterImpl() override = default;

    int32_t RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb) override;

    int32_t UnregisterNetConnCallback(std::shared_ptr<NetConnCallback> cb) override;

    int32_t GetDefaultNetConnect(NetConnectType &type, NetConnectSubtype &netConnectSubtype) override;

private:
    std::unordered_map<NetConnCallback*, sptr<NetConnectCallbackImpl>> netConnCallbackMap_;
};
}  // namespace OHOS::NWeb

#endif  // NET_CONNECT_ADAPTER_IMPL_H
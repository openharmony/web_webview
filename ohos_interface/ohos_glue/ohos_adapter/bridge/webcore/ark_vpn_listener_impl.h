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
 
#ifndef ARK_VPN_LISTENER_IMPL_H
#define ARK_VPN_LISTENER_IMPL_H
#pragma once
 
#include "net_connect_adapter.h"
#include "ohos_adapter/include/ark_net_connect_adapter.h"
 
namespace OHOS::ArkWeb {
 
class ArkVpnListenerImpl : public ArkVpnListener {
public:
    explicit ArkVpnListenerImpl(std::shared_ptr<OHOS::NWeb::VpnListener>);
 
    void OnAvailable() override;
 
    void OnLost() override;
 
private:
    std::shared_ptr<OHOS::NWeb::VpnListener> real_;
 
    IMPLEMENT_REFCOUNTING(ArkVpnListenerImpl);
};
 
} // namespace OHOS::ArkWeb
 
#endif // ARK_VPN_LISTENER_IMPL_H
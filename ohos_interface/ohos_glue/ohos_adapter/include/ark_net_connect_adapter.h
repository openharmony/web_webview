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

#ifndef ARK_NET_CONNECT_ADAPTER_H
#define ARK_NET_CONNECT_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkNetCapabilitiesAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetNetId() = 0;

    /*--ark web()--*/
    virtual uint32_t GetConnectType() = 0;

    /*--ark web()--*/
    virtual uint32_t GetConnectSubtype() = 0;
};

/*--ark web(source=webview)--*/
class ArkNetConnectionPropertiesAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetNetId() = 0;
};

/*--ark web(source=webcore)--*/
class ArkVpnListener : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnAvailable() = 0;
 
    /*--ark web()--*/
    virtual void OnLost() = 0;
};

/*--ark web(source=webcore)--*/
class ArkNetConnCallback : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t NetAvailable() = 0;

    /*--ark web()--*/
    virtual int32_t NetCapabilitiesChange(const uint32_t& netConnectType, const uint32_t& netConnectSubtype) = 0;

    /*--ark web()--*/
    virtual int32_t NetConnectionPropertiesChange() = 0;

    /*--ark web()--*/
    virtual int32_t NetUnavailable() = 0;

    /*--ark web()--*/
    virtual int32_t OnNetCapabilitiesChanged(
        const ArkWebRefPtr<ArkNetCapabilitiesAdapter> capabilites) = 0;

    /*--ark web()--*/
    virtual int32_t OnNetConnectionPropertiesChanged(
        const ArkWebRefPtr<ArkNetConnectionPropertiesAdapter> properties) = 0;
};

/*--ark web(source=webview)--*/
class ArkNetConnectAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t RegisterNetConnCallback(ArkWebRefPtr<ArkNetConnCallback> cb) = 0;

    /*--ark web()--*/
    virtual int32_t UnregisterNetConnCallback(int32_t id) = 0;

    /*--ark web()--*/
    virtual int32_t GetDefaultNetConnect(uint32_t& type, uint32_t& netConnectSubtype) = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetDnsServers() = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetDnsServersByNetId(int32_t netId) = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetDnsServersForVpn() = 0;
 
    /*--ark web()--*/
    virtual void RegisterVpnListener(ArkWebRefPtr<ArkVpnListener> cb) = 0;
 
    /*--ark web()--*/
    virtual void UnRegisterVpnListener() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_NET_CONNECT_ADAPTER_H

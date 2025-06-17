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

#include <gtest/gtest.h>

#include "core_service_client.h"
#include "cellular_data_client.h"

#define private public
#include "net_connect_adapter_impl.h"
#include "net_connect_utils.h"
#include "core_service_client.h"
#include "cellular_data_client.h"
#include "net_conn_client.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;
using namespace OHOS::Telephony;

namespace OHOS {
namespace {
int32_t g_regNetConnCallback = 0;
int32_t g_unregNetConnCallback = 0;
int32_t g_getDefaultNet = 0;
int32_t g_getNetCap = 0;
int32_t g_slotId = 0;
int32_t g_getNetProp = 0;
int32_t g_getAllNets = 0;
int32_t g_specified_net_id = 100;
int32_t g_vpnCount = 0;
using Want = OHOS::AAFwk::Want;
const int32_t DEFAULT_VALUE = -1;
const int32_t TEST_BEARER_VPN = 4;
}

namespace NetManagerStandard {
int32_t NetConnClient::RegisterNetConnCallback(const sptr<INetConnCallback> callback)
{
    return g_regNetConnCallback;
}
int32_t NetConnClient::UnregisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    return g_unregNetConnCallback;
}
int32_t NetConnClient::GetDefaultNet(NetHandle &nethamdle)
{
    return g_getDefaultNet;
}
int32_t NetConnClient::GetNetCapabilities(const NetHandle &nethamdle, NetAllCapabilities &netAllCap)
{
    netAllCap.bearerTypes_.insert(NetBearType::BEARER_WIFI);
    netAllCap.bearerTypes_.insert(NetBearType::BEARER_CELLULAR);
    if (g_vpnCount != 0) {
        netAllCap.bearerTypes_.insert(NetBearType::BEARER_VPN);
    }
    return g_getNetCap;
}
int32_t NetConnClient::GetConnectionProperties(const NetHandle &nethandle, NetLinkInfo &netLinkInfo)
{
    if (g_getNetProp != 0) {
        return g_getNetProp;
    }
    INetAddr dns;
    dns.type_ = INetAddr::IPV4;
    dns.family_ = 0x10;
    dns.prefixlen_ = 0x17;
    dns.address_ = "192.168.2.1";
    dns.netMask_ = "192.255.255.255";
    dns.hostName_ = "netAddr";
    netLinkInfo.dnsList_.push_back(dns);
    return g_getNetProp;
}
int32_t NetConnClient::GetAllNets(std::list<sptr<NetHandle>> &netList)
{
    if (g_getAllNets != 0) {
        return g_getAllNets;
    }
    sptr<NetManagerStandard::NetHandle> netHandle = std::make_unique<NetHandle>(g_specified_net_id).release();
    if (netHandle != nullptr) {
        netList.emplace_back(netHandle);
    }
    return g_getAllNets;
}
}

namespace Telephony {
int32_t CellularDataClient::GetDefaultCellularDataSlotId()
{
    return g_slotId;
}
}

namespace NWeb {
using namespace OHOS::NetManagerStandard;
class NetConnectAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NetConnectAdapterImplTest::SetUpTestCase(void)
{}

void NetConnectAdapterImplTest::TearDownTestCase(void)
{}

void NetConnectAdapterImplTest::SetUp(void)
{}

void NetConnectAdapterImplTest::TearDown(void)
{}

class NetConnCallbackTest : public NetConnCallback {
public:
    NetConnCallbackTest() = default;
    virtual ~NetConnCallbackTest() = default;
    int32_t NetAvailable() override
    {
        return 0;
    }
    int32_t NetCapabilitiesChange(const NetConnectType &netConnectType,
        const NetConnectSubtype &netConnectSubtype) override
    {
        return 0;
    }
    int32_t NetConnectionPropertiesChange() override
    {
        return 0;
    }
    int32_t NetUnavailable() override
    {
        return 0;
    }
    int32_t OnNetCapabilitiesChanged(
        const std::shared_ptr<NetCapabilitiesAdapter> capabilities) override
    {
        return 0;
    }
    int32_t OnNetConnectionPropertiesChanged(
        const std::shared_ptr<NetConnectionPropertiesAdapter> properties) override
    {
        return 0;
    }
};

class VpnListenerTest : public VpnListener {
public:
    VpnListenerTest() = default;
    virtual ~VpnListenerTest() = default;

    void OnAvailable() override
    {
        return;
    }

    void OnLost() override
    {
        return;
    }
};

/**
 * @tc.name: NetConnectAdapterImplTest_001.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_001, TestSize.Level1)
{
    std::shared_ptr<NetConnCallbackTest> cb = std::make_shared<NetConnCallbackTest>();
    std::shared_ptr<NetConnectAdapterImpl> netConnectAdapterImpl = std::make_shared<NetConnectAdapterImpl>();
    EXPECT_NE(netConnectAdapterImpl, nullptr);
    g_regNetConnCallback = static_cast<int32_t>(NETMANAGER_SUCCESS);
    EXPECT_EQ(netConnectAdapterImpl->RegisterNetConnCallback(cb), 0);
    g_regNetConnCallback = -1;
    EXPECT_EQ(netConnectAdapterImpl->RegisterNetConnCallback(nullptr), -1);
    g_unregNetConnCallback = static_cast<int32_t>(NETMANAGER_SUCCESS);
    EXPECT_EQ(netConnectAdapterImpl->UnregisterNetConnCallback(0), 0);
    EXPECT_EQ(netConnectAdapterImpl->UnregisterNetConnCallback(-1), -1);

    g_regNetConnCallback = static_cast<int32_t>(NETMANAGER_SUCCESS);
    EXPECT_EQ(netConnectAdapterImpl->RegisterNetConnCallback(cb), 1);
    g_unregNetConnCallback = -1;
    EXPECT_EQ(netConnectAdapterImpl->UnregisterNetConnCallback(1), -1);
}

/**
 * @tc.name: NetConnectAdapterImplTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_002, TestSize.Level1)
{
    std::shared_ptr<NetConnectAdapterImpl> netConnectAdapterImpl = std::make_shared<NetConnectAdapterImpl>();
    NetConnectType type = NetConnectType::CONNECTION_UNKNOWN;
    NetConnectSubtype subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    g_getDefaultNet = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_getNetCap = static_cast<int32_t>(NETMANAGER_SUCCESS);
    EXPECT_EQ(netConnectAdapterImpl->GetDefaultNetConnect(type, subtype), 0);
    g_slotId = -1;
    EXPECT_EQ(netConnectAdapterImpl->GetDefaultNetConnect(type, subtype), 0);
    g_getDefaultNet = -1;
    EXPECT_EQ(netConnectAdapterImpl->GetDefaultNetConnect(type, subtype), -1);
    g_getDefaultNet = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_getNetCap = -1;
    EXPECT_EQ(netConnectAdapterImpl->GetDefaultNetConnect(type, subtype), -1);
}

/**
 * @tc.name: NetConnectAdapterImplTest_003.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_003, TestSize.Level1)
{
    std::shared_ptr<NetConnectAdapterImpl> netConnectAdapterImpl = std::make_shared<NetConnectAdapterImpl>();
    g_getDefaultNet = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_getNetProp = static_cast<int32_t>(NETMANAGER_SUCCESS);
    std::vector<std::string> dns_servers = netConnectAdapterImpl->GetDnsServers();
    EXPECT_EQ(dns_servers.size(), 1);
    std::string dns_ip_str("192.168.2.1");
    EXPECT_EQ(dns_servers.front(), dns_ip_str);
    g_getDefaultNet = -1;
    dns_servers = netConnectAdapterImpl->GetDnsServers();
    EXPECT_EQ(dns_servers.size(), 0);
    g_getNetProp = -1;
    dns_servers = netConnectAdapterImpl->GetDnsServers();
    EXPECT_EQ(dns_servers.size(), 0);
}

/**
 * @tc.name: NetConnectAdapterImplTest_004.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_004, TestSize.Level1)
{
    std::shared_ptr<NetConnectAdapterImpl> netConnectAdapterImpl = std::make_shared<NetConnectAdapterImpl>();
    g_getDefaultNet = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_getNetProp = static_cast<int32_t>(NETMANAGER_SUCCESS);
    std::vector<std::string> dns_servers = netConnectAdapterImpl->GetDnsServersByNetId(-1);
    EXPECT_EQ(dns_servers.size(), 1);
    std::string dns_ip_str("192.168.2.1");
    EXPECT_EQ(dns_servers.front(), dns_ip_str);
    g_getDefaultNet = -1;
    dns_servers = netConnectAdapterImpl->GetDnsServersByNetId(-1);
    EXPECT_EQ(dns_servers.size(), 0);
    g_getAllNets = static_cast<int32_t>(NETMANAGER_SUCCESS);
    dns_servers = netConnectAdapterImpl->GetDnsServersByNetId(100);
    EXPECT_EQ(dns_servers.size(), 1);
    EXPECT_EQ(dns_servers.front(), dns_ip_str);
    dns_servers = netConnectAdapterImpl->GetDnsServersByNetId(101);
    EXPECT_EQ(dns_servers.size(), 0);
    g_getNetProp = -1;
    dns_servers = netConnectAdapterImpl->GetDnsServersByNetId(100);
    EXPECT_EQ(dns_servers.size(), 0);
}

/**
 * @tc.name: NetConnectAdapterImplTest_005.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_005, TestSize.Level1)
{
    std::shared_ptr<NetConnectAdapterImpl> netConnectAdapterImpl = std::make_shared<NetConnectAdapterImpl>();
    EXPECT_NE(netConnectAdapterImpl, nullptr);
    g_getAllNets = static_cast<int32_t>(NETMANAGER_ERR_PERMISSION_DENIED);
    std::vector<std::string> dns_servers = netConnectAdapterImpl->GetDnsServersForVpn();
    EXPECT_EQ(dns_servers.size(), 0);
    g_getAllNets = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_getNetProp = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_vpnCount = 1;
    dns_servers = netConnectAdapterImpl->GetDnsServersForVpn();
    EXPECT_EQ(dns_servers.size(), 1);
    std::string dns_ip_str("192.168.2.1");
    EXPECT_EQ(dns_servers.front(), dns_ip_str);
    g_vpnCount = 0;
    dns_servers = netConnectAdapterImpl->GetDnsServersForVpn();
    EXPECT_EQ(dns_servers.size(), 0);
}

/**
 * @tc.name: NetConnectAdapterImplTest_006.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_006, TestSize.Level1)
{
    std::shared_ptr<NetConnectAdapterImpl> netConnectAdapterImpl = std::make_shared<NetConnectAdapterImpl>();
    EXPECT_NE(netConnectAdapterImpl, nullptr);
    g_getAllNets = static_cast<int32_t>(NETMANAGER_ERR_PERMISSION_DENIED);
    bool result = netConnectAdapterImpl->HasVpnTransport();
    EXPECT_EQ(result, false);
    g_getAllNets = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_getNetProp = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_vpnCount = 1;
    result = netConnectAdapterImpl->HasVpnTransport();
    EXPECT_EQ(result, true);
    g_vpnCount = 0;
    result = netConnectAdapterImpl->HasVpnTransport();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: NetConnectAdapterImplTest_007.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_007, TestSize.Level1)
{
    std::shared_ptr<VpnListenerTest> cb = std::make_shared<VpnListenerTest>();
    std::shared_ptr<NetConnectAdapterImpl> netConnectAdapterImpl = std::make_shared<NetConnectAdapterImpl>();
    EXPECT_NE(netConnectAdapterImpl, nullptr);
    netConnectAdapterImpl->RegisterVpnListener(nullptr);
    netConnectAdapterImpl->UnRegisterVpnListener();
    g_getAllNets = static_cast<int32_t>(NETMANAGER_ERR_PERMISSION_DENIED);
    netConnectAdapterImpl->RegisterVpnListener(cb);
    netConnectAdapterImpl->UnRegisterVpnListener();
    g_getAllNets = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_getNetProp = static_cast<int32_t>(NETMANAGER_SUCCESS);
    g_vpnCount = 1;
    netConnectAdapterImpl->RegisterVpnListener(cb);
    netConnectAdapterImpl->UnRegisterVpnListener();
}

/**
 * @tc.name: NetConnectAdapterImplTest_008.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterImplTest, NetConnectAdapterImplTest_008, TestSize.Level1)
{
    EventFwk::CommonEventSubscribeInfo in;
    std::shared_ptr<VpnListenerTest> cb = std::make_shared<VpnListenerTest>();
    std::shared_ptr<NetVPNEventSubscriber> subscribe = std::make_shared<NetVPNEventSubscriber>(in, cb);
    EXPECT_NE(subscribe, nullptr);
    Want want;
    want.SetAction("test");
    EventFwk::CommonEventData data(want);
    subscribe->OnReceiveEvent(data);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    want.SetParam("NetType", DEFAULT_VALUE);
    EventFwk::CommonEventData data1(want);
    data1.SetCode(NetManagerStandard::NetConnState::NET_CONN_STATE_CONNECTED);
    subscribe->OnReceiveEvent(data1);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    want.SetParam("NetType", 1);
    EventFwk::CommonEventData data2(want);
    data2.SetCode(NetManagerStandard::NetConnState::NET_CONN_STATE_CONNECTED);
    subscribe->OnReceiveEvent(data2);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    want.SetParam("NetType", TEST_BEARER_VPN);
    EventFwk::CommonEventData data3(want);
    data3.SetCode(NetManagerStandard::NetConnState::NET_CONN_STATE_CONNECTED);
    subscribe->OnReceiveEvent(data3);

    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE);
    want.SetParam("NetType", TEST_BEARER_VPN);
    EventFwk::CommonEventData data4(want);
    data4.SetCode(NetManagerStandard::NetConnState::NET_CONN_STATE_DISCONNECTED);
    subscribe->OnReceiveEvent(data4);
}
}
}

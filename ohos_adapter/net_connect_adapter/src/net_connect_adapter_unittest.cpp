/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <testing/gtest/include/gtest/gtest.h>

#include <ohos_adapter_helper.h>
#define private public
#include <net_connect_adapter_impl.h>

namespace {
    int32_t g_regNetConnCallback = 0;
    int32_t g_unregNetConnCallback = 0;
    int32_t g_getDefaultNet = 0;
    int32_t g_getNetCap = 0;
    int32_t g_slotId = 0;
    Telephony_RadioResult g_radioResult = Telephony_RadioResult::TEL_RADIO_SUCCESS;
    int32_t g_getNetProp = 0;
    int32_t g_getAllNets = 0;
    int32_t g_specified_net_id = 100;
}

int32_t OH_NetConn_RegisterDefaultNetConnCallback(NetConn_NetConnCallback *cb, uint32_t *callbackId)
{
    static uint32_t count = 0;
    *callbackId = count++;
    return g_regNetConnCallback;
}

int32_t OH_NetConn_UnregisterNetConnCallback(uint32_t callbackId)
{
    return g_unregNetConnCallback;
}

int32_t OH_NetConn_GetNetCapabilities(NetConn_NetHandle *netHandle, NetConn_NetCapabilities *netCapabilities)
{
    netCapabilities->bearerTypes[0] = NetConn_NetBearerType::NETCONN_BEARER_WIFI;
    netCapabilities->bearerTypes[1] = NetConn_NetBearerType::NETCONN_BEARER_CELLULAR;
    return g_getNetCap;
}

int32_t OH_Telephony_GetDefaultCellularDataSlotId()
{
    return g_slotId;
}

Telephony_RadioResult OH_Telephony_GetNetworkStateForSlot(int32_t slotId, Telephony_NetworkState *state)
{
    return g_radioResult;
}

int32_t OH_NetConn_GetDefaultNet(NetConn_NetHandle *netHandle)
{
    return g_getDefaultNet;
}

int32_t OH_NetConn_GetConnectionProperties(NetConn_NetHandle *netHandle, NetConn_ConnectionProperties *prop)
{
    if (g_getNetProp != 0) {
        return g_getNetProp;
    }
    NetConn_NetAddr netAddr;
    std::string addr = "192.168.2.1";
    for (int i = 0; i < addr.size(); i++) {
        netAddr.address[i] = addr[i];
    }
    prop->dnsList[0] = netAddr;
    return g_getNetProp;
}

int32_t OH_NetConn_GetAllNets(NetConn_NetHandleList *netHandleList)
{
    if (g_getAllNets != 0) {
        return g_getAllNets;
    }
    NetConn_NetHandle netHandle;
    netHandle.netId = g_specified_net_id;
    netHandleList->netHandles[0] = netHandle;
    netHandleList->netHandleListSize = 1;
    return g_getAllNets;
}

namespace OHOS {
namespace NWeb {
class NetConnectAdapterTest : public testing::Test {
protected:
    NetConnectAdapterTest() {}

    ~NetConnectAdapterTest() override {}

    void SetUp() override {}

    void TearDown() override {}
};

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

/**
 * @tc.name: NetConnectAdapterTest_001.
 * @tc.desc: test RegisterNetConnCallback.
 * @tc.type: FUNC
 */
TEST_F(NetConnectAdapterTest, NetConnectAdapterTest_001)
{
    std::shared_ptr<NetConnCallbackTest> cb = std::make_shared<NetConnCallbackTest>();
    std::unique_ptr<NetConnectAdapter> adapter = OhosAdapterHelper::GetInstance().CreateNetConnectAdapter();
    EXPECT_NE(adapter, nullptr);
    g_regNetConnCallback = static_cast<int32_t>(0);
    EXPECT_EQ(adapter->RegisterNetConnCallback(cb), 0);
    g_regNetConnCallback = -1;
    EXPECT_EQ(adapter->RegisterNetConnCallback(nullptr), -1);
    g_unregNetConnCallback = static_cast<int32_t>(0);
    EXPECT_EQ(adapter->UnregisterNetConnCallback(0), 0);
    EXPECT_EQ(adapter->UnregisterNetConnCallback(-1), -1);

    g_regNetConnCallback = static_cast<int32_t>(0);
    EXPECT_EQ(adapter->RegisterNetConnCallback(cb), 1);
    g_unregNetConnCallback = -1;
    EXPECT_EQ(adapter->UnregisterNetConnCallback(1), -1);
}

/**
 * @tc.name: NetConnectAdapterTest_002.
 * @tc.desc: test GetDefaultNetConnect.
 * @tc.type: FUNC
 */
TEST_F(NetConnectAdapterTest, NetConnectAdapterTest_002)
{
    std::unique_ptr<NetConnectAdapter> adapter = OhosAdapterHelper::GetInstance().CreateNetConnectAdapter();
    NetConnectType type = NetConnectType::CONNECTION_UNKNOWN;
    NetConnectSubtype subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    g_getDefaultNet = static_cast<int32_t>(0);
    g_getNetCap = static_cast<int32_t>(0);
    EXPECT_EQ(adapter->GetDefaultNetConnect(type, subtype), 0);
    g_slotId = -1;
    EXPECT_EQ(adapter->GetDefaultNetConnect(type, subtype), 0);
    g_getDefaultNet = -1;
    EXPECT_EQ(adapter->GetDefaultNetConnect(type, subtype), -1);
    g_getDefaultNet = static_cast<int32_t>(0);
    g_getNetCap = -1;
    EXPECT_EQ(adapter->GetDefaultNetConnect(type, subtype), -1);
}

/**
 * @tc.name: NetConnectAdapterTest_003.
 * @tc.desc: test GetDnsServers.
 * @tc.type: FUNC
 */
TEST_F(NetConnectAdapterTest, NetConnectAdapterTest_003)
{
    std::unique_ptr<NetConnectAdapter> adapter = OhosAdapterHelper::GetInstance().CreateNetConnectAdapter();
    g_getDefaultNet = static_cast<int32_t>(0);
    g_getNetProp = static_cast<int32_t>(0);
    std::vector<std::string> dns_servers = adapter->GetDnsServers();
    std::string dns_ip_str("192.168.2.1");
    EXPECT_EQ(dns_servers.front(), dns_ip_str);
}

/**
 * @tc.name: NetConnectAdapterTest_004.
 * @tc.desc: test GetDnsServersByNetId.
 * @tc.type: FUNC
 */
TEST_F(NetConnectAdapterTest, NetConnectAdapterTest_004)
{
    std::unique_ptr<NetConnectAdapter> adapter = OhosAdapterHelper::GetInstance().CreateNetConnectAdapter();
    g_getDefaultNet = static_cast<int32_t>(0);
    g_getNetProp = static_cast<int32_t>(0);
    std::vector<std::string> dns_servers = adapter->GetDnsServersByNetId(-1);
    std::string dns_ip_str("192.168.2.1");
    EXPECT_EQ(dns_servers.front(), dns_ip_str);
    g_getDefaultNet = -1;
    dns_servers = adapter->GetDnsServersByNetId(-1);
    g_getAllNets = static_cast<int32_t>(0);
    int32_t netId = 100;
    dns_servers = adapter->GetDnsServersByNetId(netId);
    EXPECT_EQ(dns_servers.front(), dns_ip_str);
}
}
}

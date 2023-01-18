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
}

namespace NetManagerStandard {
int32_t NetConnClient::RegisterNetConnCallback(const sptr<INetConnCallback> &callback)
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
    return g_getNetCap;
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
    EXPECT_EQ(netConnectAdapterImpl->RegisterNetConnCallback(cb), -1);
    EXPECT_EQ(netConnectAdapterImpl->RegisterNetConnCallback(nullptr), -1);
    g_unregNetConnCallback = static_cast<int32_t>(NETMANAGER_SUCCESS);
    EXPECT_EQ(netConnectAdapterImpl->UnregisterNetConnCallback(cb), 0);
    EXPECT_EQ(netConnectAdapterImpl->UnregisterNetConnCallback(cb), -1);

    g_regNetConnCallback = static_cast<int32_t>(NETMANAGER_SUCCESS);
    EXPECT_EQ(netConnectAdapterImpl->RegisterNetConnCallback(cb), 0);
    g_unregNetConnCallback = -1;
    EXPECT_EQ(netConnectAdapterImpl->UnregisterNetConnCallback(cb), -1);
    EXPECT_EQ(netConnectAdapterImpl->UnregisterNetConnCallback(nullptr), -1);
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
}
}
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
#include "net_connect_callback_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;
using namespace OHOS::Telephony;

namespace OHOS {
namespace Telephony {
namespace {
int32_t g_slotId = 0;
}

int32_t CellularDataClient::GetDefaultCellularDataSlotId()
{
    return g_slotId;
}
}
namespace NWeb {
using namespace OHOS::NetManagerStandard;
class NetConnectCallbackImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NetConnectCallbackImplTest::SetUpTestCase(void)
{}

void NetConnectCallbackImplTest::TearDownTestCase(void)
{}

void NetConnectCallbackImplTest::SetUp(void)
{}

void NetConnectCallbackImplTest::TearDown(void)
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
 * @tc.name: NetConnectCallbackImplTest_001.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectCallbackImplTest, NetConnectCallbackImplTest_001, TestSize.Level1)
{
    std::shared_ptr<NetConnCallbackTest> cb = std::make_shared<NetConnCallbackTest>();
    std::shared_ptr<NetConnectCallbackImpl> netConnectCallbackImpl = std::make_shared<NetConnectCallbackImpl>(cb);
    EXPECT_NE(netConnectCallbackImpl, nullptr);
    sptr<NetHandle> netHandle(new NetHandle);
    sptr<NetHandle> netHandleNull(nullptr);
    sptr<NetLinkInfo> info(new NetLinkInfo);
    sptr<NetLinkInfo> infoNull(nullptr);
    EXPECT_EQ(netConnectCallbackImpl->NetAvailable(netHandle), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetAvailable(netHandleNull), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetConnectionPropertiesChange(netHandle, info), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetConnectionPropertiesChange(netHandleNull, infoNull), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetConnectionPropertiesChange(netHandle, infoNull), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetConnectionPropertiesChange(netHandleNull, info), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetLost(netHandle), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetUnavailable(), 0);

    sptr<NetAllCapabilities> netAllCap(new NetAllCapabilities);
    netAllCap->bearerTypes_.insert(NetBearType::BEARER_WIFI);
    netAllCap->bearerTypes_.insert(NetBearType::BEARER_BLUETOOTH);
    netAllCap->bearerTypes_.insert(NetBearType::BEARER_ETHERNET);
    netAllCap->bearerTypes_.insert(NetBearType::BEARER_CELLULAR);
    EXPECT_EQ(netConnectCallbackImpl->NetCapabilitiesChange(netHandle, netAllCap), 0);
    g_slotId = -1;
    EXPECT_EQ(netConnectCallbackImpl->NetCapabilitiesChange(netHandle, netAllCap), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetCapabilitiesChange(netHandle, nullptr), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetCapabilitiesChange(netHandleNull, netAllCap), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetCapabilitiesChange(netHandleNull, nullptr), 0);
}

/**
 * @tc.name: NetConnectCallbackImplTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectCallbackImplTest, NetConnectCallbackImplTest_002, TestSize.Level1)
{
    std::shared_ptr<NetConnectCallbackImpl> netConnectCallbackImpl = std::make_shared<NetConnectCallbackImpl>(nullptr);
    sptr<NetHandle> netHandle(new NetHandle);
    sptr<NetLinkInfo> info(new NetLinkInfo);
    EXPECT_EQ(netConnectCallbackImpl->NetAvailable(netHandle), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetConnectionPropertiesChange(netHandle, info), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetLost(netHandle), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetUnavailable(), 0);
    EXPECT_EQ(netConnectCallbackImpl->NetBlockStatusChange(netHandle, false), 0);
    sptr<NetAllCapabilities> netAllCap(new NetAllCapabilities);
    netAllCap->bearerTypes_.insert(NetBearType::BEARER_WIFI);
    EXPECT_EQ(netConnectCallbackImpl->NetCapabilitiesChange(netHandle, netAllCap), 0);
}
}
}

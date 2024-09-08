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
#include <vector>
#include <unordered_map>
#include <gtest/gtest.h>
#define private public
#include "net_connect_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
using namespace OHOS::NetManagerStandard;
using namespace OHOS::Telephony;
class NetConnectAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NetConnectAdapterTest::SetUpTestCase(void)
{}

void NetConnectAdapterTest::TearDownTestCase(void)
{}

void NetConnectAdapterTest::SetUp(void)
{}

void NetConnectAdapterTest::TearDown(void)
{}

/**
 * @tc.name: NetConnectAdapterTest_001.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:issueI5ORNO
 */
HWTEST_F(NetConnectAdapterTest, NetConnectAdapterTest_001, TestSize.Level1)
{
    std::unordered_map<Telephony_RadioTechnology, NetConnectType> typeMap;
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN, NetConnectType::CONNECTION_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_IWLAN, NetConnectType::CONNECTION_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_GSM, NetConnectType::CONNECTION_2G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_1XRTT, NetConnectType::CONNECTION_2G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_WCDMA, NetConnectType::CONNECTION_3G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPA, NetConnectType::CONNECTION_3G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPAP, NetConnectType::CONNECTION_3G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_TD_SCDMA, NetConnectType::CONNECTION_3G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EVDO, NetConnectType::CONNECTION_3G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EHRPD, NetConnectType::CONNECTION_3G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE, NetConnectType::CONNECTION_4G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE_CA, NetConnectType::CONNECTION_4G));
    typeMap.insert(
        std::make_pair(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_NR, NetConnectType::CONNECTION_5G));

    for (auto iter : typeMap) {
        NetConnectType exceptType = iter.second;
        Telephony_RadioTechnology type = iter.first;
        NetConnectType actualType = OHOS::NWeb::NetConnectUtils::ConvertToConnectTypeInner(type);
        EXPECT_EQ(exceptType, actualType);
    }
    EXPECT_EQ(NetConnectType::CONNECTION_UNKNOWN,
        OHOS::NWeb::NetConnectUtils::ConvertToConnectTypeInner(static_cast<Telephony_RadioTechnology>(-1)));
}

/**
 * @tc.name: NetConnectAdapterTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterTest, NetConnectAdapterTest_002, TestSize.Level1)
{
    std::unordered_map<Telephony_RadioTechnology, NetConnectSubtype> typeMap;
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN, NetConnectSubtype::SUBTYPE_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_IWLAN, NetConnectSubtype::SUBTYPE_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_WCDMA, NetConnectSubtype::SUBTYPE_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_TD_SCDMA, NetConnectSubtype::SUBTYPE_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EVDO, NetConnectSubtype::SUBTYPE_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_NR, NetConnectSubtype::SUBTYPE_UNKNOWN));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_GSM, NetConnectSubtype::SUBTYPE_GSM));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_1XRTT, NetConnectSubtype::SUBTYPE_1XRTT));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPA, NetConnectSubtype::SUBTYPE_HSPA));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPAP, NetConnectSubtype::SUBTYPE_HSPAP));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EHRPD, NetConnectSubtype::SUBTYPE_EHRPD));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE, NetConnectSubtype::SUBTYPE_LTE));
    typeMap.insert(std::make_pair(
        Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE_CA, NetConnectSubtype::SUBTYPE_LTE_ADVANCED));
    for (auto iter : typeMap) {
        NetConnectSubtype exceptType = iter.second;
        Telephony_RadioTechnology type = iter.first;
        NetConnectSubtype actualType = OHOS::NWeb::NetConnectUtils::ConvertToConnectsubtype(type);
        EXPECT_EQ(exceptType, actualType);
    }
    EXPECT_EQ(NetConnectSubtype::SUBTYPE_UNKNOWN, OHOS::NWeb::NetConnectUtils::ConvertToConnectsubtype(
        static_cast<Telephony_RadioTechnology>(-1)));
}

/**
 * @tc.name: NetConnectAdapterTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterTest, NetConnectAdapterTest_003, TestSize.Level1)
{
    std::unordered_map<NetConn_NetBearType, OHOS::NWeb::NetConnectType> typeMap;
    typeMap.insert(std::make_pair(
        NetConn_NetBearType::NETCONN_BEARER_CELLULAR, NetConnectType::CONNECTION_UNKNOWN));
    typeMap.insert(std::make_pair(
        NetConn_NetBearType::NETCONN_BEARER_WIFI, NetConnectType::CONNECTION_WIFI));
    typeMap.insert(std::make_pair(
        NetConn_NetBearType::NETCONN_BEARER_BLUETOOTH, NetConnectType::CONNECTION_BLUETOOTH));
    typeMap.insert(std::make_pair(
        NetConn_NetBearType::NETCONN_BEARER_ETHERNET, NetConnectType::CONNECTION_ETHERNET));
    typeMap.insert(std::make_pair(
        NetConn_NetBearType::NETCONN_BEARER_VPN, NetConnectType::CONNECTION_UNKNOWN));
    typeMap.insert(std::make_pair(
        NetConn_NetBearType::NETCONN_BEARER_WIFI_AWARE, NetConnectType::CONNECTION_UNKNOWN));
    typeMap.insert(std::make_pair(
        NetConn_NetBearType::NETCONN_BEARER_DEFAULT, NetConnectType::CONNECTION_UNKNOWN));
    for (auto iter : typeMap) {
        NetConn_NetBearType NetConn_NetBearType = iter.first;
        NetConnectType exceptType = iter.second;
        NetConnectType actualType = OHOS::NWeb::NetConnectUtils::ConvertToConnectType(
            NetConn_NetBearType, Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN);
        EXPECT_EQ(exceptType, actualType);
    }
    const uint32_t BEARER_ELSE = 7;
    EXPECT_EQ(NetConnectType::CONNECTION_UNKNOWN, OHOS::NWeb::NetConnectUtils::ConvertToConnectType(
        static_cast<NetConn_NetBearType>(BEARER_ELSE), Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN));
}

/**
 * @tc.name: NetConnectAdapterTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NetConnectAdapterTest, NetConnectAdapterTest_004, TestSize.Level1)
{
    std::unordered_map<OHOS::NWeb::NetConnectType, std::string> typeMap;
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_UNKNOWN, "unknown"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_ETHERNET, "ethernet"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_WIFI, "wifi"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_2G, "2g"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_3G, "3g"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_4G, "4g"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_NONE, "none"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_BLUETOOTH, "bluetooth"));
    typeMap.insert(std::make_pair(NetConnectType::CONNECTION_5G, "5g"));
    for (auto iter : typeMap) {
        OHOS::NWeb::NetConnectType connectType = iter.first;
        std::string exceptStr = iter.second;
        std::string actualStr = OHOS::NWeb::NetConnectUtils::ConnectTypeToString(connectType);
        EXPECT_STREQ(actualStr.c_str(), exceptStr.c_str());
    }
}
}
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#include "base/web/webview/sa/web_native_messaging/common/connection_native_info_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {

class ConnectionNativeInfoParcelTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<Parcel> parcel;
    ConnectionNativeInfo connectionInfo;

    int32_t TEST_CONNECTION_ID = 123;
    int32_t TEST_EXTENSION_PID = 456;
    const std::string TEST_BUNDLENAME = "com.test.app";
    const std::string TEST_EXTENSION_ORIGIN = "http://test.com";
};

void ConnectionNativeInfoParcelTest::SetUpTestCase(void) {}

void ConnectionNativeInfoParcelTest::TearDownTestCase(void) {}

void ConnectionNativeInfoParcelTest::SetUp(void)
{
    parcel = std::make_shared<Parcel>();
    connectionInfo.connectionId = TEST_CONNECTION_ID;
    connectionInfo.bundleName = TEST_BUNDLENAME;
    connectionInfo.extensionOrigin = TEST_EXTENSION_ORIGIN;
    connectionInfo.extensionPid = TEST_EXTENSION_PID;
}

void ConnectionNativeInfoParcelTest::TearDown(void) {}

/**
 * @tc.name  : OHConnectionNativeInfoParcelTest_Marshalling
 * @tc.desc  : Test Marshalling
 */
HWTEST_F(ConnectionNativeInfoParcelTest,
         OHConnectionNativeInfoParcelTest_Marshalling,
         TestSize.Level1)
{
    ConnectionNativeInfoParcel infoParcel;
    infoParcel.connectionNativeInfo_ = connectionInfo;

    bool result = infoParcel.Marshalling(*parcel);
    EXPECT_TRUE(result);
}

/**
 * @tc.name  : OHConnectionNativeInfoParcelTest_Unmarshalling_01
 * @tc.desc  : Test Unmarshalling
 */
HWTEST_F(ConnectionNativeInfoParcelTest,
         OHConnectionNativeInfoParcelTest_Unmarshalling_01,
         TestSize.Level1)
{
    parcel->WriteInt32(TEST_CONNECTION_ID);
    parcel->WriteString(TEST_BUNDLENAME);
    parcel->WriteString(TEST_EXTENSION_ORIGIN);
    parcel->WriteInt32(TEST_EXTENSION_PID);

    ConnectionNativeInfoParcel* infoParcel = nullptr;

    infoParcel = ConnectionNativeInfoParcel::Unmarshalling(*parcel);
    EXPECT_NE(infoParcel, nullptr);

    if (infoParcel != nullptr) {
        const ConnectionNativeInfo& info = infoParcel->connectionNativeInfo_;
        EXPECT_EQ(info.connectionId, TEST_CONNECTION_ID);
        EXPECT_EQ(info.bundleName, TEST_BUNDLENAME);
        EXPECT_EQ(info.extensionOrigin, TEST_EXTENSION_ORIGIN);
        EXPECT_EQ(info.extensionPid, TEST_EXTENSION_PID);
        delete infoParcel;
    }
}

/**
 * @tc.name  : OHConnectionNativeInfoParcelTest_Unmarshalling_02
 * @tc.desc  : Test Unmarshalling
 */
HWTEST_F(ConnectionNativeInfoParcelTest,
         OHConnectionNativeInfoParcelTest_Unmarshalling_02,
         TestSize.Level1)
{
    parcel->WriteInt32(TEST_CONNECTION_ID);
    parcel->WriteString("");
    parcel->WriteString(TEST_EXTENSION_ORIGIN);
    parcel->WriteInt32(TEST_EXTENSION_PID);

    ConnectionNativeInfoParcel* infoParcel = nullptr;

    infoParcel = ConnectionNativeInfoParcel::Unmarshalling(*parcel);
    EXPECT_EQ(infoParcel, nullptr);
}

/**
 * @tc.name  : OHConnectionNativeInfoParcelTest_Unmarshalling_03
 * @tc.desc  : Test Unmarshalling
 */
HWTEST_F(ConnectionNativeInfoParcelTest,
         OHConnectionNativeInfoParcelTest_Unmarshalling_03,
         TestSize.Level1)
{
    parcel->WriteInt32(TEST_CONNECTION_ID);
    parcel->WriteString(TEST_BUNDLENAME);
    parcel->WriteString("");
    parcel->WriteInt32(TEST_EXTENSION_PID);

    ConnectionNativeInfoParcel* infoParcel = nullptr;

    infoParcel = ConnectionNativeInfoParcel::Unmarshalling(*parcel);
    EXPECT_EQ(infoParcel, nullptr);
}

} // namespace NWeb
} // namespace OHOS
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

#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <unistd.h>

#include "message_parcel.h"
#include "web_native_messaging_extension_connect_info.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;

class WebNativeMessagingConnectInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}

    void SetUp(void) override
    {
        int fd = open("/dev/null", O_RDWR);
        validFd_ = (fd > 0) ? fd : 1;
    }

    void TearDown(void) override
    {
        if (validFd_ != 1) {
            close(validFd_);
        }
    }

protected:
    int validFd_ = 1;
};

HWTEST_F(WebNativeMessagingConnectInfoTest, Marshalling_Unmarshalling_ValidAll_001, TestSize.Level1)
{
    WNMEConnectionInfoParcel parcel;
    parcel.conn_info_ = { .connectionId = 1001,
        .bundleName = "com.test.app",
        .extensionOrigin = "https://test.com",
        .fdRead = validFd_,
        .fdWrite = validFd_ };
    MessageParcel out;
    bool marshalResult = parcel.Marshalling(out);
    EXPECT_TRUE(marshalResult);
    WNMEConnectionInfo connInfo;
    bool unmarshalResult = WNMEConnectionInfoParcel::Unmarshalling(out, connInfo);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_EQ(connInfo.connectionId, 1001);
    EXPECT_EQ(connInfo.bundleName, "com.test.app");
    EXPECT_EQ(connInfo.extensionOrigin, "https://test.com");
    EXPECT_GT(connInfo.fdRead, 0);
    EXPECT_GT(connInfo.fdWrite, 0);
}

HWTEST_F(WebNativeMessagingConnectInfoTest, Marshalling_Unmarshalling_IgnoreFd_002, TestSize.Level1)
{
    WNMEConnectionInfoParcel parcel;
    parcel.conn_info_ = { .connectionId = 1002,
        .bundleName = "com.test.app2",
        .extensionOrigin = "https://test2.com",
        .fdRead = -1,
        .fdWrite = -1 };
    MessageParcel out;
    bool marshalResult = parcel.Marshalling(out);
    EXPECT_TRUE(marshalResult);
    WNMEConnectionInfo connInfo;
    bool unmarshalResult = WNMEConnectionInfoParcel::Unmarshalling(out, connInfo);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_EQ(connInfo.connectionId, 1002);
    EXPECT_EQ(connInfo.bundleName, "com.test.app2");
    EXPECT_EQ(connInfo.extensionOrigin, "https://test2.com");
    EXPECT_EQ(connInfo.fdRead, -1);
    EXPECT_EQ(connInfo.fdWrite, -1);
}

HWTEST_F(WebNativeMessagingConnectInfoTest, Marshalling_Unmarshalling_PartialInvalidFd_003, TestSize.Level1)
{
    WNMEConnectionInfoParcel parcel;
    parcel.conn_info_ = { .connectionId = 1003,
        .bundleName = "com.test.app3",
        .extensionOrigin = "https://test3.com",
        .fdRead = validFd_,
        .fdWrite = -1 };
    MessageParcel out;
    bool marshalResult = parcel.Marshalling(out);
    EXPECT_TRUE(marshalResult);
    WNMEConnectionInfo connInfo;
    bool unmarshalResult = WNMEConnectionInfoParcel::Unmarshalling(out, connInfo);
    EXPECT_TRUE(unmarshalResult);
    EXPECT_EQ(connInfo.fdRead, -1);
    EXPECT_EQ(connInfo.fdWrite, -1);
}

HWTEST_F(WebNativeMessagingConnectInfoTest, Unmarshalling_Fail_MissingField_004, TestSize.Level1)
{
    MessageParcel out;
    out.WriteInt32(1004);
    WNMEConnectionInfo connInfo;
    bool unmarshalResult = WNMEConnectionInfoParcel::Unmarshalling(out, connInfo);
    EXPECT_FALSE(unmarshalResult);
}

HWTEST_F(WebNativeMessagingConnectInfoTest, Unmarshalling_Fail_InvalidFd_005, TestSize.Level1)
{
    WNMEConnectionInfoParcel parcel;
    parcel.conn_info_ = { .connectionId = 1005,
        .bundleName = "com.test.app5",
        .extensionOrigin = "https://test5.com",
        .fdRead = validFd_,
        .fdWrite = validFd_ };
    MessageParcel out;
    EXPECT_TRUE(parcel.Marshalling(out));
    out.RewindRead(0);
    size_t dataSize = out.GetDataSize() - 8;
    out.SetDataSize(dataSize);
    WNMEConnectionInfo connInfo;
    bool unmarshalResult = WNMEConnectionInfoParcel::Unmarshalling(out, connInfo);
    EXPECT_FALSE(unmarshalResult);
}

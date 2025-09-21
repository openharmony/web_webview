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

#include "connection_native_info_parcel.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "iremote_object.h"
#include "web_extension_connection_callback_stub.h"

using namespace testing;
using namespace OHOS;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
bool ConnectionNativeInfoParcel::Marshalling(Parcel& out) const
{
    return out.WriteInt32(connectionNativeInfo_.connectionId);
}

ConnectionNativeInfoParcel* ConnectionNativeInfoParcel::Unmarshalling(Parcel& in)
{
    auto* parcel = new (std::nothrow) ConnectionNativeInfoParcel();
    if (parcel == nullptr) {
        return nullptr;
    }
    if (!in.ReadInt32(parcel->connectionNativeInfo_.connectionId)) {
        delete parcel;
        return nullptr;
    }
    return parcel;
}
} // namespace OHOS::NWeb

class MockWebExtensionConnectionCallbackStub : public WebExtensionConnectionCallbackStub {
public:
    MOCK_METHOD(void, OnConnect, (ConnectionNativeInfo & info), (override));
    MOCK_METHOD(void, OnDisconnect, (ConnectionNativeInfo & info), (override));
    MOCK_METHOD(void, OnFailed, (int32_t errorNum), (override));
};

class WebExtensionConnectionCallbackStubTest : public Test {
protected:
    void SetUp() override
    {
        stub_ = new MockWebExtensionConnectionCallbackStub();
        data_ = new MessageParcel();
        reply_ = new MessageParcel();
        option_ = new MessageOption();
        validInfo_.connectionId = 1001;
    }

    void TearDown() override
    {
        delete stub_;
        delete data_;
        delete reply_;
        delete option_;
    }

    void WriteValidDescriptor()
    {
        data_->WriteInterfaceToken(IWebExtensionConnectionCallback::GetDescriptor());
    }

    sptr<ConnectionNativeInfoParcel> CreateValidParcel()
    {
        sptr<ConnectionNativeInfoParcel> parcel = new ConnectionNativeInfoParcel();
        parcel->connectionNativeInfo_ = validInfo_;
        return parcel;
    }

    MockWebExtensionConnectionCallbackStub* stub_;
    MessageParcel* data_;
    MessageParcel* reply_;
    MessageOption* option_;
    ConnectionNativeInfo validInfo_;
};

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_001
 * @tc.desc: Verify handling of invalid descriptor
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_InvalidDescriptor)
{
    data_->WriteInterfaceToken(u"invalid.descriptor");
    int32_t result =
        stub_->OnRemoteRequest(IWebExtensionConnectionCallback::ON_CONNECT_CODE, *data_, *reply_, *option_);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_002
 * @tc.desc: Verify ON_CONNECT_CODE with valid parameters
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_OnConnectCode_Valid)
{
    WriteValidDescriptor();
    sptr<ConnectionNativeInfoParcel> parcel = CreateValidParcel();
    ASSERT_TRUE(data_->WriteParcelable(parcel.GetRefPtr()));
    EXPECT_CALL(*stub_, OnConnect(_)).Times(1);
    int32_t result =
        stub_->OnRemoteRequest(IWebExtensionConnectionCallback::ON_CONNECT_CODE, *data_, *reply_, *option_);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_003
 * @tc.desc: Verify ON_CONNECT_CODE with invalid parcel
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_OnConnectCode_InvalidParcel)
{
    WriteValidDescriptor();
    data_->WriteParcelable(nullptr);
    EXPECT_CALL(*stub_, OnConnect(_)).Times(0);
    int32_t result =
        stub_->OnRemoteRequest(IWebExtensionConnectionCallback::ON_CONNECT_CODE, *data_, *reply_, *option_);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_004
 * @tc.desc: Verify ON_DISCONNECT_CODE with valid parameters
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_OnDisconnectCode_Valid)
{
    WriteValidDescriptor();
    sptr<ConnectionNativeInfoParcel> parcel = CreateValidParcel();
    ASSERT_TRUE(data_->WriteParcelable(parcel.GetRefPtr()));
    EXPECT_CALL(*stub_, OnDisconnect(_)).Times(1);
    int32_t result =
        stub_->OnRemoteRequest(IWebExtensionConnectionCallback::ON_DISCONNECT_CODE, *data_, *reply_, *option_);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_005
 * @tc.desc: Verify ON_DISCONNECT_CODE with invalid parcel
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_OnDisconnectCode_InvalidParcel)
{
    WriteValidDescriptor();
    data_->WriteParcelable(nullptr);
    EXPECT_CALL(*stub_, OnDisconnect(_)).Times(0);
    int32_t result =
        stub_->OnRemoteRequest(IWebExtensionConnectionCallback::ON_DISCONNECT_CODE, *data_, *reply_, *option_);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_006
 * @tc.desc: Verify ON_FAILED_CODE with valid parameters
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_OnFailedCode_Valid)
{
    WriteValidDescriptor();
    const int32_t testError = 500;
    data_->WriteInt32(testError);
    EXPECT_CALL(*stub_, OnFailed(testError)).Times(1);
    int32_t result = stub_->OnRemoteRequest(IWebExtensionConnectionCallback::ON_FAILED_CODE, *data_, *reply_, *option_);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_007
 * @tc.desc: Verify ON_FAILED_CODE with invalid parameters
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_OnFailedCode_InvalidParam)
{
    WriteValidDescriptor();
    EXPECT_CALL(*stub_, OnFailed(_)).Times(0);
    int32_t result = stub_->OnRemoteRequest(IWebExtensionConnectionCallback::ON_FAILED_CODE, *data_, *reply_, *option_);
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: WebExtensionConnectionCallbackStubTest_OnRemoteRequest_008
 * @tc.desc: Verify default handling for unknown code
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebExtensionConnectionCallbackStubTest, OnRemoteRequest_UnknownCode)
{
    WriteValidDescriptor();
    const uint32_t unknownCode = 999;
    int32_t result = stub_->OnRemoteRequest(unknownCode, *data_, *reply_, *option_);
    EXPECT_EQ(result, stub_->IPCObjectStub::OnRemoteRequest(unknownCode, *data_, *reply_, *option_));
}

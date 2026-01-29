/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "want.h"
#include "int_wrapper.h"
#include "string_wrapper.h"
#include "want_params_wrapper.h"
#include "iremote_stub.h"
#define private public
#include "connect_native_request.h"
#include "i_web_extension_connection_callback.h"

using namespace testing;
using namespace OHOS::NWeb;
using namespace OHOS::AAFwk;

namespace OHOS {
const std::string WANT_EXTENSION_ORIGIN_PARAM_KEY = "ohos.arkweb.extensionOrigin";
const std::string WANT_READ_PIPE_PARAM_KEY = "ohos.arkweb.messageReadPipe";
const std::string WANT_WRITE_PIPE_PARAM_KEY = "ohos.arkweb.messageWritePipe";
namespace {
const int32_t DEFAULT_CALLER_CONNECTION_ID = 10;
const int32_t DEFAULT_INNER_CONNECTION_ID = 20;
const int32_t DEFAULT_CALLER_PID = 30;
const int32_t DEFAULT_CALLER_TOKEN_ID = 40;
const int32_t DEFAULT_TARGET_PID = 50;
const int32_t SINGLE_REQUEST_SIZE = 1;
const int32_t DEFAULT_CONNECTION_ID = 99;
const int32_t INVALID_CONNECTION_ID = -1;
const int32_t TEST_ERROR_CODE_1 = 1;
const int32_t TEST_ERROR_CODE_2 = 2;
const int32_t TEST_ERROR_CODE_404 = 404;
const int32_t ZERO_REQUEST_SIZE = 0;
const int32_t DEFAULT_READ_PIPE_FD = 100;
const int32_t DEFAULT_WRITE_PIPE_FD = 101;

class MockWebExtensionConnectionCallback : public IRemoteStub<IWebExtensionConnectionCallback> {
public:
    MOCK_METHOD(void, OnConnect, (ConnectionNativeInfo& info), (override));
    MOCK_METHOD(void, OnDisconnect, (ConnectionNativeInfo& info), (override));
    MOCK_METHOD(void, OnFailed, (int32_t errorNum), (override));
};

class ConnectionNativeRequestAllTest : public testing::Test {
public:
    void TearDown() override
    {
        EXPECT_EQ(ConnectionNativeRequest::GetAliveSize(), ZERO_REQUEST_SIZE);
    }

    std::shared_ptr<ConnectionNativeRequest> CreateRequest()
    {
        auto req = std::make_shared<ConnectionNativeRequest>();
        req->callerConnectionId_ = DEFAULT_CALLER_CONNECTION_ID;
        req->innerConnectionId_ = DEFAULT_INNER_CONNECTION_ID;
        req->callerPid_ = DEFAULT_CALLER_PID;
        req->callerTokenId_ = DEFAULT_CALLER_TOKEN_ID;
        req->callerBundleName_ = "caller.bundle";
        req->callerExtensionOrigin_ = "caller.origin";
        req->targetBundleName_ = "target.bundle";
        req->targetAbilityName_ = "TargetAbility";
        return req;
    }

    AAFwk::Want CreateValidWant()
    {
        AAFwk::Want want;
        want.SetBundle("target.bundle");
        want.SetElementName("target.bundle", "TargetAbility");

        AAFwk::WantParams params;
        params.SetParam("ohos.arkweb.extensionOrigin",
                        AAFwk::String::Box("ext.origin"));

        AAFwk::WantParams readFd;
        readFd.SetParam("type", AAFwk::String::Box("FD"));
        readFd.SetParam("value", AAFwk::Integer::Box(DEFAULT_READ_PIPE_FD));
        params.SetParam("ohos.arkweb.messageReadPipe", AAFwk::WantParamWrapper::Box(readFd));

        AAFwk::WantParams writeFd;
        writeFd.SetParam("type", AAFwk::String::Box("FD"));
        writeFd.SetParam("value", AAFwk::Integer::Box(DEFAULT_WRITE_PIPE_FD));
        params.SetParam("ohos.arkweb.messageWritePipe",  AAFwk::WantParamWrapper::Box(writeFd));

        want.SetParams(params);
        return want;
    }
};
} // namespace

HWTEST_F(ConnectionNativeRequestAllTest, InsertRequestMap_Nullptr, testing::ext::TestSize.Level0)
{
    ConnectionNativeRequest::InsertRequestMap(nullptr);
    EXPECT_EQ(ConnectionNativeRequest::GetAliveSize(), ZERO_REQUEST_SIZE);
}

HWTEST_F(ConnectionNativeRequestAllTest, InsertAndGetAndRemoveRequestMap, testing::ext::TestSize.Level0)
{
    auto req = CreateRequest();
    ConnectionNativeRequest::InsertRequestMap(req);

    EXPECT_EQ(ConnectionNativeRequest::GetAliveSize(), SINGLE_REQUEST_SIZE);
    EXPECT_EQ(ConnectionNativeRequest::GetExistConnectId(DEFAULT_INNER_CONNECTION_ID), req);

    ConnectionNativeRequest::RemoveRequestMap(DEFAULT_INNER_CONNECTION_ID);
    EXPECT_EQ(ConnectionNativeRequest::GetExistConnectId(DEFAULT_INNER_CONNECTION_ID), nullptr);
}

HWTEST_F(ConnectionNativeRequestAllTest, InnerConnectionIdMap_AllBranches, testing::ext::TestSize.Level0)
{
    EXPECT_EQ(ConnectionNativeRequest::GetInnerConnectionIdFromMap(DEFAULT_CALLER_PID, DEFAULT_CALLER_TOKEN_ID,
        DEFAULT_CALLER_CONNECTION_ID), INVALID_CONNECTION_ID);

    ConnectionNativeRequest::InsertInnerConnectionIdMap(DEFAULT_CALLER_PID, DEFAULT_CALLER_TOKEN_ID,
        DEFAULT_CALLER_CONNECTION_ID, DEFAULT_CONNECTION_ID);
    EXPECT_EQ(ConnectionNativeRequest::GetInnerConnectionIdFromMap(DEFAULT_CALLER_PID, DEFAULT_CALLER_TOKEN_ID,
        DEFAULT_CALLER_CONNECTION_ID), DEFAULT_CONNECTION_ID);

    ConnectionNativeRequest::RemoveInnerConnectionIdMap(DEFAULT_CALLER_PID, DEFAULT_CALLER_TOKEN_ID,
        DEFAULT_CALLER_CONNECTION_ID);
    EXPECT_EQ(ConnectionNativeRequest::GetInnerConnectionIdFromMap(DEFAULT_CALLER_PID, DEFAULT_CALLER_TOKEN_ID,
        DEFAULT_CALLER_CONNECTION_ID), INVALID_CONNECTION_ID);
}

HWTEST_F(ConnectionNativeRequestAllTest, FillRequestWithWant_AllFailureBranches, testing::ext::TestSize.Level0)
{
    ConnectionNativeRequest req;
    AAFwk::Want want;
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);
    want.SetBundle("bundle");
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);
    want.SetElementName("bundle", "Ability");
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);
    AAFwk::WantParams params;
    params.SetParam("ohos.arkweb.extensionOrigin", AAFwk::String::Box("origin"));
    want.SetParams(params);
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);

    AAFwk::WantParams readFd;
    readFd.SetParam("type", AAFwk::String::Box("AD"));
    readFd.SetParam("value", AAFwk::Integer::Box(-1));
    AAFwk::WantParams topParams;
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readFd));
    want.SetParams(topParams);
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);

    readFd.SetParam("type", AAFwk::String::Box("FD"));
    readFd.SetParam("value", AAFwk::Integer::Box(1));
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readFd));
    want.SetParams(topParams);
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);

    readFd.SetParam("type", AAFwk::String::Box("FD"));
    readFd.SetParam("value", AAFwk::Integer::Box(-1));
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readFd));
    want.SetParams(topParams);
    AAFwk::WantParams readPipeParams = want.GetParams().GetWantParams("ohos.arkweb.messageReadPipe");
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);


    AAFwk::WantParams writeFd;
    writeFd.SetParam("type", AAFwk::String::Box("AD"));
    writeFd.SetParam("value", AAFwk::Integer::Box(-1));
    AAFwk::WantParams frompParams;
    frompParams.SetParam(WANT_WRITE_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(writeFd));
    want.SetParams(frompParams);
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);

    writeFd.SetParam("type", AAFwk::String::Box("FD"));
    writeFd.SetParam("value", AAFwk::Integer::Box(-1));
    frompParams.SetParam(WANT_WRITE_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(writeFd));
    want.SetParams(frompParams);
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::WANT_FORMAT_ERROR);
}

HWTEST_F(ConnectionNativeRequestAllTest, FillRequestWithWant_Success, testing::ext::TestSize.Level0)
{
    ConnectionNativeRequest req;
    auto want = CreateValidWant();
    EXPECT_EQ(req.FillRequestWithWant(want), ConnectNativeRet::SUCCESS);
}

HWTEST_F(ConnectionNativeRequestAllTest, OnConnect_AllBranches, testing::ext::TestSize.Level0)
{
    ConnectionNativeRequest req;
    req.isConnected_ = true;
    req.OnConnect();

    req.isConnected_ = false;
    req.appConnectCallback_ = nullptr;
    req.OnConnect();

    sptr<IRemoteObject> invalid;
    req.appConnectCallback_ = invalid;
    req.isConnected_ = false;
    req.OnConnect();

    auto callback = sptr<MockWebExtensionConnectionCallback>::MakeSptr();
    req.appConnectCallback_ = callback->AsObject();
    req.callerConnectionId_ = DEFAULT_CALLER_CONNECTION_ID;
    req.targetBundleName_ = "bundle";
    req.callerExtensionOrigin_ = "origin";
    req.targetExtensionPid_ = DEFAULT_TARGET_PID;
    req.isConnected_ = false;

    EXPECT_CALL(*callback, OnConnect(_)).Times(SINGLE_REQUEST_SIZE);
    req.OnConnect();
}

HWTEST_F(ConnectionNativeRequestAllTest, OnDisconnect_AllBranches, testing::ext::TestSize.Level0)
{
    ConnectionNativeRequest req;
    req.isConnected_ = false;
    req.OnDisconnect();

    req.isConnected_ = true;
    req.appConnectCallback_ = nullptr;
    req.OnDisconnect();

    sptr<IRemoteObject> invalid;
    req.appConnectCallback_ = invalid;
    req.isConnected_ = true;
    req.OnDisconnect();

    auto callback = sptr<MockWebExtensionConnectionCallback>::MakeSptr();
    req.appConnectCallback_ = callback->AsObject();
    req.isConnected_ = true;

    EXPECT_CALL(*callback, OnDisconnect(_)).Times(SINGLE_REQUEST_SIZE);
    req.OnDisconnect();
}

HWTEST_F(ConnectionNativeRequestAllTest, OnFailed_AllBranches, testing::ext::TestSize.Level0)
{
    ConnectionNativeRequest req;
    req.appConnectCallback_ = nullptr;
    req.OnFailed(TEST_ERROR_CODE_1);

    sptr<IRemoteObject> invalid;
    req.appConnectCallback_ = invalid;
    req.OnFailed(TEST_ERROR_CODE_2);

    auto callback = sptr<MockWebExtensionConnectionCallback>::MakeSptr();
    req.appConnectCallback_ = callback->AsObject();

    EXPECT_CALL(*callback, OnFailed(TEST_ERROR_CODE_404)).Times(SINGLE_REQUEST_SIZE);
    req.OnFailed(TEST_ERROR_CODE_404);
}

HWTEST_F(ConnectionNativeRequestAllTest, DumpAndToString, testing::ext::TestSize.Level0)
{
    auto req = CreateRequest();
    ConnectionNativeRequest::InsertRequestMap(req);
    EXPECT_FALSE(req->ToString().empty());

    ConnectionNativeRequest::DumpAllRequest(DEFAULT_CALLER_CONNECTION_ID);
    ConnectionNativeRequest::DumpInnerIdMap(DEFAULT_CALLER_CONNECTION_ID);
    ConnectionNativeRequest::RemoveRequestMap(req->innerConnectionId_);
}

HWTEST_F(ConnectionNativeRequestAllTest, DestructorCleanup, testing::ext::TestSize.Level0)
{
    {
        auto req = CreateRequest();
        ConnectionNativeRequest::InsertRequestMap(req);
        EXPECT_EQ(ConnectionNativeRequest::GetAliveSize(), SINGLE_REQUEST_SIZE);
    }
    EXPECT_EQ(ConnectionNativeRequest::GetAliveSize(), ZERO_REQUEST_SIZE);
}
} // OHOS
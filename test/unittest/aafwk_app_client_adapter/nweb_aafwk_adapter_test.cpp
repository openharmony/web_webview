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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "aafwk_app_mgr_client_adapter_impl.h"
#undef private

#include "app_mgr_client.h"
#include "ohos_adapter_helper.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace AppExecFwk;

namespace OHOS::NWeb {
namespace {
const int RESULT_OK = 0;
constexpr int GET_TIMES_MAX_CNT = 5;
std::shared_ptr<AafwkAppMgrClientAdapterImpl> g_adapter;
} // namespace

class NWebAafwkAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NWebAafwkAdapterTest::SetUpTestCase(void)
{}

void NWebAafwkAdapterTest::TearDownTestCase(void)
{}

void NWebAafwkAdapterTest::SetUp(void)
{}

void NWebAafwkAdapterTest::TearDown(void)
{}

class MockAppMgrClient : public AppMgrClient {
public:
    MOCK_METHOD4(StartRenderProcess, int(const std::string &, int32_t, int32_t, pid_t &));
    MOCK_METHOD2(GetRenderProcessTerminationStatus, int(pid_t, int &));
};

class RenderScheduler : public AafwkRenderSchedulerHostAdapter {
public:
    void NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd);
};

void RenderScheduler::NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd)
{
    (void)ipcFd;
    (void)sharedFd;
}

/**
 * @tc.name: NWebInputEvent_AafwkAppMgrClientAdapterImpl_001.
 * @tc.desc: Test the AafwkAppMgrClientAdapterImpl.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebInputEvent_AafwkAppMgrClientAdapterImpl_001, TestSize.Level1)
{
    int result = 0;
    g_adapter = std::make_shared<AafwkAppMgrClientAdapterImpl>();
    if (g_adapter == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartRenderProcess_002.
 * @tc.desc: Test the StartRenderProcess.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_002, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(0));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, renderPid);
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartRenderProcess_003.
 * @tc.desc: Test the StartRenderProcess Exception scenarios.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_003, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(1));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, renderPid);
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_AttachRenderProcess_004.
 * @tc.desc: Test the AttachRenderProcess.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_AttachRenderProcess_004, TestSize.Level1)
{
    int red = 0;
    std::shared_ptr<RenderScheduler> render = std::make_shared<RenderScheduler>();
    if (render == nullptr) {
        red = -1;
    }
    EXPECT_EQ(RESULT_OK, red);
    g_adapter->AttachRenderProcess(render);
    std::shared_ptr<RenderScheduler> renderd = nullptr;
    g_adapter->AttachRenderProcess(renderd);
}

/**
 * @tc.name: NWebAafwkAdapter_GetRenderProcessTerminationStatus_005.
 * @tc.desc: Test the GetRenderProcessTerminationStatus.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_GetRenderProcessTerminationStatus_005, TestSize.Level1)
{
    int statused = 1;
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, GetRenderProcessTerminationStatus(::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(
            ::testing::DoAll(
                ::testing::SetArgReferee<1>(statused),
                ::testing::Return(0)
            )
        );
    pid_t red = 1;
    int result = g_adapter->GetRenderProcessTerminationStatus(red, statused);
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_GetRenderProcessTerminationStatus_006.
 * @tc.desc: Test the GetRenderProcessTerminationStatus Exception scenarios.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_GetRenderProcessTerminationStatus_006, TestSize.Level1)
{
    int statused = 1;
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, GetRenderProcessTerminationStatus(::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(
            ::testing::DoAll(
                ::testing::SetArgReferee<1>(statused),
                ::testing::Return(1)
            )
        );
    pid_t red = 1;
    int result = g_adapter->GetRenderProcessTerminationStatus(red, statused);
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_GetRenderProcessTerminationStatus_007.
 * @tc.desc: Test the GetRenderProcessTerminationStatus Exception scenarios.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_GetRenderProcessTerminationStatus_007, TestSize.Level1)
{
    int statused = -1;
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, GetRenderProcessTerminationStatus(::testing::_, ::testing::_))
        .Times(GET_TIMES_MAX_CNT)
        .WillRepeatedly(
            ::testing::DoAll(
                ::testing::SetArgReferee<1>(statused),
                ::testing::Return(0)
            )
        );
    pid_t red = 1;
    int result = g_adapter->GetRenderProcessTerminationStatus(red, statused);
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartRenderProcess_008.
 * @tc.desc: In this scence StartRenderProcess will be fail.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_008, TestSize.Level1)
{
    g_adapter->appMgrClient_ = nullptr;
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, renderPid);
    EXPECT_NE(RESULT_OK, result);
    std::shared_ptr<RenderScheduler> render = std::make_shared<RenderScheduler>();
    if (render == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_adapter->AttachRenderProcess(render);
    int statused = 0;
    pid_t red = 1;
    result = g_adapter->GetRenderProcessTerminationStatus(red, statused);
    EXPECT_NE(RESULT_OK, result);
}
}
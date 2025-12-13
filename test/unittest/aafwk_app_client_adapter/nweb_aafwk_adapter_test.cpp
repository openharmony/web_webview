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
#include "aafwk_render_scheduler_impl.h"
#include "aafwk_browser_client_adapter_impl.h"
#include "aafwk_browser_host_impl.h"
#undef private

#include "native_window.h"
#include "aafwk_render_scheduler_host_adapter.h"
#include "app_mgr_client.h"
#include "ohos_adapter_helper.h"
#include "surface_utils.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace AppExecFwk;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
namespace {
const int NWEB_ERROR = -1;
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
    MOCK_METHOD6(StartRenderProcess, int(const std::string&, int32_t, int32_t, int32_t, pid_t&, bool));
    MOCK_METHOD2(GetRenderProcessTerminationStatus, int(pid_t, int &));
    MOCK_METHOD1(SaveBrowserChannel, int(sptr<IRemoteObject>));
};

class RenderScheduler : public AafwkRenderSchedulerHostAdapter {
public:
    void NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd, int32_t crashFd);
    void NotifyBrowser(int32_t ipcFd, int32_t sharedFd, int32_t crashFd,
        std::shared_ptr<AafwkBrowserClientAdapter> adapter);
};

void RenderScheduler::NotifyBrowserFd(int32_t ipcFd, int32_t sharedFd, int32_t crashFd)
{
    (void)ipcFd;
    (void)sharedFd;
    (void)crashFd;
}

void RenderScheduler::NotifyBrowser(int32_t ipcFd, int32_t sharedFd, int32_t crashFd,
    std::shared_ptr<AafwkBrowserClientAdapter> adapter)
{
    (void)ipcFd;
    (void)sharedFd;
    (void)crashFd;
    (void)adapter;
}

class MockBrowserClient : public BrowserClient {
public:
    explicit MockBrowserClient(const sptr<IRemoteObject> &impl);

    sptr<IRemoteObject> QueryRenderSurface(int32_t surface_id);

    void ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role);

    void PassSurface(sptr<Surface>surface, int64_t surface_id);

    void DestroyRenderSurface(int32_t surface_id);
};

sptr<IRemoteObject> MockBrowserClient::QueryRenderSurface(int32_t surface_id)
{
    (void)surface_id;
    return nullptr;
}

void MockBrowserClient::ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role)
{
    (void)status;
    (void)process_id;
    (void)thread_id;
    (void)role;
}

void MockBrowserClient::PassSurface(sptr<Surface>surface, int64_t surface_id)
{
    (void)surface;
    (void)surface_id;
}

void MockBrowserClient::DestroyRenderSurface(int32_t surface_id)
{
    (void)surface_id;
}

class MockMessageParcel : public MessageParcel {
public:
    MOCK_METHOD(bool, WriteInterfaceToken, (const std::string& token), ());

    MOCK_METHOD(void, WriteInt32, (int32_t value), ());

    MOCK_METHOD(sptr<IRemoteObject>, ReadRemoteObject, (), ());
};

class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}

    ~MockIRemoteObject() {}

    MOCK_METHOD(int32_t, SendRequest,
        (uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option), (override));
    
    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    bool IsProxyObject() const override
    {
        return true;
    }

    bool CheckObjectLegality() const override
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return 0;
    }

    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }
};

class MockIBrowser : public IBrowser {
public:
    MOCK_METHOD(sptr<IRemoteObject>, QueryRenderSurface, (int32_t surface_id), (override));

    MOCK_METHOD(void, ReportThread, (int32_t status, int32_t process_id,
        int32_t thread_id, int32_t role), (override));

    MOCK_METHOD(void, PassSurface, (sptr<Surface> surface, int64_t surface_id), (override));

    MOCK_METHOD(void, DestroyRenderSurface, (int32_t surface_id), (override));
    
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

class AafwkBrowserHostAdapterImpl : public AafwkBrowserHostAdapter {
public:
    void* GetSurfaceFromKernel(int32_t surface_id);
    void DestroySurfaceFromKernel(int32_t surface_id);
    void* GetSurfaceFromKernelWithRef(int32_t surface_id);
};

void* AafwkBrowserHostAdapterImpl::GetSurfaceFromKernel(int32_t surface_id)
{
    if (surface_id == 0) {
        return nullptr;
    }
    auto cSurface = IConsumerSurface::Create("test");
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    OHNativeWindow* window = ::CreateNativeWindowFromSurface(&pSurface);
    return window;
};

void AafwkBrowserHostAdapterImpl::DestroySurfaceFromKernel(int32_t surface_id)
{
    (void)surface_id;
};

void* AafwkBrowserHostAdapterImpl::GetSurfaceFromKernelWithRef(int32_t surface_id)
{
    if (surface_id == 0) {
        return nullptr;
    }
    auto cSurface = IConsumerSurface::Create("test");
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    OHNativeWindow* window = ::CreateNativeWindowFromSurface(&pSurface);
    return window;
};

/**
 * @tc.name: NWebInputEvent_AafwkAppMgrClientAdapterImpl_001.
 * @tc.desc: Test the AafwkAppMgrClientAdapterImpl.
 * @tc.type: FUNC
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
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_002, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_,
                                          ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(0));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    int32_t crashFd = 0;
    pid_t renderPid = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid);
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartRenderProcess_003.
 * @tc.desc: Test the StartRenderProcess Exception scenarios.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_003, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_,
                                          ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(1));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid);
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartRenderProcess_004.
 * @tc.desc: Test the StartRenderProcess Exception scenarios.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_004, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_,
                                          ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(1));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid);
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartRenderProcess_005.
 * @tc.desc: Test the StartRenderProcess Exception scenarios.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_005, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_,
                                          ::testing::_, ::testing::_, ::testing::_))
        .Times(10)
        .WillRepeatedly(::testing::Return(8454244));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid);
    EXPECT_NE(RESULT_OK, result);
}


/**
 * @tc.name: NWebAafwkAdapter_AttachRenderProcess_004.
 * @tc.desc: Test the AttachRenderProcess.
 * @tc.type: FUNC
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
 * @tc.type: FUNC
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
 * @tc.type: FUNC
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
 * @tc.type: FUNC
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
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartRenderProcess_008, TestSize.Level1)
{
    g_adapter->appMgrClient_ = nullptr;
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    int result = g_adapter->StartRenderProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid);
    EXPECT_NE(RESULT_OK, result);
    result = 0;
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

/**
 * @tc.name: NWebAafwkAdapter_NotifyBrowserFd_009.
 * @tc.desc: Test the NotifyBrowserFd.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_NotifyBrowserFd_009, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<AafwkRenderSchedulerHostAdapter> adapter = std::make_shared<RenderScheduler>();
    if (adapter == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    std::shared_ptr<AafwkRenderSchedulerImpl> render = std::make_shared<AafwkRenderSchedulerImpl>(adapter);
    if (render == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    int32_t ipcFd = 1;
    int32_t sharedFd = 2;
    int32_t crashFd = 3;
    render->NotifyBrowserFd(ipcFd, sharedFd, crashFd, nullptr);

    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    render->NotifyBrowserFd(ipcFd, sharedFd, crashFd, host);

    sptr<IRemoteObject> browser = nullptr;
    auto client = new BrowserClient(browser);
    ASSERT_NE(client, nullptr);
    MessageParcel data;

    client->WriteInterfaceToken(data);
    render->renderSchedulerHostAdapter_ = nullptr;
    render->NotifyBrowserFd(ipcFd, sharedFd, crashFd, nullptr);
}

/**
 * @tc.name: NWebAafwkAdapter_WriteInterfaceToken_011.
 * @tc.desc: test write interface token.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_WriteInterfaceToken_011, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    auto client = new BrowserClient(impl);
    ASSERT_NE(client, nullptr);
    // MessageParcel data;
    MessageParcel data;
    client->WriteInterfaceToken(data);
}

/**
 * @tc.name: NWebAafwkAdapter_QueryRenderSurface_012.
 * @tc.desc: test query render surface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_QueryRenderSurface_012, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    auto client = new BrowserClient(impl);
    ASSERT_NE(client, nullptr);
    int32_t surface_id = 0;
    client->QueryRenderSurface(surface_id);

    sptr<MockIRemoteObject> mockImpl = new MockIRemoteObject();
    auto mockClient = new BrowserClient(mockImpl);
    ASSERT_NE(mockClient, nullptr);
    EXPECT_CALL(*mockImpl, SendRequest(_, _, _, _)).WillOnce(Return(NWEB_ERROR));
    mockClient->QueryRenderSurface(surface_id);
    delete mockClient;
}

/**
 * @tc.name: NWebAafwkAdapter_ReportThread_013.
 * @tc.desc: test report thread.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_ReportThread_013, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    auto client = new BrowserClient(impl);
    ASSERT_NE(client, nullptr);
    int32_t status = 0;
    int32_t process_id = 0;
    int32_t thread_id = 0;
    int32_t role = 0;
    client->ReportThread(status, process_id, thread_id, role);

    sptr<MockIRemoteObject> mockImpl = new MockIRemoteObject();
    auto mockClient = new BrowserClient(mockImpl);
    ASSERT_NE(mockClient, nullptr);
    EXPECT_CALL(*mockImpl, SendRequest(_, _, _, _)).WillOnce(Return(NWEB_ERROR));
    mockClient->ReportThread(status, process_id, thread_id, role);
    delete mockClient;
}

/**
 * @tc.name: NWebAafwkAdapter_PassSurface_014.
 * @tc.desc: test pass surface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_PassSurface_014, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    auto client = new BrowserClient(impl);
    ASSERT_NE(client, nullptr);
    sptr<Surface> surface;
    int64_t surface_id = 0;
    client->PassSurface(surface, surface_id);
    auto cSurface = IConsumerSurface::Create("test");
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    client->PassSurface(pSurface, surface_id);
}

/**
 * @tc.name: NWebAafwkAdapter_DestroyRenderSurface_015.
 * @tc.desc: test destroy render surface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_DestroyRenderSurface_015, TestSize.Level1)
{
    sptr<IRemoteObject> impl;
    auto client = new BrowserClient(impl);
    ASSERT_NE(client, nullptr);
    int32_t surface_id = 0;
    client->DestroyRenderSurface(surface_id);

    sptr<MockIRemoteObject> mockImpl = new MockIRemoteObject();
    auto mockClient = new BrowserClient(mockImpl);
    ASSERT_NE(mockClient, nullptr);
    EXPECT_CALL(*mockImpl, SendRequest(_, _, _, _)).WillOnce(Return(NWEB_ERROR));
    mockClient->DestroyRenderSurface(surface_id);
    delete mockClient;
}

/**
 * @tc.name: NWebAafwkAdapter_QueryRenderSurface_016.
 * @tc.desc: test query render surface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_QueryRenderSurface_016, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserClientAdapterImpl> clientAdapter = std::make_shared<AafwkBrowserClientAdapterImpl>();
    ASSERT_NE(clientAdapter, nullptr);
    int32_t surface_id = 0;
    clientAdapter->GetInstance().browserHost_ = nullptr;
    clientAdapter->QueryRenderSurface(surface_id);

    sptr<MockIBrowser> mockBrowser = new MockIBrowser();
    clientAdapter->GetInstance().browserHost_ = mockBrowser;
    clientAdapter->QueryRenderSurface(surface_id);
    auto cSurface = IConsumerSurface::Create("test");
    EXPECT_CALL(*mockBrowser, QueryRenderSurface(surface_id))
        .WillOnce(::testing::Return(cSurface->GetProducer()->AsObject()));
    clientAdapter->QueryRenderSurface(surface_id);
}

/**
 * @tc.name: NWebAafwkAdapter_ReportThread_017.
 * @tc.desc: test report thread.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_ReportThread_017, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserClientAdapterImpl> clientAdapter = std::make_shared<AafwkBrowserClientAdapterImpl>();
    ASSERT_NE(clientAdapter, nullptr);
    ResSchedStatusAdapter status = ResSchedStatusAdapter::THREAD_CREATED;
    int32_t process_id = 0;
    int32_t thread_id = 0;
    ResSchedRoleAdapter role = ResSchedRoleAdapter::USER_INTERACT;
    clientAdapter->GetInstance().browserHost_ = nullptr;
    clientAdapter->ReportThread(status, process_id, thread_id, role);
    sptr<MockIBrowser> mockBrowser = new MockIBrowser();
    EXPECT_CALL(*mockBrowser, ReportThread(0, 0, 0, 0));
    clientAdapter->GetInstance().browserHost_ = mockBrowser;
    clientAdapter->ReportThread(status, process_id, thread_id, role);
}

/**
 * @tc.name: NWebAafwkAdapter_PassSurface_018.
 * @tc.desc: test pass surface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_PassSurface_018, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserClientAdapterImpl> clientAdapter = std::make_shared<AafwkBrowserClientAdapterImpl>();
    ASSERT_NE(clientAdapter, nullptr);
    int64_t surface_id = 0;
    clientAdapter->GetInstance().browserHost_ = nullptr;
    clientAdapter->PassSurface(surface_id);
    sptr<MockIBrowser> mockBrowser = new MockIBrowser();
    clientAdapter->GetInstance().browserHost_ = mockBrowser;
    clientAdapter->PassSurface(surface_id);

    auto cSurface = IConsumerSurface::Create("test");
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    SurfaceUtils* utils = SurfaceUtils::GetInstance();
    utils->Add(surface_id, pSurface);
    ASSERT_NE(utils->GetSurface(surface_id), nullptr);
    clientAdapter->PassSurface(surface_id);
    utils->Remove(surface_id);
}

/**
 * @tc.name: NWebAafwkAdapter_DestroyRenderSurface_019.
 * @tc.desc: test destroy render surface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_DestroyRenderSurface_019, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserClientAdapterImpl> clientAdapter = std::make_shared<AafwkBrowserClientAdapterImpl>();
    ASSERT_NE(clientAdapter, nullptr);
    int32_t surface_id = 0;
    clientAdapter->GetInstance().browserHost_ = nullptr;
    clientAdapter->DestroyRenderSurface(surface_id);
    sptr<MockIBrowser> mockBrowser = new MockIBrowser();
    EXPECT_CALL(*mockBrowser, DestroyRenderSurface(surface_id)).Times(2);
    clientAdapter->GetInstance().browserHost_ = mockBrowser;
    clientAdapter->DestroyRenderSurface(surface_id);

    auto cSurface = IConsumerSurface::Create("test");
    auto producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    OHNativeWindow* window = ::CreateNativeWindowFromSurface(&pSurface);
    clientAdapter->window_map_.emplace(surface_id, window);
    clientAdapter->DestroyRenderSurface(surface_id);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_020.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_020, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    host->OnRemoteRequest(code, data, reply, option);
    code = 1;
    data.WriteInterfaceToken(BrowserHost::GetDescriptor());
    host->OnRemoteRequest(code, data, reply, option);
    code = 100;
    data.WriteInterfaceToken(BrowserHost::GetDescriptor());
    host->OnRemoteRequest(code, data, reply, option);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_021.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_021, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    MessageParcel data;
    MessageParcel reply;
    host->HandleQueryRenderSurface(data, reply);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_022.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_022, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    MessageParcel data;
    MessageParcel reply;
    host->HandleReportThread(data, reply);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_023.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_023, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    MessageParcel data;
    MessageParcel reply;
    host->HandlePassSurface(data, reply);

    auto cSurface = IConsumerSurface::Create("test");
    data.WriteRemoteObject(cSurface->GetProducer()->AsObject());
    host->HandlePassSurface(data, reply);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_024.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_024, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    MessageParcel data;
    MessageParcel reply;
    host->HandleDestroyRenderSurface(data, reply);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_025.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_025, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    int32_t surface_id = 0;
    host->QueryRenderSurface(surface_id);

    auto mockHostAdapter = std::make_shared<AafwkBrowserHostAdapterImpl>();
    auto mockHost = new AafwkBrowserHostImpl(mockHostAdapter);
    mockHost->QueryRenderSurface(surface_id);
    mockHost->QueryRenderSurface(1);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_026.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_026, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    int32_t status = 0;
    int32_t process_id = 0;
    int32_t thread_id = 0;
    int32_t role = 0;
    host->ReportThread(status, process_id, thread_id, role);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_027.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_027, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    sptr<Surface> surface;
    int64_t surface_id = 0;
    host->PassSurface(surface, surface_id);
}

/**
 * @tc.name: NWebAafwkAdapter_browserHost_028.
 * @tc.desc: test browser host.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_browserHost_028, TestSize.Level1)
{
    std::shared_ptr<AafwkBrowserHostAdapter> hostAdapter = nullptr;
    auto host = new AafwkBrowserHostImpl(hostAdapter);
    ASSERT_NE(host, nullptr);
    int32_t surface_id = 0;
    host->DestroyRenderSurface(surface_id);
    delete host;

    auto mockHostAdapter = std::make_shared<AafwkBrowserHostAdapterImpl>();
    auto mockHost = new AafwkBrowserHostImpl(mockHostAdapter);
    mockHost->DestroyRenderSurface(surface_id);
    delete mockHost;
}

/**
 * @tc.name: NWebAafwkAdapter_StartChildProcess_001.
 * @tc.desc: Test the StartChildProcess Exception scenarios.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartChildProcess_001, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_,
                                          ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(1));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    std::string processtype = "gpu-process";
    int result = g_adapter->StartChildProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid, processtype);
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartChildProcess_002.
 * @tc.desc: Test the StartChildProcess Exception scenarios.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartChildProcess_002, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_,
                                          ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(0));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    std::string processtype = "gpu-process";
    int result = g_adapter->StartChildProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid, processtype);
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartChildProcess_003.
 * @tc.desc: Test the StartChildProcess Exception scenarios.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartChildProcess_003, TestSize.Level1)
{
    MockAppMgrClient *mock = new MockAppMgrClient();
    g_adapter->appMgrClient_.reset((AppMgrClient *)mock);
    EXPECT_CALL(*mock, StartRenderProcess(::testing::_, ::testing::_, ::testing::_,
                                          ::testing::_, ::testing::_, ::testing::_))
        .Times(10)
        .WillRepeatedly(::testing::Return(8454244));
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    std::string processtype = "gpu-process";
    int result = g_adapter->StartChildProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid, processtype);
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebAafwkAdapter_StartChildProcess_004.
 * @tc.desc: In this scence StartChildProcess will be fail.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebAafwkAdapterTest, NWebAafwkAdapter_StartChildProcess_004, TestSize.Level1)
{
    g_adapter->SaveBrowserConnect(nullptr);
    auto adapter = std::make_shared<AafwkBrowserHostAdapterImpl>();
    g_adapter->SaveBrowserConnect(adapter);
    g_adapter->appMgrClient_ = nullptr;
    std::string renderParam = "test";
    int32_t ipcFd = 0;
    int32_t sharedFd = 0;
    pid_t renderPid = 0;
    int32_t crashFd = 0;
    std::string processtype = "gpu-process";
    int result = g_adapter->StartChildProcess(renderParam, ipcFd, sharedFd, crashFd, renderPid, processtype);
    EXPECT_NE(RESULT_OK, result);
    g_adapter->SaveBrowserConnect(adapter);
}
}
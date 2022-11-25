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
#include <securec.h>
#include <unordered_map>

#include "graphic_common.h"
#include "graphic_common_c.h"
#include "key_event.h"
#include "nweb_create_window.h"

#define protected public
#define private public

#include <ui/rs_surface_node.h>
#include "nweb_window_adapter.h"
#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_controller.h"
#include "surface_buffer.h"
#include "surface_type.h"
#include "transaction/rs_interfaces.h"
#include "pointer_event.h"
#include "window.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;
using namespace OHOS::Rosen;

namespace OHOS {
namespace Rosen {

class RSInterfacesMock : public RSInterfaces {
public:
    MOCK_METHOD2(CreateVSyncReceiver, std::shared_ptr<VSyncReceiver>(const std::string&,
                 const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &));
};

namespace {
RSInterfacesMock *g_instance = new RSInterfacesMock();
sptr<VSyncConnection> vsyncConnection = nullptr;
}

class VSyncReceiverMock : public VSyncReceiver {
public:
    VSyncReceiverMock() : VSyncReceiver(vsyncConnection) {}
    MOCK_METHOD1(RequestNextVSync, VsyncError(FrameCallback));
};

RSInterfaces &RSInterfaces::GetInstance()
{
    return *g_instance;
}
}

namespace NWeb {
namespace {
sptr<OHOS::Rosen::Window> g_window;
NWebCreateInfo g_info;
sptr<SurfaceBuffer> g_surfaceBuffer = nullptr;
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
const std::string TEST_BUFFER = "test render frame buffer";
} // namespace

class NWebWindowAdapterTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void OnBufferAvailable() override;
};

void NWebWindowAdapterTest::OnBufferAvailable()
{}

void NWebWindowAdapterTest::SetUpTestCase(void)
{}

void NWebWindowAdapterTest::TearDownTestCase(void)
{}

void NWebWindowAdapterTest::SetUp(void)
{}

void NWebWindowAdapterTest::TearDown(void)
{}

/**
 * @tc.name: NWebInputEvent_GetCreateInfo_001.
 * @tc.desc: Test the GetCreateInfo.
 * @tc.type: FUNC
 * @tc.require:issueI5R6E0
 */
HWTEST_F(NWebWindowAdapterTest, NWebWindowAdapter_GetCreateInfo_001, TestSize.Level1)
{
    NWebWindowAdapter &windowAdapter = NWebWindowAdapter::Instance();
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    bool result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_TRUE(result);
    g_window = CreateWindow();
    EXPECT_NE(g_window, nullptr);
    g_info = windowAdapter.GetCreateInfo(g_window.GetRefPtr(), GetInitArgs());
    sptr<OHOS::Rosen::Window> window = nullptr;
    windowAdapter.GetCreateInfo(window.GetRefPtr(), GetInitArgs());
}

/**
 * @tc.name: NWebInputEvent_OnInputEvent_002.
 * @tc.desc: Test the OnInputEvent.
 * @tc.type: FUNC
 * @tc.require:issueI5R6E0
 */
HWTEST_F(NWebWindowAdapterTest, NWebWindowAdapter_OnInputEvent_002, TestSize.Level1)
{
    const int DEFAULT_WIDTH = 2560;
    const int DEFAULT_HEIGHT = 1396;
    auto windowAdapter = NWebWindowAdapter::Instance();
    std::shared_ptr<NWeb> mock = std::make_shared<NWebMock>();
    EXPECT_NE(mock, nullptr);
    windowAdapter.GetRenderInterface(g_window.GetRefPtr(), g_info);
    windowAdapter.RegistEventCb(g_window.GetRefPtr(), mock);

    sptr<Surface> surface = g_window->GetSurfaceNode()->GetSurface();
    EXPECT_NE(surface, nullptr);
    int32_t releaseFence = -1;
    BufferRequestConfig requestConfig = {
        .width = DEFAULT_WIDTH,
        .height = DEFAULT_HEIGHT,
        .strideAlignment = sizeof(void *),
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    surface->RequestBuffer(g_surfaceBuffer, releaseFence, requestConfig);
    EXPECT_NE(g_surfaceBuffer, nullptr);

    windowAdapter.windowInfoMap_[g_window.GetRefPtr()].cachedSurfaceBuffer = g_surfaceBuffer;
    bool result = g_info.output_render_frame(TEST_BUFFER.c_str(), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_FALSE(result);
    windowAdapter.GetRenderInterface(g_window.GetRefPtr(), g_info);
    windowAdapter.VsyncCb(g_window.GetRefPtr(), mock);

    windowAdapter.windowInfoMap_[g_window.GetRefPtr()].cachedSurfaceBuffer = nullptr;
    result = g_info.output_render_frame(TEST_BUFFER.c_str(), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_FALSE(result);
    windowAdapter.VsyncCb(g_window.GetRefPtr(), mock);
    windowAdapter.GetRenderInterface(g_window.GetRefPtr(), g_info);

    g_window.GetRefPtr()->GetSurfaceNode()->surface_ = nullptr;
    result = g_info.output_render_frame(TEST_BUFFER.c_str(), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_FALSE(result);
    windowAdapter.GetRenderInterface(g_window.GetRefPtr(), g_info);
    windowAdapter.RequestVsync(g_window.GetRefPtr(), mock);

    windowAdapter.windowInfoMap_[g_window.GetRefPtr()].cachedSurfaceBuffer = g_surfaceBuffer;
    result = g_info.output_render_frame(TEST_BUFFER.c_str(), DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_FALSE(result);
    windowAdapter.VsyncCb(g_window.GetRefPtr(), mock);
}

/**
 * @tc.name: NWebInputEvent_RequestVsync_003.
 * @tc.desc: Test the RequestVsync.
 * @tc.type: FUNC
 * @tc.require:issueI5R6E0
 */
HWTEST_F(NWebWindowAdapterTest, NWebWindowAdapter_RequestVsync_003, TestSize.Level1)
{
    auto windowAdapter = NWebWindowAdapter::Instance();
    sptr<OHOS::Rosen::Window> window = nullptr;
    std::shared_ptr<NWeb> mock = std::make_shared<NWebMock>();
    EXPECT_NE(mock, nullptr);
    windowAdapter.RequestVsync(window.GetRefPtr(), mock);
    windowAdapter.VsyncCb(window.GetRefPtr(), mock);
    windowAdapter.RegistEventCb(window.GetRefPtr(), mock);
}

/**
 * @tc.name: NWebInputEvent_RequestVsync_004.
 * @tc.desc: Test the RequestVsync.
 * @tc.type: FUNC
 * @tc.require:issueI5R6E0
 */
HWTEST_F(NWebWindowAdapterTest, NWebWindowAdapter_RequestVsync_004, TestSize.Level1)
{
    auto windowAdapter = NWebWindowAdapter::Instance();
    std::shared_ptr<NWeb> mock = std::make_shared<NWebMock>();
    EXPECT_NE(mock, nullptr);
    VSyncReceiverMock *vsyncMock = new VSyncReceiverMock();
    EXPECT_NE(vsyncMock, nullptr);
    windowAdapter.receiver_.reset(vsyncMock);
    windowAdapter.RequestVsync(g_window.GetRefPtr(), mock);
}

/**
 * @tc.name: NWebInputEvent_RequestVsync_005.
 * @tc.desc: Test the RequestVsync.
 * @tc.type: FUNC
 * @tc.require:issueI5R6E0
 */
HWTEST_F(NWebWindowAdapterTest, NWebWindowAdapter_RequestVsync_005, TestSize.Level1)
{
    if (g_instance) {
        delete g_instance;
        g_instance = nullptr;
    }
    g_instance = new RSInterfacesMock();
    auto windowAdapter = NWebWindowAdapter::Instance();
    windowAdapter.receiver_ = nullptr;
    std::shared_ptr<NWeb> mock = std::make_shared<NWebMock>();
    EXPECT_NE(mock, nullptr);
    std::shared_ptr<VSyncReceiverMock> vsmock = std::make_shared<VSyncReceiverMock>();
    EXPECT_NE(vsmock, nullptr);
    testing::Mock::AllowLeak(g_instance);
    EXPECT_CALL(*g_instance, CreateVSyncReceiver(::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(vsmock));
    VSyncReceiverMock *receiver = vsmock.get();
    testing::Mock::AllowLeak(receiver);
    EXPECT_CALL(*receiver, RequestNextVSync(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(GSError::GSERROR_OK));
    windowAdapter.RequestVsync(g_window.GetRefPtr(), mock);

    EXPECT_CALL(*g_instance, CreateVSyncReceiver(::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(nullptr));
    windowAdapter.RequestVsync(g_window.GetRefPtr(), mock);
}
}
}
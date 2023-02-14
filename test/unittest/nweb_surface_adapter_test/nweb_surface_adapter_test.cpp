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
#include "surface_buffer_impl.h"

#define protected public
#define private public

#include "nweb_surface_adapter.h"
#include <ui/rs_surface_node.h>
#include "nweb_window_adapter.h"
#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "surface_buffer.h"
#include "surface_type.h"
#include "pointer_event.h"
#include "window.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;

namespace OHOS::NWeb {
namespace {
sptr<OHOS::Rosen::Window> g_window;
NWebCreateInfo g_info;
sptr<SurfaceBuffer> g_surfaceBuffer = nullptr;
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
const uint32_t DEFAULT_WIDTH = 2560;
const uint32_t DEFAULT_HEIGHT = 1396;
constexpr int BITS_PER_PIXEL = 4;
} // namespace

class NWebSurfaceAdapterTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;
    void OnBufferAvailable() override;
};

void NWebSurfaceAdapterTest::OnBufferAvailable()
{}

void NWebSurfaceAdapterTest::SetUpTestCase(void)
{}

void NWebSurfaceAdapterTest::TearDownTestCase(void)
{}

void NWebSurfaceAdapterTest::SetUp(void)
{}

void NWebSurfaceAdapterTest::TearDown(void)
{}

class SurfaceBufferImplMock : public SurfaceBufferImpl {
public:
    MOCK_METHOD0(GetVirAddr, void *());
    MOCK_CONST_METHOD0(GetSize, uint32_t());
};

/**
 * @tc.name: NWebSurfaceAdapterTest_GetCreateInfo_001.
 * @tc.desc: Test the GetCreateInfo.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebSurfaceAdapterTest, NWebSurfaceAdapterTest_GetCreateInfo_001, TestSize.Level1)
{
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    bool result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_TRUE(result);
    g_window = CreateWindow();
    EXPECT_NE(g_window, nullptr);
    g_info = surfaceAdapter.GetCreateInfo(g_window->GetSurfaceNode()->GetSurface(), GetInitArgs(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    sptr<Surface> surface = nullptr;
    surfaceAdapter.GetCreateInfo(surface, GetInitArgs(), DEFAULT_WIDTH, DEFAULT_HEIGHT);

    NWebCreateInfo createInfo;
    surfaceAdapter.GetSize(g_window->GetSurfaceNode()->GetSurface(), createInfo,
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    surfaceAdapter.GetSize(surface, createInfo, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

/**
 * @tc.name: NWebSurfaceAdapterTest_GetRenderInterface_002.
 * @tc.desc: Test the GetRenderInterface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebSurfaceAdapterTest, NWebSurfaceAdapterTest_GetRenderInterface_002, TestSize.Level1)
{
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    surfaceAdapter.GetRenderInterface(g_window->GetSurfaceNode()->GetSurface(), g_info);
    char *src = new char[DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL] {0};
    g_info.output_render_frame(src, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    sptr<Surface> surface = nullptr;
    surfaceAdapter.GetRenderInterface(surface, g_info);
    (void)memset_s(src, DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL, 0,
        DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL);
    g_info.output_render_frame(src, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    (void)memset_s(src, DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL, 0,
        DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL);
    g_info.output_render_frame(src, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

/**
 * @tc.name: NWebSurfaceAdapterTest_RequestBuffer_003.
 * @tc.desc: Test the RequestBuffer.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebSurfaceAdapterTest, NWebSurfaceAdapterTest_RequestBuffer_003, TestSize.Level1)
{
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    sptr<SurfaceBuffer> surfaceBuffer = surfaceAdapter.RequestBuffer(g_window->GetSurfaceNode()->GetSurface(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_NE(surfaceBuffer, nullptr);
    sptr<Surface> surface = nullptr;
    surfaceBuffer = surfaceAdapter.RequestBuffer(surface, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(surfaceBuffer, nullptr);
}

/**
 * @tc.name: NWebSurfaceAdapterTest_CopyFrame_004.
 * @tc.desc: Test the CopyFrame.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebSurfaceAdapterTest, NWebSurfaceAdapterTest_CopyFrame_004, TestSize.Level1)
{
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    char *src = new char[DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL] {0};
    bool result = surfaceAdapter.CopyFrame(g_surfaceBuffer, src, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_FALSE(result);
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
    result = surfaceAdapter.CopyFrame(g_surfaceBuffer, src, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_TRUE(result);
    (void)memset_s(src, DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL, 0,
        DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL);
    SurfaceBufferImplMock *mock = new SurfaceBufferImplMock();
    EXPECT_CALL(*mock, GetVirAddr())
        .Times(1)
        .WillRepeatedly(::testing::Return(nullptr));
    result = surfaceAdapter.CopyFrame((SurfaceBuffer *)mock, src, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_FALSE(result);
    delete[] src;
}

/**
 * @tc.name: NWebSurfaceAdapterTest_FlushBuffer_005.
 * @tc.desc: Test the FlushBuffer.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebSurfaceAdapterTest, NWebSurfaceAdapterTest_FlushBuffer_005, TestSize.Level1)
{
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    bool result = surfaceAdapter.FlushBuffer(g_window->GetSurfaceNode()->GetSurface(), g_surfaceBuffer,
        DEFAULT_WIDTH, DEFAULT_WIDTH);
    EXPECT_TRUE(result);
    sptr<Surface> surface = nullptr;
    result = surfaceAdapter.FlushBuffer(surface, g_surfaceBuffer, DEFAULT_WIDTH, DEFAULT_WIDTH);
    EXPECT_FALSE(result);
}
}
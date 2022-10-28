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
#include <vsync_helper.h>
#include <ui/rs_surface_node.h>
#include "nweb_window_adapter.h"
#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "surface_buffer.h"
#include "surface_type.h"
#include "pointer_event.h"
#include "vsync_helper_impl.h"
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

class SurfaceMock : public Surface {
public:
    bool IsConsumer() const override
    {
        return true;
    }
    sptr<IBufferProducer> GetProducer() const override
    {
        return nullptr;
    }

    GSError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                          int32_t &fence, BufferRequestConfig &config) override
    {
        return GSError::GSERROR_INVALID_ARGUMENTS;
    }

    GSError CancelBuffer(sptr<SurfaceBuffer>& buffer) override
    {
        return GSError::GSERROR_OK;
    }

    GSError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                        int32_t fence, BufferFlushConfig &config) override
    {
        return GSError::GSERROR_INVALID_ARGUMENTS;
    }

    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, int32_t &fence,
                          int64_t &timestamp, Rect &damage) override
    {
        return GSError::GSERROR_OK;
    }
    GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, int32_t fence) override
    {
        return GSError::GSERROR_OK;
    }

    GSError RequestBuffer(sptr<SurfaceBuffer>& buffer,
                          sptr<SyncFence>& fence, BufferRequestConfig &config) override
    {
        return GSError::GSERROR_INVALID_ARGUMENTS;
    }
    GSError FlushBuffer(sptr<SurfaceBuffer>& buffer,
                        const sptr<SyncFence>& fence, BufferFlushConfig &config) override
    {
        return GSError::GSERROR_INVALID_ARGUMENTS;
    }
    GSError AcquireBuffer(sptr<SurfaceBuffer>& buffer, sptr<SyncFence>& fence,
                          int64_t &timestamp, Rect &damage) override
    {
        return GSError::GSERROR_OK;
    }
    GSError ReleaseBuffer(sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& fence) override
    {
        return GSError::GSERROR_OK;
    }

    GSError AttachBuffer(sptr<SurfaceBuffer>& buffer) override
    {
        return GSError::GSERROR_OK;
    }

    GSError DetachBuffer(sptr<SurfaceBuffer>& buffer) override
    {
        return GSError::GSERROR_OK;
    }

    uint32_t GetQueueSize() override
    {
        return 0;
    }
    GSError SetQueueSize(uint32_t queueSize) override
    {
        return GSError::GSERROR_OK;
    }

    GSError SetDefaultWidthAndHeight(int32_t width, int32_t height) override
    {
        return GSError::GSERROR_OK;
    }
    int32_t GetDefaultWidth() override
    {
        return 0;
    }
    int32_t GetDefaultHeight() override
    {
        return 0;
    }

    GSError SetDefaultUsage(uint32_t usage) override
    {
        return GSError::GSERROR_OK;
    }
    uint32_t GetDefaultUsage() override
    {
        return 0;
    }

    GSError SetUserData(const std::string &key, const std::string &val) override
    {
        return GSError::GSERROR_OK;
    }
    std::string GetUserData(const std::string &key) override
    {
        return "";
    }

    const std::string& GetName() override
    {
        return "";
    }
    uint64_t GetUniqueId() const override
    {
        return 0;
    }

    GSError RegisterConsumerListener(sptr<IBufferConsumerListener>& listener) override
    {
        return GSError::GSERROR_OK;
    }
    GSError RegisterConsumerListener(IBufferConsumerListenerClazz *listener) override
    {
        return GSError::GSERROR_OK;
    }
    GSError RegisterReleaseListener(OnReleaseFunc func) override
    {
        return GSError::GSERROR_OK;
    }
    GSError RegisterDeleteBufferListener(OnDeleteBufferFunc func) override
    {
        return GSError::GSERROR_OK;
    }
    GSError UnregisterConsumerListener() override
    {
        return GSError::GSERROR_OK;
    }

    GSError CleanCache() override
    {
        return GSError::GSERROR_OK;
    }
    GSError GoBackground() override
    {
        return GSError::GSERROR_OK;
    }

    GSError SetTransform(TransformType transform) override
    {
        return GSError::GSERROR_OK;
    }
    TransformType GetTransform() const override
    {
        return TransformType::ROTATE_NONE;
    }

    GSError IsSupportedAlloc(const std::vector<VerifyAllocInfo> &infos,
                             std::vector<bool> &supporteds) override
    {
        return GSError::GSERROR_OK;
    }
    GSError Disconnect() override
    {
        return GSError::GSERROR_OK;
    }
    GSError SetScalingMode(uint32_t sequence, ScalingMode scalingMode) override
    {
        return GSError::GSERROR_OK;
    }
    GSError GetScalingMode(uint32_t sequence, ScalingMode &scalingMode) override
    {
        return GSError::GSERROR_OK;
    }
    GSError SetMetaData(uint32_t sequence, const std::vector<HDRMetaData> &metaData) override
    {
        return GSError::GSERROR_OK;
    }
    GSError SetMetaDataSet(uint32_t sequence, HDRMetadataKey key,
                           const std::vector<uint8_t> &metaData) override
    {
        return GSError::GSERROR_OK;
    }
    GSError QueryMetaDataType(uint32_t sequence, HDRMetaDataType &type) const override
    {
        return GSError::GSERROR_OK;
    }
    GSError GetMetaData(uint32_t sequence, std::vector<HDRMetaData> &metaData) const override
    {
        return GSError::GSERROR_OK;
    }
    GSError GetMetaDataSet(uint32_t sequence, HDRMetadataKey &key,
                           std::vector<uint8_t> &metaData) const override
    {
        return GSError::GSERROR_OK;
    }
    GSError SetTunnelHandle(const ExtDataHandle *handle) override
    {
        return GSError::GSERROR_OK;
    }
    sptr<SurfaceTunnelHandle> GetTunnelHandle() const override
    {
        return nullptr;
    }
    GSError SetPresentTimestamp(uint32_t sequence, const PresentTimestamp &timestamp) override
    {
        return GSError::GSERROR_OK;
    }
    GSError GetPresentTimestamp(uint32_t sequence, PresentTimestampType type,
                                int64_t &time) const override
    {
        return GSError::GSERROR_OK;
    }

    void Dump(std::string &result) const override
    {}
};

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
    sptr<SurfaceMock> mock = new SurfaceMock();
    surfaceAdapter.GetRenderInterface(mock, g_info);
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
    sptr<SurfaceMock> mock = new SurfaceMock();
    surfaceBuffer = surfaceAdapter.RequestBuffer(mock, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(surfaceBuffer, nullptr);
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
    sptr<SurfaceMock> mock = new SurfaceMock();
    result = surfaceAdapter.FlushBuffer(mock, g_surfaceBuffer, DEFAULT_WIDTH, DEFAULT_WIDTH);
    EXPECT_FALSE(result);
    sptr<Surface> surface = nullptr;
    result = surfaceAdapter.FlushBuffer(surface, g_surfaceBuffer, DEFAULT_WIDTH, DEFAULT_WIDTH);
    EXPECT_FALSE(result);
}
}
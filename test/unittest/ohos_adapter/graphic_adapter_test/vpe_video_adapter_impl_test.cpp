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

#include "detail_enhancer_video.h"
#include "iconsumer_surface.h"
#include "native_window.h"
#include "surface.h"

#define private public
#include "vpe_video_adapter_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Media::VideoProcessingEngine;

namespace OHOS {
namespace Media {
namespace VideoProcessingEngine {

bool g_isProductSupport = false;
bool g_isDetailSupport = false;
bool g_isAihdrSupport = false;
bool g_createSuccess = false;

class MockVpeVideo : public VpeVideo {
public:
    MOCK_METHOD(VPEAlgoErrCode, RegisterCallback, (const std::shared_ptr<VpeVideoCallback>& callback), (override));
    MOCK_METHOD(VPEAlgoErrCode, SetOutputSurface, (const sptr<Surface>& surface), (override));
    MOCK_METHOD(sptr<Surface>, GetInputSurface, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, SetParameter, (const Format& parameter), (override));
    MOCK_METHOD(VPEAlgoErrCode, Start, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, Stop, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, Release, (), (override));
    MOCK_METHOD(VPEAlgoErrCode, ReleaseOutputBuffer, (uint32_t index, bool render), (override));
    ~MockVpeVideo() override = default;
};

std::shared_ptr<MockVpeVideo> g_mockVideo;

bool VpeVideo::IsSupported()
{
    return g_isProductSupport;
}

bool VpeVideo::IsSupported(uint32_t type, [[maybe_unused]] const Format& parameter)
{
    if (type == VIDEO_TYPE_DETAIL_ENHANCER) {
        return g_isDetailSupport;
    }
    if (type == VIDEO_TYPE_AIHDR_ENHANCER) {
        return g_isAihdrSupport;
    }
    return false;
}

std::shared_ptr<VpeVideo> VpeVideo::Create([[maybe_unused]] uint32_t type)
{
    return g_createSuccess ? g_mockVideo : nullptr;
}

bool g_isSurfaceSupport = false;
bool VpeVideo::IsSurfaceSupported(uint32_t type, const sptr<Surface>& surface)
{
    (void)type;
    (void)surface;
    return g_isSurfaceSupport;
}

} // namespace VideoProcessingEngine
} // namespace Media
} // namespace OHOS

namespace OHOS::NWeb {

namespace {
OHNativeWindow* CreateTestWindow()
{
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    if (consumer == nullptr) {
        return nullptr;
    }
    sptr<IBufferProducer> producer = consumer->GetProducer();
    sptr<Surface> surface = Surface::CreateSurfaceAsProducer(producer);
    return OH_NativeWindow_CreateNativeWindow(&surface);
}

sptr<Surface> CreateTestSurface()
{
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("vpe_input");
    if (consumer == nullptr) {
        return nullptr;
    }
    sptr<IBufferProducer> producer = consumer->GetProducer();
    return Surface::CreateSurfaceAsProducer(producer);
}
} // namespace

class VpeVideoAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void VpeVideoAdapterImplTest::SetUpTestCase(void) {}

void VpeVideoAdapterImplTest::TearDownTestCase(void) {}

void VpeVideoAdapterImplTest::SetUp(void)
{
    g_isProductSupport = false;
    g_isDetailSupport = false;
    g_isAihdrSupport = false;
    g_createSuccess = false;
    g_mockVideo.reset();
    g_isSurfaceSupport = true;
}

void VpeVideoAdapterImplTest::TearDown(void) {}

/**
 * @tc.name: VpeVideoAdapterImplTest_GetInstance_001
 * @tc.desc: GetInstance returns the same singleton.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, GetInstance001, TestSize.Level1)
{
    VpeVideoAdapter& instance1 = VpeVideoAdapterImpl::GetInstance();
    VpeVideoAdapter& instance2 = VpeVideoAdapterImpl::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_NullWindow_001
 * @tc.desc: Null window returns nullptr; destructor handles empty map and null entries.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_NullWindow, TestSize.Level1)
{
    // null window returns nullptr; manager 析构时 map 为空（destructor 空表 no-op）
    {
        VpeVideoAdapterImpl manager;
        EXPECT_EQ(manager.CreateVpeSurface(1, nullptr), nullptr);
    }
    // destructor 跳过 map 中的 null entry
    {
        VpeVideoAdapterImpl nullVideoManager;
        nullVideoManager.allVpeVideo_[1] = nullptr;
    }
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_ProductNotSupported_001
 * @tc.desc: CreateVpeSurface degrades to the original window when product unsupported.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_ProductNotSupported, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    g_isProductSupport = false;
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_TypeNotSupported_001
 * @tc.desc: CreateVpeSurface degrades when every type is unsupported.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_TypeNotSupported, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = false;
    g_isAihdrSupport = false;
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_CreateFail_001
 * @tc.desc: CreateVpeSurface degrades when VpeVideo::Create returns null.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_CreateFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = false;
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_RegisterCallbackFail_001
 * @tc.desc: CreateVpeSurface degrades when RegisterCallback fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_RegisterCallbackFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_SetOutputSurfaceFail_001
 * @tc.desc: CreateVpeSurface degrades when SetOutputSurface fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_SetOutputSurfaceFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetParameter(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetOutputSurface(_)).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_GetInputSurfaceNull_001
 * @tc.desc: CreateVpeSurface degrades when GetInputSurface returns null.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_GetInputSurfaceNull, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetParameter(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetOutputSurface(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, GetInputSurface()).WillOnce(Return(nullptr));
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_NotExist_001
 * @tc.desc: ReleaseVpeSurface is a no-op when surfaceId is absent.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_NotExist, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    manager.ReleaseVpeSurface(999);
    EXPECT_TRUE(manager.allVpeVideo_.empty());
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_Success_001
 * @tc.desc: ReleaseVpeSurface stops and releases the VpeVideo.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_Success, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    auto mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.allVpeVideo_[1] = mockVideo;
    manager.ReleaseVpeSurface(1);
    EXPECT_EQ(manager.allVpeVideo_.find(1), manager.allVpeVideo_.end());
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_StopFail_001
 * @tc.desc: ReleaseVpeSurface still erases the entry when Stop fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_StopFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    auto mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_CALL(*mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.allVpeVideo_[1] = mockVideo;
    manager.ReleaseVpeSurface(1);
    EXPECT_EQ(manager.allVpeVideo_.find(1), manager.allVpeVideo_.end());
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_ReleaseFail_001
 * @tc.desc: ReleaseVpeSurface still erases the entry when Release fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_ReleaseFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    auto mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    manager.allVpeVideo_[1] = mockVideo;
    manager.ReleaseVpeSurface(1);
    EXPECT_EQ(manager.allVpeVideo_.find(1), manager.allVpeVideo_.end());
}

/**
 * @tc.name: VpeVideoAdapterImplTest_Destructor_ClearsMap_001
 * @tc.desc: Destructor stops and releases all VpeVideos left in the map.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, Destructor_ClearsMap, TestSize.Level1)
{
    auto mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    {
        VpeVideoAdapterImpl manager;
        manager.allVpeVideo_[1] = mockVideo;
    }
    SUCCEED();
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_SetParameterFail_001
 * @tc.desc: CreateVpeSurface degrades when DETAIL quality SetParameter fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_SetParameterFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetParameter(_)).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_AihdrType_001
 * @tc.desc: AIHDR path skips DETAIL quality in SetVpeParameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_AihdrType, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = false;
    g_isAihdrSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetOutputSurface(_)).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_StartFail_001
 * @tc.desc: CreateVpeSurface degrades when Start fails after GetInputSurface.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_StartFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetParameter(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetOutputSurface(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, GetInputSurface()).WillOnce(Return(CreateTestSurface()));
    EXPECT_CALL(*g_mockVideo, Start()).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_BuildSuccess_001
 * @tc.desc: CreateVpeSurface builds VPE and returns a new native window.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_BuildSuccess, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetParameter(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetOutputSurface(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, GetInputSurface()).WillOnce(Return(CreateTestSurface()));
    EXPECT_CALL(*g_mockVideo, Start()).WillOnce(Return(VPE_ALGO_ERR_OK));
    void* result = manager.CreateVpeSurface(1, window);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result, window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage | BUFFER_USAGE_HW_COMPOSER);
    EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.ReleaseVpeSurface(1);
    OH_NativeWindow_DestroyNativeWindow(window);
    OH_NativeWindow_DestroyNativeWindow(static_cast<OHNativeWindow*>(result));
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_NullVideo_001
 * @tc.desc: ReleaseVpeSurface erases a null entry without Stop/Release.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_NullVideo, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    manager.allVpeVideo_[1] = nullptr;
    manager.ReleaseVpeSurface(1);
    EXPECT_EQ(manager.allVpeVideo_.find(1), manager.allVpeVideo_.end());
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_SurfaceNotSupported_001
 * @tc.desc: CreateVpeSurface degrades when IsSurfaceSupported returns false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_SurfaceNotSupported, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    sptr<Surface> rawSurface = window->surface;
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_isSurfaceSupport = false;  // surface 不支持该 type -> BuildVpeForType 开头降级
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    OH_NativeWindow_DestroyNativeWindow(window);
}
} // namespace OHOS::NWeb

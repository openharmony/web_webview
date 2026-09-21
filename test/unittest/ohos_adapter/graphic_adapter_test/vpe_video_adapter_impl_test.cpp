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

// Expect the full set of VPE setup calls for one successful DETAIL stage.
void ExpectBuildSuccess(const std::shared_ptr<MockVpeVideo>& video)
{
    EXPECT_CALL(*video, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*video, SetParameter(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*video, SetOutputSurface(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*video, GetInputSurface()).WillOnce(Return(CreateTestSurface()));
    EXPECT_CALL(*video, Start()).WillOnce(Return(VPE_ALGO_ERR_OK));
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
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_NullWindow_001
 * @tc.desc: Null window returns nullptr and creates no pipeline.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_NullWindow, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    EXPECT_EQ(manager.CreateVpeSurface(1, nullptr), nullptr);
    EXPECT_TRUE(manager.pipelines_.empty());
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_TypeNotSupported_001
 * @tc.desc: CreateVpeSurface degrades when every type is unsupported; usage restored.
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_CreateFail_001
 * @tc.desc: CreateVpeSurface degrades when VpeVideo::Create returns null; usage restored.
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_RegisterCallbackFail_001
 * @tc.desc: CreateVpeSurface degrades when RegisterCallback fails; usage restored.
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_SetOutputSurfaceFail_001
 * @tc.desc: CreateVpeSurface degrades when SetOutputSurface fails; usage restored.
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_GetInputSurfaceNull_001
 * @tc.desc: CreateVpeSurface degrades when GetInputSurface returns null; usage restored.
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_StartFail_001
 * @tc.desc: CreateVpeSurface degrades when Start fails after GetInputSurface; usage restored.
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_SetParameterFail_001
 * @tc.desc: CreateVpeSurface degrades when DETAIL quality SetParameter fails; usage restored.
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
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_SurfaceNotSupported_001
 * @tc.desc: CreateVpeSurface degrades when IsSurfaceSupported returns false; usage restored.
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
    g_isSurfaceSupport = false; // surface 不支持该 type -> BuildVpeStage 开头降级
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    EXPECT_TRUE(manager.pipelines_.empty());
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
    // AIHDR 类型不进 SetVpeParameter 的 DETAIL 分支，所以不会有 SetParameter 调用
    EXPECT_CALL(*g_mockVideo, RegisterCallback(_)).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, SetOutputSurface(_)).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_EQ(manager.CreateVpeSurface(1, window), window);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage);
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_BuildSuccess_001
 * @tc.desc: CreateVpeSurface builds VPE, returns a new borrowed window (not caller's), and
 *          keeps the HW_COMPOSER usage; ReleaseVpeSurface then tears it down and restores usage.
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
    ExpectBuildSuccess(g_mockVideo);
    void* result = manager.CreateVpeSurface(1, window);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result, window); // adapter 持有，返回借用指针
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage | BUFFER_USAGE_HW_COMPOSER);
    EXPECT_EQ(manager.pipelines_.size(), static_cast<size_t>(1));
    // Release：调用方不销毁返回的 window（adapter 负责）；Stop/Release 各一次
    EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.ReleaseVpeSurface(1);
    EXPECT_EQ(rawSurface->GetDefaultUsage(), originUsage); // usage 已还原
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_Idempotent_001
 * @tc.desc: Repeated Create with the same surfaceId returns the SAME borrowed window and
 *          rebuilds nothing (no extra RegisterCallback/Start calls) — true idempotency.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_Idempotent, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    ExpectBuildSuccess(g_mockVideo); // 仅这一次构建
    void* first = manager.CreateVpeSurface(1, window);
    ASSERT_NE(first, nullptr);
    ASSERT_NE(first, window);
    // 第二次：完全复用，不再触发任何 VPE 调用
    void* second = manager.CreateVpeSurface(1, window);
    EXPECT_EQ(second, first); // 同一借用指针
    EXPECT_EQ(manager.pipelines_.size(), static_cast<size_t>(1));
    EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.ReleaseVpeSurface(1);
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_CreateVpeSurface_IdempotentDifferentWindow_001
 * @tc.desc: A second Create with the same surfaceId but a DIFFERENT window still reuses the
 *          existing pipeline (idempotency wins): returns the same borrowed window, rebuilds
 *          nothing, only logs a mismatch warning. The mismatch never triggers a rebuild.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, CreateVpeSurface_IdempotentDifferentWindow, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    OHNativeWindow* otherWindow = CreateTestWindow();
    ASSERT_NE(otherWindow, nullptr);
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    ExpectBuildSuccess(g_mockVideo); // 仅第一次构建
    void* first = manager.CreateVpeSurface(1, window);
    ASSERT_NE(first, nullptr);
    ASSERT_NE(first, window);
    // 第二次：surfaceId 相同但 window 不同 -> 仅告警，复用旧 pipeline，不重建
    void* second = manager.CreateVpeSurface(1, otherWindow);
    EXPECT_EQ(second, first); // 同一借用指针，未被 otherWindow 覆盖
    EXPECT_EQ(manager.pipelines_.size(), static_cast<size_t>(1));
    EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.ReleaseVpeSurface(1);
    OH_NativeWindow_DestroyNativeWindow(window);
    OH_NativeWindow_DestroyNativeWindow(otherWindow);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_NotExist_001
 * @tc.desc: ReleaseVpeSurface on an unknown surfaceId is a no-op (idempotent), map unchanged.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_NotExist, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    manager.ReleaseVpeSurface(999); // 不存在 -> no-op，不崩溃
    EXPECT_TRUE(manager.pipelines_.empty());
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_Idempotent_001
 * @tc.desc: ReleaseVpeSurface twice: the second is a logged no-op, never crashes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_Idempotent, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    ExpectBuildSuccess(g_mockVideo);
    manager.CreateVpeSurface(1, window);
    EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.ReleaseVpeSurface(1);
    manager.ReleaseVpeSurface(1); // 再次 Release：no-op，不再调 Stop/Release
    EXPECT_TRUE(manager.pipelines_.empty());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_ReleaseVpeSurface_StopFail_001
 * @tc.desc: ReleaseVpeSurface still erases the entry and destroys the borrowed window when Stop fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, ReleaseVpeSurface_StopFail, TestSize.Level1)
{
    VpeVideoAdapterImpl manager;
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    ExpectBuildSuccess(g_mockVideo);
    manager.CreateVpeSurface(1, window);
    EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    manager.ReleaseVpeSurface(1);
    EXPECT_EQ(manager.pipelines_.find(1), manager.pipelines_.end());
    OH_NativeWindow_DestroyNativeWindow(window);
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
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    ExpectBuildSuccess(g_mockVideo);
    manager.CreateVpeSurface(1, window);
    EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
    EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_INVALID_PARAM));
    manager.ReleaseVpeSurface(1);
    EXPECT_EQ(manager.pipelines_.find(1), manager.pipelines_.end());
    OH_NativeWindow_DestroyNativeWindow(window);
}

/**
 * @tc.name: VpeVideoAdapterImplTest_Destructor_ClearsMap_001
 * @tc.desc: Destructor destroys borrowed windows and stops/releases all VPE stages left.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(VpeVideoAdapterImplTest, Destructor_ClearsMap, TestSize.Level1)
{
    OHNativeWindow* window = CreateTestWindow();
    ASSERT_NE(window, nullptr);
    g_isProductSupport = true;
    g_isDetailSupport = true;
    g_createSuccess = true;
    g_mockVideo = std::make_shared<MockVpeVideo>();
    ExpectBuildSuccess(g_mockVideo);
    {
        VpeVideoAdapterImpl manager;
        manager.CreateVpeSurface(1, window);
        EXPECT_EQ(manager.pipelines_.size(), static_cast<size_t>(1));
        EXPECT_CALL(*g_mockVideo, Stop()).WillOnce(Return(VPE_ALGO_ERR_OK));
        EXPECT_CALL(*g_mockVideo, Release()).WillOnce(Return(VPE_ALGO_ERR_OK));
    } // 析构：销毁借用的 vpeWindow + Stop/Release 每个 stage + 还原 usage
    SUCCEED();
    OH_NativeWindow_DestroyNativeWindow(window);
}
} // namespace OHOS::NWeb

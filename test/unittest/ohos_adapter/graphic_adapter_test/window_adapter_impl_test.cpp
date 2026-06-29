/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "iconsumer_surface.h"
#include "ohos_adapter_helper.h"
#include "window.h"
#include "native_window.h"

using testing::ext::TestSize;

namespace OHOS::NWeb {
namespace {
constexpr uint64_t TEST_NODE_ID = 123456;
constexpr uint64_t TEST_RS_HANDLE = 789012;
constexpr uint64_t TEST_ZERO_VALUE = 0;
constexpr uint64_t TEST_LARGE_VALUE = UINT64_MAX;
constexpr uint64_t TEST_LARGE_VALUE_MINUS_ONE = UINT64_MAX - 1;
}

/**
 * @tc.name: NormalTest.
 * @tc.desc: test WindowAdapterImpl class function.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(WindowAdapterImplTest, NormalTest, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("test");
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(nullptr);
    EXPECT_EQ(window, nullptr);
    window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(&pSurface);
    EXPECT_NE(window, nullptr);
    EXPECT_EQ(OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowSetBufferGeometry(
        window, 0, 0), OHOS::GSERROR_OK);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowSurfaceCleanCache(window);
    EXPECT_NE(window, nullptr);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowSurfaceCleanCacheWithPara(window, false);
    EXPECT_NE(window, nullptr);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().DestroyNativeWindow(window);
    window = nullptr;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowSurfaceCleanCacheWithPara(window, false);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().AddNativeWindowRef(window);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowUnRef(window);
    cSurface = IConsumerSurface::Create("test1");
    ASSERT_NE(cSurface, nullptr);
    producer = cSurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(window, nullptr);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().AddNativeWindowRef(window);
    EXPECT_NE(window, nullptr);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().NativeWindowUnRef(window);
    window = nullptr;
}

/**
 * @tc.name: WindowAdapterImplTest_001
 * @tc.desc: test WindowAdapterImpl class function.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST(WindowAdapterImplTest, WindowAdapterImplTest_001, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("test");
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(window, nullptr);
    uint32_t angle = 0;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetTransformHint(angle, window);
    EXPECT_NE(window, nullptr);
    angle = 90;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetTransformHint(angle, window);
    EXPECT_NE(window, nullptr);
    angle = 180;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetTransformHint(angle, window);
    EXPECT_NE(window, nullptr);
    angle = 270;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetTransformHint(angle, window);
    EXPECT_NE(window, nullptr);
    angle = 300;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetTransformHint(angle, window);
    EXPECT_NE(window, nullptr);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().DestroyNativeWindow(window);
}

/**
 * @tc.name: WindowAdapterImpl_SetWindowNodeIdAndRSHandle_NullWindow_002.
 * @tc.desc: Test SetWindowNodeIdAndRSHandle when nativeWindow is nullptr.
 * @tc.type: FUNC.
 * @tc.require: issue#5183
 */
HWTEST(WindowAdapterImplTest, WindowAdapterImpl_SetWindowNodeIdAndRSHandle_NullWindow_002, TestSize.Level1)
{
    auto& adapter = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance();
    void* nullWindow = nullptr;
    uint64_t nodeId = TEST_NODE_ID;
    uint64_t rsHandle = TEST_RS_HANDLE;
    adapter.SetWindowNodeIdAndRSHandle(nullWindow, nodeId, rsHandle);
    EXPECT_EQ(nullWindow, nullptr);
}

/**
 * @tc.name: WindowAdapterImpl_SetWindowNodeIdAndRSHandle_ValidSurface_003.
 * @tc.desc: Test SetWindowNodeIdAndRSHandle when surface is valid.
 * @tc.type: FUNC.
 * @tc.require: issue#5183
 */
HWTEST(WindowAdapterImplTest, WindowAdapterImpl_SetWindowNodeIdAndRSHandle_ValidSurface_003, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("test_nodeid_rshandle");
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(window, nullptr);
    auto nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    ASSERT_NE(nativeWindow, nullptr);
    ASSERT_NE(nativeWindow->surface, nullptr);
    uint64_t nodeId = TEST_NODE_ID;
    uint64_t rsHandle = TEST_RS_HANDLE;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetWindowNodeIdAndRSHandle(window, nodeId, rsHandle);
    EXPECT_NE(window, nullptr);
    if (nativeWindow->surface) {
        auto fetchedNodeId = nativeWindow->surface->GetUserData("delegate_node_id");
        auto fetchedRSHandle = nativeWindow->surface->GetUserData("delegate_connect_to_render");
        EXPECT_EQ(fetchedNodeId, std::to_string(nodeId));
        EXPECT_EQ(fetchedRSHandle, std::to_string(rsHandle));
    }
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().DestroyNativeWindow(window);
}

/**
 * @tc.name: WindowAdapterImpl_SetWindowNodeIdAndRSHandle_NullSurface_004.
 * @tc.desc: Test SetWindowNodeIdAndRSHandle when window->surface is nullptr.
 * @tc.type: FUNC.
 * @tc.require: issue#5183
 */
HWTEST(WindowAdapterImplTest, WindowAdapterImpl_SetWindowNodeIdAndRSHandle_NullSurface_004, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("test_null_surface");
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(window, nullptr);
    auto nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    ASSERT_NE(nativeWindow, nullptr);
    nativeWindow->surface = nullptr;
    uint64_t nodeId = TEST_NODE_ID;
    uint64_t rsHandle = TEST_RS_HANDLE;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetWindowNodeIdAndRSHandle(window, nodeId, rsHandle);
    EXPECT_NE(window, nullptr);
    EXPECT_EQ(nativeWindow->surface, nullptr);
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().DestroyNativeWindow(window);
}

/**
 * @tc.name: WindowAdapterImpl_SetWindowNodeIdAndRSHandle_ZeroValues_005.
 * @tc.desc: Test SetWindowNodeIdAndRSHandle with zero nodeId and rsHandle.
 * @tc.type: FUNC.
 * @tc.require: issue#5183
 */
HWTEST(WindowAdapterImplTest, WindowAdapterImpl_SetWindowNodeIdAndRSHandle_ZeroValues_005, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("test_zero_values");
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(window, nullptr);
    auto nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    ASSERT_NE(nativeWindow, nullptr);
    ASSERT_NE(nativeWindow->surface, nullptr);
    uint64_t nodeId = TEST_ZERO_VALUE;
    uint64_t rsHandle = TEST_ZERO_VALUE;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetWindowNodeIdAndRSHandle(window, nodeId, rsHandle);
    EXPECT_NE(window, nullptr);
    if (nativeWindow->surface) {
        auto fetchedNodeId = nativeWindow->surface->GetUserData("delegate_node_id");
        auto fetchedRSHandle = nativeWindow->surface->GetUserData("delegate_connect_to_render");
        EXPECT_EQ(fetchedNodeId, "0");
        EXPECT_EQ(fetchedRSHandle, "0");
    }
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().DestroyNativeWindow(window);
}

/**
 * @tc.name: WindowAdapterImpl_SetWindowNodeIdAndRSHandle_LargeValues_006.
 * @tc.desc: Test SetWindowNodeIdAndRSHandle with large nodeId and rsHandle values.
 * @tc.type: FUNC.
 * @tc.require: issue#5183
 */
HWTEST(WindowAdapterImplTest, WindowAdapterImpl_SetWindowNodeIdAndRSHandle_LargeValues_006, TestSize.Level1)
{
    auto cSurface = IConsumerSurface::Create("test_large_values");
    ASSERT_NE(cSurface, nullptr);
    auto producer = cSurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(pSurface, nullptr);
    auto window = OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().CreateNativeWindowFromSurface(&pSurface);
    ASSERT_NE(window, nullptr);
    auto nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    ASSERT_NE(nativeWindow, nullptr);
    ASSERT_NE(nativeWindow->surface, nullptr);
    uint64_t nodeId = TEST_LARGE_VALUE;
    uint64_t rsHandle = TEST_LARGE_VALUE_MINUS_ONE;
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().SetWindowNodeIdAndRSHandle(window, nodeId, rsHandle);
    EXPECT_NE(window, nullptr);
    if (nativeWindow->surface) {
        auto fetchedNodeId = nativeWindow->surface->GetUserData("delegate_node_id");
        auto fetchedRSHandle = nativeWindow->surface->GetUserData("delegate_connect_to_render");
        EXPECT_EQ(fetchedNodeId, std::to_string(nodeId));
        EXPECT_EQ(fetchedRSHandle, std::to_string(rsHandle));
    }
    OhosAdapterHelper::GetInstance().GetWindowAdapterInstance().DestroyNativeWindow(window);
}
} // namespace OHOS::NWeb

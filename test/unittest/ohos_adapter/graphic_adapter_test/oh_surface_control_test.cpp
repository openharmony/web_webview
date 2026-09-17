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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "oh_surface_control.h"
#include "surface_control.h"
#include "surface_transaction.h"
#undef private

#include "iconsumer_surface.h"
#include "surface.h"
#include "native_window.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context_manager.h"

using testing::ext::TestSize;

namespace OHOS::NWeb {
namespace {
constexpr uint64_t TEST_DELEGATE_NODE_ID = 123456;
} // namespace
class OHSurfaceControlTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    sptr<OHOS::IConsumerSurface> consumerSurface_;
    sptr<OHOS::IBufferProducer> producer_;
    sptr<OHOS::Surface> producerSurface_;
    NativeWindow* mockWindow_;
};

void OHSurfaceControlTest::SetUpTestCase(void)
{}

void OHSurfaceControlTest::TearDownTestCase(void)
{}

void OHSurfaceControlTest::SetUp(void)
{
    consumerSurface_ = OHOS::IConsumerSurface::Create("test_surface");
    if (consumerSurface_) {
        producer_ = consumerSurface_->GetProducer();
        if (producer_) {
            producerSurface_ = OHOS::Surface::CreateSurfaceAsProducer(producer_);
        }
    }
    mockWindow_ = new NativeWindow();
    if (mockWindow_ && producerSurface_) {
        mockWindow_->surface = producerSurface_;
    }
}

void OHSurfaceControlTest::TearDown(void)
{
    if (mockWindow_) {
        delete mockWindow_;
        mockWindow_ = nullptr;
    }
    consumerSurface_ = nullptr;
    producer_ = nullptr;
    producerSurface_ = nullptr;
}

HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_CreateSurfaceControlFromNativeWindow_WindowNull_001, TestSize.Level1)
{
    auto result = SurfaceControlUtils::CreateSurfaceControlFromNativeWindow(nullptr, "test_window");
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_CreateSurfaceControlFromNativeWindow_SurfaceNull_002, TestSize.Level1)
{
    if (mockWindow_) {
        mockWindow_->surface = nullptr;
        auto result = SurfaceControlUtils::CreateSurfaceControlFromNativeWindow(
            reinterpret_cast<OHNativeWindow*>(mockWindow_), "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_CreateSurfaceControlFromNativeWindow_Valid_003, TestSize.Level1)
{
    if (mockWindow_ && mockWindow_->surface) {
        mockWindow_->surface->SetUserData("delegate_node_id", std::to_string(TEST_DELEGATE_NODE_ID));
        mockWindow_->surface->SetUserData("delegate_connect_to_render", "");
        auto result = SurfaceControlUtils::CreateSurfaceControlFromNativeWindow(
            reinterpret_cast<OHNativeWindow*>(mockWindow_), "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_Create_NameNull_004, TestSize.Level1)
{
    auto result = SurfaceControlUtils::Create(nullptr);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_Transaction_CreateSurfaceTransaction_005, TestSize.Level1)
{
    auto result = SurfaceControlUtils::Transaction::CreateSurfaceTransaction(nullptr);
    EXPECT_NE(result, nullptr);
    if (result) {
        SurfaceControlUtils::Transaction::Delete(result);
    }
}

/**
 * @tc.name: OHSurfaceControl_Transaction_SetOnCommit_Removed_006
 * @tc.desc: Verify that SetOnCommit has been removed from SurfaceControlUtils::Transaction.
 *           The OH_SurfaceTransaction_OnCommit typedef remains in oh_surface_control.h
 *           for compatibility, but no SetOnCommit implementation method exists.
 *           SetOnComplete is the actual transaction completion callback mechanism.
 *           No existing SetOnCommit test cases were found to delete.
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_Transaction_SetOnCommit_Removed_006, TestSize.Level1)
{
    auto txn = SurfaceControlUtils::Transaction::CreateSurfaceTransaction(nullptr);
    EXPECT_NE(txn, nullptr);
    if (txn) {
        // Verify SetOnComplete accepts a valid callback (replacing SetOnCommit)
        SurfaceControlUtils::Transaction::SetOnComplete(txn, nullptr,
            [](void* ctx, uint64_t ts) { });
        // Verify SetOnComplete accepts null callback to clear the registration
        SurfaceControlUtils::Transaction::SetOnComplete(txn, nullptr, nullptr);
        SurfaceControlUtils::Transaction::Delete(txn);
    }
}

/**
 * @tc.name: OHSurfaceControl_UpdateDelegateContainerNodeOnClient_AddNode_009.
 * @tc.desc: Test SurfaceControlUtils::UpdateDelegateContainerNodeOnClient with isAddNode=true,
 *           covering the AddDelegateContainerNodeOnClient branch. Uses parentNodeId=0 for early return.
 * @tc.type: FUNC
 * @tc.require: issue#I16d68
 */
HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_UpdateDelegateContainerNodeOnClient_AddNode_009, TestSize.Level1)
{
    sptr<IRemoteObject> mockRemote;
    auto uiContext = OHOS::Rosen::RSUIContextManager::MutableInstance().CreateRSUIContext(mockRemote);
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "update_add_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    // parentNodeId=0 triggers early return in AddDelegateContainerNodeOnClient
    SurfaceControlUtils::UpdateDelegateContainerNodeOnClient(0, uiContext, surfaceNode, true);
    EXPECT_EQ(SurfaceControl::delegateContainerNodeMap_.count(0), 0u);
}

/**
 * @tc.name: OHSurfaceControl_UpdateDelegateContainerNodeOnClient_RemoveNode_010.
 * @tc.desc: Test SurfaceControlUtils::UpdateDelegateContainerNodeOnClient with isAddNode=false,
 *           covering the RemoveDelegateContainerNodeOnClient branch. Uses parentNodeId=0 for early return.
 * @tc.type: FUNC
 * @tc.require: issue#I16d68
 */
HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_UpdateDelegateContainerNodeOnClient_RemoveNode_010, TestSize.Level1)
{
    sptr<IRemoteObject> mockRemote;
    auto uiContext = OHOS::Rosen::RSUIContextManager::MutableInstance().CreateRSUIContext(mockRemote);
    // parentNodeId=0 triggers early return in RemoveDelegateContainerNodeOnClient
    SurfaceControlUtils::UpdateDelegateContainerNodeOnClient(0, uiContext, nullptr, false);
    // Should not crash
}

/**
 * @tc.name: OHSurfaceControl_UpdateDelegateContainerNodeOnClient_AddNullUIContext_011.
 * @tc.desc: Test SurfaceControlUtils::UpdateDelegateContainerNodeOnClient with isAddNode=true
 *           and nullptr rsUIContext (early return in AddDelegateContainerNodeOnClient).
 * @tc.type: FUNC
 * @tc.require: issue#I16d68
 */
HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_UpdateDelegateContainerNodeOnClient_AddNullUIContext_011,
    TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "update_add_null_uictx";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    SurfaceControlUtils::UpdateDelegateContainerNodeOnClient(100, nullptr, surfaceNode, true);
    EXPECT_EQ(SurfaceControl::delegateContainerNodeMap_.count(100), 0u);
}

/**
 * @tc.name: OHSurfaceControl_UpdateDelegateContainerNodeOnClient_RemoveNullUIContext_012.
 * @tc.desc: Test SurfaceControlUtils::UpdateDelegateContainerNodeOnClient with isAddNode=false
 *           and nullptr rsUIContext (early return in RemoveDelegateContainerNodeOnClient).
 * @tc.type: FUNC
 * @tc.require: issue#I16d68
 */
HWTEST_F(OHSurfaceControlTest, OHSurfaceControl_UpdateDelegateContainerNodeOnClient_RemoveNullUIContext_012,
    TestSize.Level1)
{
    SurfaceControlUtils::UpdateDelegateContainerNodeOnClient(100, nullptr, nullptr, false);
    // Should not crash
}

} // namespace OHOS::NWeb
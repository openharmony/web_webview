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

// Pre-include headers that must NOT be corrupted by #define private public.
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_ui_context.h"

#define private public
#include "surface_transaction.h"
#include "surface_control.h"
#include "oh_surface_control.h"
#undef private

// Access RSUIContextManager::isMultiInstanceOpen_ for test_019
#define private public
#include "ui/rs_ui_context_manager.h"
#undef private

#include "iconsumer_surface.h"
#include "ipc_object_stub.h"
#include "surface.h"
#include "native_window.h"
#include "surface_transaction_helper.h"
#include "surface_buffer.h"

using testing::ext::TestSize;

namespace OHOS::NWeb {

class SurfaceTransactionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

protected:
    SurfaceTransaction* transaction_;
    sptr<OHOS::IConsumerSurface> consumerSurface_;
    sptr<OHOS::IBufferProducer> producer_;
    sptr<OHOS::Surface> producerSurface_;
    NativeWindow* mockWindow_;
};

void SurfaceTransactionTest::SetUpTestCase(void)
{}

void SurfaceTransactionTest::TearDownTestCase(void)
{}

void SurfaceTransactionTest::SetUp(void)
{
    transaction_ = new SurfaceTransaction(nullptr);
    consumerSurface_ = OHOS::IConsumerSurface::Create("test_surface_txn");
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

void SurfaceTransactionTest::TearDown(void)
{
    if (transaction_) {
        delete transaction_;
        transaction_ = nullptr;
    }
    if (mockWindow_) {
        delete mockWindow_;
        mockWindow_ = nullptr;
    }
    consumerSurface_ = nullptr;
    producer_ = nullptr;
    producerSurface_ = nullptr;
}

/**
 * @tc.name: SurfaceTransaction_Constructor_001
 * @tc.desc: Test SurfaceTransaction constructor with null nativeWindow
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Constructor_001, TestSize.Level1)
{
    auto txn = new SurfaceTransaction(nullptr);
    EXPECT_NE(txn, nullptr);
    delete txn;
}

/**
 * @tc.name: SurfaceTransaction_Constructor_SurfaceNull_002
 * @tc.desc: Test SurfaceTransaction constructor with nativeWindow having null surface
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Constructor_SurfaceNull_002, TestSize.Level1)
{
    NativeWindow* window = new NativeWindow();
    window->surface = nullptr;
    auto txn = new SurfaceTransaction(reinterpret_cast<OHNativeWindow*>(window));
    EXPECT_EQ(txn->listener_, nullptr);
    delete txn;
    delete window;
}

/**
 * @tc.name: SurfaceTransaction_Constructor_EmptyRSHandle_003
 * @tc.desc: Test SurfaceTransaction constructor with surface having empty delegate_connect_to_render
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Constructor_EmptyRSHandle_003, TestSize.Level1)
{
    if (mockWindow_ && producerSurface_) {
        producerSurface_->SetUserData("delegate_connect_to_render", "");
        auto txn = new SurfaceTransaction(reinterpret_cast<OHNativeWindow*>(mockWindow_));
        EXPECT_EQ(txn->listener_, nullptr);
        delete txn;
    }
}

/**
 * @tc.name: SurfaceTransaction_Constructor_ZeroHandle_004
 * @tc.desc: Test SurfaceTransaction constructor with surface having delegate_connect_to_render = "0"
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Constructor_ZeroHandle_004, TestSize.Level1)
{
    if (mockWindow_ && producerSurface_) {
        producerSurface_->SetUserData("delegate_connect_to_render", "0");
        auto txn = new SurfaceTransaction(reinterpret_cast<OHNativeWindow*>(mockWindow_));
        EXPECT_EQ(txn->listener_, nullptr);
        delete txn;
    }
}

/**
 * @tc.name: SurfaceTransaction_Destructor_ListenerNull_005
 * @tc.desc: Test SurfaceTransaction destructor when listener_ is null (constructor failed case)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Destructor_ListenerNull_005, TestSize.Level1)
{
    auto txn = new SurfaceTransaction(nullptr);
    EXPECT_EQ(txn->listener_, nullptr);
    delete txn; // Should not crash when listener_ is null
}

/**
 * @tc.name: SurfaceTransaction_OnCompleteCallBack_CallbackNotFound_006
 * @tc.desc: Test OnCompleteCallBack when callback is not registered for given srcId/seqNum
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_OnCompleteCallBack_CallbackNotFound_006, TestSize.Level1)
{
    uint64_t timestamp = 100;
    uint64_t srcId = 999;
    std::queue<uint64_t> seqNums;
    seqNums.push(1);
    // No callback registered for srcId=999, seqNum=1
    SurfaceTransaction::OnCompleteCallBack(timestamp, srcId, seqNums);
    // Should not crash, logs error and continues
}

/**
 * @tc.name: SurfaceTransaction_OnCompleteCallBack_CallbackFound_007
 * @tc.desc: Test OnCompleteCallBack when callback is found and invoked
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_OnCompleteCallBack_CallbackFound_007, TestSize.Level1)
{
    uint64_t timestamp = 100;
    uint64_t srcId = 888;
    uint64_t seqNum = 1;
    bool callbackInvoked = false;
    uint64_t receivedTimestamp = 0;
    SurfaceTransactionCallBackHelper::GetInstance().RegisterCallBack(srcId, seqNum,
        [&callbackInvoked, &receivedTimestamp](uint64_t ts) {
            callbackInvoked = true;
            receivedTimestamp = ts;
        });
    std::queue<uint64_t> seqNums;
    seqNums.push(seqNum);
    SurfaceTransaction::OnCompleteCallBack(timestamp, srcId, seqNums);
    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ(receivedTimestamp, timestamp);
    SurfaceTransactionCallBackHelper::GetInstance().UnRegisterCallBack(srcId);
}

/**
 * @tc.name: SurfaceTransaction_Commit_UiContextNull_008
 * @tc.desc: Test Commit when uiContext is null (early return, commands not cleared)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Commit_UiContextNull_008, TestSize.Level1)
{
    // transaction_ was created with nullptr, listener_ is null
    // SurfaceControl::GetRSUIContext() returns nullptr when connectToRender_ is not set
    transaction_->transactionCommands_.push_back([]() {});
    EXPECT_EQ(transaction_->transactionCommands_.size(), 1u);
    transaction_->Commit();
    // uiContext is null → early return, commands should NOT be cleared
    EXPECT_EQ(transaction_->transactionCommands_.size(), 1u);
}

/**
 * @tc.name: SurfaceTransaction_Reparent_NullSurfaceControl_009
 * @tc.desc: Test Reparent with null surfaceControl (early return, no command pushed)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Reparent_NullSurfaceControl_009, TestSize.Level1)
{
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();
    transaction_->Reparent(nullptr, nullptr);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore);
}

/**
 * @tc.name: SurfaceTransaction_Reparent_IsRootSurfaceFalse_010
 * @tc.desc: Test Reparent with non-root SurfaceControl (IsRootSurface=false, command pushed)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Reparent_IsRootSurfaceFalse_010, TestSize.Level1)
{
    sptr<SurfaceControl> ctrl = new SurfaceControl(nullptr, nullptr, false);
    EXPECT_FALSE(ctrl->IsRootSurface()); // isRootSurface_=false
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();
    transaction_->Reparent(ctrl.GetRefPtr(), nullptr);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 1);
}

/**
 * @tc.name: SurfaceTransaction_Setters_NullSurfaceControl_011
 * @tc.desc: Test all setter functions with null surfaceControl (unified null-check test
 *           covering SetVisibility, SetZOrder, SetBuffer, SetBounds,
 *           SetPivot, SetBufferTransform, SetTranslate, SetDamageRegion, SetBufferAlpha,
 *           SetBackgroundColor, SetName, SetHardwareEnableHint, SetSrcRect, SetDisplayRect,
 *           ClearBufferQueueCache)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Setters_NullSurfaceControl_011, TestSize.Level1)
{
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();
    size_t bufCmdSizeBefore = transaction_->bufferCommands_.size();

    transaction_->SetVisibility(nullptr, true);
    transaction_->SetZOrder(nullptr, 0);
    transaction_->SetBuffer(nullptr, nullptr, 0, {});
    transaction_->SetBounds(nullptr, 0, 0, 0, 0);
    transaction_->SetPivot(nullptr, 0, 0);
    transaction_->SetBufferTransform(nullptr, GraphicTransformType::GRAPHIC_ROTATE_NONE);
    transaction_->SetDamageRegion(nullptr, nullptr, 0);
    transaction_->SetBufferAlpha(nullptr, 0);
    transaction_->SetBackgroundColor(nullptr, 0, 0, 0, 0);
    transaction_->SetName(nullptr, "test");
    transaction_->SetHardwareEnableHint(nullptr, true);
    transaction_->SetSrcRect(nullptr, 0, 0, 0, 0);
    transaction_->SetDisplayRect(nullptr, 0, 0, 0, 0);
    transaction_->ClearBufferQueueCache(nullptr, true);

    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore);
    EXPECT_EQ(transaction_->bufferCommands_.size(), bufCmdSizeBefore);
}

/**
 * @tc.name: SurfaceTransaction_Setters_ValidSurfaceControl_012
 * @tc.desc: Test simple setter functions with valid SurfaceControl (unified valid test
 *           verifying each setter pushes a command to transactionCommands_)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Setters_ValidSurfaceControl_012, TestSize.Level1)
{
    sptr<SurfaceControl> ctrl = new SurfaceControl(nullptr, nullptr, false);
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();

    transaction_->SetVisibility(ctrl.GetRefPtr(), true);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 1);

    transaction_->SetZOrder(ctrl.GetRefPtr(), 5);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 2);

    transaction_->SetBounds(ctrl.GetRefPtr(), 1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 3);

    transaction_->SetPivot(ctrl.GetRefPtr(), 0.5f, 0.5f);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 4);

    transaction_->SetBufferAlpha(ctrl.GetRefPtr(), 0.5f);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 5);

    transaction_->SetBackgroundColor(ctrl.GetRefPtr(), 0.0f, 1.0f, 0.0f, 1.0f);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 6);

    transaction_->SetName(ctrl.GetRefPtr(), "test_name");
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 7);

    transaction_->SetHardwareEnableHint(ctrl.GetRefPtr(), true);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 8);

    transaction_->SetSrcRect(ctrl.GetRefPtr(), 1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 9);

    transaction_->SetDisplayRect(ctrl.GetRefPtr(), 1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 10);

    transaction_->ClearBufferQueueCache(ctrl.GetRefPtr(), true);
}

/**
 * @tc.name: SurfaceTransaction_SetBuffer_Valid_013
 * @tc.desc: Test SetBuffer with valid SurfaceControl (command pushed to bufferCommands_
 *           and SurfaceControl added to surfaceControls_)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_SetBuffer_Valid_013, TestSize.Level1)
{
    sptr<SurfaceControl> ctrl = new SurfaceControl(nullptr, nullptr, false);
    size_t bufCmdSizeBefore = transaction_->bufferCommands_.size();
    size_t surfCtrlSizeBefore = transaction_->surfaceControls_.size();

    transaction_->SetBuffer(ctrl.GetRefPtr(), nullptr, -1, [](int) {});

    EXPECT_EQ(transaction_->bufferCommands_.size(), bufCmdSizeBefore + 1);
    EXPECT_EQ(transaction_->surfaceControls_.size(), surfCtrlSizeBefore + 1);
}

/**
 * @tc.name: SurfaceTransaction_SetBufferTransform_Valid_014
 * @tc.desc: Test SetBufferTransform with valid SurfaceControl (command pushed to
 *           transactionCommands_ and SurfaceControl added to surfaceControls_)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_SetBufferTransform_Valid_014, TestSize.Level1)
{
    sptr<SurfaceControl> ctrl = new SurfaceControl(nullptr, nullptr, false);
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();
    size_t surfCtrlSizeBefore = transaction_->surfaceControls_.size();

    transaction_->SetBufferTransform(ctrl.GetRefPtr(), GraphicTransformType::GRAPHIC_ROTATE_NONE);

    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 1);
    EXPECT_EQ(transaction_->surfaceControls_.size(), surfCtrlSizeBefore + 1);
}

/**
 * @tc.name: SurfaceTransaction_SetDamageRegion_CountZero_015
 * @tc.desc: Test SetDamageRegion with count=0 (damageRects shared_ptr stays null,
 *           command still pushed to transactionCommands_)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_SetDamageRegion_CountZero_015, TestSize.Level1)
{
    sptr<SurfaceControl> ctrl = new SurfaceControl(nullptr, nullptr, false);
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();

    transaction_->SetDamageRegion(ctrl.GetRefPtr(), nullptr, 0);

    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 1);
}

/**
 * @tc.name: SurfaceTransaction_SetDamageRegion_ValidRectsCountZero_015b
 * @tc.desc: Test SetDamageRegion with non-null rects but count=0,
 *           covering if (count > 0) false branch (damageRects stays empty).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_SetDamageRegion_ValidRectsCountZero_015b, TestSize.Level1)
{
    sptr<SurfaceControl> ctrl = new SurfaceControl(nullptr, nullptr, false);
    OH_Rect dummyRect = {0, 0, 0, 0};
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();
 
    transaction_->SetDamageRegion(ctrl.GetRefPtr(), &dummyRect, 0);
 
    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 1);
}

/**
 * @tc.name: SurfaceTransaction_SetDamageRegion_ValidCount_016
 * @tc.desc: Test SetDamageRegion with valid count and rects (damageRects populated,
 *           command pushed to transactionCommands_)
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_SetDamageRegion_ValidCount_016, TestSize.Level1)
{
    sptr<SurfaceControl> ctrl = new SurfaceControl(nullptr, nullptr, false);
    OH_Rect rects[] = {{1, 2, 3, 4}, {5, 6, 7, 8}};
    size_t cmdSizeBefore = transaction_->transactionCommands_.size();

    transaction_->SetDamageRegion(ctrl.GetRefPtr(), rects, 2);

    EXPECT_EQ(transaction_->transactionCommands_.size(), cmdSizeBefore + 1);
}

/**
 * @tc.name: SurfaceTransaction_SetOnComplete_017
 * @tc.desc: Test SetOnComplete callback setting and invocation
 * @tc.type: FUNC
 * @tc.require: issueNumber
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_SetOnComplete_017, TestSize.Level1)
{
    bool callbackInvoked = false;
    transaction_->SetOnComplete([&callbackInvoked](uint64_t timestamp) {
        callbackInvoked = true;
    });
    EXPECT_TRUE(transaction_->onCompleteCallback_);
    transaction_->onCompleteCallback_(100);
    EXPECT_TRUE(callbackInvoked);
}

/**
 * @tc.name: SurfaceTransaction_Constructor_ValidRSHandle_018
 * @tc.desc: Test SurfaceTransaction constructor with a valid non-zero RS handle string,
 *           covering the false branches of:
 *           if (ec != std::errc{} || (ptr != fetchedRSHandle.data() + fetchedRSHandle.size()) || handle == 0)
 *           Uses a real IRemoteObject address as the handle value.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_Constructor_ValidRSHandle_018, TestSize.Level1)
{
    if (mockWindow_ && producerSurface_) {
        // Create a real IRemoteObject and use its address as the handle
        sptr<IRemoteObject> realRemote = new OHOS::IPCObjectStub();
        uint64_t handle = reinterpret_cast<uint64_t>(realRemote.GetRefPtr());
        std::string handleStr = std::to_string(handle);
 
        producerSurface_->SetUserData("delegate_connect_to_render", handleStr);
 
        auto txn = new SurfaceTransaction(reinterpret_cast<OHNativeWindow*>(mockWindow_));
        // Should reach SetConnectToRenderObject and listener creation
        if (txn) {
            // Verify the connectToRender was set
            EXPECT_NE(SurfaceControl::GetConnectToRenderObject(), nullptr);
            EXPECT_EQ(txn->listener_ != nullptr, true);
        }
        delete txn;
 
        // Clean up
        SurfaceControl::SetConnectToRenderObject(nullptr);
    }
}
 
/**
 * @tc.name: SurfaceTransaction_SetDamageRegion_Commit_019
 * @tc.desc: Test SetDamageRegion with valid surfaceControl and RSUIContext,
 *           then Commit to execute the lambda, covering
 *           surface->SetDamageRegion(std::move(damageRects)).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceTransactionTest, SurfaceTransaction_SetDamageRegion_Commit_019, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "test_damage_region";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    ASSERT_NE(surfaceNode, nullptr);
 
    sptr<SurfaceControl> ctrl = new SurfaceControl(std::move(surfaceNode), nullptr, false);
    ASSERT_NE(ctrl, nullptr);
 
    // Set up RSUIContext so Commit() can proceed
    auto& manager = OHOS::Rosen::RSUIContextManager::MutableInstance();
    manager.isMultiInstanceOpen_ = true;
    sptr<IRemoteObject> mockRemote = new OHOS::IPCObjectStub();
    auto uiContext = manager.CreateRSUIContext(mockRemote);
    ASSERT_NE(uiContext, nullptr);
 
    auto director = OHOS::Rosen::RSUIDirector::Create(nullptr, uiContext);
    ASSERT_NE(director, nullptr);
    SurfaceControl::rsUIDirector_ = director;
 
    // Create a new SurfaceTransaction with valid RSHandle
    ASSERT_NE(mockWindow_, nullptr);
    ASSERT_NE(producerSurface_, nullptr);
    uint64_t handle = reinterpret_cast<uint64_t>(mockRemote.GetRefPtr());
    producerSurface_->SetUserData("delegate_connect_to_render", std::to_string(handle));
    auto txn = new SurfaceTransaction(reinterpret_cast<OHNativeWindow*>(mockWindow_));
    ASSERT_NE(txn, nullptr);
 
    // SetDamageRegion pushes a command, then Commit executes it
    size_t cmdSizeBefore = txn->transactionCommands_.size();
    OH_Rect rects[] = {{1, 2, 3, 4}};
    txn->SetDamageRegion(ctrl.GetRefPtr(), rects, 1);
    EXPECT_EQ(txn->transactionCommands_.size(), cmdSizeBefore + 1);
 
    // Commit executes the lambda, calling surface->SetDamageRegion
    txn->Commit();
    EXPECT_EQ(txn->transactionCommands_.size(), 0u);
 
    delete txn;
 
    // Clean up
    SurfaceControl::rsUIDirector_ = nullptr;
    SurfaceControl::SetConnectToRenderObject(nullptr);
    manager.isMultiInstanceOpen_ = false;
}

} // namespace OHOS::NWeb

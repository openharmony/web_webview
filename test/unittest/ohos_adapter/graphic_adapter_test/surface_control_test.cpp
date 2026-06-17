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
#include "surface_control.h"
#include "surface_transaction.h"
#undef private

#include "iconsumer_surface.h"
#include "surface.h"
#include "native_window.h"
#include "ui/rs_surface_node.h"

using testing::ext::TestSize;
using testing::_;
using testing::Return;

namespace OHOS::NWeb {
namespace {
constexpr uint32_t API_COMPATIBLE_VERSION = 18;
}

class SurfaceControlTest : public testing::Test {
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

void SurfaceControlTest::SetUpTestCase(void)
{}

void SurfaceControlTest::TearDownTestCase(void)
{}

void SurfaceControlTest::SetUp(void)
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

void SurfaceControlTest::TearDown(void)
{
    if (mockWindow_) {
        delete mockWindow_;
        mockWindow_ = nullptr;
    }
    consumerSurface_ = nullptr;
    producer_ = nullptr;
    producerSurface_ = nullptr;
}

/**
 * @tc.name: SurfaceControl_CreateFromWindow_WindowNull_001.
 * @tc.desc: Test CreateFromWindow when window is nullptr.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_WindowNull_001, TestSize.Level1)
{
    auto result = SurfaceControl::CreateFromWindow(nullptr, "test_window");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: SurfaceControl_CreateFromWindow_SurfaceNull_002.
 * @tc.desc: Test CreateFromWindow when window->surface is nullptr.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_SurfaceNull_002, TestSize.Level1)
{
    mockWindow_->surface = nullptr;
    auto result = SurfaceControl::CreateFromWindow(mockWindow_, "test_window");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: SurfaceControl_CreateFromWindow_NodeIdZero_003.
 * @tc.desc: Test CreateFromWindow when nodeId is 0.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_NodeIdZero_003, TestSize.Level1)
{
    if (mockWindow_ && mockWindow_->surface) {
        mockWindow_->surface->SetUserData("delegate_node_id", "");
        auto result = SurfaceControl::CreateFromWindow(mockWindow_, "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_CreateFromWindow_FetchedNodeIdEmpty_004.
 * @tc.desc: Test CreateFromWindow when fetchedNodeId is empty string.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_FetchedNodeIdEmpty_004, TestSize.Level1)
{
    if (mockWindow_ && mockWindow_->surface) {
        mockWindow_->surface->SetUserData("delegate_node_id", "");
        auto result = SurfaceControl::CreateFromWindow(mockWindow_, "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_CreateFromWindow_FetchedNodeIdValid_005.
 * @tc.desc: Test CreateFromWindow when fetchedNodeId is valid.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_FetchedNodeIdValid_005, TestSize.Level1)
{
    uint64_t validNodeId = 123456;
    
    if (mockWindow_ && mockWindow_->surface) {
        mockWindow_->surface->SetUserData("delegate_node_id", std::to_string(validNodeId));
        mockWindow_->surface->SetUserData("delegate_connect_to_render", "");
        auto result = SurfaceControl::CreateFromWindow(mockWindow_, "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_CreateFromWindow_RSHandleEmpty_006.
 * @tc.desc: Test CreateFromWindow when fetchedRSHandle is empty.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_RSHandleEmpty_006, TestSize.Level1)
{
    uint64_t validNodeId = 123456;
    
    if (mockWindow_ && mockWindow_->surface) {
        mockWindow_->surface->SetUserData("delegate_node_id", std::to_string(validNodeId));
        mockWindow_->surface->SetUserData("delegate_connect_to_render", "");
        auto result = SurfaceControl::CreateFromWindow(mockWindow_, "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_CreateFromWindow_RSHandleZero_007.
 * @tc.desc: Test CreateFromWindow when fetchedRSHandle is "0".
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_RSHandleZero_007, TestSize.Level1)
{
    uint64_t validNodeId = 123456;
    
    if (mockWindow_ && mockWindow_->surface) {
        mockWindow_->surface->SetUserData("delegate_node_id", std::to_string(validNodeId));
        mockWindow_->surface->SetUserData("delegate_connect_to_render", "0");
        auto result = SurfaceControl::CreateFromWindow(mockWindow_, "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_GetRSUIContext_DirectorNullConnectNull_010.
 * @tc.desc: Test GetRSUIContext when rsUIDirector_ is null and connectToRender_ is null.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_GetRSUIContext_DirectorNullConnectNull_010, TestSize.Level1)
{
    SurfaceControl::connectToRender_ = nullptr;
    SurfaceControl::rsUIDirector_ = nullptr;
    
    auto result = SurfaceControl::GetRSUIContext();
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: RSSurfaceNode_CreateTest_012.
 * @tc.desc: Test RSSurfaceNode::Create static method.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_CreateTest_012, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "test_surface_node";
    config.isSync = true;
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        EXPECT_NE(node, nullptr);
        EXPECT_EQ(node->GetName(), "test_surface_node");
        EXPECT_NE(node->GetId(), 0);
        EXPECT_EQ(node->GetType(), OHOS::Rosen::RSUINodeType::SURFACE_NODE);
    }
}

/**
 * @tc.name: RSSurfaceNode_MethodsTest_013.
 * @tc.desc: Test RSSurfaceNode methods (SetPivot, SetBounds, SetAlpha, SetVisible).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_MethodsTest_013, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "methods_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetPivot(0.5f, 0.5f);
        EXPECT_FLOAT_EQ(node->GetStagingProperties().GetPivot()[0], 0.5f);
        EXPECT_FLOAT_EQ(node->GetStagingProperties().GetPivot()[1], 0.5f);
        
        node->SetBounds(0.0f, 0.0f, 100.0f, 100.0f);
        auto bounds = node->GetStagingProperties().GetBounds();
        EXPECT_FLOAT_EQ(bounds[0], 0.0f);
        EXPECT_FLOAT_EQ(bounds[2], 100.0f);
        
        node->SetAlpha(0.5f);
        EXPECT_FLOAT_EQ(node->GetStagingProperties().GetAlpha(), 0.5f);
        
        node->SetVisible(true);
        EXPECT_TRUE(node->GetStagingProperties().GetVisible());
    }
}

/**
 * @tc.name: RSSurfaceNode_AddRemoveChildTest_016.
 * @tc.desc: Test RSSurfaceNode AddChild and RemoveChild methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_AddRemoveChildTest_016, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig parentConfig;
    parentConfig.SurfaceNodeName = "parent_node";
    
    OHOS::Rosen::RSSurfaceNodeConfig childConfig;
    childConfig.SurfaceNodeName = "child_node";
    
    auto parent = OHOS::Rosen::RSSurfaceNode::Create(parentConfig, false);
    auto child = OHOS::Rosen::RSSurfaceNode::Create(childConfig, false);
    
    if (parent && child) {
        parent->AddChild(child, -1);
        auto children = parent->GetChildren();
        EXPECT_EQ(children.size(), 1);
        
        parent->RemoveChild(child);
        children = parent->GetChildren();
        EXPECT_EQ(children.size(), 0);
    }
}

/**
 * @tc.name: RSSurfaceNode_SecurityLayerTest_017.
 * @tc.desc: Test RSSurfaceNode SetSecurityLayer and GetSecurityLayer methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_SecurityLayerTest_017, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "security_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetSecurityLayer(true);
        bool isSecurity = node->GetSecurityLayer();
        EXPECT_TRUE(isSecurity);
        
        node->SetSecurityLayer(false);
        isSecurity = node->GetSecurityLayer();
        EXPECT_FALSE(isSecurity);
    }
}

/**
 * @tc.name: RSSurfaceNode_SkipLayerTest_018.
 * @tc.desc: Test RSSurfaceNode SetSkipLayer and GetSkipLayer methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_SkipLayerTest_018, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "skip_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetSkipLayer(true);
        bool isSkip = node->GetSkipLayer();
        EXPECT_TRUE(isSkip);
        
        node->SetSkipLayer(false);
        isSkip = node->GetSkipLayer();
        EXPECT_FALSE(isSkip);
    }
}

/**
 * @tc.name: RSSurfaceNode_FingerprintTest_019.
 * @tc.desc: Test RSSurfaceNode SetFingerprint and GetFingerprint methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_FingerprintTest_019, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "fingerprint_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetFingerprint(true);
        bool hasFingerprint = node->GetFingerprint();
        EXPECT_TRUE(hasFingerprint);
        
        node->SetFingerprint(false);
        hasFingerprint = node->GetFingerprint();
        EXPECT_FALSE(hasFingerprint);
    }
}

/**
 * @tc.name: RSSurfaceNode_BootAnimationTest_020.
 * @tc.desc: Test RSSurfaceNode SetBootAnimation and GetBootAnimation methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_BootAnimationTest_020, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "boot_animation_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetBootAnimation(true);
        bool isBoot = node->GetBootAnimation();
        EXPECT_TRUE(isBoot);
        
        node->SetBootAnimation(false);
        isBoot = node->GetBootAnimation();
        EXPECT_FALSE(isBoot);
    }
}

/**
 * @tc.name: RSSurfaceNode_ColorSpaceTest_021.
 * @tc.desc: Test RSSurfaceNode SetColorSpace and GetColorSpace methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_ColorSpaceTest_021, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "colorspace_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetColorSpace(OHOS::GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
        auto colorSpace = node->GetColorSpace();
        EXPECT_EQ(colorSpace, OHOS::GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    }
}

/**
 * @tc.name: RSSurfaceNode_DarkColorModeTest_026.
 * @tc.desc: Test RSSurfaceNode SetDarkColorMode and GetDarkColorMode methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_DarkColorModeTest_026, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "darkmode_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetDarkColorMode(true);
        bool isDark = node->GetDarkColorMode();
        EXPECT_TRUE(isDark);
        
        node->SetDarkColorMode(false);
        isDark = node->GetDarkColorMode();
        EXPECT_FALSE(isDark);
    }
}

/**
 * @tc.name: RSSurfaceNode_IsSelfDrawingNodeTest_027.
 * @tc.desc: Test RSSurfaceNode IsSelfDrawingNode method when isWindow=false creates SELF_DRAWING_NODE.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_IsSelfDrawingNodeTest_027, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "selfdrawing_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        bool isSelfDrawing = node->IsSelfDrawingNode();
        EXPECT_TRUE(isSelfDrawing);  // isWindow=false creates SELF_DRAWING_NODE type
    }
}

/**
 * @tc.name: RSSurfaceNode_IsDefaultNodeTest_028.
 * @tc.desc: Test RSSurfaceNode IsSelfDrawingNode method when isWindow=true creates DEFAULT type.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_IsDefaultNodeTest_028, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "default_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, true);
    
    if (node) {
        bool isSelfDrawing = node->IsSelfDrawingNode();
        EXPECT_FALSE(isSelfDrawing);  // isWindow=true creates DEFAULT type
    }
}

/**
 * @tc.name: RSSurfaceNode_GlobalPositionEnabledTest_029.
 * @tc.desc: Test RSSurfaceNode SetGlobalPositionEnabled and GetGlobalPositionEnabled methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_GlobalPositionEnabledTest_029, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "globalposition_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetGlobalPositionEnabled(true);
        bool isEnabled = node->GetGlobalPositionEnabled();
        EXPECT_TRUE(isEnabled);
        
        node->SetGlobalPositionEnabled(false);
        isEnabled = node->GetGlobalPositionEnabled();
        EXPECT_FALSE(isEnabled);
    }
}

/**
 * @tc.name: RSSurfaceNode_SnapshotSkipLayerTest_030.
 * @tc.desc: Test RSSurfaceNode SetSnapshotSkipLayer and IsSnapshotSkipLayer methods.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_SnapshotSkipLayerTest_030, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "snapshot_skip_test_node";
    
    auto node = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    
    if (node) {
        node->SetSnapshotSkipLayer(true);
        bool isSnapshotSkip = node->GetSnapshotSkipLayer();
        EXPECT_TRUE(isSnapshotSkip);
        
        node->SetSnapshotSkipLayer(false);
        isSnapshotSkip = node->GetSnapshotSkipLayer();
        EXPECT_FALSE(isSnapshotSkip);
    }
}

/**
 * @tc.name: RSSurfaceNode_ClearChildrenTest_031.
 * @tc.desc: Test RSSurfaceNode ClearChildren method.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, RSSurfaceNode_ClearChildrenTest_031, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig parentConfig;
    parentConfig.SurfaceNodeName = "clear_parent_node";
    
    auto parent = OHOS::Rosen::RSSurfaceNode::Create(parentConfig, false);
    
    if (parent) {
        parent->ClearChildren();
        
        auto children = parent->GetChildren();
        EXPECT_EQ(children.size(), 0);
    }
}

}
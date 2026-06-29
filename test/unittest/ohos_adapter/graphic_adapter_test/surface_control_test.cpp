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

/**
 * @tc.name: SurfaceControl_CreateFromWindow_ValidNodeIdUiContextNull_032.
 * @tc.desc: Test CreateFromWindow when nodeId is valid but uiContext is null (line 179 branch).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_CreateFromWindow_ValidNodeIdUiContextNull_032, TestSize.Level1)
{
    SurfaceControl::connectToRender_ = nullptr;
    SurfaceControl::rsUIDirector_ = nullptr;

    if (mockWindow_ && mockWindow_->surface) {
        mockWindow_->surface->SetUserData("delegate_node_id", "123456");
        auto result = SurfaceControl::CreateFromWindow(mockWindow_, "test_window");
        EXPECT_EQ(result, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_Create_UiContextNull_033.
 * @tc.desc: Test Create when uiContext is null (line 209 branch).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_Create_UiContextNull_033, TestSize.Level1)
{
    SurfaceControl::connectToRender_ = nullptr;
    SurfaceControl::rsUIDirector_ = nullptr;

    auto result = SurfaceControl::Create("test_create");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: SurfaceControl_Constructor_SurfaceNodeNonNullParentNonNull_034.
 * @tc.desc: Test constructor with non-null surfaceNode and non-null parentNode.
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_Constructor_SurfaceNodeNonNullParentNonNull_034, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "constructor_test_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    OHOS::Rosen::RSSurfaceNodeConfig parentConfig;
    parentConfig.SurfaceNodeName = "constructor_parent_node";
    auto parentSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(parentConfig, false);

    if (surfaceNode && parentSurfaceNode) {
        std::shared_ptr<OHOS::Rosen::RSNode> parentNode = parentSurfaceNode;
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), std::move(parentNode), true);
        EXPECT_NE(sc, nullptr);
        EXPECT_NE(sc->surfaceNode_, nullptr);
        EXPECT_NE(sc->parentNode_, nullptr);
        EXPECT_FLOAT_EQ(sc->surfaceNode_->GetStagingProperties().GetPivot()[0], 0.0f);
        EXPECT_FLOAT_EQ(sc->surfaceNode_->GetStagingProperties().GetPivot()[1], 0.0f);
        EXPECT_TRUE(sc->IsRootSurface());
    }
}

/**
 * @tc.name: SurfaceControl_Constructor_SurfaceNodeNonNullParentNull_035.
 * @tc.desc: Test constructor with non-null surfaceNode and null parentNode (line 221 and 234 branches).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_Constructor_SurfaceNodeNonNullParentNull_035, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "constructor_nullparent_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);
        EXPECT_NE(sc, nullptr);
        EXPECT_NE(sc->surfaceNode_, nullptr);
        EXPECT_EQ(sc->parentNode_, nullptr);
        EXPECT_FLOAT_EQ(sc->surfaceNode_->GetStagingProperties().GetPivot()[0], 0.0f);
        EXPECT_FLOAT_EQ(sc->surfaceNode_->GetStagingProperties().GetPivot()[1], 0.0f);
        EXPECT_FALSE(sc->IsRootSurface());
    }
}

/**
 * @tc.name: SurfaceControl_Constructor_SurfaceNodeNull_036.
 * @tc.desc: Test constructor with null surfaceNode (line 221 else branch).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_Constructor_SurfaceNodeNull_036, TestSize.Level1)
{
    sptr<SurfaceControl> sc = new SurfaceControl(nullptr, nullptr, false);
    EXPECT_NE(sc, nullptr);
    EXPECT_EQ(sc->surfaceNode_, nullptr);
    EXPECT_EQ(sc->parentNode_, nullptr);
    EXPECT_FALSE(sc->IsRootSurface());
}

/**
 * @tc.name: SurfaceControl_Destructor_ParentNonNull_037.
 * @tc.desc: Test destructor when parentNode is non-null (line 247 branch: RemoveChild called).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_Destructor_ParentNonNull_037, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig childConfig;
    childConfig.SurfaceNodeName = "destructor_child_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(childConfig, false);
    auto surfaceNodeRef = surfaceNode;

    OHOS::Rosen::RSSurfaceNodeConfig parentConfig;
    parentConfig.SurfaceNodeName = "destructor_parent_node";
    auto parentSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(parentConfig, false);
    auto parentRef = parentSurfaceNode;

    if (surfaceNode && parentSurfaceNode) {
        parentSurfaceNode->AddChild(surfaceNode, -1);
        EXPECT_EQ(parentRef->GetChildren().size(), 1);

        std::shared_ptr<OHOS::Rosen::RSNode> parentNode = parentSurfaceNode;
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), std::move(parentNode), true);

        sc = nullptr;
        EXPECT_EQ(parentRef->GetChildren().size(), 0);
    }
}

/**
 * @tc.name: SurfaceControl_Destructor_ParentNull_038.
 * @tc.desc: Test destructor when parentNode is null (line 247 else: no RemoveChild).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_Destructor_ParentNull_038, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "destructor_nullparent_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);
        EXPECT_NE(sc, nullptr);
        sc = nullptr;
    }
}

/**
 * @tc.name: SurfaceControl_Destructor_SurfaceNodeNull_039.
 * @tc.desc: Test destructor when surfaceNode is null (UnRegisterNode null nodePtr branch).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_Destructor_SurfaceNodeNull_039, TestSize.Level1)
{
    sptr<SurfaceControl> sc = new SurfaceControl(nullptr, nullptr, false);
    EXPECT_NE(sc, nullptr);
    sc = nullptr;
}

/**
 * @tc.name: SurfaceControl_SetParent_OldParentNonNullNewParentValid_040.
 * @tc.desc: Test SetParent when old parentNode non-null and newParent valid (lines 254 and 257).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetParent_OldParentNonNullNewParentValid_040, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig childConfig;
    childConfig.SurfaceNodeName = "setparent_child_node";
    auto childSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(childConfig, false);
    auto childRef = childSurfaceNode;

    OHOS::Rosen::RSSurfaceNodeConfig oldParentConfig;
    oldParentConfig.SurfaceNodeName = "setparent_old_parent_node";
    auto oldParentSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(oldParentConfig, false);
    auto oldParentRef = oldParentSurfaceNode;

    OHOS::Rosen::RSSurfaceNodeConfig newParentConfig;
    newParentConfig.SurfaceNodeName = "setparent_new_parent_node";
    auto newParentSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(newParentConfig, false);

    if (childSurfaceNode && oldParentSurfaceNode && newParentSurfaceNode) {
        oldParentSurfaceNode->AddChild(childSurfaceNode, -1);
        EXPECT_EQ(oldParentRef->GetChildren().size(), 1);

        std::shared_ptr<OHOS::Rosen::RSNode> oldParentNode = oldParentSurfaceNode;
        sptr<SurfaceControl> childSc = new SurfaceControl(std::move(childSurfaceNode), std::move(oldParentNode), true);
        sptr<SurfaceControl> newParentSc = new SurfaceControl(std::move(newParentSurfaceNode), nullptr, false);

        childSc->SetParent(newParentSc.GetRefPtr());

        EXPECT_EQ(oldParentRef->GetChildren().size(), 0);
        EXPECT_EQ(newParentSc->surfaceNode_->GetChildren().size(), 1);
        EXPECT_NE(childSc->parentNode_, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_SetParent_OldParentNullNewParentValid_041.
 * @tc.desc: Test SetParent when old parentNode null and newParent valid (line 254 else, line 257).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetParent_OldParentNullNewParentValid_041, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig childConfig;
    childConfig.SurfaceNodeName = "setparent_nullold_child_node";
    auto childSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(childConfig, false);

    OHOS::Rosen::RSSurfaceNodeConfig newParentConfig;
    newParentConfig.SurfaceNodeName = "setparent_nullold_new_parent_node";
    auto newParentSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(newParentConfig, false);

    if (childSurfaceNode && newParentSurfaceNode) {
        sptr<SurfaceControl> childSc = new SurfaceControl(std::move(childSurfaceNode), nullptr, false);
        sptr<SurfaceControl> newParentSc = new SurfaceControl(std::move(newParentSurfaceNode), nullptr, false);

        childSc->SetParent(newParentSc.GetRefPtr());

        EXPECT_EQ(newParentSc->surfaceNode_->GetChildren().size(), 1);
        EXPECT_NE(childSc->parentNode_, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_SetParent_NewParentNull_042.
 * @tc.desc: Test SetParent when newParent is nullptr (line 261 else branch).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetParent_NewParentNull_042, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig childConfig;
    childConfig.SurfaceNodeName = "setparent_nullnew_child_node";
    auto childSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(childConfig, false);

    OHOS::Rosen::RSSurfaceNodeConfig parentConfig;
    parentConfig.SurfaceNodeName = "setparent_nullnew_parent_node";
    auto parentSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(parentConfig, false);
    auto parentRef = parentSurfaceNode;

    if (childSurfaceNode && parentSurfaceNode) {
        parentSurfaceNode->AddChild(childSurfaceNode, -1);
        EXPECT_EQ(parentRef->GetChildren().size(), 1);

        std::shared_ptr<OHOS::Rosen::RSNode> parentNode = parentSurfaceNode;
        sptr<SurfaceControl> childSc = new SurfaceControl(std::move(childSurfaceNode), std::move(parentNode), true);

        childSc->SetParent(nullptr);

        EXPECT_EQ(parentRef->GetChildren().size(), 0);
    }
}

/**
 * @tc.name: SurfaceControl_SetParent_NewParentNonNullSurfaceNodeNull_043.
 * @tc.desc: Test SetParent when newParent non-null but surfaceNode_ null (line 261 else branch).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetParent_NewParentNonNullSurfaceNodeNull_043, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig childConfig;
    childConfig.SurfaceNodeName = "setparent_nullsurfacenode_child";
    auto childSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(childConfig, false);

    if (childSurfaceNode) {
        sptr<SurfaceControl> childSc = new SurfaceControl(std::move(childSurfaceNode), nullptr, false);
        sptr<SurfaceControl> nullSurfaceParentSc = new SurfaceControl(nullptr, nullptr, false);

        childSc->SetParent(nullSurfaceParentSc.GetRefPtr());

        EXPECT_EQ(childSc->parentNode_, nullptr);
    }
}

/**
 * @tc.name: SurfaceControl_SetZOrder_SurfaceNodeNull_044.
 * @tc.desc: Test SetZOrder when surfaceNode is null (CHECK_NULL_POINTER early return).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetZOrder_SurfaceNodeNull_044, TestSize.Level1)
{
    sptr<SurfaceControl> sc = new SurfaceControl(nullptr, nullptr, false);
    sc->SetZOrder(5);
}

/**
 * @tc.name: SurfaceControl_SetZOrder_ParentExists_045.
 * @tc.desc: Test SetZOrder when surfaceNode has a parent (line 275: MoveChild called).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetZOrder_ParentExists_045, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig parentConfig;
    parentConfig.SurfaceNodeName = "setzorder_parent_node";
    auto parentSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(parentConfig, false);

    OHOS::Rosen::RSSurfaceNodeConfig childConfig;
    childConfig.SurfaceNodeName = "setzorder_child_node";
    auto childSurfaceNode = OHOS::Rosen::RSSurfaceNode::Create(childConfig, false);

    if (parentSurfaceNode && childSurfaceNode) {
        parentSurfaceNode->AddChild(childSurfaceNode, -1);

        std::shared_ptr<OHOS::Rosen::RSNode> parentNode = parentSurfaceNode;
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(childSurfaceNode), std::move(parentNode), true);

        EXPECT_NE(sc->surfaceNode_->GetParent(), nullptr);
        sc->SetZOrder(5);
    }
}

/**
 * @tc.name: SurfaceControl_SetZOrder_ParentNull_046.
 * @tc.desc: Test SetZOrder when surfaceNode has no parent (line 275 implicit else: no MoveChild).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetZOrder_ParentNull_046, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "setzorder_noparent_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);

        EXPECT_EQ(sc->surfaceNode_->GetParent(), nullptr);
        sc->SetZOrder(5);
    }
}

/**
 * @tc.name: SurfaceControl_SetBounds_SurfaceNodeNull_047.
 * @tc.desc: Test SetBounds when surfaceNode is null (CHECK_NULL_POINTER early return).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetBounds_SurfaceNodeNull_047, TestSize.Level1)
{
    sptr<SurfaceControl> sc = new SurfaceControl(nullptr, nullptr, false);
    sc->SetBounds(0.0f, 0.0f, 100.0f, 100.0f);
}

/**
 * @tc.name: SurfaceControl_SetBounds_NameNotDelegateContainer_048.
 * @tc.desc: Test SetBounds when name does NOT contain "delegate_container" (line 310 early return).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetBounds_NameNotDelegateContainer_048, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "regular_surface_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);

        sc->surfaceNode_->SetBounds(10.0f, 20.0f, 50.0f, 60.0f);
        auto boundsBefore = sc->surfaceNode_->GetStagingProperties().GetBounds();

        sc->SetBounds(0.0f, 0.0f, 100.0f, 100.0f);

        auto boundsAfter = sc->surfaceNode_->GetStagingProperties().GetBounds();
        EXPECT_FLOAT_EQ(boundsAfter[0], boundsBefore[0]);
        EXPECT_FLOAT_EQ(boundsAfter[1], boundsBefore[1]);
        EXPECT_FLOAT_EQ(boundsAfter[2], boundsBefore[2]);
        EXPECT_FLOAT_EQ(boundsAfter[3], boundsBefore[3]);
    }
}

/**
 * @tc.name: SurfaceControl_SetBounds_DelegateContainerInvalidBounds_049.
 * @tc.desc: Test SetBounds with "delegate_container" name but invalid w/h (line 316 early return).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetBounds_DelegateContainerInvalidBounds_049, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "delegate_container";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);

        sc->surfaceNode_->SetBounds(10.0f, 20.0f, 50.0f, 60.0f);
        auto boundsBefore = sc->surfaceNode_->GetStagingProperties().GetBounds();

        sc->SetBounds(0.0f, 0.0f, 0.0f, 100.0f);

        auto boundsAfter = sc->surfaceNode_->GetStagingProperties().GetBounds();
        EXPECT_FLOAT_EQ(boundsAfter[2], boundsBefore[2]);
        EXPECT_FLOAT_EQ(boundsAfter[3], boundsBefore[3]);
    }
}

/**
 * @tc.name: SurfaceControl_SetBounds_DelegateContainerValidBounds_050.
 * @tc.desc: Test SetBounds with "delegate_container" name and valid w/h (lines 289-290).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetBounds_DelegateContainerValidBounds_050, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "delegate_container";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);

        sc->SetBounds(0.0f, 0.0f, 100.0f, 200.0f);

        auto bounds = sc->surfaceNode_->GetStagingProperties().GetBounds();
        EXPECT_FLOAT_EQ(bounds[0], 0.0f);
        EXPECT_FLOAT_EQ(bounds[1], 0.0f);
        EXPECT_FLOAT_EQ(bounds[2], 100.0f);
        EXPECT_FLOAT_EQ(bounds[3], 200.0f);
    }
}

/**
 * @tc.name: SurfaceControl_SetBufferAlpha_SurfaceNodeNull_051.
 * @tc.desc: Test SetBufferAlpha when surfaceNode is null (CHECK_NULL_POINTER early return).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetBufferAlpha_SurfaceNodeNull_051, TestSize.Level1)
{
    sptr<SurfaceControl> sc = new SurfaceControl(nullptr, nullptr, false);
    sc->SetBufferAlpha(0.5f);
}

/**
 * @tc.name: SurfaceControl_SetBufferAlpha_NegativeAlpha_052.
 * @tc.desc: Test SetBufferAlpha with negative alpha (line 358: early return, no SetAlpha).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetBufferAlpha_NegativeAlpha_052, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "alpha_negative_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);

        sc->surfaceNode_->SetAlpha(1.0f);
        float alphaBefore = sc->surfaceNode_->GetStagingProperties().GetAlpha();

        sc->SetBufferAlpha(-1.0f);

        float alphaAfter = sc->surfaceNode_->GetStagingProperties().GetAlpha();
        EXPECT_FLOAT_EQ(alphaAfter, alphaBefore);
    }
}

/**
 * @tc.name: SurfaceControl_SetBufferAlpha_ValidAlpha_053.
 * @tc.desc: Test SetBufferAlpha with valid alpha (line 358 else: SetAlpha called).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_SetBufferAlpha_ValidAlpha_053, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "alpha_valid_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);

    if (surfaceNode) {
        sptr<SurfaceControl> sc = new SurfaceControl(std::move(surfaceNode), nullptr, false);

        sc->SetBufferAlpha(0.5f);

        float alpha = sc->surfaceNode_->GetStagingProperties().GetAlpha();
        EXPECT_FLOAT_EQ(alpha, 0.5f);
    }
}

/**
 * @tc.name: SurfaceControl_RegisterNode_AlreadyInMap_054.
 * @tc.desc: Test RegisterNode when node is already in map (line 449: second SurfaceControl with same surfaceNode).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_RegisterNode_AlreadyInMap_054, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "register_duplicate_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    auto surfaceNodeCopy = surfaceNode;

    if (surfaceNode) {
        sptr<SurfaceControl> sc1 = new SurfaceControl(std::move(surfaceNode), nullptr, false);
        EXPECT_NE(sc1, nullptr);

        sptr<SurfaceControl> sc2 = new SurfaceControl(std::move(surfaceNodeCopy), nullptr, false);
        EXPECT_NE(sc2, nullptr);

        EXPECT_EQ(sc1->surfaceNode_.get(), sc2->surfaceNode_.get());
    }
}

/**
 * @tc.name: SurfaceControl_UnRegisterNode_NullNodePtr_055.
 * @tc.desc: Test UnRegisterNode with null nodePtr (line 461: via destructor with null surfaceNode).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_UnRegisterNode_NullNodePtr_055, TestSize.Level1)
{
    sptr<SurfaceControl> sc = new SurfaceControl(nullptr, nullptr, false);
    EXPECT_NE(sc, nullptr);
    sc = nullptr;
}

/**
 * @tc.name: SurfaceControl_UnRegisterNode_NotInMap_056.
 * @tc.desc: Test UnRegisterNode when node not found in map (line 467 else: findit=false).
 * @tc.type: FUNC
 * @tc.require: issue#5183
 */
HWTEST_F(SurfaceControlTest, SurfaceControl_UnRegisterNode_NotInMap_056, TestSize.Level1)
{
    OHOS::Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "unregister_not_in_map_node";
    auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(config, false);
    auto surfaceNodeCopy = surfaceNode;

    if (surfaceNode) {
        sptr<SurfaceControl> sc1 = new SurfaceControl(std::move(surfaceNode), nullptr, false);
        sptr<SurfaceControl> sc2 = new SurfaceControl(std::move(surfaceNodeCopy), nullptr, false);

        sc1 = nullptr;
        sc2 = nullptr;
    }
}

}
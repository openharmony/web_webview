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

#include <gtest/gtest.h>
#define private public
#include "display_manager_adapter_impl.h"
#include "screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"
#undef private

#include "syspara/parameters.h"
#include "display_info.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::NWeb;

namespace OHOS {
namespace {
    DMError g_unregister = DMError::DM_ERROR_NULLPTR;
    constexpr uint64_t DISPLAY_ID_INVALID = -1ULL;
}

namespace Rosen {
DMError DisplayManager::UnregisterDisplayListener(sptr<IDisplayListener> listener)
{
    return g_unregister;
}
}

std::string g_deviceType = "";
namespace system {
std::string GetDeviceType()
{
    return g_deviceType;
}
}

namespace NWeb {
class DisplayListenerAdapterTest : public DisplayListenerAdapter {
public:
    DisplayListenerAdapterTest() = default;

    virtual ~DisplayListenerAdapterTest() = default;

    void OnCreate(DisplayId) override {}
    void OnDestroy(DisplayId) override {}
    void OnChange(DisplayId) override {}
};

class FoldStatusListenerAdapterTest : public FoldStatusListenerAdapter {
public:
    FoldStatusListenerAdapterTest() = default;

    virtual ~FoldStatusListenerAdapterTest() = default;

    void OnFoldStatusChanged(FoldStatus foldstatus) override {}
};

class DisplayManagerAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DisplayManagerAdapterImplTest::SetUpTestCase(void)
{}

void DisplayManagerAdapterImplTest::TearDownTestCase(void)
{}

void DisplayManagerAdapterImplTest::SetUp(void)
{}

void DisplayManagerAdapterImplTest::TearDown(void)
{}

/**
 * @tc.name: DisplayManagerAdapterImplTest_001.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_001, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);
    EXPECT_EQ(displayAdapterImpl->GetId(), static_cast<DisplayId>(-1));
    EXPECT_EQ(displayAdapterImpl->GetWidth(), -1);
    EXPECT_EQ(displayAdapterImpl->GetHeight(), -1);
    EXPECT_EQ(displayAdapterImpl->GetVirtualPixelRatio(), -1);
    EXPECT_EQ(displayAdapterImpl->GetRotation(), RotationType::ROTATION_BUTT);
    EXPECT_EQ(displayAdapterImpl->GetOrientation(), OrientationType::BUTT);
    EXPECT_EQ(displayAdapterImpl->GetDisplayOrientation(), DisplayOrientation::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
    EXPECT_EQ(displayAdapterImpl->GetFoldStatus(), FoldStatus::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->IsFoldable(), false);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_002, TestSize.Level1)
{
    std::shared_ptr<DisplayListenerAdapter> listener = std::make_shared<DisplayListenerAdapterTest>();
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(listener);
    displayListenerAdapterImpl->OnCreate(static_cast<DisplayId>(1));
    displayListenerAdapterImpl->OnDestroy(static_cast<DisplayId>(1));
    displayListenerAdapterImpl->OnChange(static_cast<DisplayId>(1));
    EXPECT_NE(displayListenerAdapterImpl, nullptr);
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl1 =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    displayListenerAdapterImpl1->OnCreate(static_cast<DisplayId>(1));
    displayListenerAdapterImpl1->OnDestroy(static_cast<DisplayId>(1));
    displayListenerAdapterImpl1->OnChange(static_cast<DisplayId>(1));
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_003.
 * @tc.desc: test ConvertRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_003, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::Rotation type = OHOS::Rosen::Rotation::ROTATION_0;
    OHOS::NWeb::RotationType retType;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_0);

    type = OHOS::Rosen::Rotation::ROTATION_90;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_90);

    type = OHOS::Rosen::Rotation::ROTATION_180;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_180);

    type = OHOS::Rosen::Rotation::ROTATION_270;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_270);

    const uint32_t ROTATION_butt = 4;
    type = static_cast<OHOS::Rosen::Rotation>(ROTATION_butt);
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_BUTT);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_004.
 * @tc.desc: test ConvertRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_004, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::Orientation type = OHOS::Rosen::Orientation::UNSPECIFIED;
    OHOS::NWeb::OrientationType retType;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::UNSPECIFIED);

    type = OHOS::Rosen::Orientation::VERTICAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::VERTICAL);

    type = OHOS::Rosen::Orientation::HORIZONTAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::HORIZONTAL);

    type = OHOS::Rosen::Orientation::REVERSE_VERTICAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::REVERSE_VERTICAL);

    type = OHOS::Rosen::Orientation::REVERSE_HORIZONTAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::REVERSE_HORIZONTAL);

    type = OHOS::Rosen::Orientation::SENSOR;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::SENSOR);

    type = OHOS::Rosen::Orientation::SENSOR_VERTICAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::SENSOR_VERTICAL);

    type = OHOS::Rosen::Orientation::SENSOR_HORIZONTAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::SENSOR_HORIZONTAL);

    type = OHOS::Rosen::Orientation::AUTO_ROTATION_RESTRICTED;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::BUTT);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_005.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_005, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    std::shared_ptr<DisplayListenerAdapter> listener = std::make_shared<DisplayListenerAdapterTest>();
    EXPECT_NE(displayManagerAdapterImpl, nullptr);
    EXPECT_NE(displayManagerAdapterImpl->GetDefaultDisplayId(), DISPLAY_ID_INVALID);
    EXPECT_NE(displayManagerAdapterImpl->GetDefaultDisplay(), nullptr);
    EXPECT_EQ(displayManagerAdapterImpl->RegisterDisplayListener(listener), 1);
    EXPECT_FALSE(displayManagerAdapterImpl->UnregisterDisplayListener(1));
    EXPECT_FALSE(displayManagerAdapterImpl->UnregisterDisplayListener(2));

    std::shared_ptr<FoldStatusListenerAdapter> foldListener = std::make_shared<FoldStatusListenerAdapterTest>();
    EXPECT_EQ(displayManagerAdapterImpl->RegisterFoldStatusListener(foldListener), 0);
    EXPECT_EQ(displayManagerAdapterImpl->UnregisterFoldStatusListener(1), false);
    EXPECT_EQ(displayManagerAdapterImpl->UnregisterFoldStatusListener(2), false);
    g_unregister = DMError::DM_OK;
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_006.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_006, TestSize.Level1)
{
    sptr<Display> display = DisplayManager::GetInstance().GetDefaultDisplay();
    ScreenId defaultScreenId = display->GetScreenId();
    auto defaultScreen = ScreenManager::GetInstance().GetScreenById(defaultScreenId);
    defaultScreen->SetOrientation(Orientation::REVERSE_HORIZONTAL);
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(display);
    EXPECT_NE(displayAdapterImpl, nullptr);
    EXPECT_NE(displayAdapterImpl->GetId(), static_cast<DisplayId>(-1));
    EXPECT_NE(displayAdapterImpl->GetWidth(), -1);
    EXPECT_NE(displayAdapterImpl->GetHeight(), -1);
    EXPECT_NE(displayAdapterImpl->GetVirtualPixelRatio(), -1);
    EXPECT_NE(displayAdapterImpl->GetRotation(), RotationType::ROTATION_BUTT);
    EXPECT_NE(displayAdapterImpl->GetOrientation(), OrientationType::BUTT);
    EXPECT_NE(displayAdapterImpl->GetDisplayOrientation(), DisplayOrientation::UNKNOWN);
    EXPECT_NE(displayAdapterImpl->GetDpi(), -1);
    EXPECT_EQ(displayAdapterImpl->GetFoldStatus(), FoldStatus::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->IsFoldable(), false);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_007.
 * @tc.desc: test IsDefaultPortrait.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_007, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    EXPECT_NE(displayManagerAdapterImpl, nullptr);
    bool result = displayManagerAdapterImpl->IsDefaultPortrait();
    std::string deviceType = OHOS::system::GetDeviceType();
    if (deviceType == "phone" || deviceType == "default") {
        EXPECT_TRUE(result);
    } else {
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_008.
 * @tc.desc: test ConvertDisplayRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_008, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::DisplayOrientation type = OHOS::Rosen::DisplayOrientation::UNKNOWN;
    OHOS::NWeb::DisplayOrientation retType;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::UNKNOWN);

    type = OHOS::Rosen::DisplayOrientation::PORTRAIT;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::PORTRAIT);

    type = OHOS::Rosen::DisplayOrientation::LANDSCAPE;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::LANDSCAPE);

    type = OHOS::Rosen::DisplayOrientation::PORTRAIT_INVERTED;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED);

    type = OHOS::Rosen::DisplayOrientation::LANDSCAPE_INVERTED;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::LANDSCAPE_INVERTED);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_009.
 * @tc.desc: test fold change.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_009, TestSize.Level1)
{
    std::shared_ptr<FoldStatusListenerAdapter> listener = std::make_shared<FoldStatusListenerAdapterTest>();
    std::unique_ptr<FoldStatusListenerAdapterImpl> foldStatusListenerAdapterImpl =
        std::make_unique<FoldStatusListenerAdapterImpl>(listener);
    foldStatusListenerAdapterImpl->OnFoldStatusChanged(
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL);
    EXPECT_NE(foldStatusListenerAdapterImpl, nullptr);
    std::unique_ptr<FoldStatusListenerAdapterImpl> foldStatusListenerAdapterImpl2 =
        std::make_unique<FoldStatusListenerAdapterImpl>(nullptr);
    foldStatusListenerAdapterImpl->OnFoldStatusChanged(
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_010.
 * @tc.desc: test ConvertRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_010, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    NativeDisplayManager_FoldDisplayMode type =
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL;
    OHOS::NWeb::FoldStatus retType;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::FULL);
    
    type = NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::MAIN);
    
    type = NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::SUB);
    
    type = NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::COORDINATION);
}

/**
 * @tc.name: GetDpi_001.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_001, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_001.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_001, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_011.
 * @tc.desc: test newer get method.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_011, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    ASSERT_NE(displayAdapterImpl, nullptr);
    EXPECT_EQ(displayAdapterImpl->GetName(), "");
    EXPECT_EQ(displayAdapterImpl->GetAvailableWidth(), 0);
    EXPECT_EQ(displayAdapterImpl->GetAvailableHeight(), 0);
    EXPECT_EQ(displayAdapterImpl->GetAliveStatus(), true);
    EXPECT_EQ(displayAdapterImpl->GetDisplayState(), DisplayState::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->GetDensityDpi(), 0);
    EXPECT_EQ(displayAdapterImpl->GetX(), 0);
    EXPECT_EQ(displayAdapterImpl->GetY(), 0);
    EXPECT_EQ(displayAdapterImpl->GetDisplaySourceMode(), DisplaySourceMode::NONE);
    EXPECT_EQ(displayAdapterImpl->GetPhysicalWidth(), 0);
    EXPECT_EQ(displayAdapterImpl->GetPhysicalHeight(), 0);
    EXPECT_FLOAT_EQ(displayAdapterImpl->GetDefaultVirtualPixelRatio(), 0);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_012.
 * @tc.desc: test newer get method.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_012, TestSize.Level1)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(display, nullptr);
    auto displayInfo = display->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(display);

    EXPECT_NE(displayAdapterImpl->GetName(), "");
    EXPECT_GE(displayAdapterImpl->GetAvailableWidth(), 0);
    EXPECT_GE(displayAdapterImpl->GetAvailableHeight(), 0);
    EXPECT_EQ(displayAdapterImpl->GetAliveStatus(), true);
    EXPECT_NE(displayAdapterImpl->GetDisplayState(), DisplayState::UNKNOWN);
    EXPECT_GT(displayAdapterImpl->GetDensityDpi(), 0);
    EXPECT_GE(displayAdapterImpl->GetX(), 0);
    EXPECT_GE(displayAdapterImpl->GetY(), 0);
    EXPECT_NE(displayAdapterImpl->GetDisplaySourceMode(), DisplaySourceMode::NONE);
    EXPECT_GE(displayAdapterImpl->GetPhysicalWidth(), 0);
    EXPECT_GE(displayAdapterImpl->GetPhysicalHeight(), 0);
    EXPECT_GT(displayAdapterImpl->GetDefaultVirtualPixelRatio(), 0);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_013.
 * @tc.desc: test ConvertDisplayState.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_013, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    ASSERT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::DisplayState state = OHOS::Rosen::DisplayState::UNKNOWN;
    OHOS::NWeb::DisplayState retType;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::UNKNOWN);

    state = OHOS::Rosen::DisplayState::OFF;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::OFF);

    state = OHOS::Rosen::DisplayState::ON;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::ON);

    state = OHOS::Rosen::DisplayState::DOZE;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::DOZE);

    state = OHOS::Rosen::DisplayState::DOZE_SUSPEND;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::DOZE_SUSPEND);

    state = OHOS::Rosen::DisplayState::VR;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::VR);

    state = OHOS::Rosen::DisplayState::ON_SUSPEND;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::ON_SUSPEND);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_014.
 * @tc.desc: test ConvertDisplaySourceMode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_014, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    ASSERT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::DisplaySourceMode mode = OHOS::Rosen::DisplaySourceMode::NONE;
    OHOS::NWeb::DisplaySourceMode retType;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::NONE);

    mode = OHOS::Rosen::DisplaySourceMode::MAIN;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::MAIN);

    mode = OHOS::Rosen::DisplaySourceMode::MIRROR;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::MIRROR);

    mode = OHOS::Rosen::DisplaySourceMode::EXTEND;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::EXTEND);

    mode = OHOS::Rosen::DisplaySourceMode::ALONE;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::ALONE);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_015.
 * @tc.desc: test GetPrimaryDisplay.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_015, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    auto primaryDisplayBase = DisplayManager::GetInstance().GetPrimaryDisplaySync();
    ASSERT_NE(primaryDisplayBase, nullptr);
    auto primaryDisplay = displayManagerAdapterImpl->GetPrimaryDisplay();
    EXPECT_NE(primaryDisplay, nullptr);
    EXPECT_EQ(primaryDisplay->GetId(), primaryDisplayBase->GetId());
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_016.
 * @tc.desc: test GetAllDisplays.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_016, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    auto allDisplaysBase = DisplayManager::GetInstance().GetAllDisplays();
    EXPECT_FALSE(allDisplaysBase.empty());
    auto allDisplays = displayManagerAdapterImpl->GetAllDisplays();
    EXPECT_EQ(allDisplays.size(), allDisplaysBase.size());
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_017.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_017, TestSize.Level1)
{
    std::unique_ptr<FoldStatusListenerAdapterImpl> displayAdapterImpl = 
            std::make_unique<FoldStatusListenerAdapterImpl>(nullptr);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL), 
                OHOS::NWeb::FoldStatus::FULL);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN), 
                OHOS::NWeb::FoldStatus::MAIN);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB), 
                OHOS::NWeb::FoldStatus::SUB);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(
        DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION), OHOS::NWeb::FoldStatus::COORDINATION);
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_018.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_018, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = 
                std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL), 
                OHOS::NWeb::FoldStatus::FULL);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN), 
                OHOS::NWeb::FoldStatus::MAIN);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB), 
                OHOS::NWeb::FoldStatus::SUB);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION), 
                OHOS::NWeb::FoldStatus::COORDINATION);
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_019.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_019, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_FALSE(displayAdapterImpl->IsFoldable());
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_020.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_020, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl->GetFoldStatus(), OHOS::NWeb::FoldStatus::FULL);
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_021.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_021, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
    std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    int32_t id = 0;
    EXPECT_FALSE(displayManagerAdapterImpl->UnregisterFoldStatusListener(id));
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_022.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_022, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
    std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    EXPECT_GE(displayManagerAdapterImpl->RegisterFoldStatusListener(nullptr), 0);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_001.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_023, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);
    EXPECT_EQ(displayAdapterImpl->GetId(), static_cast<DisplayId>(-1));
    EXPECT_EQ(displayAdapterImpl->GetWidth(), -1);
    EXPECT_EQ(displayAdapterImpl->GetHeight(), -1);
    EXPECT_EQ(displayAdapterImpl->GetVirtualPixelRatio(), -1);
    EXPECT_EQ(displayAdapterImpl->GetRotation(), RotationType::ROTATION_BUTT);
    EXPECT_EQ(displayAdapterImpl->GetOrientation(), OrientationType::BUTT);
    EXPECT_EQ(displayAdapterImpl->GetDisplayOrientation(), DisplayOrientation::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
    EXPECT_EQ(displayAdapterImpl->GetFoldStatus(), FoldStatus::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->IsFoldable(), false);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_024, TestSize.Level1)
{
    std::shared_ptr<DisplayListenerAdapter> listener = std::make_shared<DisplayListenerAdapterTest>();
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(listener);
    displayListenerAdapterImpl->OnCreate(static_cast<DisplayId>(1));
    displayListenerAdapterImpl->OnDestroy(static_cast<DisplayId>(1));
    displayListenerAdapterImpl->OnChange(static_cast<DisplayId>(1));
    EXPECT_NE(displayListenerAdapterImpl, nullptr);
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl1 =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    displayListenerAdapterImpl1->OnCreate(static_cast<DisplayId>(1));
    displayListenerAdapterImpl1->OnDestroy(static_cast<DisplayId>(1));
    displayListenerAdapterImpl1->OnChange(static_cast<DisplayId>(1));
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_003.
 * @tc.desc: test ConvertRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_025, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::Rotation type = OHOS::Rosen::Rotation::ROTATION_0;
    OHOS::NWeb::RotationType retType;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_0);

    type = OHOS::Rosen::Rotation::ROTATION_90;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_90);

    type = OHOS::Rosen::Rotation::ROTATION_180;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_180);

    type = OHOS::Rosen::Rotation::ROTATION_270;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_270);

    const uint32_t ROTATION_butt = 4;
    type = static_cast<OHOS::Rosen::Rotation>(ROTATION_butt);
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_BUTT);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_004.
 * @tc.desc: test ConvertRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_026, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::Orientation type = OHOS::Rosen::Orientation::UNSPECIFIED;
    OHOS::NWeb::OrientationType retType;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::UNSPECIFIED);

    type = OHOS::Rosen::Orientation::VERTICAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::VERTICAL);

    type = OHOS::Rosen::Orientation::HORIZONTAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::HORIZONTAL);

    type = OHOS::Rosen::Orientation::REVERSE_VERTICAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::REVERSE_VERTICAL);

    type = OHOS::Rosen::Orientation::REVERSE_HORIZONTAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::REVERSE_HORIZONTAL);

    type = OHOS::Rosen::Orientation::SENSOR;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::SENSOR);

    type = OHOS::Rosen::Orientation::SENSOR_VERTICAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::SENSOR_VERTICAL);

    type = OHOS::Rosen::Orientation::SENSOR_HORIZONTAL;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::SENSOR_HORIZONTAL);

    type = OHOS::Rosen::Orientation::AUTO_ROTATION_RESTRICTED;
    retType = displayAdapterImpl->ConvertOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::OrientationType::BUTT);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_005.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_027, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    std::shared_ptr<DisplayListenerAdapter> listener = std::make_shared<DisplayListenerAdapterTest>();
    EXPECT_NE(displayManagerAdapterImpl, nullptr);
    EXPECT_NE(displayManagerAdapterImpl->GetDefaultDisplayId(), DISPLAY_ID_INVALID);
    EXPECT_NE(displayManagerAdapterImpl->GetDefaultDisplay(), nullptr);
    EXPECT_EQ(displayManagerAdapterImpl->RegisterDisplayListener(listener), 2);
    EXPECT_FALSE(displayManagerAdapterImpl->UnregisterDisplayListener(1));
    EXPECT_TRUE(displayManagerAdapterImpl->UnregisterDisplayListener(2));

    std::shared_ptr<FoldStatusListenerAdapter> foldListener = std::make_shared<FoldStatusListenerAdapterTest>();
    EXPECT_EQ(displayManagerAdapterImpl->RegisterFoldStatusListener(foldListener), 0);
    EXPECT_EQ(displayManagerAdapterImpl->UnregisterFoldStatusListener(1), false);
    EXPECT_EQ(displayManagerAdapterImpl->UnregisterFoldStatusListener(2), false);
    g_unregister = DMError::DM_OK;
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_028.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_028, TestSize.Level1)
{
    sptr<Display> display = DisplayManager::GetInstance().GetDefaultDisplay();
    ScreenId defaultScreenId = display->GetScreenId();
    auto defaultScreen = ScreenManager::GetInstance().GetScreenById(defaultScreenId);
    defaultScreen->SetOrientation(Orientation::REVERSE_HORIZONTAL);
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(display);
    EXPECT_NE(displayAdapterImpl, nullptr);
    EXPECT_NE(displayAdapterImpl->GetId(), static_cast<DisplayId>(-1));
    EXPECT_NE(displayAdapterImpl->GetWidth(), -1);
    EXPECT_NE(displayAdapterImpl->GetHeight(), -1);
    EXPECT_NE(displayAdapterImpl->GetVirtualPixelRatio(), -1);
    EXPECT_NE(displayAdapterImpl->GetRotation(), RotationType::ROTATION_BUTT);
    EXPECT_NE(displayAdapterImpl->GetOrientation(), OrientationType::BUTT);
    EXPECT_NE(displayAdapterImpl->GetDisplayOrientation(), DisplayOrientation::UNKNOWN);
    EXPECT_NE(displayAdapterImpl->GetDpi(), -1);
    EXPECT_EQ(displayAdapterImpl->GetFoldStatus(), FoldStatus::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->IsFoldable(), false);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_029.
 * @tc.desc: test IsDefaultPortrait.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_029, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    EXPECT_NE(displayManagerAdapterImpl, nullptr);
    bool result = displayManagerAdapterImpl->IsDefaultPortrait();
    std::string deviceType = OHOS::system::GetDeviceType();
    if (deviceType == "phone" || deviceType == "default") {
        EXPECT_TRUE(result);
    } else {
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_030.
 * @tc.desc: test ConvertDisplayRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_030, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::DisplayOrientation type = OHOS::Rosen::DisplayOrientation::UNKNOWN;
    OHOS::NWeb::DisplayOrientation retType;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::UNKNOWN);

    type = OHOS::Rosen::DisplayOrientation::PORTRAIT;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::PORTRAIT);

    type = OHOS::Rosen::DisplayOrientation::LANDSCAPE;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::LANDSCAPE);

    type = OHOS::Rosen::DisplayOrientation::PORTRAIT_INVERTED;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED);

    type = OHOS::Rosen::DisplayOrientation::LANDSCAPE_INVERTED;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::LANDSCAPE_INVERTED);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_031.
 * @tc.desc: test fold change.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_031, TestSize.Level1)
{
    std::shared_ptr<FoldStatusListenerAdapter> listener = std::make_shared<FoldStatusListenerAdapterTest>();
    std::unique_ptr<FoldStatusListenerAdapterImpl> foldStatusListenerAdapterImpl =
        std::make_unique<FoldStatusListenerAdapterImpl>(listener);
    foldStatusListenerAdapterImpl->OnFoldStatusChanged(
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL);
    EXPECT_NE(foldStatusListenerAdapterImpl, nullptr);
    std::unique_ptr<FoldStatusListenerAdapterImpl> foldStatusListenerAdapterImpl2 =
        std::make_unique<FoldStatusListenerAdapterImpl>(nullptr);
    foldStatusListenerAdapterImpl->OnFoldStatusChanged(
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_032.
 * @tc.desc: test ConvertRotationType.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_032, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    NativeDisplayManager_FoldDisplayMode type =
        NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL;
    OHOS::NWeb::FoldStatus retType;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::FULL);
    
    type = NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::MAIN);
    
    type = NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::SUB);
    
    type = NativeDisplayManager_FoldDisplayMode::DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION;
    retType = displayAdapterImpl->ConvertFoldStatus(type);
    EXPECT_EQ(retType, OHOS::NWeb::FoldStatus::COORDINATION);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_033.
 * @tc.desc: test newer get method.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_033, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    ASSERT_NE(displayAdapterImpl, nullptr);
    EXPECT_EQ(displayAdapterImpl->GetName(), "");
    EXPECT_EQ(displayAdapterImpl->GetAvailableWidth(), 0);
    EXPECT_EQ(displayAdapterImpl->GetAvailableHeight(), 0);
    EXPECT_EQ(displayAdapterImpl->GetAliveStatus(), true);
    EXPECT_EQ(displayAdapterImpl->GetDisplayState(), DisplayState::UNKNOWN);
    EXPECT_EQ(displayAdapterImpl->GetDensityDpi(), 0);
    EXPECT_EQ(displayAdapterImpl->GetX(), 0);
    EXPECT_EQ(displayAdapterImpl->GetY(), 0);
    EXPECT_EQ(displayAdapterImpl->GetDisplaySourceMode(), DisplaySourceMode::NONE);
    EXPECT_EQ(displayAdapterImpl->GetPhysicalWidth(), 0);
    EXPECT_EQ(displayAdapterImpl->GetPhysicalHeight(), 0);
    EXPECT_FLOAT_EQ(displayAdapterImpl->GetDefaultVirtualPixelRatio(), 0);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_034.
 * @tc.desc: test newer get method.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_034, TestSize.Level1)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(display, nullptr);
    auto displayInfo = display->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(display);

    EXPECT_NE(displayAdapterImpl->GetName(), "");
    EXPECT_GE(displayAdapterImpl->GetAvailableWidth(), 0);
    EXPECT_GE(displayAdapterImpl->GetAvailableHeight(), 0);
    EXPECT_EQ(displayAdapterImpl->GetAliveStatus(), true);
    EXPECT_NE(displayAdapterImpl->GetDisplayState(), DisplayState::UNKNOWN);
    EXPECT_GT(displayAdapterImpl->GetDensityDpi(), 0);
    EXPECT_GE(displayAdapterImpl->GetX(), 0);
    EXPECT_GE(displayAdapterImpl->GetY(), 0);
    EXPECT_NE(displayAdapterImpl->GetDisplaySourceMode(), DisplaySourceMode::NONE);
    EXPECT_GE(displayAdapterImpl->GetPhysicalWidth(), 0);
    EXPECT_GE(displayAdapterImpl->GetPhysicalHeight(), 0);
    EXPECT_GT(displayAdapterImpl->GetDefaultVirtualPixelRatio(), 0);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_035.
 * @tc.desc: test ConvertDisplayState.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_035, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    ASSERT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::DisplayState state = OHOS::Rosen::DisplayState::UNKNOWN;
    OHOS::NWeb::DisplayState retType;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::UNKNOWN);

    state = OHOS::Rosen::DisplayState::OFF;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::OFF);

    state = OHOS::Rosen::DisplayState::ON;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::ON);

    state = OHOS::Rosen::DisplayState::DOZE;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::DOZE);

    state = OHOS::Rosen::DisplayState::DOZE_SUSPEND;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::DOZE_SUSPEND);

    state = OHOS::Rosen::DisplayState::VR;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::VR);

    state = OHOS::Rosen::DisplayState::ON_SUSPEND;
    retType = displayAdapterImpl->ConvertDisplayState(state);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayState::ON_SUSPEND);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_036.
 * @tc.desc: test ConvertDisplaySourceMode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_036, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    ASSERT_NE(displayAdapterImpl, nullptr);

    OHOS::Rosen::DisplaySourceMode mode = OHOS::Rosen::DisplaySourceMode::NONE;
    OHOS::NWeb::DisplaySourceMode retType;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::NONE);

    mode = OHOS::Rosen::DisplaySourceMode::MAIN;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::MAIN);

    mode = OHOS::Rosen::DisplaySourceMode::MIRROR;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::MIRROR);

    mode = OHOS::Rosen::DisplaySourceMode::EXTEND;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::EXTEND);

    mode = OHOS::Rosen::DisplaySourceMode::ALONE;
    retType = displayAdapterImpl->ConvertDisplaySourceMode(mode);
    EXPECT_EQ(retType, OHOS::NWeb::DisplaySourceMode::ALONE);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_037.
 * @tc.desc: test GetPrimaryDisplay.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_037, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    auto primaryDisplayBase = DisplayManager::GetInstance().GetPrimaryDisplaySync();
    ASSERT_NE(primaryDisplayBase, nullptr);
    auto primaryDisplay = displayManagerAdapterImpl->GetPrimaryDisplay();
    EXPECT_NE(primaryDisplay, nullptr);
    EXPECT_EQ(primaryDisplay->GetId(), primaryDisplayBase->GetId());
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_038.
 * @tc.desc: test GetAllDisplays.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_038, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    auto allDisplaysBase = DisplayManager::GetInstance().GetAllDisplays();
    EXPECT_FALSE(allDisplaysBase.empty());
    auto allDisplays = displayManagerAdapterImpl->GetAllDisplays();
    EXPECT_EQ(allDisplays.size(), allDisplaysBase.size());
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_039.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_039, TestSize.Level1)
{
    std::unique_ptr<FoldStatusListenerAdapterImpl> displayAdapterImpl = 
            std::make_unique<FoldStatusListenerAdapterImpl>(nullptr);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL), 
                OHOS::NWeb::FoldStatus::FULL);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN), 
                OHOS::NWeb::FoldStatus::MAIN);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB), 
                OHOS::NWeb::FoldStatus::SUB);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(
        DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION), OHOS::NWeb::FoldStatus::COORDINATION);
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_040.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_040, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = 
                std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_FULL), 
                OHOS::NWeb::FoldStatus::FULL);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_MAIN), 
                OHOS::NWeb::FoldStatus::MAIN);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_SUB), 
                OHOS::NWeb::FoldStatus::SUB);
    EXPECT_EQ(displayAdapterImpl->ConvertFoldStatus(DISPLAY_MANAGER_FOLD_DISPLAY_MODE_COORDINATION), 
                OHOS::NWeb::FoldStatus::COORDINATION);
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_041.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_041, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_FALSE(displayAdapterImpl->IsFoldable());
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_042.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_042, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl->GetFoldStatus(), OHOS::NWeb::FoldStatus::FULL);
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_043.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_043, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
    std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    int32_t id = 0;
    EXPECT_FALSE(displayManagerAdapterImpl->UnregisterFoldStatusListener(id));
}
 
/**
 * @tc.name: DisplayManagerAdapterImplTest_044.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_044, TestSize.Level1)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
    std::make_shared<DisplayManagerAdapterImpl>();
    ASSERT_NE(displayManagerAdapterImpl, nullptr);
    EXPECT_GE(displayManagerAdapterImpl->RegisterFoldStatusListener(nullptr), 0);
}

/**
 * @tc.name: GetDpi_002.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_002, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_002.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_002, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_003.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_003, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_003.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_003, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_004.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_004, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_005.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_005, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_006.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_006, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_007.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_007, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_007.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_007, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_008.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_008, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_008.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_008, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_009.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_009, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_009.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_009, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_010.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_010, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_010.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_010, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_011.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_011, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_011.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_011, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_012.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_012, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_012.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_012, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_013.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_013, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_013.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_013, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_014.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_014, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_014.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_014, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: OnChange_015.
 * @tc.desc: test OnChange.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, OnChange_015, TestSize.Level1)
{
    std::unique_ptr<DisplayListenerAdapterImpl> displayListenerAdapterImpl =
        std::make_unique<DisplayListenerAdapterImpl>(nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(static_cast<DisplayId>(1)));
    auto displayPtr = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(displayPtr, nullptr);
    auto displayInfo = displayPtr->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    EXPECT_FALSE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
    auto nwebDisplayInfo =  displayListenerAdapterImpl->ConvertDisplayInfo(*displayInfo);
    displayListenerAdapterImpl->cachedDisplayedInfo_ = nwebDisplayInfo;
    displayListenerAdapterImpl->cachedDisplayedInfo_.refreshRate_ = 100;
    EXPECT_TRUE(displayListenerAdapterImpl->CheckOnlyRefreshRateDecreased(
        DisplayManager::GetInstance().GetDefaultDisplayId()));
}

/**
 * @tc.name: GetDpi_015.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_015, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_016.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_016, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_017.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_017, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_018.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_018, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_019.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_019, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_020.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_020, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_021.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_021, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_022.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_022, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_023.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_023, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_024.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_024, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_025.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_025, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_026.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_026, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_027.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_027, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_028.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_028, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_029.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_029, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_030.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_030, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_031.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_031, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_032.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_032, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_033.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_033, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_034.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_034, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_035.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_035, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_036.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_036, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_037.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_037, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_038.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_038, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_039.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_039, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_040.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_040, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_041.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_041, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_042.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_042, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_043.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_043, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_044.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_044, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_045.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_045, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_046.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_046, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_047.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_047, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_048.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_048, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_049.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_049, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_050.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_050, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}

/**
 * @tc.name: GetDpi_051.
 * @tc.desc: test GetDpi.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DisplayManagerAdapterImplTest, GetDpi_051, TestSize.Level1)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>(nullptr);
    EXPECT_NE(displayAdapterImpl, nullptr);

    displayAdapterImpl->display_ = nullptr;
    EXPECT_EQ(displayAdapterImpl->GetDpi(), -1);
}
}
}

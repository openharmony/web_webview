/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#define private public
#include "display_manager_adapter_impl.h"
#include <window_manager/oh_display_info.h>
#include <window_manager/oh_display_manager.h>
#undef private

using namespace testing;
using namespace OHOS;
using namespace OHOS::NWeb;

namespace OHOS {

namespace {
    constexpr uint64_t DISPLAY_ID_INVALID = -1ULL;
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
 */
TEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_001)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>();
    EXPECT_NE(displayAdapterImpl, nullptr);
    EXPECT_NE(displayAdapterImpl->GetId(), static_cast<DisplayId>(-1));
    EXPECT_NE(displayAdapterImpl->GetWidth(), -1);
    EXPECT_NE(displayAdapterImpl->GetHeight(), -1);
    EXPECT_NE(displayAdapterImpl->GetVirtualPixelRatio(), -1);
    EXPECT_NE(displayAdapterImpl->GetRotation(), RotationType::ROTATION_BUTT);
    EXPECT_EQ(displayAdapterImpl->GetOrientation(), OrientationType::BUTT);
    EXPECT_NE(displayAdapterImpl->GetDpi(), -1);
    EXPECT_NE(displayAdapterImpl->GetDisplayOrientation(), DisplayOrientation::UNKNOWN);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_002.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 */
TEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_002)
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
 */
TEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_003)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>();
    EXPECT_NE(displayAdapterImpl, nullptr);

    NativeDisplayManager_Rotation type = NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_0;
    OHOS::NWeb::RotationType retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_0);

    type = NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_90;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_90);

    type = NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_180;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_180);

    type = NativeDisplayManager_Rotation::DISPLAY_MANAGER_ROTATION_270;
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_270);

    const uint32_t ROTATION_butt = 4;
    type = static_cast<NativeDisplayManager_Rotation>(ROTATION_butt);
    retType = displayAdapterImpl->ConvertRotationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::RotationType::ROTATION_BUTT);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_005.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 */
TEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_005)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    std::shared_ptr<DisplayListenerAdapter> listener = std::make_shared<DisplayListenerAdapterTest>();
    EXPECT_NE(displayManagerAdapterImpl, nullptr);
    EXPECT_NE(displayManagerAdapterImpl->GetDefaultDisplayId(), DISPLAY_ID_INVALID);
    EXPECT_NE(displayManagerAdapterImpl->GetDefaultDisplay(), nullptr);
    uint32_t id = 1;
    EXPECT_EQ(displayManagerAdapterImpl->RegisterDisplayListener(listener), id);
    uint32_t id2 = 2;
    EXPECT_TRUE(displayManagerAdapterImpl->UnregisterDisplayListener(id));
    EXPECT_FALSE(displayManagerAdapterImpl->UnregisterDisplayListener(id2));
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_006.
 * @tc.desc: test lock type.
 * @tc.type: FUNC
 */
TEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_006)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>();
    EXPECT_NE(displayAdapterImpl, nullptr);
    EXPECT_NE(displayAdapterImpl->GetId(), static_cast<DisplayId>(-1));
    EXPECT_NE(displayAdapterImpl->GetWidth(), -1);
    EXPECT_NE(displayAdapterImpl->GetHeight(), -1);
    EXPECT_NE(displayAdapterImpl->GetVirtualPixelRatio(), -1);
    EXPECT_NE(displayAdapterImpl->GetRotation(), RotationType::ROTATION_BUTT);
    EXPECT_NE(displayAdapterImpl->GetDpi(), -1);
    EXPECT_NE(displayAdapterImpl->GetDisplayOrientation(), DisplayOrientation::UNKNOWN);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_007.
 * @tc.desc: test IsDefaultPortrait.
 * @tc.type: FUNC
 */
TEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_007)
{
    std::shared_ptr<DisplayManagerAdapterImpl> displayManagerAdapterImpl =
        std::make_shared<DisplayManagerAdapterImpl>();
    EXPECT_NE(displayManagerAdapterImpl, nullptr);
    bool result = displayManagerAdapterImpl->IsDefaultPortrait();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DisplayManagerAdapterImplTest_008.
 * @tc.desc: test ConvertDisplayRotationType.
 * @tc.type: FUNC
 */
TEST_F(DisplayManagerAdapterImplTest, DisplayManagerAdapterImplTest_008)
{
    std::unique_ptr<DisplayAdapterImpl> displayAdapterImpl = std::make_unique<DisplayAdapterImpl>();
    EXPECT_NE(displayAdapterImpl, nullptr);

    NativeDisplayManager_Orientation type = NativeDisplayManager_Orientation::DISPLAY_MANAGER_UNKNOWN;
    OHOS::NWeb::DisplayOrientation retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::UNKNOWN);

    type = NativeDisplayManager_Orientation::DISPLAY_MANAGER_PORTRAIT;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::PORTRAIT);

    type = NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::LANDSCAPE);

    type = NativeDisplayManager_Orientation::DISPLAY_MANAGER_PORTRAIT_INVERTED;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED);

    type = NativeDisplayManager_Orientation::DISPLAY_MANAGER_LANDSCAPE_INVERTED;
    retType = displayAdapterImpl->ConvertDisplayOrientationType(type);
    EXPECT_EQ(retType, OHOS::NWeb::DisplayOrientation::LANDSCAPE_INVERTED);
}
}
}
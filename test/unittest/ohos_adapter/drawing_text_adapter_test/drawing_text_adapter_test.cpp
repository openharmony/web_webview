/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <sys/mman.h>

#include "ohos_adapter_helper.h"

#define private public
#include "ohos_drawing_text_adapter_impl.h"
#include "ohos_drawing_text_adapter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {
namespace {
const int NWEB_ERROR = -1;
const int RESULT_OK = 0;
}

class DrawingTextAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DrawingTextAdapterImplTest::SetUpTestCase() {}

void DrawingTextAdapterImplTest::TearDownTestCase() {}

void DrawingTextAdapterImplTest::SetUp() {}

void DrawingTextAdapterImplTest::TearDown() {}

/**
 * @tc.name: DrawingTextAdapterImplTest_001.
 * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNamesByType.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_001, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::ALL;
    void* drawingArray = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray);
    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(drawingArray, nullptr);

    systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray);
    EXPECT_EQ(errorCode, RESULT_OK);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_002.
 * @tc.desc: test DrawingTextFontAdapterImpl GetFontDescriptorByFullName.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_002, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void* drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, &drawingFontDescriptor);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    EXPECT_EQ(drawingFontDescriptor, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_003.
 * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNameByIndex.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_003, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingArray = nullptr;
    int32_t indexOfFullName = 0;
    const void* drawingString = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNameByIndex(
        drawingArray, indexOfFullName, &drawingString);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    EXPECT_EQ(drawingArray, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_004.
 * @tc.desc: test DrawingTextFontAdapterImpl DestroySystemFontFullNames.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_004, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingArray = nullptr;
    drawingTextFontAdapter->DestroySystemFontFullNames(drawingArray);

    EXPECT_EQ(drawingArray, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_005.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl GetSystemFontConfigInfo.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_005, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* fontConfigInfoErrorCode = nullptr;
    void* fontConfigInfo = nullptr;
    int32_t errorCode = drawingTextTypographyAdapter->GetSystemFontConfigInfo(fontConfigInfoErrorCode,
                    &fontConfigInfo);

    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(fontConfigInfo, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_006.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl GetDrawingArraySize.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_006, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* drawingArray = nullptr;
    int32_t sizeOfArray = 0;
    int32_t errorCode = drawingTextTypographyAdapter->GetDrawingArraySize(drawingArray, sizeOfArray);

    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(sizeOfArray, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_007.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl DestroySystemFontConfigInfo.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_007, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* fontConfigInfo = nullptr;
    drawingTextTypographyAdapter->DestroySystemFontConfigInfo(fontConfigInfo);

    EXPECT_EQ(fontConfigInfo, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_008.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl DestroySystemFontDescriptor.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_008, TestSize.Level1)
{
   std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void* drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, &drawingFontDescriptor);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    drawingTextFontAdapter->DestroyFontDescriptor(drawingFontDescriptor);
    drawingFontDescriptor = nullptr;
}

/**
* @tc.name  : DrawingTextAdapterImplTest_009
* @tc.number: Test GetSystemFontFullNamesByType function when drawingArray is nullptr
* @tc.desc  : FUNC.
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_009, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    int32_t systemFontType = -1;
    void* drawingArray = nullptr;
    EXPECT_EQ(-1, drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray));
}

/**
* @tc.name  : DrawingTextAdapterImplTest_010
* @tc.number: Test if GetInstance() returns the same instance when called multiple times.
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_010, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    auto& instance1 = drawingTextFontAdapter->GetInstance();
    auto& instance2 = drawingTextFontAdapter->GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
* @tc.name  : DrawingTextAdapterImplTest_011
* @tc.number: Test GetInstance function to ensure it returns the same instance on multiple calls
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_011, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    // Call GetInstance multiple times and check if the returned instances are the same
    auto& instance1 = drawingTextFontAdapter->GetInstance();
    auto& instance2 = drawingTextFontAdapter->GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
* @tc.name  : DrawingTextAdapterImplTest_012
* @tc.number: Test GetSystemFontFullNamesByType function when drawingArray is nullptr
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_012, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void** drawingArray = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(
        systemFontType, drawingArray);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_013.
 * @tc.desc: test GetFontDescriptorByFullName when drawingFontDescriptor is nullptr
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_013, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void** drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, drawingFontDescriptor);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/*
 * @tc.name: DrawingTextAdapterImplTest_014.
 * @tc.desc: test GetSystemFontFullNameByIndex when drawingString is nullptr
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_014, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();

    void* drawingArray = nullptr;
    int32_t indexOfFullName = 0;
    const void** drawingString = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNameByIndex(
        drawingArray, indexOfFullName, drawingString);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_015.
 * @tc.desc: test GetSystemFontConfigInfo when fontConfigInfo is nullptr.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_015, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();

    void* fontConfigInfoErrorCode = nullptr;
    void** fontConfigInfo = nullptr;
    int32_t errorCode = drawingTextTypographyAdapter->GetSystemFontConfigInfo(
        fontConfigInfoErrorCode,fontConfigInfo);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}


/**
 * @tc.name: DrawingTextAdapterImplTest_016.
 * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNamesByType.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_016, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::ALL;
    void* drawingArray = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray);
    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(drawingArray, nullptr);

    systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray);
    EXPECT_EQ(errorCode, RESULT_OK);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_017.
 * @tc.desc: test DrawingTextFontAdapterImpl GetFontDescriptorByFullName.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_017, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void* drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, &drawingFontDescriptor);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    EXPECT_EQ(drawingFontDescriptor, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_018.
 * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNameByIndex.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_018, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingArray = nullptr;
    int32_t indexOfFullName = 0;
    const void* drawingString = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNameByIndex(
        drawingArray, indexOfFullName, &drawingString);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    EXPECT_EQ(drawingArray, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_019.
 * @tc.desc: test DrawingTextFontAdapterImpl DestroySystemFontFullNames.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_019, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingArray = nullptr;
    drawingTextFontAdapter->DestroySystemFontFullNames(drawingArray);

    EXPECT_EQ(drawingArray, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_020.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl GetSystemFontConfigInfo.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_020, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* fontConfigInfoErrorCode = nullptr;
    void* fontConfigInfo = nullptr;
    int32_t errorCode = drawingTextTypographyAdapter->GetSystemFontConfigInfo(fontConfigInfoErrorCode,
                    &fontConfigInfo);

    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(fontConfigInfo, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_021.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl GetDrawingArraySize.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_021, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* drawingArray = nullptr;
    int32_t sizeOfArray = 0;
    int32_t errorCode = drawingTextTypographyAdapter->GetDrawingArraySize(drawingArray, sizeOfArray);

    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(sizeOfArray, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_022.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl DestroySystemFontConfigInfo.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_022, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* fontConfigInfo = nullptr;
    drawingTextTypographyAdapter->DestroySystemFontConfigInfo(fontConfigInfo);

    EXPECT_EQ(fontConfigInfo, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_023.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl DestroySystemFontDescriptor.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_023, TestSize.Level1)
{
   std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void* drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, &drawingFontDescriptor);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    drawingTextFontAdapter->DestroyFontDescriptor(drawingFontDescriptor);
    drawingFontDescriptor = nullptr;
}

/**
* @tc.name  : DrawingTextAdapterImplTest_024
* @tc.number: Test GetSystemFontFullNamesByType function when drawingArray is nullptr
* @tc.desc  : FUNC.
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_024, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    int32_t systemFontType = -1;
    void* drawingArray = nullptr;
    EXPECT_EQ(-1, drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray));
}

/**
* @tc.name  : DrawingTextAdapterImplTest_025
* @tc.number: Test if GetInstance() returns the same instance when called multiple times.
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_025, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    auto& instance1 = drawingTextFontAdapter->GetInstance();
    auto& instance2 = drawingTextFontAdapter->GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
* @tc.name  : DrawingTextAdapterImplTest_026
* @tc.number: Test GetInstance function to ensure it returns the same instance on multiple calls
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_026, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    // Call GetInstance multiple times and check if the returned instances are the same
    auto& instance1 = drawingTextFontAdapter->GetInstance();
    auto& instance2 = drawingTextFontAdapter->GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
* @tc.name  : DrawingTextAdapterImplTest_027
* @tc.number: Test GetSystemFontFullNamesByType function when drawingArray is nullptr
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_027, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void** drawingArray = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(
        systemFontType, drawingArray);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_028.
 * @tc.desc: test GetFontDescriptorByFullName when drawingFontDescriptor is nullptr
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_028, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void** drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, drawingFontDescriptor);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/*
 * @tc.name: DrawingTextAdapterImplTest_029.
 * @tc.desc: test GetSystemFontFullNameByIndex when drawingString is nullptr
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_029, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();

    void* drawingArray = nullptr;
    int32_t indexOfFullName = 0;
    const void** drawingString = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNameByIndex(
        drawingArray, indexOfFullName, drawingString);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_030.
 * @tc.desc: test GetSystemFontConfigInfo when fontConfigInfo is nullptr.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_030, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();

    void* fontConfigInfoErrorCode = nullptr;
    void** fontConfigInfo = nullptr;
    int32_t errorCode = drawingTextTypographyAdapter->GetSystemFontConfigInfo(
        fontConfigInfoErrorCode,fontConfigInfo);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_031.
 * @tc.desc: test GetSystemFontConfigInfo when fontConfigInfo is nullptr.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_031, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();

    void* fontConfigInfoErrorCode = nullptr;
    void** fontConfigInfo = nullptr;
    int32_t errorCode = drawingTextTypographyAdapter->GetSystemFontConfigInfo(
        fontConfigInfoErrorCode,fontConfigInfo);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}


/**
 * @tc.name: DrawingTextAdapterImplTest_032.
 * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNamesByType.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_032, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::ALL;
    void* drawingArray = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray);
    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(drawingArray, nullptr);

    systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray);
    EXPECT_EQ(errorCode, RESULT_OK);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_033.
 * @tc.desc: test DrawingTextFontAdapterImpl GetFontDescriptorByFullName.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_033, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void* drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, &drawingFontDescriptor);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    EXPECT_EQ(drawingFontDescriptor, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_034.
 * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNameByIndex.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_034, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingArray = nullptr;
    int32_t indexOfFullName = 0;
    const void* drawingString = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNameByIndex(
        drawingArray, indexOfFullName, &drawingString);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    EXPECT_EQ(drawingArray, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_035.
 * @tc.desc: test DrawingTextFontAdapterImpl DestroySystemFontFullNames.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_035, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingArray = nullptr;
    drawingTextFontAdapter->DestroySystemFontFullNames(drawingArray);

    EXPECT_EQ(drawingArray, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_036.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl GetSystemFontConfigInfo.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_036, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* fontConfigInfoErrorCode = nullptr;
    void* fontConfigInfo = nullptr;
    int32_t errorCode = drawingTextTypographyAdapter->GetSystemFontConfigInfo(fontConfigInfoErrorCode,
                    &fontConfigInfo);

    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(fontConfigInfo, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_037.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl GetDrawingArraySize.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_037, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* drawingArray = nullptr;
    int32_t sizeOfArray = 0;
    int32_t errorCode = drawingTextTypographyAdapter->GetDrawingArraySize(drawingArray, sizeOfArray);

    EXPECT_EQ(errorCode, RESULT_OK);
    EXPECT_NE(sizeOfArray, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_038.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl DestroySystemFontConfigInfo.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_038, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    EXPECT_TRUE(drawingTextTypographyAdapter != nullptr);

    void* fontConfigInfo = nullptr;
    drawingTextTypographyAdapter->DestroySystemFontConfigInfo(fontConfigInfo);

    EXPECT_EQ(fontConfigInfo, nullptr);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_039.
 * @tc.desc: test OhosDrawingTextTypographyAdapterImpl DestroySystemFontDescriptor.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_039, TestSize.Level1)
{
   std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void* drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, &drawingFontDescriptor);

    EXPECT_EQ(errorCode, NWEB_ERROR);
    drawingTextFontAdapter->DestroyFontDescriptor(drawingFontDescriptor);
    drawingFontDescriptor = nullptr;
}

/**
* @tc.name  : DrawingTextAdapterImplTest_040
* @tc.number: Test GetSystemFontFullNamesByType function when drawingArray is nullptr
* @tc.desc  : FUNC.
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_040, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    int32_t systemFontType = -1;
    void* drawingArray = nullptr;
    EXPECT_EQ(-1, drawingTextFontAdapter->GetSystemFontFullNamesByType(systemFontType, &drawingArray));
}

/**
* @tc.name  : DrawingTextAdapterImplTest_041
* @tc.number: Test if GetInstance() returns the same instance when called multiple times.
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_041, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    auto& instance1 = drawingTextFontAdapter->GetInstance();
    auto& instance2 = drawingTextFontAdapter->GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
* @tc.name  : DrawingTextAdapterImplTest_042
* @tc.number: Test GetInstance function to ensure it returns the same instance on multiple calls
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_042, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextTypographyAdapterImpl>();
    // Call GetInstance multiple times and check if the returned instances are the same
    auto& instance1 = drawingTextFontAdapter->GetInstance();
    auto& instance2 = drawingTextFontAdapter->GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
* @tc.name  : DrawingTextAdapterImplTest_043
* @tc.number: Test GetSystemFontFullNamesByType function when drawingArray is nullptr
* @tc.desc  : FUNC
*/
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_043, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
    std::make_shared<OhosDrawingTextFontAdapterImpl>();
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void** drawingArray = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNamesByType(
        systemFontType, drawingArray);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_044.
 * @tc.desc: test GetFontDescriptorByFullName when drawingFontDescriptor is nullptr
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_044, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();
    EXPECT_TRUE(drawingTextFontAdapter != nullptr);

    void* drawingString = nullptr;
    ArkWeb_Drawing_SystemFontType systemFontType = ArkWeb_Drawing_SystemFontType::GENERIC;
    void** drawingFontDescriptor = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetFontDescriptorByFullName(
        drawingString, systemFontType, drawingFontDescriptor);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/*
 * @tc.name: DrawingTextAdapterImplTest_045.
 * @tc.desc: test GetSystemFontFullNameByIndex when drawingString is nullptr
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_045, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextFontAdapterImpl> drawingTextFontAdapter =
        std::make_shared<OhosDrawingTextFontAdapterImpl>();

    void* drawingArray = nullptr;
    int32_t indexOfFullName = 0;
    const void** drawingString = nullptr;
    int32_t errorCode = drawingTextFontAdapter->GetSystemFontFullNameByIndex(
        drawingArray, indexOfFullName, drawingString);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}

/**
 * @tc.name: DrawingTextAdapterImplTest_046.
 * @tc.desc: test GetSystemFontConfigInfo when fontConfigInfo is nullptr.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(DrawingTextAdapterImplTest, DrawingTextAdapterImplTest_046, TestSize.Level1)
{
    std::shared_ptr<OhosDrawingTextTypographyAdapterImpl> drawingTextTypographyAdapter =
        std::make_shared<OhosDrawingTextTypographyAdapterImpl>();

    void* fontConfigInfoErrorCode = nullptr;
    void** fontConfigInfo = nullptr;
    int32_t errorCode = drawingTextTypographyAdapter->GetSystemFontConfigInfo(
        fontConfigInfoErrorCode,fontConfigInfo);
    EXPECT_EQ(errorCode, NWEB_ERROR);
}
} // namespace NWeb
} // namespace OHOS
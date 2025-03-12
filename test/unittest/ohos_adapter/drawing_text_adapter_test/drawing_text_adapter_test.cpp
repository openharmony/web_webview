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
  * @tc.name: DrawingTextAdapterImplTest_001
  * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNamesByType
  * @tc.type: FUNC
  * @tc.require
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
  * @tc.name: DrawingTextAdapterImplTest_002
  * @tc.desc: test DrawingTextFontAdapterImpl GetFontDescriptorByFullName
  * @tc.type: FUNC
  * @tc.require
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
  * @tc.name: DrawingTextAdapterImplTest_003
  * @tc.desc: test DrawingTextFontAdapterImpl GetSystemFontFullNameByIndex
  * @tc.type: FUNC
  * @tc.require
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
  * @tc.name: DrawingTextAdapterImplTest_004
  * @tc.desc: test DrawingTextFontAdapterImpl DestroySystemFontFullNames
  * @tc.type: FUNC
  * @tc.require
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
  * @tc.name: DrawingTextAdapterImplTest_005
  * @tc.desc: test DrawingTextTypographyAdapterImpl GetSystemFontConfigInfo
  * @tc.type: FUNC
  * @tc.require
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
  * @tc.name: DrawingTextAdapterImplTest_006
  * @tc.desc: test DrawingTextTypographyAdapterImpl GetDrawingArraySize
  * @tc.type: FUNC
  * @tc.require
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
  * @tc.name: DrawingTextAdapterImplTest_007
  * @tc.desc: test DrawingTextTypographyAdapterImpl GetDrawingArraySize
  * @tc.type: FUNC
  * @tc.require
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
  * @tc.name: DrawingTextAdapterImplTest_008
  * @tc.desc: test OhosDrawingTextTypographyAdapterImpl DestroySystemFontDescriptor
  * @tc.type: FUNC
  * @tc.require
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
 } // namespace NWeb
 } // namespace OHOS
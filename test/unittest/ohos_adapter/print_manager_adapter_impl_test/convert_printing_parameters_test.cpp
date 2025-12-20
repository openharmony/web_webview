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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "print_manager_adapter_impl.h"
#include "print_manager_adapter.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;

class ConvertPrintingParametersTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    Print::PrintAttributes defaultAttrs_;
};

void ConvertPrintingParametersTest::SetUpTestCase(void)
{}

void ConvertPrintingParametersTest::TearDownTestCase(void)
{}

void ConvertPrintingParametersTest::SetUp(void)
{
    // Set basic options
    defaultAttrs_.SetCopyNumber(2);
    Print::PrintRange range;
    range.SetStartPage(1);
    range.SetEndPage(5);
    range.SetPages({1, 5});
    defaultAttrs_.SetPageRange(range);
    defaultAttrs_.SetIsSequential(true);
    Print::PrintPageSize pageSize;
    pageSize.SetWidth(100);
    pageSize.SetHeight(200);
    defaultAttrs_.SetPageSize(pageSize);
    defaultAttrs_.SetIsLandscape(false);
    defaultAttrs_.SetColorMode(1);
    defaultAttrs_.SetDuplexMode(1);
    Print::PrintMargin margin;
    margin.SetTop(1);
    margin.SetBottom(2);
    margin.SetLeft(3);
    margin.SetRight(4);
    defaultAttrs_.SetMargin(margin);

}

void ConvertPrintingParametersTest::TearDown(void)
{}

/**
 * @tc.name  : ConvertPrintingParameters_CustomOptions_001
 * @tc.desc  : Test ConvertPrintingParameters without custom options
 * @tc.type  : FUNC
 */
HWTEST_F(ConvertPrintingParametersTest, ConvertPrintingParameters_CustomOptions_001, TestSize.Level0)
{
    // Test
    PrintDocumentAdapterImpl adapter(nullptr);
    PrintAttributesAdapter result = adapter.ConvertPrintingParameters(defaultAttrs_);

    // Check basic options
    EXPECT_EQ(result.copyNumber, 2);
    EXPECT_EQ(result.pageRange.startPage, 1);
    EXPECT_EQ(result.pageRange.endPage, 5);
    EXPECT_EQ(result.pageRange.pages.size(), 2);
    if (result.pageRange.pages.size() == 2) {
        EXPECT_EQ(result.pageRange.pages[0], 1);
        EXPECT_EQ(result.pageRange.pages[1], 5);
    }
    EXPECT_TRUE(result.isSequential);
    EXPECT_EQ(result.pageSize.width, 100);
    EXPECT_EQ(result.pageSize.height, 200);
    EXPECT_FALSE(result.isLandscape);
    EXPECT_EQ(result.colorMode, 1);
    EXPECT_EQ(result.duplexMode, 1);
    EXPECT_EQ(result.margin.top, 1);
    EXPECT_EQ(result.margin.bottom, 2);
    EXPECT_EQ(result.margin.left, 3);
    EXPECT_EQ(result.margin.right, 4);
    // Check custom options, UINT32_MAX means no custom option
    EXPECT_EQ(result.display_header_footer, UINT32_MAX);
    EXPECT_EQ(result.print_backgrounds, UINT32_MAX);
}

/**
 * @tc.name  : ConvertPrintingParameters_CustomOptions_002
 * @tc.desc  : Test ConvertPrintingParameters with custom options
 * @tc.type  : FUNC
 */
HWTEST_F(ConvertPrintingParametersTest, ConvertPrintingParameters_CustomOptions_002, TestSize.Level0)
{
    // Init
    auto attrs = defaultAttrs_;
    std::vector<Print::PrintCustomOption> customOption;

    // Set custom options
    Print::PrintCustomOption optionA;
    optionA.SetOptionName("display_header_footer");
    optionA.SetType(static_cast<uint32_t>(OHOS::Print::ComponentType::SWITCH));
    optionA.SetOptionResourceName("rid_display_header_footer");
    optionA.SetIsSelect(true);
    customOption.push_back(optionA);

    Print::PrintCustomOption optionB;
    optionB.SetOptionName("print_backgrounds");
    optionB.SetType(static_cast<uint32_t>(OHOS::Print::ComponentType::SWITCH));
    optionB.SetOptionResourceName("rid_print_backgrounds");
    optionB.SetIsSelect(false);
    customOption.push_back(optionB);

    attrs.SetCustomOption(customOption);

    // Test
    PrintDocumentAdapterImpl adapter(nullptr);
    PrintAttributesAdapter result = adapter.ConvertPrintingParameters(attrs);

    // Check custom options
    EXPECT_EQ(result.display_header_footer, 1);
    EXPECT_EQ(result.print_backgrounds, 0);
}
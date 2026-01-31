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

#define DEFAULT_COPY_NUMBER 2
#define DEFAULT_START_PAGE 1
#define DEFAULT_END_PAGE 5
#define DEFAULT_PAGE_RANGE_SIZE 2
#define DEFAULT_WIDTH 100
#define DEFAULT_HEIGHT 200
#define DEFAULT_COLOR_MODE 1
#define DEFAULT_DUPLEX_MODE 1
#define DEFAULT_MARGIN_TOP 1
#define DEFAULT_MARGIN_BOTTOM 2
#define DEFAULT_MARGIN_LEFT 3
#define DEFAULT_MARGIN_RIGHT 4

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
    defaultAttrs_.SetCopyNumber(DEFAULT_COPY_NUMBER);
    Print::PrintRange range;
    range.SetStartPage(DEFAULT_START_PAGE);
    range.SetEndPage(DEFAULT_END_PAGE);
    range.SetPages({DEFAULT_START_PAGE, DEFAULT_END_PAGE});
    defaultAttrs_.SetPageRange(range);
    defaultAttrs_.SetIsSequential(true);
    Print::PrintPageSize pageSize;
    pageSize.SetWidth(DEFAULT_WIDTH);
    pageSize.SetHeight(DEFAULT_HEIGHT);
    defaultAttrs_.SetPageSize(pageSize);
    defaultAttrs_.SetIsLandscape(false);
    defaultAttrs_.SetColorMode(DEFAULT_COLOR_MODE);
    defaultAttrs_.SetDuplexMode(DEFAULT_DUPLEX_MODE);
    Print::PrintMargin margin;
    margin.SetTop(DEFAULT_MARGIN_TOP);
    margin.SetBottom(DEFAULT_MARGIN_BOTTOM);
    margin.SetLeft(DEFAULT_MARGIN_LEFT);
    margin.SetRight(DEFAULT_MARGIN_RIGHT);
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
    EXPECT_EQ(result.copyNumber, DEFAULT_COPY_NUMBER);
    EXPECT_EQ(result.pageRange.startPage, DEFAULT_START_PAGE);
    EXPECT_EQ(result.pageRange.endPage, DEFAULT_END_PAGE);
    EXPECT_EQ(result.pageRange.pages.size(), DEFAULT_PAGE_RANGE_SIZE);
    if (result.pageRange.pages.size() == DEFAULT_PAGE_RANGE_SIZE) {
        EXPECT_EQ(result.pageRange.pages[0], DEFAULT_START_PAGE);
        EXPECT_EQ(result.pageRange.pages[1], DEFAULT_END_PAGE);
    }
    EXPECT_TRUE(result.isSequential);
    EXPECT_EQ(result.pageSize.width, DEFAULT_WIDTH);
    EXPECT_EQ(result.pageSize.height, DEFAULT_HEIGHT);
    EXPECT_FALSE(result.isLandscape);
    EXPECT_EQ(result.colorMode, DEFAULT_COLOR_MODE);
    EXPECT_EQ(result.duplexMode, DEFAULT_DUPLEX_MODE);
    EXPECT_EQ(result.margin.top, DEFAULT_MARGIN_TOP);
    EXPECT_EQ(result.margin.bottom, DEFAULT_MARGIN_BOTTOM);
    EXPECT_EQ(result.margin.left, DEFAULT_MARGIN_LEFT);
    EXPECT_EQ(result.margin.right, DEFAULT_MARGIN_RIGHT);
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

    // Set custom options
    std::string option = "{\"header_and_footer\":{\"value\":true},\"background_graphics\":{\"value\":false}}";
    attrs.SetOption(option);

    // Test
    PrintDocumentAdapterImpl adapter(nullptr);
    PrintAttributesAdapter result = adapter.ConvertPrintingParameters(attrs);

    // Check custom options
    EXPECT_EQ(result.display_header_footer, 1);
    EXPECT_EQ(result.print_backgrounds, 0);
}
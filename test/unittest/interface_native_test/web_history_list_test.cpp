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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "base/web/webview/interfaces/kits/nativecommon/web_history_list.h"
#include "base/web/webview/ohos_interface/include/ohos_nweb/nweb_history_list.h"

using namespace testing::ext;

namespace OHOS {
namespace NWeb {
class MockNWebHistoryItem : public NWebHistoryItem {
public:
    std::string GetHistoryRawUrl() override
    {
        return "mock_raw_url";
    }
    std::string GetHistoryTitle() override
    {
        return "mock_title";
    }
    std::string GetHistoryUrl() override
    {
        return "mock_url";
    }
    bool GetFavicon(void** data, int& width, int& height,
        ImageColorType& colorType, ImageAlphaType& alphaType) override
    {
        return false;
    }
};

class MockNWebHistoryList : public NWebHistoryList {
public:
    int32_t GetCurrentIndex() override
    {
        return 0;
    }
    std::shared_ptr<NWebHistoryItem> GetItem(int32_t index) override
    {
        return std::make_shared<MockNWebHistoryItem>();
    }
    int32_t GetListSize() override
    {
        return 1;
    }
};

class WebHistoryListTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
};

HWTEST_F(WebHistoryListTest, GetListSize_HistoryListIsNull_ReturnsZero, TestSize.Level1)
{
    WebHistoryList webHistoryList(nullptr);
    int32_t size = webHistoryList.GetListSize();
    EXPECT_EQ(0, size);
}

HWTEST_F(WebHistoryListTest, GetListSize_HistoryListIsNotNull_ReturnsListSize, TestSize.Level1)
{
    std::shared_ptr<MockNWebHistoryList> mockList = std::make_shared<MockNWebHistoryList>();
    WebHistoryList webHistoryList(mockList);
    int32_t size = webHistoryList.GetListSize();
    EXPECT_EQ(1, size);
}

HWTEST_F(WebHistoryListTest, GetItem_HistoryListIsNull_ReturnsNull, TestSize.Level1)
{
    WebHistoryList webHistoryList(nullptr);
    std::shared_ptr<NWebHistoryItem> item = webHistoryList.GetItem(0);
    EXPECT_EQ(nullptr, item);
}

HWTEST_F(WebHistoryListTest, GetItem_HistoryListIsNotNull_ReturnsItem, TestSize.Level1)
{
    std::shared_ptr<MockNWebHistoryList> mockList = std::make_shared<MockNWebHistoryList>();
    WebHistoryList webHistoryList(mockList);
    std::shared_ptr<NWebHistoryItem> item = webHistoryList.GetItem(0);
    EXPECT_NE(nullptr, item);
}
} // namespace NWeb
} // namespace OHOS
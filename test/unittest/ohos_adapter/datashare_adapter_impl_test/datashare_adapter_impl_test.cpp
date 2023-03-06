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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "datashare_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
class DatashareAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DatashareAdapterImplTest::SetUpTestCase(void)
{}

void DatashareAdapterImplTest::TearDownTestCase(void)
{}

void DatashareAdapterImplTest::SetUp(void)
{}

void DatashareAdapterImplTest::TearDown(void)
{}

/**
 * @tc.name: DatashareAdapterImplTest_GetInstance_001
 * @tc.desc: GetInstance.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DatashareAdapterImplTest, DatashareAdapterImplTest_GetInstance_001, TestSize.Level1)
{
    std::string uriStr = "datashare:///media";
    int result = DatashareAdapterImpl::GetInstance().OpenDataShareUriForRead(uriStr);
    EXPECT_EQ(result, -1);
    std::string displayName = DatashareAdapterImpl::GetInstance().GetFileDisplayName(uriStr);
    EXPECT_EQ(displayName, "");
}
} // namespace OHOS
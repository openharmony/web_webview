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

#include "ohos_init_web_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
class OhosInitWebAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void OhosInitWebAdapterTest::SetUpTestCase(void)
{}

void OhosInitWebAdapterTest::TearDownTestCase(void)
{}

void OhosInitWebAdapterTest::SetUp(void)
{}

void OhosInitWebAdapterTest::TearDown(void)
{}

/**
 * @tc.name: OhosInitWebAdapterTest_SetRunWebInitedCallback_001
 * @tc.desc: SetRunWebInitedCallback.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosInitWebAdapterTest, OhosInitWebAdapterTest_SetRunWebInitedCallback_001, TestSize.Level1)
{
    OhosInitWebAdapterImpl adapter;
    adapter.SetRunWebInitedCallback(nullptr);
    WebRunInitedCallback* callbackImpl = adapter.GetRunWebInitedCallback();
    EXPECT_EQ(callbackImpl, nullptr);
}
} // namespace OHOS
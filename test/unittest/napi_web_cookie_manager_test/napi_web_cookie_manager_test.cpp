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
#include "nweb_adapter_helper.h"
#include "nweb_cookie_manager.h"
#include "nweb_helper.h"
#include "nweb.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::NWeb {
namespace {
const std::string MOCK_NWEB_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.arkwebcore";
} // namespace

class NapiWebCookieManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NapiWebCookieManagerTest::SetUpTestCase(void)
{}

void NapiWebCookieManagerTest::TearDownTestCase(void)
{}

void NapiWebCookieManagerTest::SetUp(void)
{}

void NapiWebCookieManagerTest::TearDown(void)
{}

NWebHelper& g_nwebHelperInstance = NWebHelper::Instance();
/**
 * @tc.name: NapiWebCookieManagerTest_001
 * @tc.desc: GetCookieManager.
 * @tc.type: FUNC
 * @tc.require: I5P91V
 */
HWTEST_F(NapiWebCookieManagerTest, NapiWebCookieManagerTest_001, TestSize.Level1)
{
    std::string hapPath = "";
    if (access(MOCK_NWEB_INSTALLATION_DIR.c_str(), F_OK) == 0) {
        hapPath = MOCK_NWEB_INSTALLATION_DIR;
    }
    g_nwebHelperInstance.SetBundlePath(hapPath);
    NWebAdapterHelper::Instance().Init(false);
    std::shared_ptr<NWebCookieManager> cookieManager = g_nwebHelperInstance.GetCookieManager();
    EXPECT_EQ(cookieManager, nullptr);
}
} // namespace OHOS::NWeb

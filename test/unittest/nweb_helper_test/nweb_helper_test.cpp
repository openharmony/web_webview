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
#include <gtest/gtest.h>
#include <securec.h>
#include <ui/rs_surface_node.h>
#include <unordered_map>

#define private public
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_adapter_helper.h"
#include "nweb_create_window.h"
#include "nweb_c_api.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;

namespace OHOS::NWeb {
namespace {
sptr<Surface> g_surface = nullptr;
const bool RESULT_OK = true;
const int DEFAULT_WIDTH = 2560;
const int DEFAULT_HEIGHT = 1396;
const int32_t NWEB_MAX_WIDTH = 7681;
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
} // namespace

class NwebHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NwebHelperTest::SetUpTestCase(void)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "webTestSurfaceName";
    auto surfaceNode = RSSurfaceNode::Create(config, false);
    EXPECT_NE(surfaceNode, nullptr);
    g_surface = surfaceNode->GetSurface();
    EXPECT_NE(g_surface, nullptr);
}

void NwebHelperTest::TearDownTestCase(void)
{}

void NwebHelperTest::SetUp(void)
{}

void NwebHelperTest::TearDown(void)
{}

std::unordered_map<std::string, std::string> g_argsMap;

/**
 * @tc.name: NWebHelper_SetBundlePath_001
 * @tc.desc: SetBundlePath.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetBundlePath_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    bool result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_FALSE(result);
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_EQ(RESULT_OK, result);
    NWebCreateInfo create_info;
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_NE(nweb, nullptr);
    NWebDOHConfig config;
    NWebHelper::Instance().SetHttpDns(config);
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper.lock(), nullptr);
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
    result = NWebHelper::Instance().InitAndRun(false);
    EXPECT_FALSE(result);
    result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    WebDownloadManager_PutDownloadCallback(nullptr);
}

/**
 * @tc.name: NWebHelper_GetWebStorage_002
 * @tc.desc: GetWebStorage.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetWebStorage_002, TestSize.Level1)
{
    auto web_storage = NWebHelper::Instance().GetWebStorage();
    bool result = false;
    if (web_storage != nullptr) {
        result = true;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::string config = NWebAdapterHelper::Instance().ParsePerfConfig("web", "test");
    EXPECT_TRUE(config.empty());
    NWebAdapterHelper::Instance().perfConfig_.emplace("web/test", "web_test");
    config = NWebAdapterHelper::Instance().ParsePerfConfig("web", "test");
    EXPECT_FALSE(config.empty());
}

/**
 * @tc.name: NWebHelper_GetDataBase_003
 * @tc.desc: GetDataBase.
 * @tc.type: FUNC
 * @tc.require:issueI5OESN
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetDataBase_003, TestSize.Level1)
{
    auto dataBase = NWebHelper::Instance().GetDataBase();
    bool result = false;
    if (dataBase != nullptr) {
        result = true;
    }
    EXPECT_EQ(RESULT_OK, result);

    NWebHelper::Instance().libHandleWebEngine_ = nullptr;

    void *enhanceSurfaceInfo = nullptr;
    int32_t temp = 1;
    std::shared_ptr<NWeb> nweb =
        NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    enhanceSurfaceInfo = static_cast<void *>(&temp);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    DEFAULT_WIDTH, NWEB_MAX_WIDTH);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    NWEB_MAX_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    NWebCreateInfo create_info;
    nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
}

/**
 * @tc.name: NWebHelper_TryPreReadLib_004
 * @tc.desc: TryPreReadLib.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_TryPreReadLib_004, TestSize.Level1)
{
    NWebHelper::Instance().TryPreReadLib(false, MOCK_INSTALLATION_DIR);
    NWebHelper::Instance().TryPreReadLib(true, MOCK_INSTALLATION_DIR);
    bool result = NWebHelper::Instance().Init(false);
    EXPECT_TRUE(result);
    sptr<Surface> surface = nullptr;
    std::shared_ptr<NWeb> nweb =
        NWebAdapterHelper::Instance().CreateNWeb(surface, GetInitArgs(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(g_surface, GetInitArgs(),
                                                    DEFAULT_WIDTH, NWEB_MAX_WIDTH);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(g_surface, GetInitArgs(),
                                                    NWEB_MAX_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
}

/**
 * @tc.name: NWebHelper_GetConfigPath_005
 * @tc.desc: GetConfigPath.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetConfigPath_005, TestSize.Level1)
{
    std::string configFileName = "test";
    std::string figPath = NWebAdapterHelper::Instance().GetConfigPath(configFileName);
    EXPECT_FALSE(figPath.empty());
    NWebInitArgs initArgs;
    NWebAdapterHelper::Instance().ParseConfig(initArgs);
    NWebHelper::Instance().libHandleWebEngine_ = nullptr;
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
    NWebHelper::Instance().bundlePath_.clear();
    bool result = NWebHelper::Instance().InitAndRun(false);
    EXPECT_FALSE(result);
    NWebHelper::Instance().SetWebDebuggingAccess(true);
    NWebHelper::Instance().SetConnectionTimeout(1);
}
} // namespace OHOS::NWeb

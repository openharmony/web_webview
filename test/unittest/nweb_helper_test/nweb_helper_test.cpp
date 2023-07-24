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
const std::string ARG_URL = "--url";
const std::string ARG_DUMP = "--dump-path";
const std::string ARG_FRAME_INFO = "--frame-info";
const std::string ARG_ADD_WEB_ENGINE_ARG = "--add-args";
const std::string ARG_DELETE_WEB_ENGINE_ARG = "--delete-args";
const std::string ARG_MULTI_RENDER_PROCESS = "--multi-renderer-process";
const std::string ARG_NWEB_TEST_MOCK_BUNDLEPATH = "--bundle-installation-dir";
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
const std::string ARG_WIDTH = "--width";
const std::string ARG_HEIGHT = "--height";
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

bool HasArg(const std::string& arg)
{
    return (!g_argsMap.empty()) && (g_argsMap.find(arg) != g_argsMap.end());
}

std::string GetArgValue(const std::string& arg)
{
    if (!HasArg(arg)) {
        return "";
    }
    return g_argsMap.at(arg);
}

std::list<std::string> GetWebEngineArgs(const std::string& arg)
{
    std::string webEngineArgValue = GetArgValue(arg);
    std::list<std::string> webEngineArgList;
    if (webEngineArgValue.empty()) {
        return webEngineArgList;
    }
    uint32_t start = 0;
    uint32_t pos = 0;
    while (pos < webEngineArgValue.size()) {
        if (webEngineArgValue[pos] == ',') {
            webEngineArgList.emplace_back(webEngineArgValue.substr(start, pos - start));
            pos++;
            start = pos;
        } else {
            pos++;
        }
    }
    webEngineArgList.emplace_back(webEngineArgValue.substr(start, pos - start));
    webEngineArgList.emplace_back(ARG_NWEB_TEST_MOCK_BUNDLEPATH + "=" + MOCK_INSTALLATION_DIR);
    return webEngineArgList;
}

NWebInitArgs GetInitArgs()
{
    NWebInitArgs initArgs = {
        .dump_path = GetArgValue(ARG_DUMP),
        .frame_info_dump = HasArg(ARG_FRAME_INFO) ? true : false,
        .web_engine_args_to_add = GetWebEngineArgs(ARG_ADD_WEB_ENGINE_ARG),
        .web_engine_args_to_delete = GetWebEngineArgs(ARG_DELETE_WEB_ENGINE_ARG),
        .multi_renderer_process = HasArg(ARG_MULTI_RENDER_PROCESS) ? true : false,
    };
    return initArgs;
}

/**
 * @tc.name: NWebHelper_SetBundlePath_001
 * @tc.desc: SetBundlePath.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetBundlePath_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    bool result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_EQ(RESULT_OK, result);
    NWebCreateInfo create_info;
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_NE(nweb, nullptr);
    NWebDOHConfig config;
    NWebHelper::Instance().SetHttpDns(config);
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper.lock(), nullptr);
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
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
    NWebCreateInfo createInfo;
    NWebAdapterHelper::Instance().ParseConfig(createInfo);
    NWebHelper::Instance().libHandleWebEngine_ = nullptr;
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
}
} // namespace OHOS::NWeb
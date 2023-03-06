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
#include "window.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::NWeb {
namespace {
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
{}

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

uint32_t GetNumFromArgs(const std::string& arg)
{
    if (!HasArg(arg)) {
        return 0;
    }
    return std::stoi(GetArgValue(arg));
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

sptr<OHOS::Rosen::Window> CreateWindow()
{
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    int width = HasArg(ARG_WIDTH) ? GetNumFromArgs(ARG_WIDTH) : DEFAULT_WIDTH;
    int height = HasArg(ARG_HEIGHT) ? GetNumFromArgs(ARG_HEIGHT) : DEFAULT_HEIGHT;
    OHOS::Rosen::Rect windowRect = { 0, 0, width, height };
    option->SetWindowRect(windowRect);
    auto window = OHOS::Rosen::Window::Create("nweb_test_window", option);
    return window;
}

/**
 * @tc.name: NWebHelper_SetBundlePath_001
 * @tc.desc: SetBundlePath.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetBundlePath_001, TestSize.Level1)
{
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    bool result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_EQ(RESULT_OK, result);
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

    sptr<OHOS::Rosen::Window> window = CreateWindow();
    NWebHelper::Instance().libHandleWebEngine_ = nullptr;
    std::shared_ptr<NWeb> nweb =
       NWebAdapterHelper::Instance().CreateNWeb(window.GetRefPtr(), GetInitArgs());
    EXPECT_EQ(nweb, nullptr);

    void *enhanceSurfaceInfo = nullptr;
    int32_t temp = 1;
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
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
}
} // namespace OHOS::NWeb

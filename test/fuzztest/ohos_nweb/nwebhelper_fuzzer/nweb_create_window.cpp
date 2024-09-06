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

#include "nweb_create_window.h"

#include <cstring>
#include <securec.h>
#include <ui/rs_surface_node.h>
#include <unordered_map>

#include "nweb_adapter_helper.h"

using namespace OHOS;
namespace OHOS::NWeb {
namespace {
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
std::unordered_map<std::string, std::string> g_argsMap;
sptr<Surface> g_surface = nullptr;
} // namespace

static bool HasArg(const std::string& arg)
{
    return (!g_argsMap.empty()) && (g_argsMap.find(arg) != g_argsMap.end());
}

static std::string GetArgValue(const std::string& arg)
{
    if (!HasArg(arg)) {
        return "";
    }
    return g_argsMap.at(arg);
}

static std::list<std::string> GetWebEngineArgs(const std::string& arg)
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

std::shared_ptr<NWebEngineInitArgsImpl> GetInitArgs(void)
{
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs = std::make_shared<NWebEngineInitArgsImpl>();
    initArgs->SetDumpPath(GetArgValue(ARG_DUMP));
    initArgs->SetIsFrameInfoDump(HasArg(ARG_FRAME_INFO) ? true : false);
    initArgs->SetIsMultiRendererProcess(HasArg(ARG_MULTI_RENDER_PROCESS) ? true : false);
    initArgs->SetArgsToAdd(GetWebEngineArgs(ARG_ADD_WEB_ENGINE_ARG));
    initArgs->SetArgsToDelete(GetWebEngineArgs(ARG_DELETE_WEB_ENGINE_ARG));
    return initArgs;
}

std::shared_ptr<NWeb> GetNwebForTest()
{
    if (!g_surface) {
        Rosen::RSSurfaceNodeConfig config;
        config.SurfaceNodeName = "webTestSurfaceName";
        auto surfaceNode = Rosen::RSSurfaceNode::Create(config, false);
        if (surfaceNode == nullptr) {
            return nullptr;
        }
        g_surface = surfaceNode->GetSurface();
        if (g_surface == nullptr) {
            return nullptr;
        }
    }
    return NWebAdapterHelper::Instance().CreateNWeb(g_surface, GetInitArgs());
}
} // namespace OHOS::NWeb

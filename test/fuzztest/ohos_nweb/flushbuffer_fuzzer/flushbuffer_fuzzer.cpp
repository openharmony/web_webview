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

#include "flushbuffer_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <ui/rs_surface_node.h>

#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "nweb_handler_impl_test.h"

#define private public
#include "nweb_surface_adapter.h"
#include "window.h"

using namespace OHOS::NWeb;

namespace OHOS {
namespace {
std::unordered_map<std::string, std::string> g_argsMap;
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
const int DEFAULT_WIDTH = 2560;
const int DEFAULT_HEIGHT = 1396;
}

bool HasArg(const std::string &arg)
{
    return (!g_argsMap.empty()) && (g_argsMap.find(arg) != g_argsMap.end());
}

std::string GetArgValue(const std::string &arg)
{
    if (!HasArg(arg)) {
        return "";
    }
    return g_argsMap.at(arg);
}

uint32_t GetNumFromArgs(const std::string &arg)
{
    if (!HasArg(arg)) {
        return 0;
    }
    return std::stoi(GetArgValue(arg));
}

std::list<std::string> GetWebEngineArgs(const std::string &arg)
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

OHOS::NWeb::NWebInitArgs GetInitArgs()
{
    OHOS::NWeb::NWebInitArgs initArgs = {
        .dump_path = GetArgValue(ARG_DUMP),
        .frame_info_dump = HasArg(ARG_FRAME_INFO) ? true : false,
        .web_engine_args_to_add = GetWebEngineArgs(ARG_ADD_WEB_ENGINE_ARG),
        .web_engine_args_to_delete = GetWebEngineArgs(ARG_DELETE_WEB_ENGINE_ARG),
        .multi_renderer_process = HasArg(ARG_MULTI_RENDER_PROCESS) ? true : false,
    };
    return initArgs;
}

sptr<Rosen::Window> CreateWindow()
{
    sptr<Rosen::WindowOption> option = new Rosen::WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    int width = HasArg(ARG_WIDTH) ? GetNumFromArgs(ARG_WIDTH) : DEFAULT_WIDTH;
    int height = HasArg(ARG_HEIGHT) ? GetNumFromArgs(ARG_HEIGHT) : DEFAULT_HEIGHT;
    option->SetWindowRect({0, 0, width, height});
    auto window = Rosen::Window::Create("nweb_test_window", option);
    return window;
}
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(uint32_t))) {
        return false;
    }
    sptr<Rosen::Window> window = CreateWindow();
    if (window == nullptr) {
        return false;
    }
    sptr<Surface> surface = window->GetSurfaceNode()->GetSurface();
    if (surface == nullptr) {
        return false;
    }
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    BufferRequestConfig requestConfig = {
        .width = DEFAULT_WIDTH,
        .height = DEFAULT_HEIGHT,
        .strideAlignment = sizeof(void *),
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    surface->RequestBuffer(surfaceBuffer, releaseFence, requestConfig);
    if (surfaceBuffer == nullptr) {
        return false;
    }
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    uint32_t width;
    uint32_t height;
    if (memcpy_s(&width, sizeof(uint32_t), data, sizeof(uint32_t)) != 0) {
        return false;
    }
    if (memcpy_s(&height, sizeof(uint32_t), data, sizeof(uint32_t)) != 0) {
        return false;
    }
    std::string result(reinterpret_cast<const char *>(data), size);
    surfaceAdapter.FlushBuffer(surface, surfaceBuffer, width, height);
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

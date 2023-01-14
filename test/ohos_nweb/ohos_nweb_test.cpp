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
#include <display_gralloc.h>
#include <functional>
#include <iservice_registry.h>
#include <securec.h>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <zlib.h>

#include <ui/rs_surface_node.h>
#include "event_handler.h"
#include "nweb_test_log.h"
#include "nweb.h"
#include "nweb_handler_impl_test.h"
#include "nweb_handler_impl_test_for_intercept.h"
#include "nweb_cookie_manager.h"
#include "nweb_downloadlistener_impl_test.h"
#include "nweb_adapter_helper.h"
#include "nweb_hit_testresult.h"
#include "nweb_preference.h"
#include "nweb_input_event_consumer_test.h"
#include "nweb_javascript_result_callback_test.h"
#include "nweb_js_dialog_impl_test.h"
#include "nweb_cookie_test_callback.h"
#include "window.h"

using namespace OHOS;

namespace {
sptr<Rosen::Window> g_window = nullptr;
sptr<Rosen::Window> g_window2 = nullptr;
std::shared_ptr<OHOS::NWeb::NWeb> g_nweb = nullptr;
std::shared_ptr<OHOS::NWeb::NWeb> g_nweb2 = nullptr;
std::shared_ptr<OHOS::NWeb::NWebPreference> g_webSettings = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> g_handler = nullptr;
std::string g_url = "www.baidu.com";

std::unordered_map<std::string, std::string> g_argsMap;
const std::string ARG_URL = "--url";
const std::string ARG_DUMP = "--dump-path";
const std::string ARG_FRAME_INFO = "--frame-info";
const std::string ARG_ADD_WEB_ENGINE_ARG = "--add-args";
const std::string ARG_DELETE_WEB_ENGINE_ARG = "--delete-args";
const std::string ARG_MULTI_RENDER_PROCESS = "--multi-renderer-process";
const std::string ARG_NWEB_TEST_MOCK_BUNDLEPATH = "--bundle-installation-dir";
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
const int DEFAULT_FONT_SIZE = 13;
const int DEFAULT_FONT_SIZE_TWO = 16;
const int DEFAULT_FONT_SIZE_MAX = 72;
const int MINI_LOGICAL_FONT_SIZE = 8;
const int TEXT_SIZE_PERCENT_MIN = 100;
const int TEXT_SIZE_PERCENT_MAX = 500;
const int TIME_SECONDS_FIVE = 5;
const int TIME_SECONDS_THREE = 3;
const int MINI_FONT_SIZE = 8;
const int STEP_TWO = 2;
const std::string ARG_WIDTH = "--width";
const std::string ARG_HEIGHT = "--height";

const int DEFAULT_WIDTH = 2560;
const int DEFAULT_HEIGHT = 1396;

void InitArgs(int32_t argc, const char * const argv[])
{
    constexpr int leastArgsCount = 2;
    if (argc <= leastArgsCount) {
        return;
    }
    for (auto i = leastArgsCount; i < argc; ++i) {
        std::string arg = argv[i];
        uint64_t pos = 0;
        while (pos < arg.size()) {
            if (arg[pos] == '=') {
                break;
            }
            ++pos;
        }
        if (pos == arg.size()) {
            g_argsMap.emplace(std::make_pair(arg, ""));
        } else {
            g_argsMap.emplace(std::make_pair(arg.substr(0, pos), arg.substr(pos + 1)));
        }
    }
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
        TESTLOG_E("fail to new option.");
        return nullptr;
    }
    int width = HasArg(ARG_WIDTH) ? GetNumFromArgs(ARG_WIDTH) : DEFAULT_WIDTH;
    int height = HasArg(ARG_HEIGHT) ? GetNumFromArgs(ARG_HEIGHT) : DEFAULT_HEIGHT;
    option->SetWindowRect({0, 0, width, height});
    auto window = Rosen::Window::Create("nweb_test_window", option);
    return window;
}

void RegistEventCb(sptr<Rosen::Window> window, std::shared_ptr<OHOS::NWeb::NWeb> nweb)
{
    if (window == nullptr) {
        return;
    }

    std::shared_ptr<Rosen::IInputEventConsumer> inputEventListener =
        std::make_shared<NWeb::NWebInputEventConsumerTest>(nweb);

    window->SetInputEventConsumer(inputEventListener);
}

void TestPrepare()
{
    TESTLOG_I("TestPrepare");
    OHOS::NWeb::NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    if (!OHOS::NWeb::NWebAdapterHelper::Instance().Init(false)) {
        TESTLOG_E("fail to init NWebAdapterHelper, test end");
        return;
    }
    g_window = CreateWindow();
    if (g_window == nullptr) {
        return;
    }

    g_nweb = NWeb::NWebAdapterHelper::Instance().CreateNWeb(
        g_window->GetSurfaceNode()->GetSurface(), GetInitArgs());
    if (g_nweb == nullptr) {
        g_window = nullptr;
        TESTLOG_E("fail to get nweb instance, test end");
        return;
    }
    g_nweb->Resize(g_window->GetRequestRect().width_, g_window->GetRequestRect().height_);
    RegistEventCb(g_window, g_nweb);

    TESTLOG_I("CreateNWeb SUCCESS");
    auto nwebHandler = std::make_shared<OHOS::NWeb::NWebHandlerImplTest>();
    if (nwebHandler == nullptr) {
        TESTLOG_E("fail to new NWeb Handler, test end");
        return;
    }
    g_nweb->SetNWebHandler(nwebHandler);
    g_url = HasArg(ARG_URL) ? GetArgValue(ARG_URL) : "www.baidu.com"; // load www.baidu.com by default
    TESTLOG_I("ready to load url=%{public}s", g_url.c_str());

    g_webSettings = g_nweb->GetPreference();
}

void TestPrepareWithClient(std::shared_ptr<OHOS::NWeb::NWebHandler> client)
{
    TESTLOG_I("TestPrepare");
    OHOS::NWeb::NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    if (!OHOS::NWeb::NWebAdapterHelper::Instance().Init(false)) {
        TESTLOG_E("fail to init NWebAdapterHelper, test end");
        return;
    }

    g_window = CreateWindow();
    if (g_window == nullptr) {
        return;
    }

    g_nweb = NWeb::NWebAdapterHelper::Instance().CreateNWeb(
        g_window->GetSurfaceNode()->GetSurface(), GetInitArgs());
    if (g_nweb == nullptr) {
        g_window = nullptr;
        TESTLOG_E("fail to get nweb instance, test end");
        return;
    }
    g_nweb->Resize(g_window->GetRequestRect().width_, g_window->GetRequestRect().height_);
    RegistEventCb(g_window, g_nweb);

    TESTLOG_I("CreateNWeb SUCCESS");
    g_nweb->SetNWebHandler(client);
    g_url = HasArg(ARG_URL) ? GetArgValue(ARG_URL) : "www.baidu.com"; // load www.baidu.com by default
    TESTLOG_I("ready to load url=%{public}s", g_url.c_str());

    g_webSettings = g_nweb->GetPreference();
}

void TestWebSettingsForNormalWeb()
{
    if (g_webSettings != nullptr) {
        g_webSettings->PutJavaScriptEnabled(true);
        g_webSettings->PutIsCreateWindowsByJavaScriptAllowed(true);
        g_webSettings->PutDomStorageEnabled(true);
    }
}

// create nweb and load url with fullscreen
void Test1()
{
    TESTLOG_I("Test1 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("end");
}

void Test200()
{
    TESTLOG_I("Test200 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool content_access = g_webSettings->EnableContentAccess();
        if (!content_access) {
            TESTLOG_E("Error! content_access default value:%{public}d != true.", content_access);
        } else {
            TESTLOG_I("content_access default value: %{public}d", content_access);
        }

        TESTLOG_I("Set content_access to false.");
        g_webSettings->PutEnableContentAccess(false);
        content_access = g_webSettings->EnableContentAccess();
        if (content_access) {
            TESTLOG_E("Error! content_access :%{public}d != false.", content_access);
        } else {
            TESTLOG_I("content_access=%{public}d", content_access);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test200 end");
}

void Test201()
{
    TESTLOG_I("Test201 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool file_access = g_webSettings->EnableRawFileAccess();
        if (file_access) {
            TESTLOG_E("Error! file_access default value:%{public}d != false.", file_access);
        } else {
            TESTLOG_I("file_access default value: %{public}d", file_access);
        }

        TESTLOG_I("Set file_access to true.");
        g_webSettings->PutEnableRawFileAccess(true);
        file_access = g_webSettings->EnableRawFileAccess();
        if (!file_access) {
            TESTLOG_E("Error! file_access :%{public}d != true.", file_access);
        } else {
            TESTLOG_I("file_access=%{public}d", file_access);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test201 end");
}

void Test202()
{
    TESTLOG_I("Test202 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool file_access_from_file_urls = g_webSettings->EnableRawFileAccessFromFileURLs();
        if (file_access_from_file_urls) {
            TESTLOG_E("Error! file_access_from_file_urls default value:%{public}d != false.",
                file_access_from_file_urls);
        } else {
            TESTLOG_I("file_access_from_file_urls default value: %{public}d", file_access_from_file_urls);
        }

        TESTLOG_I("Set file_access_from_file_urls to true.");
        g_webSettings->PutEnableRawFileAccessFromFileURLs(true);
        file_access_from_file_urls = g_webSettings->EnableRawFileAccessFromFileURLs();
        if (!file_access_from_file_urls) {
            TESTLOG_E("Error! file_access_from_file_urls :%{public}d != true.", file_access_from_file_urls);
        } else {
            TESTLOG_I("file_access_from_file_urls=%{public}d", file_access_from_file_urls);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test202 end");
}

void Test203()
{
    TESTLOG_I("Test203 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool universal_access_from_file_urls = g_webSettings->EnableUniversalAccessFromFileURLs();
        if (universal_access_from_file_urls) {
            TESTLOG_E("Error! universal_access_from_file_urls default value:%{public}d != false.",
                universal_access_from_file_urls);
        } else {
            TESTLOG_I("universal_access_from_file_urls default value: %{public}d", universal_access_from_file_urls);
        }

        TESTLOG_I("Set universal_access_from_file_urls to true.");
        g_webSettings->PutEnableUniversalAccessFromFileURLs(true);
        universal_access_from_file_urls = g_webSettings->EnableUniversalAccessFromFileURLs();
        if (!universal_access_from_file_urls) {
            TESTLOG_E("Error! universal_access_from_file_urls :%{public}d != true.", universal_access_from_file_urls);
        } else {
            TESTLOG_I("universal_access_from_file_urls=%{public}d", universal_access_from_file_urls);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test203 end");
}

void Test204()
{
    TESTLOG_I("Test204 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool images_enabled_ = !g_webSettings->IsLoadImageFromNetworkDisabled();
        if (!images_enabled_) {
            TESTLOG_E("Error! images_enabled_ default value:%{public}d != true.", images_enabled_);
        } else {
            TESTLOG_I("images_enabled_ default value: %{public}d", images_enabled_);
        }

        TESTLOG_I("Set images_enabled_ to false.");
        g_webSettings->PutLoadImageFromNetworkDisabled(true);
        images_enabled_ = !g_webSettings->IsLoadImageFromNetworkDisabled();
        if (images_enabled_) {
            TESTLOG_E("Error! images_enabled_ :%{public}d != false.", images_enabled_);
        } else {
            TESTLOG_I("images_enabled_=%{public}d", images_enabled_);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test204 end");
}

void Test205()
{
    TESTLOG_I("Test205 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string cursive_font_family = g_webSettings->CursiveFontFamilyName();
        if (cursive_font_family != "cursive") {
            TESTLOG_E("Error! cursive_font_family default value:%{public}s != cursive.", cursive_font_family.c_str());
        } else {
            TESTLOG_I("cursive_font_family default value: %{public}s", cursive_font_family.c_str());
        }

        TESTLOG_I("Set cursive_font_family to cursive.");
        g_webSettings->PutCursiveFontFamilyName("fantasy");
        cursive_font_family = g_webSettings->CursiveFontFamilyName();
        if (cursive_font_family != "cursive") {
            TESTLOG_E("Error! cursive_font_family :%{public}s != cursive.", cursive_font_family.c_str());
        } else {
            TESTLOG_I("cursive_font_family=%{public}s", cursive_font_family.c_str());
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test205 end");
}

void Test206()
{
    TESTLOG_I("Test206 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool databases_enabled = g_webSettings->IsDataBaseEnabled();
        if (databases_enabled) {
            TESTLOG_E("Error! databases_enabled default value:%{public}d != false.", databases_enabled);
        } else {
            TESTLOG_I("databases_enabled default value: %{public}d", databases_enabled);
        }

        TESTLOG_I("Set databases_enabled to true.");
        g_webSettings->PutDatabaseAllowed(true);
        databases_enabled = g_webSettings->IsDataBaseEnabled();
        if (!databases_enabled) {
            TESTLOG_E("Error! databases_enabled :%{public}d != true.", databases_enabled);
        } else {
            TESTLOG_I("databases_enabled=%{public}d", databases_enabled);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test206 end");
}

void Test207()
{
    TESTLOG_I("Test207 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        int default_fixed_font_size = g_webSettings->DefaultFixedFontSize();
        if (default_fixed_font_size != DEFAULT_FONT_SIZE) {
            TESTLOG_E("Error! default_fixed_font_size default value:%{public}d != 13.", default_fixed_font_size);
        } else {
            TESTLOG_I("default_fixed_font_size default value: %{public}d", default_fixed_font_size);
        }

        TESTLOG_I("Set default_fixed_font_size to 72.");
        g_webSettings->PutDefaultFixedFontSize(DEFAULT_FONT_SIZE_MAX);
        default_fixed_font_size = g_webSettings->DefaultFixedFontSize();
        if (default_fixed_font_size != DEFAULT_FONT_SIZE_MAX) {
            TESTLOG_E("Error! default_fixed_font_size :%{public}d != 72.", default_fixed_font_size);
        } else {
            TESTLOG_I("default_fixed_font_size=%{public}d", default_fixed_font_size);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test207 end");
}

void Test208()
{
    TESTLOG_I("Test208 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        int default_font_size = g_webSettings->DefaultFontSize();
        if (default_font_size != DEFAULT_FONT_SIZE_TWO) {
            TESTLOG_E("Error! default_font_size default value:%{public}d != 16.", default_font_size);
        } else {
            TESTLOG_I("default_font_size default value: %{public}d", default_font_size);
        }

        TESTLOG_I("Set default_font_size to 72.");
        g_webSettings->PutDefaultFontSize(DEFAULT_FONT_SIZE_MAX);
        default_font_size = g_webSettings->DefaultFontSize();
        if (default_font_size != DEFAULT_FONT_SIZE_MAX) {
            TESTLOG_E("Error! default_font_size :%{public}d != 72.", default_font_size);
        } else {
            TESTLOG_I("default_font_size=%{public}d", default_font_size);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test208 end");
}

void Test209()
{
    TESTLOG_I("Test209 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string default_encoding = g_webSettings->DefaultTextEncodingFormat();
        if (default_encoding != "UTF-8") {
            TESTLOG_E("Error! default_encoding default value:%{public}s != UTF-8.", default_encoding.c_str());
        } else {
            TESTLOG_I("default_encoding default value: %{public}s", default_encoding.c_str());
        }

        TESTLOG_I("Set default_encoding to gb2312.");
        g_webSettings->PutDefaultTextEncodingFormat("gb2312");
        default_encoding = g_webSettings->DefaultTextEncodingFormat();
        if (default_encoding != "gb2312") {
            TESTLOG_E("Error! default_encoding :%{public}s != gb2312.", default_encoding.c_str());
        } else {
            TESTLOG_I("default_encoding=%{public}s", default_encoding.c_str());
        }
    }

    std::stringstream ss;
    ss << "<html><body><font size=7>编码格式</font></body></html>";

    g_nweb->LoadWithDataAndBaseUrl("", ss.str(), "text/html", "", "");
    g_window->Show();

    TESTLOG_I("Test209 end");
}

void Test231()
{
    TESTLOG_I("Test231 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string default_encoding = g_webSettings->DefaultTextEncodingFormat();
        if (default_encoding != "UTF-8") {
            TESTLOG_E("Error! default_encoding default value:%{public}s != UTF-8.", default_encoding.c_str());
        } else {
            TESTLOG_I("default_encoding default value: %{public}s", default_encoding.c_str());
        }
    }

    std::stringstream ss;
    ss << "<html><body><font size=7>编码格式</font></body></html>";

    g_nweb->LoadWithDataAndBaseUrl("", ss.str(), "text/html", "", "");
    g_window->Show();

    TESTLOG_I("Test231 end");
}

void Test210()
{
    TESTLOG_I("Test210 start");

    TestPrepare();

    if (g_webSettings != nullptr) {
        g_webSettings->PutJavaScriptEnabled(true);
        g_webSettings->PutIsCreateWindowsByJavaScriptAllowed(true);

        bool dom_storage = g_webSettings->IsDomStorageEnabled();
        if (dom_storage) {
            TESTLOG_E("Error! dom_storage default value:%{public}d != false.", dom_storage);
        } else {
            TESTLOG_I("dom_storage default value: %{public}d", dom_storage);
        }

        TESTLOG_I("Set dom_storage to true.");
        g_webSettings->PutDomStorageEnabled(true);
        dom_storage = g_webSettings->IsDomStorageEnabled();
        if (!dom_storage) {
            TESTLOG_E("Error! dom_storage :%{public}d != true.", dom_storage);
        } else {
            TESTLOG_I("dom_storage=%{public}d", dom_storage);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test210 end");
}

void Test228()
{
    TESTLOG_I("Test228 start");

    TestPrepare();

    if (g_webSettings != nullptr) {
        g_webSettings->PutJavaScriptEnabled(true);
        g_webSettings->PutIsCreateWindowsByJavaScriptAllowed(true);

        bool dom_storage = g_webSettings->IsDomStorageEnabled();
        if (dom_storage) {
            TESTLOG_E("Error! dom_storage default value:%{public}d != false.", dom_storage);
        } else {
            TESTLOG_I("dom_storage default value: %{public}d", dom_storage);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test228 end");
}

void Test211()
{
    TESTLOG_I("Test211 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string fantasy_font_family = g_webSettings->FantasyFontFamilyName();
        if (fantasy_font_family != "fantasy") {
            TESTLOG_E("Error! fantasy_font_family default value:%{public}s != fantasy.", fantasy_font_family.c_str());
        } else {
            TESTLOG_I("fantasy_font_family default value: %{public}s", fantasy_font_family.c_str());
        }

        TESTLOG_I("Set fantasy_font_family to fantasy.");
        g_webSettings->PutFantasyFontFamilyName("fantasy");
        fantasy_font_family = g_webSettings->FantasyFontFamilyName();
        if (fantasy_font_family != "fantasy") {
            TESTLOG_E("Error! fantasy_font_family :%{public}s != fantasy.", fantasy_font_family.c_str());
        } else {
            TESTLOG_I("fantasy_font_family=%{public}s", fantasy_font_family.c_str());
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test211 end");
}

void Test212()
{
    TESTLOG_I("Test212 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string fixed_font_family = g_webSettings->FixedFontFamilyName();
        if (fixed_font_family != "monospace") {
            TESTLOG_E("Error! fixed_font_family default value:%{public}s != monospace.", fixed_font_family.c_str());
        } else {
            TESTLOG_I("fixed_font_family default value: %{public}s", fixed_font_family.c_str());
        }

        TESTLOG_I("Set fixed_font_family to monospace.");
        g_webSettings->PutFixedFontFamilyName("monospace");
        fixed_font_family = g_webSettings->FixedFontFamilyName();
        if (fixed_font_family != "monospace") {
            TESTLOG_E("Error! fixed_font_family :%{public}s != monospace.", fixed_font_family.c_str());
        } else {
            TESTLOG_I("fixed_font_family=%{public}s", fixed_font_family.c_str());
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test212 end");
}

void Test213()
{
    TESTLOG_I("Test213 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool force_dark_mode_enabled = g_webSettings->ForceDarkModeEnabled();
        if (force_dark_mode_enabled) {
            TESTLOG_E("Error! force_dark_mode_enabled default value:%{public}d != false.", force_dark_mode_enabled);
        } else {
            TESTLOG_I("force_dark_mode_enabled default value: %{public}d", force_dark_mode_enabled);
        }

        TESTLOG_I("Set force_dark_mode_enabled to true.");
        g_webSettings->PutForceDarkModeEnabled(true);
        force_dark_mode_enabled = g_webSettings->ForceDarkModeEnabled();
        if (!force_dark_mode_enabled) {
            TESTLOG_E("Error! force_dark_mode_enabled :%{public}d != true.", force_dark_mode_enabled);
        } else {
            TESTLOG_I("force_dark_mode_enabled=%{public}d", force_dark_mode_enabled);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test213 end");
}

void Test234()
{
    TESTLOG_I("Test234 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool dark_scheme_enabled = g_webSettings->DarkSchemeEnabled();
        if (dark_scheme_enabled) {
            TESTLOG_E("Error! dark_scheme_enabled default value:%{public}d != false.", dark_scheme_enabled);
        } else {
            TESTLOG_I("dark_scheme_enabled default value: %{public}d", dark_scheme_enabled);
        }

        TESTLOG_I("Set dark_scheme_enabled to true.");
        g_webSettings->PutDarkSchemeEnabled(true);
        dark_scheme_enabled = g_webSettings->DarkSchemeEnabled();
        if (!dark_scheme_enabled) {
            TESTLOG_E("Error! dark_scheme_enabled :%{public}d != true.", dark_scheme_enabled);
        } else {
            TESTLOG_I("dark_scheme_enabled=%{public}d", dark_scheme_enabled);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test234 end");
}

void Test214()
{
    TESTLOG_I("Test214 start");

    TestPrepare();

    if (g_webSettings != nullptr) {
        g_webSettings->PutJavaScriptEnabled(true);
        g_webSettings->PutDomStorageEnabled(true);

        bool javascript_can_open_windows_automatically = g_webSettings->IsCreateWindowsByJavaScriptAllowed();
        if (javascript_can_open_windows_automatically) {
            TESTLOG_E("Error! javascript_can_open_windows_automatically default value:%{public}d != false.",
                javascript_can_open_windows_automatically);
        } else {
            TESTLOG_I("javascript_can_open_windows_automatically default value: %{public}d",
                javascript_can_open_windows_automatically);
        }

        TESTLOG_I("Set javascript_can_open_windows_automatically to true.");
        g_webSettings->PutIsCreateWindowsByJavaScriptAllowed(true);
        javascript_can_open_windows_automatically = g_webSettings->IsCreateWindowsByJavaScriptAllowed();
        if (!javascript_can_open_windows_automatically) {
            TESTLOG_E("Error! javascript_can_open_windows_automatically :%{public}d != true.",
                javascript_can_open_windows_automatically);
        } else {
            TESTLOG_I("javascript_can_open_windows_automatically=%{public}d",
                javascript_can_open_windows_automatically);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test214 end");
}
void Test229()
{
    TESTLOG_I("Test229 start");

    TestPrepare();

    if (g_webSettings != nullptr) {
        g_webSettings->PutJavaScriptEnabled(true);
        g_webSettings->PutDomStorageEnabled(true);

        bool javascript_can_open_windows_automatically = g_webSettings->IsCreateWindowsByJavaScriptAllowed();
        if (javascript_can_open_windows_automatically) {
            TESTLOG_E("Error! javascript_can_open_windows_automatically default value:%{public}d != false.",
                javascript_can_open_windows_automatically);
        } else {
            TESTLOG_I("javascript_can_open_windows_automatically default value: %{public}d",
                javascript_can_open_windows_automatically);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test229 end");
}

void Test215()
{
    TESTLOG_I("Test215 start");

    TestPrepare();

    if (g_webSettings != nullptr) {
        g_webSettings->PutIsCreateWindowsByJavaScriptAllowed(true);
        g_webSettings->PutDomStorageEnabled(true);

        bool javascript_enabled = g_webSettings->IsJavaScriptAllowed();
        if (javascript_enabled) {
            TESTLOG_E("Error! javascript_enabled default value:%{public}d != false.", javascript_enabled);
        } else {
            TESTLOG_I("javascript_enabled default value: %{public}d", javascript_enabled);
        }

        TESTLOG_I("Set javascript_enabled to true.");
        g_webSettings->PutJavaScriptEnabled(true);
        javascript_enabled = g_webSettings->IsJavaScriptAllowed();
        if (!javascript_enabled) {
            TESTLOG_E("Error! javascript_enabled :%{public}d != true.", javascript_enabled);
        } else {
            TESTLOG_I("javascript_enabled=%{public}d", javascript_enabled);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test215 end");
}

void Test230()
{
    TESTLOG_I("Test230 start");

    TestPrepare();

    if (g_webSettings != nullptr) {
        g_webSettings->PutIsCreateWindowsByJavaScriptAllowed(true);
        g_webSettings->PutDomStorageEnabled(true);

        bool javascript_enabled = g_webSettings->IsJavaScriptAllowed();
        if (javascript_enabled) {
            TESTLOG_E("Error! javascript_enabled default value:%{public}d != false.", javascript_enabled);
        } else {
            TESTLOG_I("javascript_enabled default value: %{public}d", javascript_enabled);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test230 end");
}

void Test216()
{
    TESTLOG_I("Test216 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool loads_images_automatically = g_webSettings->IsImageLoadingAllowed();
        if (!loads_images_automatically) {
            TESTLOG_E("Error! loads_images_automatically default value:%{public}d != true.",
                loads_images_automatically);
        } else {
            TESTLOG_I("loads_images_automatically default value: %{public}d", loads_images_automatically);
        }

        TESTLOG_I("Set loads_images_automatically to false.");
        g_webSettings->PutImageLoadingAllowed(false);
        loads_images_automatically = g_webSettings->IsImageLoadingAllowed();
        if (loads_images_automatically) {
            TESTLOG_E("Error! loads_images_automatically :%{public}d != false.", loads_images_automatically);
        } else {
            TESTLOG_I("loads_images_automatically=%{public}d", loads_images_automatically);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test216 end");
}

void Test217()
{
    TESTLOG_I("Test217 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        int minimum_font_size = g_webSettings->FontSizeLowerLimit();
        if (minimum_font_size != MINI_LOGICAL_FONT_SIZE) {
            TESTLOG_E("Error! minimum_font_size default value:%{public}d != 8.", minimum_font_size);
        } else {
            TESTLOG_I("minimum_font_size default value: %{public}d", minimum_font_size);
        }

        TESTLOG_I("Set minimum_font_size to 72.");
        g_webSettings->PutFontSizeLowerLimit(DEFAULT_FONT_SIZE_MAX);
        minimum_font_size = g_webSettings->FontSizeLowerLimit();
        if (minimum_font_size != DEFAULT_FONT_SIZE_MAX) {
            TESTLOG_E("Error! minimum_font_size :%{public}d != 72.", minimum_font_size);
        } else {
            TESTLOG_I("minimum_font_size=%{public}d", minimum_font_size);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test217 end");
}

void Test218()
{
    TESTLOG_I("Test218 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        int minimum_logical_font_size = g_webSettings->LogicalFontSizeLowerLimit();
        if (minimum_logical_font_size != MINI_FONT_SIZE) {
            TESTLOG_E("Error! minimum_logical_font_size default value:%{public}d != 8.", minimum_logical_font_size);
        } else {
            TESTLOG_I("minimum_logical_font_size default value: %{public}d", minimum_logical_font_size);
        }

        TESTLOG_I("Set minimum_logical_font_size to 72.");
        g_webSettings->PutLogicalFontSizeLowerLimit(DEFAULT_FONT_SIZE_MAX);
        minimum_logical_font_size = g_webSettings->LogicalFontSizeLowerLimit();
        if (minimum_logical_font_size != DEFAULT_FONT_SIZE_MAX) {
            TESTLOG_E("Error! minimum_logical_font_size :%{public}d != 72.", minimum_logical_font_size);
        } else {
            TESTLOG_I("minimum_logical_font_size=%{public}d", minimum_logical_font_size);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test218 end");
}

void Test219()
{
    TESTLOG_I("Test219 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string sans_serif_font_family = g_webSettings->SansSerifFontFamilyName();
        if (sans_serif_font_family != "sans-serif") {
            TESTLOG_E("Error! sans_serif_font_family default value:%{public}s != sans-serif.",
                sans_serif_font_family.c_str());
        } else {
            TESTLOG_I("sans_serif_font_family default value: %{public}s", sans_serif_font_family.c_str());
        }

        TESTLOG_I("Set sans_serif_font_family to sans-serif.");
        g_webSettings->PutSansSerifFontFamilyName("sans-serif");
        sans_serif_font_family = g_webSettings->SansSerifFontFamilyName();
        if (sans_serif_font_family != "sans-serif") {
            TESTLOG_E("Error! sans_serif_font_family :%{public}s != sans-serif.", sans_serif_font_family.c_str());
        } else {
            TESTLOG_I("sans_serif_font_family=%{public}s", sans_serif_font_family.c_str());
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test219 end");
}

void Test220()
{
    TESTLOG_I("Test220 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string serif_font_family = g_webSettings->SerifFontFamilyName();
        if (serif_font_family != "serif") {
            TESTLOG_E("Error! serif_font_family default value:%{public}s != serif.", serif_font_family.c_str());
        } else {
            TESTLOG_I("serif_font_family default value: %{public}s", serif_font_family.c_str());
        }

        TESTLOG_I("Set serif_font_family to serif.");
        g_webSettings->PutSerifFontFamilyName("serif");
        serif_font_family = g_webSettings->SerifFontFamilyName();
        if (serif_font_family != "serif") {
            TESTLOG_E("Error! serif_font_family :%{public}s != serif.", serif_font_family.c_str());
        } else {
            TESTLOG_I("serif_font_family=%{public}s", serif_font_family.c_str());
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test220 end");
}

void Test221()
{
    TESTLOG_I("Test221 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string standard_font_family = g_webSettings->StandardFontFamilyName();
        if (standard_font_family != "sans-serif") {
            TESTLOG_E("Error! standard_font_family default value:%{public}s != sans-serif.",
                standard_font_family.c_str());
        } else {
            TESTLOG_I("standard_font_family default value: %{public}s", standard_font_family.c_str());
        }

        TESTLOG_I("Set standard_font_family to Times New Roman.");
        g_webSettings->PutStandardFontFamilyName("Times New Roman");
        standard_font_family = g_webSettings->StandardFontFamilyName();
        if (standard_font_family != "Times New Roman") {
            TESTLOG_E("Error! standard_font_family :%{public}s != Times New Roman.", standard_font_family.c_str());
        } else {
            TESTLOG_I("standard_font_family=%{public}s", standard_font_family.c_str());
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test221 end");
}

void Test222()
{
    TESTLOG_I("Test222 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        std::string user_agent = g_webSettings->UserAgent();
        std::string default_user_agent = g_webSettings->DefaultUserAgent();
        if (user_agent != default_user_agent) {
            TESTLOG_E("Error! user_agent default value:%{public}s != %{public}s.", user_agent.c_str(),
                default_user_agent.c_str());
        } else {
            TESTLOG_I("user_agent default value: %{public}s", user_agent.c_str());
        }

        std::string new_user_agent = "Mozilla/5.0 (Linux; LIO-AL00 Build/HUAWEILIO-AL00; wv) " \
            "AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/90.0.4430.91 Mobile Safari/537.36";
        TESTLOG_I("Set user_agent to %{public}s.", new_user_agent.c_str());
        g_webSettings->PutUserAgent(new_user_agent);
        user_agent = g_webSettings->UserAgent();
        if (user_agent != new_user_agent) {
            TESTLOG_E("Error! user_agent :%{public}s != %{public}s.", user_agent.c_str(), new_user_agent.c_str());
        } else {
            TESTLOG_I("user_agent=%{public}s", user_agent.c_str());
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test222 end");
}

void Test223()
{
    TESTLOG_I("Test223 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        int text_size_percent = g_webSettings->ZoomingForTextFactor();
        if (text_size_percent != TEXT_SIZE_PERCENT_MIN) {
            TESTLOG_E("Error! text_size_percent default value:%{public}d != 100.", text_size_percent);
        } else {
            TESTLOG_I("text_size_percent default value: %{public}d", text_size_percent);
        }

        TESTLOG_I("Set text_size_percent to 500.");
        g_webSettings->PutZoomingForTextFactor(TEXT_SIZE_PERCENT_MAX);
        text_size_percent = g_webSettings->ZoomingForTextFactor();
        if (text_size_percent != TEXT_SIZE_PERCENT_MAX) {
            TESTLOG_E("Error! text_size_percent :%{public}d != 500.", text_size_percent);
        } else {
            TESTLOG_I("text_size_percent=%{public}d", text_size_percent);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test223 end");
}

void Test224()
{
    TESTLOG_I("Test224 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool geolocation_enabled = g_webSettings->GeolocationAllowed();
        if (!geolocation_enabled) {
            TESTLOG_E("Error! geolocation_enabled default value:%{public}d != true.", geolocation_enabled);
        } else {
            TESTLOG_I("geolocation_enabled default value: %{public}d", geolocation_enabled);
        }

        TESTLOG_I("Set geolocation_enabled to false.");
        g_webSettings->PutGeolocationAllowed(false);
        geolocation_enabled = g_webSettings->GeolocationAllowed();
        if (geolocation_enabled) {
            TESTLOG_E("Error! geolocation_enabled :%{public}d != false.", geolocation_enabled);
        } else {
            TESTLOG_I("geolocation_enabled=%{public}d", geolocation_enabled);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test224 end");
}

void Test225()
{
    TESTLOG_I("Test225 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        OHOS::NWeb::NWebPreference::AccessMode mixed_content_mode =
            g_webSettings->AccessModeForSecureOriginLoadFromInsecure();
        if (mixed_content_mode != OHOS::NWeb::NWebPreference::AccessMode::NEVER_ALLOW) {
            TESTLOG_E("Error! mixed_content_mode default value:%{public}d != %{public}d.", mixed_content_mode,
                OHOS::NWeb::NWebPreference::AccessMode::NEVER_ALLOW);
        } else {
            TESTLOG_I("mixed_content_mode default value: %{public}d", mixed_content_mode);
        }

        TESTLOG_I("Set mixed_content_mode to ALWAYS_ALLOW.");
        g_webSettings->PutAccessModeForSecureOriginLoadFromInsecure(
            OHOS::NWeb::NWebPreference::AccessMode::ALWAYS_ALLOW);
        mixed_content_mode = g_webSettings->AccessModeForSecureOriginLoadFromInsecure();
        if (mixed_content_mode != OHOS::NWeb::NWebPreference::AccessMode::ALWAYS_ALLOW) {
            TESTLOG_E("Error! mixed_content_mode :%{public}d != %{public}d.", mixed_content_mode,
                OHOS::NWeb::NWebPreference::AccessMode::ALWAYS_ALLOW);
        } else {
            TESTLOG_I("mixed_content_mode=%{public}d", mixed_content_mode);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test225 end");
}

void Test226()
{
    TESTLOG_I("Test226 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        OHOS::NWeb::NWebPreference::AccessMode mixed_content_mode =
            g_webSettings->AccessModeForSecureOriginLoadFromInsecure();
        if (mixed_content_mode != OHOS::NWeb::NWebPreference::AccessMode::NEVER_ALLOW) {
            TESTLOG_E("Error! mixed_content_mode default value:%{public}d != %{public}d.", mixed_content_mode,
                OHOS::NWeb::NWebPreference::AccessMode::NEVER_ALLOW);
        } else {
            TESTLOG_I("mixed_content_mode default value: %{public}d", mixed_content_mode);
        }

        TESTLOG_I("Set mixed_content_mode to COMPATIBILITY_MODE.");
        g_webSettings->PutAccessModeForSecureOriginLoadFromInsecure(
            OHOS::NWeb::NWebPreference::AccessMode::COMPATIBILITY_MODE);
        mixed_content_mode = g_webSettings->AccessModeForSecureOriginLoadFromInsecure();
        if (mixed_content_mode != OHOS::NWeb::NWebPreference::AccessMode::COMPATIBILITY_MODE) {
            TESTLOG_E("Error! mixed_content_mode :%{public}d != %{public}d.", mixed_content_mode,
                OHOS::NWeb::NWebPreference::AccessMode::COMPATIBILITY_MODE);
        } else {
            TESTLOG_I("mixed_content_mode=%{public}d", mixed_content_mode);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test226 end");
}

void Test227()
{
    TESTLOG_I("Test227 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool support_zoom = g_webSettings->ZoomingfunctionEnabled();
        if (!support_zoom) {
            TESTLOG_E("Error! support_zoom default value:%{public}d != true.", support_zoom);
        } else {
            TESTLOG_I("support_zoom default value: %{public}d", support_zoom);
        }

        TESTLOG_I("Set support_zoom to false.");
        g_webSettings->PutZoomingFunctionEnabled(false);
        support_zoom = g_webSettings->ZoomingfunctionEnabled();
        if (support_zoom) {
            TESTLOG_E("Error! support_zoom :%{public}d != false.", support_zoom);
        } else {
            TESTLOG_I("support_zoom=%{public}d", support_zoom);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test227 end");
}

void Test232()
{
    TESTLOG_I("Test232 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool is_network_blocked = g_webSettings->IsNetworkBlocked();
        if (is_network_blocked) {
            TESTLOG_E("Error! is_network_blocked default value:%{public}d != false.", is_network_blocked);
        } else {
            TESTLOG_I("is_network_blocked default value: %{public}d", is_network_blocked);
        }

        TESTLOG_I("Set is_network_blocked to true.");
        g_webSettings->PutBlockNetwork(true);
        is_network_blocked = g_webSettings->IsNetworkBlocked();
        if (!is_network_blocked) {
            TESTLOG_E("Error! is_network_blocked :%{public}d != false.", is_network_blocked);
        } else {
            TESTLOG_I("is_network_blocked=%{public}d", is_network_blocked);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test232 end");
}

void Test233()
{
    TESTLOG_I("Test233 start");

    TestPrepare();
    TestWebSettingsForNormalWeb();

    if (g_webSettings != nullptr) {
        bool isWebDebugging = g_webSettings->IsWebDebuggingAccess();
        if (isWebDebugging) {
            TESTLOG_E("Error! isWebDebugging default value:%{public}d != false.", isWebDebugging);
        } else {
            TESTLOG_I("isWebDebugging default value: %{public}d", isWebDebugging);
        }

        TESTLOG_I("Set isWebDebugging to true.");
        g_webSettings->PutWebDebuggingAccess(true);
        isWebDebugging = g_webSettings->IsWebDebuggingAccess();
        if (!isWebDebugging) {
            TESTLOG_E("Error! isWebDebugging :%{public}d != true.", isWebDebugging);
        } else {
            TESTLOG_I("isWebDebugging=%{public}d", isWebDebugging);
        }
    }

    g_nweb->Load(g_url);
    g_window->Show();

    TESTLOG_I("Test233 end");
}

void Test90()
{
    TESTLOG_I("start90");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);
    std::thread dump_thread([]() {
        std::stringstream ss;
        ss << "change_test()";
        std::this_thread::sleep_for(std::chrono::seconds(TIME_SECONDS_FIVE));
        g_nweb->ExecuteJavaScript(ss.str());
    });
    dump_thread.detach();
    g_window->Show();
    TESTLOG_I("end90");
}

void Test91()
{
    TESTLOG_I("start91");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    g_nweb->PutBackgroundColor(0xFFFFFF33);
    g_nweb->Load(g_url);
    g_window->Show();
    TESTLOG_I("end91");
}

void Test92()
{
    TESTLOG_I("start92");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);
    g_window->Show();
    TESTLOG_I("end");
}

void Test93()
{
    TESTLOG_I("start93");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);
    std::thread dump_thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(TIME_SECONDS_FIVE));
        std::string title = g_nweb->Title();
        TESTLOG_I("url tilte =%{public}s", title.c_str());
    });
    dump_thread.detach();
    g_window->Show();
    TESTLOG_I("end93");
}

void Test94()
{
    TESTLOG_I("start94");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);
    int progress = g_nweb->PageLoadProgress();
    TESTLOG_I("progress is =%{public}d", progress);
    g_window->Show();
    TESTLOG_I("end94");
}

void Test95()
{
    TESTLOG_I("start95");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);

    std::thread dump_thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(TIME_SECONDS_FIVE));
        int content_height = g_nweb->ContentHeight();
        TESTLOG_I("Content height is =%{public}d", content_height);
    });
    dump_thread.detach();
    g_window->Show();
    TESTLOG_I("end95");
}

void Test96()
{
    TESTLOG_I("start96");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);
    float scale = g_nweb->Scale();
    TESTLOG_I("Scale is =%{public}f", scale);
    g_window->Show();
    TESTLOG_I("end96");
}

void Test97()
{
    TESTLOG_I("start97");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    std::map<std::string, std::string> extraHeaders;
    extraHeaders["Authorization"] =  "user-token-here";
    extraHeaders["diy"] = "diy";
    g_nweb->Load(g_url, extraHeaders);
    g_window->Show();
    TESTLOG_I("end97");
}

void Test98()
{
    TESTLOG_I("start98");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">Source:<pre>" << "source" << "</pre></body></html>";

    g_nweb->LoadWithDataAndBaseUrl("", ss.str(), "text/html", "UTF-8", "");
    g_window->Show();
    TESTLOG_I("end98");
}

void Test99()
{
    TESTLOG_I("start99");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">Source:<pre>" << "source" << "</pre></body></html>";

    g_nweb->LoadWithDataAndBaseUrl("", "", "text/html", "UTF-8", "http://www.baidu.com");
    float scale = g_nweb->Scale();
    TESTLOG_I("Scale is =%{public}f", scale);
    g_window->Show();
    TESTLOG_I("end99");
}

void Test100()
{
    TESTLOG_I("start100");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    std::shared_ptr<OHOS::NWeb::NWebJavaScriptResultCallBackTest> result_callback
        = std::make_shared<OHOS::NWeb::NWebJavaScriptResultCallBackTest>();
    g_nweb->SetNWebJavaScriptResultCallBack(result_callback);
    g_nweb->Load(g_url);
    std::this_thread::sleep_for(std::chrono::seconds(TIME_SECONDS_FIVE));
    std::string object_name = "methodtest";
    std::string object_name2 = "ohoh";
    std::vector<std::string> list;
    list.emplace_back("toString");
    g_nweb->RegisterArkJSfunction(object_name, list);
    result_callback->RegisterArkJSfunction(object_name);
    g_nweb->RegisterArkJSfunction(object_name2, list);
    result_callback->RegisterArkJSfunction(object_name2);
    std::thread ppthread([result_callback]() {
        std::stringstream ss;
        ss << "methodtest.toString()";
        std::this_thread::sleep_for(std::chrono::seconds(TIME_SECONDS_FIVE));
        g_nweb->ExecuteJavaScript(ss.str());
    });
    ppthread.detach();
    g_window->Show();
    TESTLOG_I("end100");
}

void Test101()
{
    TESTLOG_I("start101");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    g_nweb->Load(g_url);
    g_nweb->Zoom(0.1f);
    g_window->Show();
    TESTLOG_I("end101");
}

void Test102()
{
    TESTLOG_I("start102");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    g_nweb->Load(g_url);
    std::thread dump_thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(TIME_SECONDS_FIVE));
        g_nweb->Reload();
    });
    dump_thread.detach();
    g_window->Show();
    TESTLOG_I("end102");
}

void Test103()
{
    TESTLOG_I("start103");
    OHOS::NWeb::NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    if (!OHOS::NWeb::NWebAdapterHelper::Instance().Init(false)) {
        TESTLOG_E("fail to init NWebAdapterHelper, test end");
        return;
    }

    g_window = CreateWindow();
    if (g_window == nullptr) {
        return;
    }
    g_nweb = OHOS::NWeb::NWebAdapterHelper::Instance().CreateNWeb(g_window.GetRefPtr(), GetInitArgs());
    if (g_nweb == nullptr) {
        g_window = nullptr;
        TESTLOG_E("fail to get nweb instance, test end");
        return;
    }

    TESTLOG_I("Createnweb SUCCESS");
    g_nweb->SetNWebHandler(std::make_shared<OHOS::NWeb::NWebHandlerImplTestForIntercept>());
    std::string url = HasArg(ARG_URL) ? GetArgValue(ARG_URL) : "www.baidu.com"; // load www.baidu.com by default
    TESTLOG_I("ready to load url=%{public}s", url.c_str());

    TestWebSettingsForNormalWeb();
    g_nweb->Load(url);
    g_window->Show();
    TESTLOG_I("end103");
}

void Test104()
{
    TESTLOG_I("start104");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    std::string data = "<html><body><h1>LoadData test 104</h1></body></html>";

    g_nweb->LoadWithData(data, "text/html", "UTF-8");
    g_window->Show();
    TESTLOG_I("end104");
}

class JavaScriptResultCb : public OHOS::NWeb::NWebValueCallback<std::string> {
    void OnReceiveValue(std::string result) override
    {
        TESTLOG_I("JavaScript execute result = %{public}s", result.c_str());
    }
};

void Test105()
{
    TESTLOG_I("start105");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    g_nweb->Load(g_url);
    g_window->Show();
    std::string ss = "(function() { console.log('ExecuteJavaScript'); return 'ExecuteJavaScript'; })()";
    std::shared_ptr<OHOS::NWeb::NWebValueCallback<std::string>> callback = std::make_shared<JavaScriptResultCb>();
    g_nweb->ExecuteJavaScript(ss, callback);
    TESTLOG_I("end105");
}

void Test106()
{
    TESTLOG_I("start106");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    g_nweb->Load(g_url);
    g_window->Show();
    // load execute_javaScript_test.html, and will invoke ExecuteJavaScript when NWebHandlerImplTest::OnPageFinished
    TESTLOG_I("end106");
}

void Test107()
{
    TESTLOG_I("Test107 start");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    std::string testUrl = "https://image.baidu.com/search/down?tn=download&word=download&ie"
                          "=utf8&fr=detail&url=https%3A%2F%2Fgimg2.baidu.com%2Fimage_search%2Fsrc%3Dhttp%253A%252"
                          "F%252Fimgm3.cnmo.com%252Fuser%252Farticle%252F147422%252Forigin%252Farad9U4eIRFo.jpeg%"
                          "26refer%3Dhttp%253A%252F%252Fimgm3.cnmo.com%26app%3D2002%26size%3Df9999%2C10000%26q%3D"
                          "a80%26n%3D0%26g%3D0n%26fmt%3Djpeg%3Fsec%3D1647499762%26t%3Dee4725de24c35d3a273abb8bcfc"
                          "157e7&thumburl=https%3A%2F%2Fimg1.baidu.com%2Fit%2Fu%3D2016552768%2C2104120132%26fm%3D"
                          "253%26fmt%3Dauto%26app%3D120%26f%3DJPEG%3Fw%3D683%26h%3D393";
    TESTLOG_I("Test107 SetDownloadListener begin");
    g_nweb->PutDownloadCallback(std::make_shared<OHOS::NWeb::NWebDownloadListenerImplTest>());
    TESTLOG_I("Test107 SetDownloadListener end");
    g_nweb->Load(testUrl);
    g_window->Show();
    TESTLOG_I("end");
}

void Test108()
{
    TESTLOG_I("start108");
    TestPrepare();
    TestWebSettingsForNormalWeb();
    g_nweb->Load(g_url);
    g_window->Show();
    std::thread test_thread([]() {
        int cmd;
        while (std::cin >> cmd) {
            switch (cmd) {
                case 0:
                    TESTLOG_I("IsNavigatebackwardAllowed:%{public}d", g_nweb->IsNavigatebackwardAllowed());
                    break;
                case 1:
                    TESTLOG_I("IsNavigateForwardAllowed:%{public}d", g_nweb->IsNavigateForwardAllowed());
                    break;
                case STEP_TWO:
                    int steps;
                    std::cin >> steps;
                    TESTLOG_I("CanNavigateBackOrForward:%{public}d", g_nweb->CanNavigateBackOrForward(steps));
                    break;
                default:
                    return;
            }
        }
    });
    test_thread.detach();
    TESTLOG_I("end108");
}

void Test109()
{
    TESTLOG_I("start109");
    TestPrepareWithClient(std::make_shared<OHOS::NWeb::NWebJSDialogImplTest>(
        OHOS::NWeb::NWebJSDialogImplTest::Type::CONFIRM
    ));
    TestWebSettingsForNormalWeb();
    g_nweb->Load("file:///data/local/cef/cef_user_data/JsDiaTest.html");
    g_window->Show();
    TESTLOG_I("end109");
}

void Test110()
{
    TESTLOG_I("start110");
    TestPrepareWithClient(std::make_shared<OHOS::NWeb::NWebJSDialogImplTest>(
        OHOS::NWeb::NWebJSDialogImplTest::Type::CANCEL
    ));
    TestWebSettingsForNormalWeb();
    g_nweb->Load("file:///data/local/cef/cef_user_data/JsDiaTest.html");
    g_window->Show();
    TESTLOG_I("end110");
}

void Test111()
{
    TESTLOG_I("start111");
    TestPrepareWithClient(std::make_shared<OHOS::NWeb::NWebJSDialogImplTest>(
        OHOS::NWeb::NWebJSDialogImplTest::Type::CONFIRM
    ));
    TestWebSettingsForNormalWeb();
    g_nweb->Load("file:///data/local/cef/cef_user_data/JsBeforeUnloadTest.html");
    std::thread dump_thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(STEP_TWO));
        g_nweb->Reload();
    });
    dump_thread.detach();
    g_window->Show();
    TESTLOG_I("end111");
}

void Test112()
{
    TESTLOG_I("start112");
    TestPrepareWithClient(std::make_shared<OHOS::NWeb::NWebJSDialogImplTest>(
        OHOS::NWeb::NWebJSDialogImplTest::Type::CANCEL
    ));
    TestWebSettingsForNormalWeb();
    g_nweb->Load("file:///data/local/cef/cef_user_data/JsBeforeUnloadTest.html");
    std::thread dump_thread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(STEP_TWO));
        g_nweb->Reload();
    });
    dump_thread.detach();
    g_window->Show();
    TESTLOG_I("end112");
}

void Test113()
{
    TESTLOG_I("start113");
    TestPrepare();
    TestWebSettingsForNormalWeb();

    g_nweb->Load(g_url);
    std::thread dump_thread([]() {
        for (int i = 0; i < TEXT_SIZE_PERCENT_MIN; i++) {
            std::this_thread::sleep_for(std::chrono::seconds(TIME_SECONDS_THREE));
            OHOS::NWeb::HitTestResult data = g_nweb->GetHitTestResult();
            TESTLOG_I("type = %{public}d", data.GetType());
            TESTLOG_I("extra = %{public}s", data.GetExtra().c_str());
        }
    });
    dump_thread.detach();
    g_window->Show();
    TESTLOG_I("end113");
}

struct OhosNWebTest {
    int32_t id;
    const char *desc;
    void (*func)();
};

#define ADD_TEST(tests, id_, desc_)   \
    (tests).push_back({               \
        .id = (id_),                  \
        .desc = (desc_),              \
        .func = Test##id_ })

void InitTest(std::vector<struct OhosNWebTest> &tests)
{
    ADD_TEST(tests, 1, "nweb use window");
    ADD_TEST(tests, 90, "ExecuteJavaScript");
    ADD_TEST(tests, 91, "SetBackgroundColor(0xFFFFFF33)");
    ADD_TEST(tests, 92, "nweb handle");
    ADD_TEST(tests, 93, "gettitle");
    ADD_TEST(tests, 94, "getprogress");
    ADD_TEST(tests, 95, "GetContentHeight");
    ADD_TEST(tests, 96, "GetScale");
    ADD_TEST(tests, 97, "LoadUrl(url,extraHeaders)");
    ADD_TEST(tests, 98, "LoadWithDataAndBaseUrl");
    ADD_TEST(tests, 99, "LoadWithDataAndBaseUrl history");
    ADD_TEST(tests, 100, "ark js");
    ADD_TEST(tests, 101, "Zoomby");
    ADD_TEST(tests, 102, "reload");
    ADD_TEST(tests, 103, "nweb client ShouldInterceptRequest");
    ADD_TEST(tests, 104, "LoadwithData test");
    ADD_TEST(tests, 105, "ExecuteJavaScript with result callback, console output");
    ADD_TEST(tests, 106, "ExecuteJavaScript with result callback, change html item by js function");
    ADD_TEST(tests, 107, "Test websettings api:SetDownloadlistener");
    ADD_TEST(tests, 108, "CanGoBackOrForward");
    ADD_TEST(tests, 109, "js dialog confirm");
    ADD_TEST(tests, 110, "js dialog cancel");
    ADD_TEST(tests, 111, "js beforeUnload confirm");
    ADD_TEST(tests, 112, "js beforeUnload cancel");
    ADD_TEST(tests, 113, "GetHitTestResult");
    ADD_TEST(tests, 200, "Test websettings api:PutEnableContentAccess and EnableContentAccess");
    ADD_TEST(tests, 201, "Test websettings api:PutEnableRawFileAccess and EnableRawFileAccess");
    ADD_TEST(tests, 202, "Test websettings api:PutEnableRawFileAccessFromFileURLs and EnableRawFileAccessFromFileURLs");
    ADD_TEST(tests, 203, "Test websettings api:PutEnableUniversalAccessFromFileURLs and " \
        "EnableUniversalAccessFromFileURLs");
    ADD_TEST(tests, 204, "Test websettings api:PutLoadImageFromNetworkDisabled and IsLoadImageFromNetworkDisabled");
    ADD_TEST(tests, 205, "Test websettings api:PutCursiveFontFamilyName and CursiveFontFamilyName");
    ADD_TEST(tests, 206, "Test websettings api:PutDatabaseAllowed and IsDataBaseEnabled");
    ADD_TEST(tests, 207, "Test websettings api:PutDefaultFixedFontSize and DefaultFixedFontSize");
    ADD_TEST(tests, 208, "Test websettings api:PutDefaultFontSize and DefaultFontSize");
    ADD_TEST(tests, 209, "Test websettings api:PutDefaultTextEncodingFormat and DefaultTextEncodingFormat");
    ADD_TEST(tests, 210, "Test websettings api:PutDomStorageEnabled and IsDomStorageEnabled");
    ADD_TEST(tests, 211, "Test websettings api:PutFantasyFontFamilyName and FantasyFontFamilyName");
    ADD_TEST(tests, 212, "Test websettings api:PutFixedFontFamilyName and FixedFontFamilyName");
    ADD_TEST(tests, 213, "Test websettings api:PutForceDarkModeEnabled and ForceDarkModeEnabled");
    ADD_TEST(tests, 214, "Test websettings api:PutIsCreateWindowsByJavaScriptAllowed and " \
        "IsCreateWindowsByJavaScriptAllowed");
    ADD_TEST(tests, 215, "Test websettings api:PutJavaScriptEnabled and IsJavaScriptAllowed");
    ADD_TEST(tests, 216, "Test websettings api:PutImageLoadingAllowed and IsImageLoadingAllowed");
    ADD_TEST(tests, 217, "Test websettings api:PutFontSizeLowerLimit and FontSizeLowerLimit");
    ADD_TEST(tests, 218, "Test websettings api:PutLogicalFontSizeLowerLimit and LogicalFontSizeLowerLimit");
    ADD_TEST(tests, 219, "Test websettings api:PutSansSerifFontFamilyName and SansSerifFontFamilyName");
    ADD_TEST(tests, 220, "Test websettings api:PutSerifFontFamilyName and SerifFontFamilyName");
    ADD_TEST(tests, 221, "Test websettings api:PutStandardFontFamilyName and StandardFontFamilyName");
    ADD_TEST(tests, 222, "Test websettings api:PutUserAgent and UserAgent");
    ADD_TEST(tests, 223, "Test websettings api:PutZoomingForTextFactor and ZoomingForTextFactor");
    ADD_TEST(tests, 224, "Test websettings api:PutGeolocationAllowed and GeolocationAllowed");
    ADD_TEST(tests, 225, "Test websettings api:PutAccessModeForSecureOriginLoadFromInsecure and " \
        "AccessModeForSecureOriginLoadFromInsecure");
    ADD_TEST(tests, 226, "Test websettings api:PutAccessModeForSecureOriginLoadFromInsecure and " \
        "AccessModeForSecureOriginLoadFromInsecure");
    ADD_TEST(tests, 227, "Test websettings api:PutZoomingFunctionEnabled and ZoomingfunctionEnabled");
    ADD_TEST(tests, 228, "Test websettings api:PutDomStorageEnabled and IsDomStorageEnabled");
    ADD_TEST(tests, 229, "Test websettings api:PutIsCreateWindowsByJavaScriptAllowed and " \
        "IsCreateWindowsByJavaScriptAllowed");
    ADD_TEST(tests, 230, "Test websettings api:PutJavaScriptEnabled and IsJavaScriptAllowed");
    ADD_TEST(tests, 231, "Test websettings api:PutDefaultTextEncodingFormat and DefaultTextEncodingFormat");
    ADD_TEST(tests, 232, "Test websettings api:PutBlockNetwork and IsNetworkBlocked");
    ADD_TEST(tests, 233, "Test websettings api:PutWebDebuggingAccess and IsWebDebuggingAccess");
    ADD_TEST(tests, 234, "Test websettings api:PutDarkSchemeEnabled and DarkSchemeEnabled");
}

void Usage(const char *argv0, const std::vector<struct OhosNWebTest> &tests)
{
    TESTLOG_I("Usage: %{public}s test_id", argv0);
    for (auto it = tests.begin(); it != tests.end(); it++) {
        TESTLOG_I("test %{public}d: %{public}s", it->id, it->desc);
    }
}
} // anonymous namespace

int32_t main(int32_t argc, const char * const argv[])
{
    TESTLOG_I("ohos nweb test start");

    std::vector<struct OhosNWebTest> tests;
    InitTest(tests);

    if (argc <= 1) {
        Usage(argv[0], tests);
        return 0;
    }

    int32_t testcase = -1;
    int ret = sscanf_s(argv[1], "%d", &testcase);
    if (ret == 0) {
        Usage(argv[0], tests);
        return 1;
    }

    InitArgs(argc, argv);

    auto runner = AppExecFwk::EventRunner::Create(false);
    g_handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    if (g_handler == nullptr) {
        TESTLOG_E("fail to new g_handler.");
        return 0;
    }
    auto condition = [testcase] (auto &item) { return item.id == testcase; };
    auto test = std::find_if(tests.begin(), tests.end(), condition);
    if (test != tests.end()) {
        g_handler->PostTask(test->func);
        TESTLOG_I("%{public}d %{public}s run!", test->id, test->desc);
    } else {
        TESTLOG_E("not found test %{public}d", testcase);
        return 0;
    }

    runner->Run();
    return 0;
}

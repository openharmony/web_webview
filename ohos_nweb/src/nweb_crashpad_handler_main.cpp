/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
#include <string>
#include <string_view>
#include <optional>

#include "nweb_log.h"

namespace {
#if defined(webview_arm64)
const std::string CRASHPAD_HANDLER_PATH = "/data/storage/el1/bundle/nweb/libs/arm64";
#elif defined(webview_x86_64)
const std::string CRASHPAD_HANDLER_PATH = "/data/storage/el1/bundle/nweb/libs/x86_64";
#elif defined(webview_arm)
const std::string CRASHPAD_HANDLER_PATH = "/data/storage/el1/bundle/nweb/libs/arm";
#else
const std::string CRASHPAD_HANDLER_PATH = "unsupport";
#endif

const std::string LIB_CRASHPAD_HANDLER = "libchrome_crashpad_handler.so";
}

std::optional<std::string> GetEngineType(int argc, char* argv[])
{
    constexpr std::string_view prefix = "--engine-type=";
    for (int i = 0; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg.size() > prefix.size() && arg.substr(0, prefix.size()) == prefix) {
            return std::string(arg.substr(prefix.size()));
        }
    }
    return std::nullopt;
}

int main(int argc, char* argv[])
{
    std::string libCrashpadHandler;
    std::string webPath = WEBVIEW_SANDBOX_LIB_PATH;
#if defined(IS_ASAN) && defined(webview_arm64)
    if (access(std::string(WEBVIEW_SANDBOX_LIB_PATH_ASAN).c_str(), F_OK) == 0) {
        webPath = WEBVIEW_SANDBOX_LIB_PATH_ASAN;
    }
#endif
    if (auto engineType = GetEngineType(argc, argv); engineType.has_value() && engineType.value() == "LEGACY") {
        WVLOG_I("crashpad handler start from legacy");
        libCrashpadHandler = std::string(LEGACY_WEBVIEW_SANDBOX_LIB_PATH) + "/"
                                            + std::string(WEBVIEW_CRASHPAD_HANDLER_SO);
    } else {
        WVLOG_I("crashpad handler start from default");
        libCrashpadHandler = webPath + "/" + std::string(WEBVIEW_CRASHPAD_HANDLER_SO);
    }

    Dl_namespace dlns;
    dlns_init(&dlns, "nweb_ns");
    dlns_create(&dlns, webPath.c_str());

    Dl_namespace ndkns;
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");

    void *handle = dlopen_ns(&dlns, libCrashpadHandler.c_str(), RTLD_NOW | RTLD_GLOBAL);
    
    if (handle == nullptr) {
        const std::string libCrashpadHandler2 = CRASHPAD_HANDLER_PATH + "/" + LIB_CRASHPAD_HANDLER;
        handle = dlopen(libCrashpadHandler2.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (handle == nullptr) {
            WVLOG_E("crashpad, fail to dlopen %{public}s, errmsg=%{public}s", libCrashpadHandler2.c_str(), dlerror());
            return -1;
        }
    }
    int ret = 0;
    using FuncType = int (*)(int argc, char* argv[]);
    FuncType crashpadHandlerFunc = reinterpret_cast<FuncType>(dlsym(handle, "CrashpadHandlerMain"));
    if (crashpadHandlerFunc == nullptr) {
        WVLOG_E("crashpad, fail to dlsym CrashpadHandlerMain, errmsg=%{public}s", dlerror());
        ret = dlclose(handle);
        if (ret != 0) {
            WVLOG_E("crashpad, fail to dlclose, errmsg=%{public}s", dlerror());
        }
        return -1;
    }

    WVLOG_I("crashpad, success to dlopen and dlsym, enter CrashpadHandlerMain");
    ret = crashpadHandlerFunc(argc, argv);
    if (ret != 0) {
        WVLOG_E("crashpad dump failed, CrashpadHandlerMain return: %{public}d", ret);
        _exit(1);
    }
    _exit(0);
}

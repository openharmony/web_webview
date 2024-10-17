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

int main(int argc, char* argv[])
{
    const std::string libCrashpadHandler = CRASHPAD_HANDLER_PATH + "/" + LIB_CRASHPAD_HANDLER;
    void *handle = dlopen(libCrashpadHandler.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (handle == nullptr) {
        WVLOG_E("crashpad, fail to dlopen %{public}s, errmsg=%{public}s", libCrashpadHandler.c_str(), dlerror());
        return -1;
    }

    using FuncType = int (*)(int argc, char* argv[]);
    FuncType crashpadHandlerFunc = reinterpret_cast<FuncType>(dlsym(handle, "CrashpadHandlerMain"));
    if (crashpadHandlerFunc == nullptr) {
        WVLOG_E("crashpad, fail to dlsym CrashpadHandlerMain, errmsg=%{public}s", dlerror());
        int ret = dlclose(handle);
        if (ret != 0) {
            WVLOG_E("crashped, fail to dlclose, errmsg=%{public}s", dlerror());
        }
        return -1;
    }

    WVLOG_I("crashpad, success to dlopen and dlsym, enter CrashpadHandlerMain");
    return crashpadHandlerFunc(argc, argv);
}

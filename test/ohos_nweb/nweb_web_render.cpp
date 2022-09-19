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

#include <unistd.h>
#include <dlfcn.h>

#include <iosfwd>
#include <ostream>
#include <sstream>
#include <string>

#include "hilog/log_cpp.h"
#include "nweb_test_log.h"

using namespace OHOS;

int main(int argc, const char* argv[])
{
    TESTLOG_I("nweb subprocess %{public}s begin, pid=%{public}d", argv[0], getpid());
#ifdef __MUSL__
    Dl_namespace dlns;
    dlns_init(&dlns, "nweb_test_ns");
    dlns_create(&dlns, "/data/app/el1/bundle/public/com.ohos.nweb/libs/arm");
    const std::string LIB_PATH_NWEB_RENDER = "libnweb_render.so";
    const std::string LIB_PATH_WEB_ENGINE = "libweb_engine.so";
    void *libHandleWebEngine = dlopen_ns(&dlns, LIB_PATH_WEB_ENGINE.c_str(), RTLD_NOW);
    if (libHandleWebEngine == nullptr) {
        TESTLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", LIB_PATH_WEB_ENGINE.c_str(), dlerror());
        return -1;
    }
    void *libHandleNWebRender = dlopen_ns(&dlns, LIB_PATH_NWEB_RENDER.c_str(), RTLD_NOW);
    if (libHandleNWebRender == nullptr) {
        TESTLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", LIB_PATH_NWEB_RENDER.c_str(), dlerror());
        return -1;
    }
#else
    const std::string LOAD_LIB_DIR_INSTALLATION = "/data/app/el1/bundle/public/com.ohos.nweb/libs/arm";
    const std::string LIB_PATH_WEB_ENGINE = LOAD_LIB_DIR_INSTALLATION + "/libweb_engine.so";
    const std::string LIB_PATH_NWEB_RENDER = LOAD_LIB_DIR_INSTALLATION + "/libnweb_render.so";
    void *libHandleWebEngine = ::dlopen(LIB_PATH_WEB_ENGINE.c_str(), RTLD_NOW);
    if (libHandleWebEngine == nullptr) {
        TESTLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", LIB_PATH_WEB_ENGINE.c_str(), dlerror());
        return -1;
    }
    void *libHandleNWebRender = ::dlopen(LIB_PATH_NWEB_RENDER.c_str(), RTLD_NOW);
    if (libHandleNWebRender == nullptr) {
        TESTLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", LIB_PATH_NWEB_RENDER.c_str(), dlerror());
        return -1;
    }
#endif
    using NWebRenderMainFunc = void (*)(const char* args);
    const std::string NWEB_RENDER_MAIN = "NWebRenderMain";
    NWebRenderMainFunc func =
            reinterpret_cast<NWebRenderMainFunc>(dlsym(libHandleNWebRender, NWEB_RENDER_MAIN.c_str()));
    if (func == nullptr) {
        TESTLOG_E("fail to dlsym %{public}s from libnweb_render.so", NWEB_RENDER_MAIN.c_str());
        return -1;
    }
    std::stringstream argv_ss;
    const char separator = '#';
    for (int i = 0; i < argc - 1; ++i) {
        argv_ss << argv[i] << separator;
    }
    argv_ss << argv[argc - 1];
    func(argv_ss.str().data());
    if (libHandleWebEngine != nullptr) {
        ::dlclose(libHandleWebEngine);
        libHandleWebEngine = nullptr;
    }
    if (libHandleNWebRender != nullptr) {
        ::dlclose(libHandleNWebRender);
        libHandleNWebRender = nullptr;
    }

    TESTLOG_I("nweb subprocess %{public}s end, pid=%{public}d", argv[0], getpid());
}

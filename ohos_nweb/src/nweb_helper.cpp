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

#include "nweb_helper.h"

#include <iostream>
#include <thread>
#include <dlfcn.h>
#include "nweb_log.h"
#include "nweb_adapter_helper.h"
#include "nweb_window_adapter.h"
#include "nweb_surface_adapter.h"

namespace {
const uint32_t NWEB_SURFACE_MAX_WIDTH = 7680;
const uint32_t NWEB_SURFACE_MAX_HEIGHT = 7680;
const std::string RELATIVE_PATH_FOR_MOCK = "libs/arm";
const std::string RELATIVE_PATH_FOR_BUNDLE = "nweb/libs/arm";
const std::string LIB_NAME_WEB_ENGINE = "libweb_engine.so";
const std::string LIB_NAME_NWEB_ADAPTER = "libnweb_adapter.so";
}

namespace  OHOS::NWeb {
NWebHelper &NWebHelper::Instance()
{
    static NWebHelper helper;
    return helper;
}

#ifdef __MUSL__
bool NWebHelper::LoadLib(bool from_ark)
{
    if (libHandleNWebAdapter_ != nullptr && libHandleWebEngine_ != nullptr) {
        return true;
    }
    if (bundlePath_.empty()) {
        return false;
    }
    std::string loadLibPath;
    if (from_ark) {
        loadLibPath = bundlePath_ + "/" + RELATIVE_PATH_FOR_BUNDLE;
    } else {
        loadLibPath = bundlePath_ + "/" + RELATIVE_PATH_FOR_MOCK;
    }
    Dl_namespace dlns;
    dlns_init(&dlns, "nweb_ns");
    dlns_create(&dlns, loadLibPath.c_str());
    void *libHandleWebEngine = dlopen_ns(&dlns, LIB_NAME_WEB_ENGINE.c_str(), RTLD_NOW);
    if (libHandleWebEngine == nullptr) {
        WVLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", LIB_NAME_WEB_ENGINE.c_str(), dlerror());
        return false;
    }
    libHandleWebEngine_ = libHandleWebEngine;
    void *libHandleNWebAdapter = dlopen_ns(&dlns, LIB_NAME_NWEB_ADAPTER.c_str(), RTLD_NOW);
    if (libHandleNWebAdapter == nullptr) {
        WVLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", LIB_NAME_NWEB_ADAPTER.c_str(), dlerror());
        return false;
    }
    libHandleNWebAdapter_ = libHandleNWebAdapter;
    return true;
}
#else
bool NWebHelper::LoadLib(bool from_ark)
{
    if (libHandleNWebAdapter_ != nullptr && libHandleWebEngine_ != nullptr) {
        return true;
    }
    if (bundlePath_.empty()) {
        return false;
    }
    std::string loadLibPath;
    if (from_ark) {
        loadLibPath = bundlePath_ + "/" + RELATIVE_PATH_FOR_BUNDLE;
    } else {
        loadLibPath = bundlePath_ + "/" + RELATIVE_PATH_FOR_MOCK;
    }
    const std::string libPathWebEngine = loadLibPath + "/" + LIB_NAME_WEB_ENGINE;
    const std::string libPathWebAdapter = loadLibPath + "/" + LIB_NAME_NWEB_ADAPTER;
    void *libHandleWebEngine = ::dlopen(libPathWebEngine.c_str(), RTLD_NOW);
    if (libHandleWebEngine == nullptr) {
        WVLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", libPathWebEngine.c_str(), dlerror());
        return false;
    }
    libHandleWebEngine_ = libHandleWebEngine;
    void *libHandleNWebAdapter = ::dlopen(libPathWebAdapter.c_str(), RTLD_NOW);
    if (libHandleNWebAdapter == nullptr) {
        WVLOG_E("fail to dlopen %{public}s, errmsg=%{public}s", libPathWebAdapter.c_str(), dlerror());
        return false;
    }
    libHandleNWebAdapter_ = libHandleNWebAdapter;
    return true;
}
#endif

void NWebHelper::UnloadLib()
{
    if (libHandleNWebAdapter_ != nullptr) {
        ::dlclose(libHandleNWebAdapter_);
        libHandleNWebAdapter_ = nullptr;
    }
    if (libHandleWebEngine_ != nullptr) {
        ::dlclose(libHandleWebEngine_);
        libHandleWebEngine_ = nullptr;
    }
}

bool NWebHelper::Init(bool from_ark)
{
    return LoadLib(from_ark);
}

void NWebHelper::SetBundlePath(const std::string& path)
{
    bundlePath_ = path;
}

NWebHelper::~NWebHelper()
{
    UnloadLib();
}

using CreateNWebFuncType = NWeb *(*)(const NWebCreateInfo &);
std::shared_ptr<NWeb> NWebHelper::CreateNWeb(const NWebCreateInfo &create_info)
{
    if (libHandleNWebAdapter_ == nullptr) {
        return nullptr;
    }

    const std::string CREATE_NWEB_FUNC_NAME = "CreateNWeb";
    CreateNWebFuncType funcCreateNWeb =
        reinterpret_cast<CreateNWebFuncType>(dlsym(libHandleNWebAdapter_, CREATE_NWEB_FUNC_NAME.c_str()));
    if (funcCreateNWeb == nullptr) {
        WVLOG_E("fail to dlsym %{public}s from libohoswebview.so", CREATE_NWEB_FUNC_NAME.c_str());
        return nullptr;
    }

    std::shared_ptr<NWeb> nweb(funcCreateNWeb(create_info));
    if (nweb == nullptr) {
        WVLOG_E("fail to create nweb");
        return nullptr;
    }

    return nweb;
}

NWebAdapterHelper &NWebAdapterHelper::Instance()
{
    static NWebAdapterHelper helper;
    return helper;
}

bool NWebAdapterHelper::Init(bool from_ark)
{
    return NWebHelper::Instance().Init(from_ark);
}

std::shared_ptr<NWeb> NWebAdapterHelper::CreateNWeb(Rosen::Window *window, const NWebInitArgs &initArgs)
{
    if (window == nullptr) {
        WVLOG_E("fail to create nweb, input window is nullptr");
        return nullptr;
    }
    auto createInfo = NWebWindowAdapter::Instance().GetCreateInfo(window, initArgs);
    auto nweb = NWebHelper::Instance().CreateNWeb(createInfo);
    if (nweb == nullptr) {
        WVLOG_E("fail to create nweb instance");
        return nullptr;
    }
    NWebWindowAdapter::Instance().RegistEventCb(window, nweb);
    NWebWindowAdapter::Instance().RequestVsync(window, nweb);
    return nweb;
}

std::shared_ptr<NWeb> NWebAdapterHelper::CreateNWeb(sptr<Surface> surface,
                                                    const NWebInitArgs &initArgs,
                                                    uint32_t width,
                                                    uint32_t height)
{
    if (surface == nullptr) {
        WVLOG_E("fail to create nweb, input surface is nullptr");
        return nullptr;
    }
    if (width > NWEB_SURFACE_MAX_WIDTH || height > NWEB_SURFACE_MAX_HEIGHT) {
        WVLOG_E("input size %{public}u*%{public}u is invalid.", width, height);
        return nullptr;
    }
    auto createInfo = NWebSurfaceAdapter::Instance().GetCreateInfo(surface, initArgs, width, height);
    auto nweb = NWebHelper::Instance().CreateNWeb(createInfo);
    if (nweb == nullptr) {
        WVLOG_E("fail to create nweb instance");
    }
    return nweb;
}
} // namespace OHOS::NWeb

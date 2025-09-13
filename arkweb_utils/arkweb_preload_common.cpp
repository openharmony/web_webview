/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "arkweb_preload_common.h"
#include "arkweb_utils.h"
#include "nweb_log.h"
#include "parameters.h"
#include <dlfcn.h>
#ifndef webview_x86_64
#include <sys/sysinfo.h>
#endif

namespace OHOS::ArkWeb {

#if defined(webview_arm64)
const std::string OHOS_ADAPTER_GLUE_SRC_LIB_PATH = "/system/lib64/libohos_adapter_glue_source.z.so";
#elif defined(webview_x86_64)
const std::string OHOS_ADAPTER_GLUE_SRC_LIB_PATH = "";
#else
const std::string OHOS_ADAPTER_GLUE_SRC_LIB_PATH = "/system/lib/libohos_adapter_glue_source.z.so";
#endif

const std::string ARK_WEB_ENGINE_LIB_NAME = "libarkweb_engine.so";
const int32_t RAM_SIZE_8G = 8;
const int32_t SIZE_KB = 1024;

enum class RenderPreLoadMode {
    PRELOAD_NO = 0,         // 不预加载
    PRELOAD_PARTIAL = 1,    // 只预加载libohos_adapter_glue_source.z.so
    PRELOAD_FULL = 2        // 预加载libohos_adapter_glue_source.z.so和libarkweb_engine.so
};

std::string GetArkwebBundleInstallLibPath() {
    std::string bundleName = OHOS::system::GetParameter("persist.arkwebcore.package_name", "");
    if (bundleName.empty()) {
        WVLOG_E("Fail to get persist.arkwebcore.package_name");
        return "";
    }
    if (getActiveWebEngineType() == ArkWebEngineType::LEGACY) {
        bundleName += "legacy";
    }
#ifdef webview_arm64
    const std::string arkwebEngineLibPath = "/data/app/el1/bundle/public/" + std::string(bundleName) + "/libs/arm64";
#elif webview_x86_64
    const std::string arkwebEngineLibPath = "";
#else
    const std::string arkwebEngineLibPath = "/data/app/el1/bundle/public/" + std::string(bundleName) + "/libs/arm";
#endif
    return arkwebEngineLibPath;
}
 
std::string GetOhosAdptGlueSrcLibPath() {
    return OHOS_ADAPTER_GLUE_SRC_LIB_PATH;
}

static void PreloadArkWebEngineLib()
{
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, "nweb_ns");
    const std::string arkWebEngineLibPath = GetArkwebBundleInstallLibPath();
    const std::string arkwebEngineSandboxLibPath = GetArkwebLibPath();
    if (arkWebEngineLibPath.empty()) {
        WVLOG_E("Fail to get libarkweb_engine.so path");
        return;
    }
    std::string libNsPath = arkWebEngineLibPath + ":" + arkwebEngineSandboxLibPath;
    dlns_create(&dlns, libNsPath.c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    void *webEngineHandle = dlopen_ns(&dlns, ARK_WEB_ENGINE_LIB_NAME.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (webEngineHandle == nullptr) {
        WVLOG_E("Fail to dlopen libarkweb_engine.so, errno: %{public}d", errno);
    }
}
 
static void PreloadOHOSAdptGlueSrcLib()
{
    const std::string ohosAdptGlueSrcLibPath = OHOS::ArkWeb::GetOhosAdptGlueSrcLibPath();
    if (ohosAdptGlueSrcLibPath.empty()) {
        WVLOG_E("Fail to get libohos_adapter_glue_source.z.so path");
        return;
    }
    void *ohosAdptGlueSrcHandle = dlopen(ohosAdptGlueSrcLibPath.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (ohosAdptGlueSrcHandle == nullptr) {
        WVLOG_E("Fail to dlopen libohos_adapter_glue_source.z.so, errno: %{public}d", errno);
    }
}

static int32_t GetSysTotalRAMInfo()
{
#if defined(webview_x86_64) || defined(ASAN_DETECTOR)
    return 0;
#else
    struct sysinfo sysInfo;
    if (sysinfo(&sysInfo) != 0) {
        return 0;
    }
    int64_t memSize = static_cast<int64_t>(sysInfo.totalram);
    return ceil(memSize / (SIZE_KB * SIZE_KB));
#endif
}

static int GetRenderPreLoadMode(const int32_t &ramSize)
{
#if defined(PRELOAD_RENDER_LIB) && (!defined(ASAN_DETECTOR))
    return static_cast<int>(RenderPreLoadMode::PRELOAD_FULL);
#endif
    int preloadMode = OHOS::system::GetIntParameter("const.startup.nwebspawn.preloadMode", 0);
    if (preloadMode == static_cast<int>(RenderPreLoadMode::PRELOAD_NO)) {
        return preloadMode;
    }
    if (ramSize <= RAM_SIZE_8G * SIZE_KB) {
        return static_cast<int>(RenderPreLoadMode::PRELOAD_NO);
    }
    return static_cast<int>(RenderPreLoadMode::PRELOAD_PARTIAL);
}

static void PreloadArkWebLibForRender(const int &preloadMode)
{
    if (preloadMode == static_cast<int>(RenderPreLoadMode::PRELOAD_PARTIAL)) {
        PreloadOHOSAdptGlueSrcLib();
    }
    if (preloadMode == static_cast<int>(RenderPreLoadMode::PRELOAD_FULL)) {
        PreloadOHOSAdptGlueSrcLib();        
        PreloadArkWebEngineLib();
    }
}
 
void PreloadArkWebLibForRender()
{
#ifdef ASAN_DETECTOR
    return;
#endif
    const int32_t ramSize = GetSysTotalRAMInfo();
    const int preloadMode = GetRenderPreLoadMode(ramSize);
    WVLOG_I("NwebSpawn preload render lib mode: %{public}d", preloadMode);
    PreloadArkWebLibForRender(preloadMode);
}
} // namespace OHOS::ArkWeb
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
#include "arkweb_utils.h"

#include "parameters.h"
#include "nweb_log.h"
#include "json/json.h"
#include <cerrno>
#include <cstring>
#include <dlfcn.h>
#include <fstream>

namespace OHOS::ArkWeb {

static int g_appEngineVersion = static_cast<int>(ArkWebEngineVersion::SYSTEM_DEFAULT);
static bool g_webEngineInitFlag = false;

#if defined(webview_arm64)
const std::string ARK_WEB_CORE_MOCK_HAP_LIB_PATH =
    "/data/storage/el1/bundle/libs/arm64";
const std::string ARK_WEB_CORE_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcore/libs/arm64";
const std::string ARK_WEB_CORE_LEGACY_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcorelegacy/libs/arm64";
const std::string ARK_WEB_CORE_PATH_FOR_MOCK = "libs/arm64";
const std::string ARK_WEB_CORE_PATH_FOR_BUNDLE = "arkwebcore/libs/arm64";
const std::string ARK_WEB_CORE_LEGACY_PATH_FOR_BUNDLE = "arkwebcorelegacy/libs/arm64";
#elif defined(webview_x86_64)
const std::string ARK_WEB_CORE_MOCK_HAP_LIB_PATH =
    "/data/storage/el1/bundle/libs/x86_64";
const std::string ARK_WEB_CORE_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcore/libs/x86_64";
const std::string ARK_WEB_CORE_LEGACY_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcorelegacy/libs/x86_64";
const std::string ARK_WEB_CORE_PATH_FOR_MOCK = "libs/x86_64";
const std::string ARK_WEB_CORE_PATH_FOR_BUNDLE = "arkwebcore/libs/x86_64";
const std::string ARK_WEB_CORE_LEGACY_PATH_FOR_BUNDLE = "arkwebcorelegacy/libs/x86_64";
#else
const std::string ARK_WEB_CORE_MOCK_HAP_LIB_PATH =
    "/data/storage/el1/bundle/libs/arm";
const std::string ARK_WEB_CORE_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcore/libs/arm";
const std::string ARK_WEB_CORE_LEGACY_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcorelegacy/libs/arm";
const std::string ARK_WEB_CORE_PATH_FOR_MOCK = "libs/arm";
const std::string ARK_WEB_CORE_PATH_FOR_BUNDLE = "arkwebcore/libs/arm";
const std::string ARK_WEB_CORE_LEGACY_PATH_FOR_BUNDLE = "arkwebcorelegacy/libs/arm";
#endif

#if defined(IS_ASAN)
#if defined(webview_arm64)
const std::string ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE = "arkwebcore_asan/libs/arm64";
const std::string WEBVIEW_RELATIVE_SANDBOX_PATH_FOR_LIBRARY =
                    "data/storage/el1/bundle/arkwebcore_asan/libs/arm64/libarkweb_engine.so";
const std::string ARK_WEB_CORE_HAP_LIB_PATH_ASAN = "/data/storage/el1/bundle/arkwebcore_asan/libs/arm64";
#elif defined(webview_x86_64)
const std::string ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE = "arkwebcore_asan/libs/x86_64";
#else
const std::string ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE = "arkwebcore_asan/libs/arm";
#endif
#endif

const std::string PRECONFIG_LEGACY_HAP_PATH = "/system/app/ArkWebCoreLegacy/ArkWebCoreLegacy.hap";
const std::string  PRECONFIG_EVERGREEN_HAP_PATH =
    "/system/app/com.ohos.arkwebcore/ArkWebCore.hap";
const std::string PRECONFIG_EVERGREEN_WATCH_HAP_PATH =
    "/system/app/NWeb/NWeb.hap";
const std::string SANDBOX_LEGACY_HAP_PATH = "/data/storage/el1/bundle/arkwebcorelegacy/entry.hap";
const std::string SANDBOX_EVERGREEN_HAP_PATH = "/data/storage/el1/bundle/arkwebcore/entry.hap";

const std::string JSON_CONFIG_PATH =
    "/data/service/el1/public/update/param_service/install/system/etc/ArkWebSafeBrowsing/generic/ArkWebCoreCfg.json";
const std::string WEB_PARAM_PREFIX = "web.engine.";

static bool validateSpecialParams(const std::string& key, int value)
{
    if (key == "web.engine.default") {
        if (value != static_cast<int>(ArkWebEngineType::EVERGREEN) &&
            value != static_cast<int>(ArkWebEngineType::LEGACY)) {
            WVLOG_E("Invalid value for %{public}s: %{public}d, must be EVERGREEN or LEGACY",
                key.c_str(), value);
            return false;
        }
    } 
    else if (key == "web.engine.enforce") {
        if (value != static_cast<int>(ArkWebEngineType::EVERGREEN)) {
            WVLOG_E("Invalid value for %{public}s: %{public}d, must be EVERGREEN", 
                key.c_str(), value);
            return false;
        }
    }
    return true;
}

static void processJsonConfig(const Json::Value& root)
{
    if (!root.isObject()) {
        WVLOG_E("Not a JSON object");
        return;
    }

    Json::Value::Members keys = root.getMemberNames();
    for (const auto& key : keys) {
        const Json::Value& value = root[key];
        if (!value.isInt() || key.rfind(WEB_PARAM_PREFIX, 0) != 0) {
            WVLOG_E("Invalid param %{public}s", key.c_str());
            continue;
        }

        int intValue = value.asInt();
        
        // 验证特殊参数
        if (!validateSpecialParams(key, intValue)) {
            continue;  // 跳过非法值
        }

        // 设置有效参数
        std::string valueStr = std::to_string(intValue);
        if (OHOS::system::SetParameter(key, valueStr)) {
            WVLOG_I("Set param %{public}s with %{public}s", key.c_str(), valueStr.c_str());
        } else {
            WVLOG_E("Set param %{public}s with %{public}s failed", key.c_str(), valueStr.c_str());
        }
    }
}

static void updateCfgToSystemParam()
{
    std::ifstream jsonFile(JSON_CONFIG_PATH.c_str());
    if (!jsonFile.is_open()) {
        WVLOG_E("Failed to open file reason: %{public}s", strerror(errno));
        return;
    }

    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string parseErrors;
    bool parsingSuccessful = Json::parseFromStream(readerBuilder, jsonFile, &root, &parseErrors);

    if (!parsingSuccessful) {
        WVLOG_E("JSON parse failed, parseErrors:%{public}s", parseErrors.c_str());
        return;
    }

    processJsonConfig(root);
}

void PreloadArkWebLibForBrowser()
{
    updateCfgToSystemParam();
    if (!(access(PRECONFIG_LEGACY_HAP_PATH.c_str(), F_OK) == 0)) {
        if (OHOS::system::SetParameter("web.engine.enforce",
                std::to_string(static_cast<int>(ArkWebEngineType::EVERGREEN)))) {
            WVLOG_I("Set param web.engine.enforce with %{public}d", static_cast<int>(ArkWebEngineType::EVERGREEN));
        } else {
            WVLOG_I("Set param web.engine.enforce with %{public}d failed",
                    static_cast<int>(ArkWebEngineType::EVERGREEN));
        }
    }
    return;
}

void setActiveWebEngineVersion(ArkWebEngineVersion version)
{
    if (g_webEngineInitFlag) {
        WVLOG_E("library resources have been loaded, can't set appEngineVersion");
        return;
    }

    if (static_cast<int>(version) != static_cast<int>(ArkWebEngineType::LEGACY) &&
      static_cast<int>(version) != static_cast<int>(ArkWebEngineType::EVERGREEN) &&
      static_cast<int>(version) != static_cast<int>(ArkWebEngineVersion::SYSTEM_DEFAULT) &&
      static_cast<int>(version) != static_cast<int>(ArkWebEngineVersion::SYSTEM_EVERGREEN)) {
        WVLOG_I("set EngineVersion not support, setVersion: %{public}d", static_cast<int>(version));
        return;
    }
    g_appEngineVersion = static_cast<int>(version);
    WVLOG_I("set appEngineVersion: %{public}d", g_appEngineVersion);
}

ArkWebEngineVersion getAppWebEngineVersion()
{
    return static_cast<ArkWebEngineVersion>(g_appEngineVersion);
}

ArkWebEngineVersion getActiveWebEngineVersion()
{
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    if (webEngineEnforce == static_cast<int>(ArkWebEngineType::EVERGREEN)) {
        WVLOG_I("WebEngineVersionResult, enforce EVERGREEN");
        return static_cast<ArkWebEngineVersion>(ArkWebEngineType::EVERGREEN);
    }

    if (g_appEngineVersion != static_cast<int>(ArkWebEngineVersion::SYSTEM_DEFAULT)) {
        WVLOG_I("get appEngineVersion: %{public}d", g_appEngineVersion);
        if (g_appEngineVersion == static_cast<int>(ArkWebEngineVersion::SYSTEM_EVERGREEN)) {
            return static_cast<ArkWebEngineVersion>(ArkWebEngineType::EVERGREEN);
        }
        return static_cast<ArkWebEngineVersion>(g_appEngineVersion);
    }

    int webEngineDefault = OHOS::system::GetIntParameter("web.engine.default",
        static_cast<int>(ArkWebEngineType::EVERGREEN));
    if (webEngineDefault != static_cast<int>(ArkWebEngineType::LEGACY) &&
      webEngineDefault != static_cast<int>(ArkWebEngineType::EVERGREEN)) {
        WVLOG_E("webEngineDefault is not EVERGREEN or LEGACY: %{public}d", webEngineDefault);
        return static_cast<ArkWebEngineVersion>(ArkWebEngineType::EVERGREEN);
    }

    WVLOG_I("get webEngineDefault: %{public}d", webEngineDefault);
    return static_cast<ArkWebEngineVersion>(webEngineDefault);
}

ArkWebEngineType getActiveWebEngineType()
{
    return static_cast<ArkWebEngineType>(getActiveWebEngineVersion());
}

bool IsActiveWebEngineEvergreen()
{
    if (getActiveWebEngineType() == ArkWebEngineType::EVERGREEN) {
        return true;
    }
    return false;
}

void LogForUnsupportedFunc(ArkWebEngineVersion version, const char* msg)
{
    WVLOG_W("%{public}s unsupported engine version: %{public}d",
        msg, static_cast<int>(version));
}

std::string GetArkwebLibPath()
{
    std::string path;
    if (getActiveWebEngineType() == ArkWebEngineType::LEGACY) {
        path =  ARK_WEB_CORE_LEGACY_HAP_LIB_PATH;
    } else {
        path = ARK_WEB_CORE_HAP_LIB_PATH;
#if defined(IS_ASAN) && defined(webview_arm64)
        if ((access(WEBVIEW_RELATIVE_SANDBOX_PATH_FOR_LIBRARY.c_str(), F_OK) == 0)) {
            path = ARK_WEB_CORE_HAP_LIB_PATH_ASAN;
        }
#endif
    }
    WVLOG_I("get arkweb lib path: %{public}s", path.c_str());
    return path;
}

std::string GetArkwebLibPathForMock()
{
    std::string path =  ARK_WEB_CORE_MOCK_HAP_LIB_PATH;
    WVLOG_I("get arkweb lib mock path: %{public}s", path.c_str());
    return path;
}

std::string GetArkwebNameSpace()
{
    std::string ns;
    if (getActiveWebEngineType() == ArkWebEngineType::LEGACY) {
        ns = "nweb_ns_legacy";
    } else {
        ns = "nweb_ns";
    }
    WVLOG_I("get arkweb name space: %{public}s", ns.c_str());
    return ns;
}

std::string GetArkwebRelativePathForBundle()
{
    std::string path;
#if defined(IS_ASAN) && defined(webview_arm64)
    if (!(access(WEBVIEW_RELATIVE_SANDBOX_PATH_FOR_LIBRARY.c_str(), F_OK) == 0)) {
        path = "arkwebcore/libs/arm64";
    } else {
        path = ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE;
    }
#else
    if (getActiveWebEngineType() == ArkWebEngineType::LEGACY) {
        path = ARK_WEB_CORE_LEGACY_PATH_FOR_BUNDLE;
    } else {
        path = ARK_WEB_CORE_PATH_FOR_BUNDLE;
    }
#endif
    WVLOG_I("get arkweb relative bundle path: %{public}s", path.c_str());
    return path;
}

std::string GetArkwebRelativePathForMock()
{
    std::string path = ARK_WEB_CORE_PATH_FOR_MOCK;
    WVLOG_I("get arkweb mock path: %{public}s", path.c_str());
    return path;
}

std::string GetArkwebInstallPath()
{
    std::vector<std::string> legacyPaths = {SANDBOX_LEGACY_HAP_PATH, PRECONFIG_LEGACY_HAP_PATH,};
    std::vector<std::string> greenPaths = {SANDBOX_EVERGREEN_HAP_PATH, PRECONFIG_EVERGREEN_HAP_PATH,
        PRECONFIG_EVERGREEN_WATCH_HAP_PATH};

    std::vector<std::string> workPaths;
    if (getActiveWebEngineType() == ArkWebEngineType::LEGACY) {
        workPaths = legacyPaths;
    } else {
        workPaths = greenPaths;
    }

    std::string installPath = "";
    for (auto path : workPaths) {
        if (access(path.c_str(), F_OK) == 0) {
            installPath = path;
            break;
        }
    }
    if (installPath == "") {
        WVLOG_E("failed to find hap path");
    }
    WVLOG_I("get arkweb install path: %{public}s", installPath.c_str());
    return installPath;
}

void* ArkWebBridgeHelperLoadLibFile(int openMode, const std::string& libFilePath,
    bool isPrintLog = true) 
{
    void* libFileHandler = ::dlopen(libFilePath.c_str(), openMode);
    if (!libFileHandler) {
        if (isPrintLog) {
            WVLOG_E("failed to load lib file %{public}s", libFilePath.c_str());
        }
        return nullptr;
    }

    if (isPrintLog) {
        WVLOG_I("succeed to load lib file %{public}s", libFilePath.c_str());
    }
    return libFileHandler;
}

void* ArkWebBridgeHelperLoadLibFile(int openMode, const std::string& libNsName,
    const std::string& libDirPath, const std::string& libFileName, bool isPrintLog = true)
{
    Dl_namespace dlns;

    dlns_init(&dlns, libNsName.c_str());

    int ret = dlns_create(&dlns, libDirPath.c_str());
    if (ret != 0) {
        WVLOG_E("dlns_create failed for '%{public}s': %{public}s (errno=%{public}d)", 
                libDirPath.c_str(), strerror(errno), ret);
    }

    Dl_namespace ndkns;
    ret = dlns_get("ndk", &ndkns);
    if (ret != 0) {
        WVLOG_E("dlns_get(ndk) failed: %{public}s (errno=%{public}d)", strerror(errno), ret);
    }

    ret = dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    if (ret != 0) {
        WVLOG_E("dlns_inherit failed: %{public}s (errno=%{public}d)", strerror(errno), ret);
    }

    void* libFileHandler = dlopen_ns(&dlns, libFileName.c_str(), openMode);
    if (!libFileHandler) {
        if (isPrintLog) {
            WVLOG_E(
                "failed to load lib file %{public}s/%{public}s", libDirPath.c_str(), libFileName.c_str());
        }
        return nullptr;
    }

    if (isPrintLog) {
        WVLOG_I(
            "succeed to load lib file %{public}s/%{public}s", libDirPath.c_str(), libFileName.c_str());
    }

    return libFileHandler;
}

void* ArkWebBridgeHelperSharedInit(bool runMode)
{
    std::string libFileName = "libarkweb_engine.so";

    std::string libDirPath;
    if (runMode) {
        libDirPath = GetArkwebLibPath();
    } else {
        libDirPath = GetArkwebLibPathForMock();
    }

    std::string libNsName = GetArkwebNameSpace();

    void* libFileHandler;

#ifdef __MUSL__
    libFileHandler = ArkWebBridgeHelperLoadLibFile(RTLD_NOW | RTLD_GLOBAL, libNsName, libDirPath, libFileName);
#else
    libFileHandler = ArkWebBridgeHelperLoadLibFile(RTLD_NOW, libDirPath + "/" + libFileName)
#endif

    if (libFileHandler != nullptr) {
        g_webEngineInitFlag = true;
        WVLOG_I("g_webEngineInitFlag set to true. setActiveWebEngineVersion will be ignored.");
    }

    return libFileHandler;
}
}
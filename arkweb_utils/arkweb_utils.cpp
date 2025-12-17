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
#include <dlfcn_ext.h>
#include <fcntl.h>
#include <fstream>
#include <filesystem>
#include <policycoreutils.h>
#include <system_error>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <unordered_set>
#include <regex>
#include <iomanip>

#if (defined(webview_arm64) && !defined(ASAN_DETECTOR))
#include <sys/mount.h>
#endif

namespace OHOS::ArkWeb {

static int g_appEngineVersion = static_cast<int>(ArkWebEngineVersion::SYSTEM_DEFAULT);
static bool g_webEngineInitFlag = false;
static ArkWebEngineVersion g_activeEngineVersion = ArkWebEngineVersion::SYSTEM_DEFAULT;
static int g_cloudEnableAppVersion = static_cast<int>(ArkWebEngineVersion::SYSTEM_DEFAULT);
static std::unique_ptr<std::unordered_set<std::string>> g_legacyApp = nullptr;

static void* g_reservedAddress = nullptr;
static size_t g_reservedSize = 0;
const int SHARED_RELRO_UID = 1037;
const std::string ARK_WEB_ENGINE_LIB_NAME = "libarkweb_engine.so";
const std::string SHARED_RELRO_DIR = "/data/service/el1/public/for-all-app/shared_relro";
const std::string NWEB_RELRO_PATH = SHARED_RELRO_DIR + "/libwebviewchromium64.relro";
const size_t RESERVED_VMA_SIZE = 512 * 1024 * 1024;

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
const std::string SYSTEM_PARAM_VERSION_PATH =
    "/system/etc/ArkWebSafeBrowsing/generic/version.txt";
const std::string UPDATE_PARAM_VERSION_PATH =
    "/data/service/el1/public/update/param_service/install/system/etc/ArkWebSafeBrowsing/generic/version.txt";
const int VERSION_TAG_LEN = 3;

#if (defined(webview_arm64) && !defined(ASAN_DETECTOR))
const int MAX_DLCLOSE_COUNT = 10;
const std::string LIB_ARKWEB_ENGINE = "libarkweb_engine.so";
const std::string PERSIST_ARKWEBCORE_PACKAGE_NAME = "persist.arkwebcore.package_name";
const std::string EL1_BUNDLE_PUBLIC = "/data/app/el1/bundle/public/";
const std::string SANDBOX_REAL_PATH = "/data/storage/el1/bundle/arkwebcore";
const std::string APPSPAWN_PRELOAD_ARKWEB_ENGINE = "const.startup.appspawn.preload.arkwebEngine";
#endif

// 前向声明
static ArkWebEngineVersion CalculateActiveWebEngineVersion();

static void SetIntParameter(const std::string& key, const int& intValue)
{
    std::string valueStr = std::to_string(intValue);
    if (OHOS::system::SetParameter(key, valueStr)) {
        WVLOG_I("Successfully set integer param %{public}s with %{public}s", key.c_str(), valueStr.c_str());
    } else {
        WVLOG_E("Failed to set integer param %{public}s with %{public}s", key.c_str(), valueStr.c_str());
    }
}

static void ProcessDefaultParam(const Json::Value& value)
{
    if (!value.isInt()) {
        WVLOG_E("Unsupported type for param web.engine.default, must be integer");
        return;
    }

    int intValue = value.asInt();
    if (intValue != static_cast<int>(ArkWebEngineType::LEGACY) &&
        intValue != static_cast<int>(ArkWebEngineType::EVERGREEN)) {
        WVLOG_E("Invalid value for web.engine.default: %{public}d, "
            "must be LEGACY(%{public}d) or EVERGREEN(%{public}d)",
            intValue,
            static_cast<int>(ArkWebEngineType::LEGACY),
            static_cast<int>(ArkWebEngineType::EVERGREEN));
        return;
    }

    SetIntParameter("web.engine.default", intValue);
}

static void ProcessEnforceParam(const Json::Value& value)
{
    if (!value.isInt()) {
        WVLOG_E("Unsupported type for param web.engine.enforce, must be integer");
        return;
    }

    int intValue = value.asInt();
    if (intValue != static_cast<int>(ArkWebEngineType::EVERGREEN)) {
        WVLOG_E("Invalid value for web.engine.enforce: %{public}d, "
            "must be EVERGREEN(%{public}d)",
            intValue,
            static_cast<int>(ArkWebEngineType::EVERGREEN));
        return;
    }

    SetIntParameter("web.engine.enforce", intValue);
}

static void ProcessLegacyAppParam(const Json::Value& value)
{
    if (!value.isArray()) {
        WVLOG_E("Unsupported type for param web.engine.legacyApp, must be array");
        return;
    }

    auto appSet = std::make_unique<std::unordered_set<std::string>>();
    for (const auto& item : value) {
        if (item.isString()) {
            appSet->insert(item.asString());
        } else {
            WVLOG_E("Non-string item found in array for web.engine.legacyApp, skipping.");
        }
    }

    g_legacyApp = std::move(appSet);
    WVLOG_I("Successfully stored legacyApp in heap memory using smart pointer.");
}

static void ProcessParamItem(const std::string& key, const Json::Value& value)
{
    if (key.find(WEB_PARAM_PREFIX) != 0) {
        WVLOG_E("Skipping param with incorrect prefix %{public}s", key.c_str());
        return;
    }

    if (key == "web.engine.default") {
        ProcessDefaultParam(value);
    } else if (key == "web.engine.enforce") {
        ProcessEnforceParam(value);
    } else if (key == "web.engine.legacyApp") {
        ProcessLegacyAppParam(value);
    } else {
        WVLOG_E("Unsupported key for param %{public}s", key.c_str());
    }
}

static void ProcessJsonConfig(const Json::Value& root)
{
    if (!root.isObject()) {
        WVLOG_E("Root element is not a JSON object");
        return;
    }

    Json::Value::Members keys = root.getMemberNames();
    for (const auto& key : keys) {
        const Json::Value& value = root[key];
        ProcessParamItem(key, value);
    }
}

static bool GetVersionString(const std::string& versionFilePath, std::string& versionStr)
{
    std::ifstream file(versionFilePath);
    if(!file.is_open()) {
        WVLOG_E("can not open version file: %{public}s", versionFilePath.c_str());
        return false;
    }

    if (std::getline(file, versionStr)) {
        return true;
    }

    WVLOG_E("%{public}s is empty.", versionFilePath.c_str());
    return false;
}

static bool HandleVersionString(const std::string& versionStr, long long& versionNum)
{
    std::regex pattern(R"(version\s{0,1}=\s{0,1}(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3}))");
    std::smatch matches;
    if (std::regex_match(versionStr, matches, pattern)) {
        std::ostringstream versionStream;
        for (size_t i = 1; i < matches.size(); ++i) {
            int num = std::stoi(matches[i].str());
            versionStream << std::setw(VERSION_TAG_LEN) << std::setfill('0') << num;
        }
        versionNum = std::stoll(versionStream.str());
        return true;
    }

    WVLOG_E("incorrect version format, must be aa.bb.xx.yy: %{public}s", versionStr.c_str());
    return false;
}

static bool CheckCloudCfgVersion(const std::string& systemParamVersionPath,
    const std::string& updateParamVersionPath)
{
    std::string systemParamVersionStr;
    std::string updateParamVersionStr;
    if (!GetVersionString(systemParamVersionPath, systemParamVersionStr) ||
        !GetVersionString(updateParamVersionPath, updateParamVersionStr)) {
        return false;
    }

    long long systemParamVersionNum;
    long long updateParamVersionNum;
    if (!HandleVersionString(systemParamVersionStr, systemParamVersionNum) ||
        !HandleVersionString(updateParamVersionStr, updateParamVersionNum)) {
        return false;
    }

    if (updateParamVersionNum >= systemParamVersionNum) {
        WVLOG_I("web param update version %{public}lld is more than system version %{public}lld, update valid.",
            updateParamVersionNum, systemParamVersionNum);
        return true;
    } else {
        WVLOG_I("web param update version %{public}lld is not more than system version %{public}lld, update invalid.",
            updateParamVersionNum, systemParamVersionNum);
        return false;
    }
}

static void ParseCloudCfg()
{
    std::ifstream jsonFile(JSON_CONFIG_PATH.c_str());
    if (!jsonFile.is_open()) {
        WVLOG_E("Failed to open file %{public}s, reason: %{public}s", JSON_CONFIG_PATH.c_str(), strerror(errno));
        return;
    }

    if (!CheckCloudCfgVersion(SYSTEM_PARAM_VERSION_PATH, UPDATE_PARAM_VERSION_PATH)) {
        return;
    }

    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string parseErrors;
    bool parsingSuccessful = Json::parseFromStream(readerBuilder, jsonFile, &root, &parseErrors);
    if (!parsingSuccessful) {
        WVLOG_E("JSON parse failed, parseErrors: %{public}s", parseErrors.c_str());
        return;
    }

    ProcessJsonConfig(root);
}

void SelectWebcoreBeforeProcessRun(const std::string& appBundleName)
{
    WVLOG_I("SelectWebcoreBeforeProcessRun for app %{public}s.", appBundleName.c_str());

    if (g_legacyApp && g_legacyApp->find(appBundleName) != g_legacyApp->end()) {
        g_cloudEnableAppVersion = static_cast<int>(ArkWebEngineType::LEGACY);
    }

    g_activeEngineVersion = CalculateActiveWebEngineVersion();

    g_legacyApp.reset();
}

void PreloadArkWebLibForBrowser()
{
    ParseCloudCfg();
    if (!(access(PRECONFIG_LEGACY_HAP_PATH.c_str(), F_OK) == 0)) {
        if (OHOS::system::SetParameter("web.engine.enforce",
                std::to_string(static_cast<int>(ArkWebEngineType::EVERGREEN)))) {
            WVLOG_I("Set param web.engine.enforce with %{public}d", static_cast<int>(ArkWebEngineType::EVERGREEN));
        } else {
            WVLOG_I("Set param web.engine.enforce with %{public}d failed",
                    static_cast<int>(ArkWebEngineType::EVERGREEN));
        }
    }
    g_activeEngineVersion = CalculateActiveWebEngineVersion();
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
    WVLOG_I("app setActiveWebEngineVersion: %{public}d", g_appEngineVersion);
    g_activeEngineVersion = CalculateActiveWebEngineVersion();
}

void SetActiveWebEngineVersionInner(ArkWebEngineVersion version)
{
    g_activeEngineVersion = version;
}

ArkWebEngineVersion getActiveWebEngineVersion()
{
    return g_activeEngineVersion;
}

ArkWebEngineVersion CalculateActiveWebEngineVersion()
{
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    if (webEngineEnforce == static_cast<int>(ArkWebEngineType::EVERGREEN)) {
        WVLOG_I("CalculateActiveWebEngineVersion, enforce EVERGREEN");
        return static_cast<ArkWebEngineVersion>(ArkWebEngineType::EVERGREEN);
    }

    if (g_appEngineVersion != static_cast<int>(ArkWebEngineVersion::SYSTEM_DEFAULT)) {
        WVLOG_I("CalculateActiveWebEngineVersion appEngineVersion: %{public}d", g_appEngineVersion);
        if (g_appEngineVersion == static_cast<int>(ArkWebEngineVersion::SYSTEM_EVERGREEN)) {
            return static_cast<ArkWebEngineVersion>(ArkWebEngineType::EVERGREEN);
        }
        return static_cast<ArkWebEngineVersion>(g_appEngineVersion);
    }
    
    if (g_cloudEnableAppVersion == static_cast<int>(ArkWebEngineType::LEGACY)) {
        WVLOG_I("CalculateActiveWebEngineVersion CloudEnableAppVersion: %{public}d", g_cloudEnableAppVersion);
        return static_cast<ArkWebEngineVersion>(g_cloudEnableAppVersion);
    }

    int webEngineDefault = OHOS::system::GetIntParameter("web.engine.default",
        static_cast<int>(ArkWebEngineType::EVERGREEN));
    if (webEngineDefault != static_cast<int>(ArkWebEngineType::LEGACY) &&
      webEngineDefault != static_cast<int>(ArkWebEngineType::EVERGREEN)) {
        WVLOG_E("CalculateActiveWebEngineVersion, webEngineDefault error: %{public}d", webEngineDefault);
        return static_cast<ArkWebEngineVersion>(ArkWebEngineType::EVERGREEN);
    }

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

std::string GetArkwebLibPathForMock(const std::string& mockBundlePath)
{
    std::string path;
    if (!mockBundlePath.empty()) {
        path = mockBundlePath + "/" + ARK_WEB_CORE_PATH_FOR_MOCK;
    } else {
        path = ARK_WEB_CORE_MOCK_HAP_LIB_PATH;
    }
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

void* ArkWebBridgeHelperSharedInit(bool runMode, const std::string& mockBundlePath)
{
    std::string libFileName = "libarkweb_engine.so";

    std::string libDirPath;
    if (runMode) {
        libDirPath = GetArkwebLibPath();
    } else {
        libDirPath = GetArkwebLibPathForMock(mockBundlePath);
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

#if (defined(webview_arm64) && !defined(ASAN_DETECTOR))
bool CreateRealSandboxPath()
{
    namespace fs = std::filesystem;
    std::error_code ec;
    if (fs::exists(SANDBOX_REAL_PATH, ec)) {
        WVLOG_I("CreateRealSandboxPath %{public}s already exists", SANDBOX_REAL_PATH.c_str());
        return true;
    }

    if (fs::create_directories(SANDBOX_REAL_PATH, ec)) {
        return true;
    }

    WVLOG_E("CreateRealSandboxPath create_directories failed");
    return false;
}

#endif
void DlopenArkWebLib()
{
#if (defined(webview_arm64) && !defined(ASAN_DETECTOR))
    if (!OHOS::system::GetBoolParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, false)) {
        WVLOG_E("DlopenArkWebLib preload arkweb turn off!");
        return;
    }
    const std::string bundleName = OHOS::system::GetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "");
    if (bundleName.empty()) {
        WVLOG_E("DlopenArkWebLib bundleName is null");
        return;
    }

    if (!CreateRealSandboxPath()) {
        return;
    }

    const std::string realPath = EL1_BUNDLE_PUBLIC + bundleName;
    WVLOG_I("DlopenArkWebLib realPath: %{public}s, SANDBOX_REAL_PATH: %{public}s",
        realPath.c_str(),
        SANDBOX_REAL_PATH.c_str());
    if (mount(realPath.c_str(), SANDBOX_REAL_PATH.c_str(), nullptr, MS_BIND | MS_REC, nullptr) != 0) {
        WVLOG_E("DlopenArkWebLib mount error: %{public}s", strerror(errno));
        return;
    }

    ArkWebBridgeHelperLoadLibFile(
        RTLD_NOW | RTLD_GLOBAL,
        "nweb_ns",
        ARK_WEB_CORE_HAP_LIB_PATH.c_str(),
        LIB_ARKWEB_ENGINE.c_str()
    );
#endif
}

#if (defined(webview_arm64) && !defined(ASAN_DETECTOR))
static bool IsNWebLibLoaded(Dl_namespace dlns)
{
    void* handler = dlopen_ns(&dlns, LIB_ARKWEB_ENGINE.c_str(), RTLD_NOW | RTLD_NOLOAD);
    if (handler) {
        dlclose(handler);
        return true;
    }
    return false;
}
#endif

int DlcloseArkWebLib()
{
#if (defined(webview_arm64) && !defined(ASAN_DETECTOR))
    if (!OHOS::system::GetBoolParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, false)) {
        WVLOG_E("DlcloseArkWebLib preload arkweb turn off!");
        return 0;
    }
    Dl_namespace dlns;
    if (dlns_get("nweb_ns", &dlns) != 0) {
        WVLOG_I("Failed to get nweb_ns");
        return 0;
    }

    void* webEngineHandle = dlopen_ns(&dlns, LIB_ARKWEB_ENGINE.c_str(), RTLD_NOW | RTLD_NOLOAD);
    if (!webEngineHandle) {
        WVLOG_E("FAILED to find %{public}s, error: %{public}s", LIB_ARKWEB_ENGINE.c_str(), dlerror());
        return 0;
    }

    int cnt = MAX_DLCLOSE_COUNT;
    do {
        cnt--;
        dlclose(webEngineHandle);
    } while (cnt > 0 && IsNWebLibLoaded(dlns));

    if (cnt == 0 && IsNWebLibLoaded(dlns)) {
        return -1;
    }
#endif
    return 0;
}

bool NeedShareRelro()
{
    // Check if share relro is enabled
    static bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    if (!isShareRelroEnabled) {
        WVLOG_I("NeedShareRelro: share relro is disabled.");
        return false;
    }

    // Check if ark_web_engine_lib is accessible
    std::string arkwebLibPath = GetArkwebLibPath();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/" + ARK_WEB_ENGINE_LIB_NAME;
    std::error_code ec;
    static bool arkwebEngineAccessible = std::filesystem::exists(arkwebEngineLibPath, ec);
    if (ec) {
        WVLOG_E("NeedShareRelro: error checking file: %{public}s.", ec.message().c_str());
        return false;
    }

    // Need to share relro only when share relro is enabled and ark_web_engine_lib is accessible
    WVLOG_I("NeedShareRelro:share relro is enabled, webEngine accessible:%{public}d.", arkwebEngineAccessible);
    return arkwebEngineAccessible;
}

bool ReserveAddressSpace()
{
    if (!NeedShareRelro()) {
        return false;
    }
    void* addr = mmap(nullptr, RESERVED_VMA_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        WVLOG_E("ReserveAddressSpace: mmap failed, error=[%{public}s]", strerror(errno));
        return false;
    }
    prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, addr, RESERVED_VMA_SIZE, "libwebview reservation");
    g_reservedAddress = addr;
    g_reservedSize = RESERVED_VMA_SIZE;
    WVLOG_I("1.Reserve address space success, size:%{public}zd bytes", g_reservedSize);
    return true;
}

void* CreateRelroFile(const std::string& lib, Dl_namespace* dlns)
{
    if (!NeedShareRelro()) {
        return nullptr;
    }
    if (!g_reservedAddress) {
        WVLOG_E("CreateRelroFile: the reserved address is nullptr.");
        return nullptr;
    }

    WVLOG_I("2.CreateRelroFile begin.");
    if (unlink(NWEB_RELRO_PATH.c_str()) != 0 && errno != ENOENT) {
        WVLOG_E("CreateRelroFile unlink failed, error=[%{public}s]", strerror(errno));
    }
    int relroFd = open(NWEB_RELRO_PATH.c_str(), O_RDWR | O_TRUNC | O_CLOEXEC | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    if (relroFd < 0) {
        WVLOG_E("CreateRelroFile open failed, error=[%{public}s]", strerror(errno));
        return nullptr;
    }

    WVLOG_I("CreateRelroFile: dlopen_ns_ext extinfo with reserved address.");
    dl_extinfo extinfo = {
        .flag = DL_EXT_WRITE_RELRO | DL_EXT_RESERVED_ADDRESS_RECURSIVE | DL_EXT_RESERVED_ADDRESS,
        .relro_fd = relroFd,
        .reserved_addr = g_reservedAddress,
        .reserved_size = g_reservedSize,
    };
    void* result = dlopen_ns_ext(dlns, lib.c_str(), RTLD_NOW | RTLD_GLOBAL, &extinfo);
    if (!result) {
        WVLOG_E("CreateRelroFile: dlopen_ns_ext failed, error=[%{public}s]", strerror(errno));
    }
    close(relroFd);
    return result;
}

void CreateRelroFileInSubProc()
{
    pid_t pid = fork();
    if (pid < 0) {
        WVLOG_E("Fork sub process failed.");
    } else if (pid == 0) {
        WVLOG_I("Fork sub process success.");
        if (prctl(PR_SET_NAME, "shared_relro")) {
            WVLOG_E("Set sub process name failed, error=[%{public}s]", strerror(errno));
        }
        if (setuid(SHARED_RELRO_UID) || setgid(SHARED_RELRO_UID)) {
            WVLOG_E("Set uid/gid failed, error=[%{public}s]", strerror(errno));
            _exit(0);
        }
        if (RestoreconRecurse(SHARED_RELRO_DIR.c_str())) {
            WVLOG_E("Restorecon failed, error=[%{public}s]", strerror(errno));
            _exit(0);
        }

        const std::string libNsName = GetArkwebNameSpace();
        const std::string libpath = GetArkwebLibPath();
        Dl_namespace dlns;
        dlns_init(&dlns, libNsName.c_str());
        dlns_create(&dlns, libpath.c_str());
        Dl_namespace ndkns;
        dlns_get("ndk", &ndkns);
        dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");

        void* webEngineHandle = CreateRelroFile(ARK_WEB_ENGINE_LIB_NAME, &dlns);
        if (webEngineHandle) {
            WVLOG_I("Create relro file success.");
        } else {
            WVLOG_E("Create relro file failed.");
        }
        _exit(0);
    }
}

void* LoadWithRelroFile(const std::string& lib, Dl_namespace* dlns)
{
    if (!NeedShareRelro()) {
        return nullptr;
    }
    if (!g_reservedAddress) {
        WVLOG_E("LoadWithRelroFile: the reserved address is nullptr.");
        return nullptr;
    }

    WVLOG_I("3.LoadWithRelroFile begin.");
    int relroFd = open(NWEB_RELRO_PATH.c_str(), O_RDONLY);
    if (relroFd < 0) {
        WVLOG_E("LoadWithRelroFile open failed, error=[%{public}s]", strerror(errno));
        return nullptr;
    }

    WVLOG_I("LoadWithRelroFile: dlopen_ns_ext extinfo with reserved address.");
    dl_extinfo extinfo = {
        .flag = DL_EXT_USE_RELRO | DL_EXT_RESERVED_ADDRESS_RECURSIVE | DL_EXT_RESERVED_ADDRESS,
        .relro_fd =relroFd,
        .reserved_addr = g_reservedAddress,
        .reserved_size = g_reservedSize,
    };
    void* result = dlopen_ns_ext(dlns, lib.c_str(), RTLD_NOW | RTLD_GLOBAL, &extinfo);
    close(relroFd);
    return result;
}
}
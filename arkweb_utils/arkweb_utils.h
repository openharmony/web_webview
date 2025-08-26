/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef WEB_WEBVIEW_ARKWEB_UTILS_ARKWEB_UTILS_H
#define WEB_WEBVIEW_ARKWEB_UTILS_ARKWEB_UTILS_H

#pragma once

#include <climits>
#include <string>

#ifndef ARKWEB_EXPORT
#define ARKWEB_EXPORT __attribute__((visibility("default")))
#endif

#if !defined(CROSS_PLATFORM)
#define RETURN_IF_UNSUPPORTED_ENGINE(minVersion, funName)                   \
do {                                                                        \
    auto engineVersion = OHOS::ArkWeb::getActiveWebEngineVersion();         \
    if (engineVersion < (minVersion)) {    \
        OHOS::ArkWeb::LogForUnsupportedFunc(engineVersion, funName);        \
        return;                                                             \
    }                                                                       \
} while (0)
#else
#define RETURN_IF_UNSUPPORTED_ENGINE(minVersion, funName)
#endif

#if !defined(CROSS_PLATFORM)
#define IS_CALLING_FROM_M114() \
    (OHOS::ArkWeb::getActiveWebEngineVersion() == OHOS::ArkWeb::ArkWebEngineVersion::M114)
#else
#define IS_CALLING_FROM_M114() false
#endif

#if !defined(CROSS_PLATFORM)
#define RETURN_IF_CALLING_FROM_M114() \
do { \
    if (IS_CALLING_FROM_M114()) { \
        OHOS::ArkWeb::LogForUnsupportedFunc(OHOS::ArkWeb::ArkWebEngineVersion::M114, __func__); \
        return; \
    } \
} while (0)
#else
#define RETURN_IF_CALLING_FROM_M114()
#endif

#define APP_ENGINE_VERSION_PREFIX "#--appEngineVersion="

namespace OHOS::ArkWeb {

enum class ArkWebEngineVersion {
    SYSTEM_DEFAULT = 0,
    M114 = 1,
    M132 = 2,
    SYSTEM_EVERGREEN = 99999
};

enum class ArkWebEngineType {
    LEGACY = static_cast<int>(ArkWebEngineVersion::M114),
    EVERGREEN = static_cast<int>(ArkWebEngineVersion::M132),
};

ARKWEB_EXPORT void setActiveWebEngineVersion(ArkWebEngineVersion version);
ARKWEB_EXPORT ArkWebEngineVersion getActiveWebEngineVersion();
ARKWEB_EXPORT ArkWebEngineType getActiveWebEngineType();
ARKWEB_EXPORT void SetActiveWebEngineVersionInner(ArkWebEngineVersion version);
ARKWEB_EXPORT bool IsActiveWebEngineEvergreen();
ARKWEB_EXPORT void SelectWebcoreBeforeProcessRun(const std::string& appBundleName);

ARKWEB_EXPORT std::string GetArkwebLibPath();
ARKWEB_EXPORT std::string GetArkwebNameSpace();
ARKWEB_EXPORT std::string GetArkwebRelativePathForBundle();
ARKWEB_EXPORT std::string GetArkwebRelativePathForMock();
ARKWEB_EXPORT std::string GetArkwebInstallPath();
ARKWEB_EXPORT void PreloadArkWebLibForBrowser();

ARKWEB_EXPORT void LogForUnsupportedFunc(ArkWebEngineVersion version, const char* msg);

ARKWEB_EXPORT void* ArkWebBridgeHelperSharedInit(bool runMode = true);
} // namespace OHOS::ArkWeb

#endif // WEB_WEBVIEW_ARKWEB_UTILS_ARKWEB_UTILS_H
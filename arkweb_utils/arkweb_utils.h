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

#pragma once

#include <climits>

#ifndef ARKWEB_EXPORT
#define ARKWEB_EXPORT __attribute__((visibility("default")))
#endif

#if !defined(CROSS_PLATFORM)
#define RETURN_IF_UNSUPPORTED_ENGINE(minVersion, funName)                   \
do {                                                                        \
    auto engineVersion = OHOS::ArkWeb::getActiveWebEngineVersion();         \
    if (engineVersion < minVersion) {    \
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
#define IS_CALLING_FROM_M114()
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

namespace OHOS::ArkWeb {

enum class ArkWebEngineVersion {
    M114 = 0,
    M132 = 1,
    UNDEFINED = INT_MAX,
};

enum class ArkWebEngineType {
    LEGACY = static_cast<int>(ArkWebEngineVersion::M114),
    EVERGREEN = static_cast<int>(ArkWebEngineVersion::M132),
    SYSTEM_DEFAILT,
    UNDEFINED = INT_MAX,
};


ARKWEB_EXPORT void setActiveWebEngineVersion(ArkWebEngineVersion version);

ARKWEB_EXPORT ArkWebEngineVersion getActiveWebEngineVersion();

ARKWEB_EXPORT void setActiveWebEngineType(ArkWebEngineType type);

ARKWEB_EXPORT ArkWebEngineType getActiveWebEngineType();

ARKWEB_EXPORT void LogForUnsupportedFunc(ArkWebEngineVersion miniVersion, const char* msg);

} // namespace OHOS::ArkWeb

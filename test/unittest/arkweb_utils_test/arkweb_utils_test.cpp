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

#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "arkweb_utils.h"
#include "nweb_config_helper.h"
#include "nweb_log.h"
#include "parameters.h"
#include "system_properties_adapter_impl.h"

using namespace testing::ext;

namespace OHOS::ArkWeb {
#if defined(webview_arm64)
const std::string ARK_WEB_CORE_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcore/libs/arm64";
const std::string ARK_WEB_CORE_LEGACY_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcorelegacy/libs/arm64";
const std::string ARK_WEB_CORE_PATH_FOR_MOCK = "libs/arm64";
const std::string ARK_WEB_CORE_PATH_FOR_BUNDLE = "arkwebcore/libs/arm64";
const std::string ARK_WEB_CORE_LEGACY_PATH_FOR_BUNDLE = "arkwebcorelegacy/libs/arm64";
#elif defined(webview_x86_64)
const std::string ARK_WEB_CORE_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcore/libs/x86_64";
const std::string ARK_WEB_CORE_LEGACY_HAP_LIB_PATH =
    "/data/storage/el1/bundle/arkwebcorelegacy/libs/x86_64";
const std::string ARK_WEB_CORE_PATH_FOR_MOCK = "libs/x86_64";
const std::string ARK_WEB_CORE_PATH_FOR_BUNDLE = "arkwebcore/libs/x86_64";
const std::string ARK_WEB_CORE_LEGACY_PATH_FOR_BUNDLE = "arkwebcorelegacy/libs/x86_64";
#else
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
#elif defined(webview_x86_64)
const std::string ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE = "arkwebcore_asan/libs/x86_64";
#else
const std::string ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE = "arkwebcore_asan/libs/arm";
#endif
#endif

const std::string PRECONFIG_LEGACY_HAP_PATH = "/system/app/ArkWebCoreLegacy/ArkWebCoreLegacy.hap";
const std::string  PRECONFIG_EVERGREEN_HAP_PATH =
    "/system/app/ArkWebCore/ArkWebCore.hap";
const std::string SANDBOX_LEGACY_HAP_PATH = "/data/storage/el1/bundle/arkwebcorelegacy/entry.hap";
const std::string SANDBOX_EVERGREEN_HAP_PATH = "/data/storage/el1/bundle/arkwebcore/entry.hap";

const std::string JSON_CONFIG_PATH =
    "/data/service/el1/public/update/param_service/install/system/etc/ArkWebSafeBrowsing/generic/ArkWebCoreCfg.json";

TEST(arkweb_utils_test, setActiveWebEngineVersion001) {
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    EXPECT_EQ(getAppWebEngineVersion(), ArkWebEngineVersion::SYSTEM_DEFAULT);
}

TEST(arkweb_utils_test, setActiveWebEngineVersion002) {
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    EXPECT_EQ(getAppWebEngineVersion(), ArkWebEngineVersion::M114);
}

TEST(arkweb_utils_test, setActiveWebEngineVersion003) {
    setActiveWebEngineVersion(ArkWebEngineVersion::M132);

    EXPECT_EQ(getAppWebEngineVersion(), ArkWebEngineVersion::M132);
}

TEST(arkweb_utils_test, setActiveWebEngineVersion004) {
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);
    setActiveWebEngineVersion(static_cast<ArkWebEngineVersion>(3));

    EXPECT_EQ(getAppWebEngineVersion(), ArkWebEngineVersion::SYSTEM_DEFAULT);
}

TEST(arkweb_utils_test, getActiveWebEngineVersion001) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(1));

    auto res = getActiveWebEngineVersion();
    EXPECT_EQ(res, ArkWebEngineVersion::M132);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, getActiveWebEngineVersion002) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = getActiveWebEngineVersion();
    EXPECT_EQ(res, ArkWebEngineVersion::M132);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, getActiveWebEngineVersion003) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = getActiveWebEngineVersion();
    EXPECT_EQ(res, ArkWebEngineVersion::M114);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, getActiveWebEngineType001) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(2));

    auto res = getActiveWebEngineType();
    EXPECT_EQ(res, ArkWebEngineType::EVERGREEN);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, getActiveWebEngineType002) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = getActiveWebEngineType();
    EXPECT_EQ(res, ArkWebEngineType::EVERGREEN);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, getActiveWebEngineType003) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = getActiveWebEngineType();
    EXPECT_EQ(res, ArkWebEngineType::LEGACY);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebLibPath001) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = GetArkwebLibPath();
    EXPECT_EQ(res, ARK_WEB_CORE_LEGACY_HAP_LIB_PATH);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebLibPath002) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = GetArkwebLibPath();
    EXPECT_EQ(res, ARK_WEB_CORE_HAP_LIB_PATH);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebNameSpace001) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = GetArkwebNameSpace();
    EXPECT_EQ(res, "nweb_ns_legacy");
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebNameSpace002) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = GetArkwebNameSpace();
    EXPECT_EQ(res, "nweb_ns");
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebRelativePathForBundle001) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = GetArkwebRelativePathForBundle();
#if defined(IS_ASAN)
    EXPECT_EQ(res, ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE);
#else
    EXPECT_EQ(res, ARK_WEB_CORE_LEGACY_PATH_FOR_BUNDLE);
#endif
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebRelativePathForBundle002) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = GetArkwebRelativePathForBundle();
#if defined(IS_ASAN)
    EXPECT_EQ(res, ARK_WEB_CORE_ASAN_PATH_FOR_BUNDLE);
#else
    EXPECT_EQ(res, ARK_WEB_CORE_PATH_FOR_BUNDLE);
#endif
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebInstallPath001) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto aclPath = GetArkwebInstallPath();
    bool res = (aclPath == SANDBOX_LEGACY_HAP_PATH || aclPath == PRECONFIG_LEGACY_HAP_PATH);
    NWeb::ProductDeviceType result = NWeb::SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType();
    if (result == NWeb::ProductDeviceType::DEVICE_TYPE_UNKNOWN) {
        EXPECT_FALSE(res);
    } else {
        EXPECT_TRUE(res);
    }

    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

TEST(arkweb_utils_test, GetArkwebInstallPath002) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto aclPath = GetArkwebInstallPath();
    bool res = (aclPath == SANDBOX_EVERGREEN_HAP_PATH || aclPath == PRECONFIG_EVERGREEN_HAP_PATH);
    NWeb::ProductDeviceType result = NWeb::SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType();
    if (result == NWeb::ProductDeviceType::DEVICE_TYPE_UNKNOWN) {
        EXPECT_FALSE(res);
    } else {
        EXPECT_TRUE(res);
    }

    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}
} // namespace OHOS::NWeb
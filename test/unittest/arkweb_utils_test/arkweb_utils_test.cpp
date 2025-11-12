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

#include "arkweb_utils.cpp"
#include "parameters.h"
#include "system_properties_adapter_impl.h"

using namespace testing::ext;

namespace OHOS::ArkWeb {
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

class FileProcessor {
public:
    static void WriteFile(const std::string& fileName, const std::string& content)
    {
        std::ofstream file(fileName);
        if(!file.is_open()) {
            return;
        }

        file << content;
    }
};

class ArkWebUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::string systemTmpVersionFile_ = "system_version_tmp.txt";
    std::string updateTmpVersionFile_ = "update_version_tmp.txt";
    static int paramEnforce;
    static int paramDefault;
};

int ArkWebUtilsTest::paramEnforce = 0;
int ArkWebUtilsTest::paramDefault = 0;

void ArkWebUtilsTest::SetUpTestCase(void)
{
    paramEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    paramDefault = OHOS::system::GetIntParameter("web.engine.default", 0);
}

void ArkWebUtilsTest::TearDownTestCase(void)
{
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(paramEnforce));
    OHOS::system::SetParameter("web.engine.default", std::to_string(paramDefault));
}

void ArkWebUtilsTest::SetUp(void)
{}

void ArkWebUtilsTest::TearDown(void)
{}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_SetActiveWebEngineVersionInner_001, TestSize.Level1) {
    SetActiveWebEngineVersionInner(ArkWebEngineVersion::SYSTEM_DEFAULT);

    EXPECT_EQ(getActiveWebEngineVersion(), ArkWebEngineVersion::SYSTEM_DEFAULT);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_SetActiveWebEngineVersionInner_002, TestSize.Level1) {
    SetActiveWebEngineVersionInner(ArkWebEngineVersion::M114);

    EXPECT_EQ(getActiveWebEngineVersion(), ArkWebEngineVersion::M114);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_SetActiveWebEngineVersionInner_003, TestSize.Level1) {
    SetActiveWebEngineVersionInner(ArkWebEngineVersion::M132);

    EXPECT_EQ(getActiveWebEngineVersion(), ArkWebEngineVersion::M132);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_getActiveWebEngineType_001, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(2));

    auto res = getActiveWebEngineType();
    EXPECT_EQ(res, ArkWebEngineType::EVERGREEN);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_getActiveWebEngineType_002, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = getActiveWebEngineType();
    EXPECT_EQ(res, ArkWebEngineType::EVERGREEN);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_getActiveWebEngineType_003, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = getActiveWebEngineType();
    EXPECT_EQ(res, ArkWebEngineType::LEGACY);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebLibPath_001, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = GetArkwebLibPath();
    EXPECT_EQ(res, ARK_WEB_CORE_LEGACY_HAP_LIB_PATH);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebLibPath_002, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = GetArkwebLibPath();
    EXPECT_EQ(res, ARK_WEB_CORE_HAP_LIB_PATH);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebNameSpace_001, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto res = GetArkwebNameSpace();
    EXPECT_EQ(res, "nweb_ns_legacy");
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebNameSpace_002, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = GetArkwebNameSpace();
    EXPECT_EQ(res, "nweb_ns");
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebRelativePathForBundle_001, TestSize.Level1) {
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

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebRelativePathForBundle_002, TestSize.Level1) {
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

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebInstallPath_001, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);

    auto aclPath = GetArkwebInstallPath();
    bool res = (aclPath == SANDBOX_LEGACY_HAP_PATH || aclPath == PRECONFIG_LEGACY_HAP_PATH);
    if (access(PRECONFIG_LEGACY_HAP_PATH.c_str(), F_OK) == 0) {
        EXPECT_TRUE(res);
    } else {
        EXPECT_FALSE(res);
    }
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebInstallPath_002, TestSize.Level1) {
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto aclPath = GetArkwebInstallPath();
    bool res = (aclPath == SANDBOX_EVERGREEN_HAP_PATH || aclPath == PRECONFIG_EVERGREEN_HAP_PATH);
    EXPECT_TRUE(res);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_IsActiveWebEngineEvergreen_001, TestSize.Level1)
{
    OHOS::system::SetParameter("web.engine.enforce", "0");
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);
    EXPECT_FALSE(IsActiveWebEngineEvergreen());
    setActiveWebEngineVersion(ArkWebEngineVersion::M132);
    EXPECT_TRUE(IsActiveWebEngineEvergreen());
    setActiveWebEngineVersion(ArkWebEngineVersion::M114);
    EXPECT_FALSE(IsActiveWebEngineEvergreen());
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_EVERGREEN);
    EXPECT_TRUE(IsActiveWebEngineEvergreen());
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_SelectWebcoreBeforeProcessRun_001, TestSize.Level1) {
    std::string appBundleName = "com.example.app";
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));

    SelectWebcoreBeforeProcessRun(appBundleName);
    EXPECT_EQ(getActiveWebEngineVersion(), ArkWebEngineVersion::M132);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_SelectWebcoreBeforeProcessRun_002, TestSize.Level1) {
    std::string appBundleName = "com.example.app2";
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(0));

    SelectWebcoreBeforeProcessRun(appBundleName);
    EXPECT_EQ(getActiveWebEngineVersion(), ArkWebEngineVersion::M132);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

#if (defined(webview_arm64) && !defined(ASAN_DETECTOR))
HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_DlopenArkWebLib_001, TestSize.Level1) {
    auto flg = OHOS::system::GetParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, "false");
    OHOS::system::SetParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, "false");
    DlopenArkWebLib();
    OHOS::system::SetParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, flg);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_DlcloseArkWebLib_001, TestSize.Level1) {
    auto flg = OHOS::system::GetParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, "false");
    OHOS::system::SetParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, "false");
    int ret = DlcloseArkWebLib();
    EXPECT_EQ(ret, 0);
    OHOS::system::SetParameter(APPSPAWN_PRELOAD_ARKWEB_ENGINE, flg);
}
#endif

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessDefaultParam_001, TestSize.Level1)
{
    OHOS::system::SetParameter("web.engine.default", "0");
    std::string key = "web.engine.default";
    Json::Value value = 1000;
    ProcessDefaultParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 0);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessDefaultParam_002, TestSize.Level1)
{
    std::string key = "web.engine.default";
    Json::Value value = std::to_string(static_cast<int>(ArkWebEngineType::LEGACY));
    ProcessDefaultParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 0);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessDefaultParam_003, TestSize.Level1)
{
    std::string key = "web.engine.default";
    Json::Value value = static_cast<int>(ArkWebEngineType::LEGACY);
    ProcessDefaultParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 1);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessDefaultParam_004, TestSize.Level1)
{
    std::string key = "web.engine.default";
    Json::Value value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessDefaultParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 2);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessEnforceParam_001, TestSize.Level1)
{
    std::string key = "web.engine.enforce";
    Json::Value value = 1000;
    ProcessEnforceParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 0);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessEnforceParam_002, TestSize.Level1)
{
    std::string key = "web.engine.enforce";
    Json::Value value = std::to_string(static_cast<int>(ArkWebEngineType::LEGACY));
    ProcessEnforceParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 0);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessEnforceParam_003, TestSize.Level1)
{
    std::string key = "web.engine.enforce";
    Json::Value value = static_cast<int>(ArkWebEngineType::LEGACY);
    ProcessEnforceParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 0);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessEnforceParam_004, TestSize.Level1)
{
    std::string key = "web.engine.enforce";
    Json::Value value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessEnforceParam(value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 2);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessLegacyAppParam_001, TestSize.Level1)
{
    Json::Value value = "com.example.myapplication";
    ProcessLegacyAppParam(value);
    EXPECT_EQ(g_legacyApp, nullptr);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessLegacyAppParam_002, TestSize.Level1)
{
    std::string appBundleName = "com.example.myapplication";
    Json::Value value(Json::arrayValue);
    ProcessLegacyAppParam(value);
    EXPECT_NE(g_legacyApp, nullptr);
    EXPECT_EQ(g_legacyApp->find(appBundleName) != g_legacyApp->end(), false);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessLegacyAppParam_003, TestSize.Level1)
{
    std::string appBundleName = "com.example.myapplication";
    Json::Value value(Json::arrayValue);
    value.append(2);
    value.append("test");
    ProcessLegacyAppParam(value);
    EXPECT_NE(g_legacyApp, nullptr);
    EXPECT_EQ(g_legacyApp->find(appBundleName) != g_legacyApp->end(), false);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessLegacyAppParam_004, TestSize.Level1)
{
    std::string appBundleName = "com.example.myapplication";
    Json::Value value(Json::arrayValue);
    value.append(appBundleName);
    ProcessLegacyAppParam(value);
    EXPECT_NE(g_legacyApp, nullptr);
    EXPECT_EQ(g_legacyApp->find(appBundleName) != g_legacyApp->end(), true);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessParamItem_001, TestSize.Level1)
{
    std::string key = "Arkweb.engine.default";
    Json::Value value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessParamItem(key, value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 0);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessParamItem_002, TestSize.Level1)
{
    std::string key = "web.engine.test";
    Json::Value value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessParamItem(key, value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 0);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessParamItem_003, TestSize.Level1)
{
    std::string key = "web.engine.default";
    Json::Value value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessParamItem(key, value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 2);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessParamItem_004, TestSize.Level1)
{
    std::string key = "web.engine.enforce";
    Json::Value value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessParamItem(key, value);
    EXPECT_EQ(OHOS::system::GetIntParameter(key, 0), 2);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_ProcessParamItem_005, TestSize.Level1)
{
    std::string key = "web.engine.legacyApp";
    std::string appBundleName = "com.example.myapplication";
    Json::Value value(Json::arrayValue);
    value.append(appBundleName);
    ProcessParamItem(key, value);
    EXPECT_NE(g_legacyApp, nullptr);
    EXPECT_EQ(g_legacyApp->find(appBundleName) != g_legacyApp->end(), true);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetVersionString_001, TestSize.Level1)
{
    std::string systemParamVersionPath = "system_version_tmp_not_exist.txt";
    std::string systemParamVersionStr;
    EXPECT_EQ(GetVersionString(systemParamVersionPath, systemParamVersionStr), false);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_CheckCloudCfgVersion_001, TestSize.Level1)
{
    EXPECT_EQ(CheckCloudCfgVersion(systemTmpVersionFile_, updateTmpVersionFile_), false);

    std::string wrongVersionTxt = "version=1000.00.0.0";
    FileProcessor::WriteFile(systemTmpVersionFile_, wrongVersionTxt);
    EXPECT_EQ(CheckCloudCfgVersion(systemTmpVersionFile_, updateTmpVersionFile_), false);

    FileProcessor::WriteFile(updateTmpVersionFile_, wrongVersionTxt);
    EXPECT_EQ(CheckCloudCfgVersion(systemTmpVersionFile_, updateTmpVersionFile_), false);

    std::string versionTxt = "version=1.10.25.100";
    FileProcessor::WriteFile(systemTmpVersionFile_, versionTxt);
    EXPECT_EQ(CheckCloudCfgVersion(systemTmpVersionFile_, updateTmpVersionFile_), false);

    FileProcessor::WriteFile(updateTmpVersionFile_, versionTxt);
    EXPECT_EQ(CheckCloudCfgVersion(systemTmpVersionFile_, updateTmpVersionFile_), true);

    std::string higherVersionTxt = "version=1.10.25.101";
    FileProcessor::WriteFile(systemTmpVersionFile_, higherVersionTxt);
    EXPECT_EQ(CheckCloudCfgVersion(systemTmpVersionFile_, updateTmpVersionFile_), false);
}

HWTEST_F(ArkWebUtilsTest, ArkWebUtilsTest_GetArkwebLibPathForMock_001, TestSize.Level1)
{
    std::string res;
    res = GetArkwebLibPathForMock("");
    EXPECT_EQ(res, ARK_WEB_CORE_MOCK_HAP_LIB_PATH);
    std::string bundlePath = "/data/app/el1/bundle/public/com.ohos.arkwebcore";
    res = GetArkwebLibPathForMock(bundlePath);
    EXPECT_EQ(res, bundlePath + "/" + ARK_WEB_CORE_PATH_FOR_MOCK);
}
} // namespace OHOS::NWeb
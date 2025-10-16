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

#include "arkwebutils_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "arkweb_utils.cpp"
#include "parameters.h"

namespace OHOS::ArkWeb {
extern int g_cloudEnableAppVersion;
extern bool g_webEngineInitFlag;
namespace {
    constexpr uint8_t MAX_STRING_LENGTH = 64;
    constexpr int32_t TEST_NUMBER = 1000;
    constexpr int TEST_PARAMETER = 3;
    constexpr int RANDOM_NUM = 123;
}

bool ArkwebUtils001Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::string key = "web.engine.default";
    Json::Value value = TEST_NUMBER;
    ProcessDefaultParam(value);
    ProcessParamItem(key, value);
    value = std::to_string(static_cast<int>(ArkWebEngineType::LEGACY));
    ProcessDefaultParam(value);
    ProcessParamItem(key, value);
    value = static_cast<int>(ArkWebEngineType::LEGACY);
    ProcessDefaultParam(value);
    ProcessParamItem(key, value);
    value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessDefaultParam(value);
    ProcessParamItem(key, value);

    key = "web.engine.enforce";
    value = TEST_NUMBER;
    ProcessDefaultParam(value);
    ProcessParamItem(key, value);
    value = std::to_string(static_cast<int>(ArkWebEngineType::LEGACY));
    ProcessDefaultParam(value);
    ProcessParamItem(key, value);
    value = static_cast<int>(ArkWebEngineType::EVERGREEN);
    ProcessDefaultParam(value);
    ProcessParamItem(key, value);

    std::string systemParamVersionPath = "system_version_tmp_not_exist.txt";
    std::string systemParamVersionStr;
    GetVersionString(systemParamVersionPath, systemParamVersionStr);
    return true;
}

bool ArkwebUtils002Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    PreloadArkWebLibForBrowser();
    ArkWebEngineVersion version = ArkWebEngineVersion::SYSTEM_DEFAULT;
    setActiveWebEngineVersion(version);
    SetActiveWebEngineVersionInner(version);
    version = ArkWebEngineVersion::M114;
    setActiveWebEngineVersion(version);
    SetActiveWebEngineVersionInner(version);
    version = ArkWebEngineVersion::M132;
    setActiveWebEngineVersion(version);
    SetActiveWebEngineVersionInner(version);
    version = ArkWebEngineVersion::SYSTEM_EVERGREEN;
    setActiveWebEngineVersion(version);
    SetActiveWebEngineVersionInner(version);

    getActiveWebEngineVersion();
    CalculateActiveWebEngineVersion();
    getActiveWebEngineType();

    version = ArkWebEngineVersion::M114;
    SetActiveWebEngineVersionInner(version);
    IsActiveWebEngineEvergreen();

    version = ArkWebEngineVersion::M132;
    SetActiveWebEngineVersionInner(version);
    IsActiveWebEngineEvergreen();

    std::string testmsg = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    const char* msg = testmsg.c_str();
    LogForUnsupportedFunc(version, msg);
    GetArkwebLibPath();
    GetArkwebRelativePathForMock();
    GetArkwebNameSpace();
    GetArkwebRelativePathForBundle();
    GetArkwebRelativePathForMock();
    GetArkwebInstallPath();
    return true;
}

bool ArkwebUtils003Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    SetActiveWebEngineVersionInner(ArkWebEngineVersion::M114);
    GetArkwebInstallPath();
    GetArkwebNameSpace();
    GetArkwebLibPath();
    SetActiveWebEngineVersionInner(ArkWebEngineVersion::M132);
    int openMode = 1;
    std::string libFilePath = "/code/base/";
    bool isPrintLog = true;
    ArkWebBridgeHelperLoadLibFile(openMode, libFilePath, isPrintLog);
    isPrintLog = false;
    ArkWebBridgeHelperLoadLibFile(openMode, libFilePath, isPrintLog);

    SetIntParameter("web.engine.enforce", 1);
    ArkWebEngineVersion version = ArkWebEngineVersion::SYSTEM_EVERGREEN;
    setActiveWebEngineVersion(version);
    version = ArkWebEngineVersion::M132;
    setActiveWebEngineVersion(version);
    version = ArkWebEngineVersion::SYSTEM_DEFAULT;
    g_cloudEnableAppVersion = static_cast<int>(ArkWebEngineType::LEGACY);
    setActiveWebEngineVersion(version);
    g_cloudEnableAppVersion = static_cast<int>(ArkWebEngineType::EVERGREEN);
    SetIntParameter("web.engine.default", TEST_PARAMETER);
    setActiveWebEngineVersion(version);
    SetIntParameter("web.engine.default", static_cast<int>(ArkWebEngineType::EVERGREEN));
    setActiveWebEngineVersion(version);
    g_webEngineInitFlag = true;
    version = ArkWebEngineVersion::SYSTEM_DEFAULT;
    setActiveWebEngineVersion(version);
    g_webEngineInitFlag = false;
    std::string appBundleName = "com.example.app";
    SelectWebcoreBeforeProcessRun(appBundleName);
    return true;
}

bool ArkwebUtils004Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    Json::Value value = "not an array";
    ProcessLegacyAppParam(value);

    Json::Value value1 = Json::arrayValue;
    value1.append("valid_string");
    ProcessLegacyAppParam(value1);

    Json::Value value2 = Json::arrayValue;
    value2.append(Json::Value(RANDOM_NUM));
    ProcessLegacyAppParam(value2);
    return true;
}

bool ArkwebUtils005Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    Json::Value root = Json::nullValue;
    ProcessJsonConfig(root);

    root = Json::Value(RANDOM_NUM);
    ProcessJsonConfig(root);
    return true;
}

bool ArkwebUtils006Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::string versionStr1 = "version = 1.2.3.4";
    long long versionNum1 = 0;
    HandleVersionString(versionStr1, versionNum1);

    std::string versionStr2 = "version = 1.2.3";
    long long versionNum2 = 1;
    HandleVersionString(versionStr2, versionNum2);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ArkWeb::ArkwebUtils001Test(data, size);
    OHOS::ArkWeb::ArkwebUtils002Test(data, size);
    OHOS::ArkWeb::ArkwebUtils003Test(data, size);
    OHOS::ArkWeb::ArkwebUtils004Test(data, size);
    OHOS::ArkWeb::ArkwebUtils005Test(data, size);
    OHOS::ArkWeb::ArkwebUtils006Test(data, size);
    return 0;
}
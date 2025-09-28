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
namespace {
    constexpr uint8_t MAX_STRING_LENGTH = 64;
    constexpr int32_t TEST_NUMBER = 1000;
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
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::ArkWeb::ArkwebUtils001Test(data, size);
    OHOS::ArkWeb::ArkwebUtils002Test(data, size);
    return 0;
}
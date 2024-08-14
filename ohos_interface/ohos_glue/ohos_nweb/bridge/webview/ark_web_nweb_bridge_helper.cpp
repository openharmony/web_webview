/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_nweb_bridge_helper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

const std::string LIB_FILE_NAME = "libweb_engine.so";

#if defined(webview_arm64)
const std::string RELATIVE_PATH_FOR_MOCK = "libs/arm64";
const std::string RELATIVE_PATH_FOR_BUNDLE = "nweb/libs/arm64";
#elif defined(webview_x86_64)
const std::string RELATIVE_PATH_FOR_MOCK = "libs/x86_64";
const std::string RELATIVE_PATH_FOR_BUNDLE = "nweb/libs/x86_64";
#else
const std::string RELATIVE_PATH_FOR_MOCK = "libs/arm";
const std::string RELATIVE_PATH_FOR_BUNDLE = "nweb/libs/arm";
#endif

ArkWebNWebBridgeHelper& ArkWebNWebBridgeHelper::GetInstance()
{
    static ArkWebNWebBridgeHelper helper;
    return helper;
}

bool ArkWebNWebBridgeHelper::InitArkWeb(
    bool runMode, const std::string& baseDir,
    const std::string& relativeLibPath, const std::string& arkWebEngineSo)
{
    std::string libDirPath = baseDir + "/" + RELATIVE_PATH_FOR_MOCK;
    if (runMode) {
        libDirPath = baseDir + "/" + relativeLibPath;
    }
#ifdef __MUSL__
    return LoadLibFile(RTLD_NOW | RTLD_GLOBAL, "nweb_ns", libDirPath, arkWebEngineSo);
#else
    std::string libFilePath = libDirPath + "/" + arkWebEngineSo;
    return LoadLibFile(RTLD_NOW, libFilePath);
#endif
}

bool ArkWebNWebBridgeHelper::Init(bool runMode, const std::string& baseDir)
{
#ifdef __MUSL__
    std::string libDirPath = GetDirPath(runMode, baseDir);
    return LoadLibFile(RTLD_NOW | RTLD_GLOBAL, "nweb_ns", libDirPath, LIB_FILE_NAME);
#else
    std::string dirPath = GetDirPath(runMode, baseDir);
    std::string libFilePath = dirPath + "/" + LIB_FILE_NAME;
    return LoadLibFile(RTLD_NOW, libFilePath);
#endif
}

std::string ArkWebNWebBridgeHelper::GetDirPath(bool runMode, const std::string& baseDir)
{
    if (runMode) {
        return baseDir + "/" + RELATIVE_PATH_FOR_BUNDLE;
    }

    return baseDir + "/" + RELATIVE_PATH_FOR_MOCK;
}

} // namespace OHOS::ArkWeb

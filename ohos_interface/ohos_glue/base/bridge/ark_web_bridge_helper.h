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

#ifndef ARK_WEB_BRIDGE_HELPER_H_
#define ARK_WEB_BRIDGE_HELPER_H_
#pragma once

#include <dlfcn.h>

#include "base/include/ark_web_bridge_types.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

extern int g_ark_web_init_addr;
#define ARK_WEB_INIT_ADDR &g_ark_web_init_addr

#if defined(OHOS_WEBVIEW_GLUE)
#if defined(webview_arm64)
const std::string WEBVIEW_RELATIVE_PATH_FOR_MOCK = "libs/arm64";
const std::string WEBVIEW_RELATIVE_PATH_FOR_BUNDLE = "arkwebcore/libs/arm64";
#elif defined(webview_x86_64)
const std::string WEBVIEW_RELATIVE_PATH_FOR_MOCK = "libs/x86_64";
const std::string WEBVIEW_RELATIVE_PATH_FOR_BUNDLE = "arkwebcore/libs/x86_64";
#else
const std::string WEBVIEW_RELATIVE_PATH_FOR_MOCK = "libs/arm";
const std::string WEBVIEW_RELATIVE_PATH_FOR_BUNDLE = "arkwebcore/libs/arm";
#endif
#endif

using ArkWebMemberCheckFunc = void* (*)(ArkWebBridgeType, const ArkWebString*);

class ArkWebBridgeHelper {
public:
    virtual ~ArkWebBridgeHelper();

    void* LoadFuncSymbol(const char* funcName, bool isPrintLog = true);

    void RegisterFuncMember(ArkWebBridgeType bridgeType, const std::map<std::string, void*>& funcMemberMap);

    void* CheckFuncMemberForCalled(ArkWebBridgeType bridgeType, const std::string& funcName);

    void* CheckFuncMemberForCaller(ArkWebBridgeType bridgeType, const std::string& funcName);

protected:
    ArkWebBridgeHelper() = default;

    bool LoadLibFile(int openMode, const std::string& libFilePath, bool isPrintLog = true);

#if defined(OHOS_WEBVIEW_GLUE)
    bool LoadLibFile(int openMode, const std::string& libNsName, const std::string& libDirPath,
        const std::string& libFileName, bool isPrintLog = true);
#endif

    static void PrereadLibFile(const std::string& libFilePath, bool isPrintLog = true);

    void InitFuncMemberMaps(ArkWebBridgeType init, ArkWebBridgeType butt, bool isPrintLog = true);

private:
    void UnloadLibFile();

protected:
    ArkWebMemberCheckFunc memberCheckFunc_ = nullptr;
    std::map<int, std::map<std::string, void*>> funcMemberMaps_;

private:
    void* libFileHandler_ = nullptr;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_BRIDGE_HELPER_H_

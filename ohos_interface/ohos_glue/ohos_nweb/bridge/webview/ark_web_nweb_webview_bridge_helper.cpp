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

#include "ohos_nweb/bridge/ark_web_nweb_webview_bridge_helper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

const std::string NWEB_LIB_FILE_NAME = "libarkweb_engine.so";

ArkWebNWebWebviewBridgeHelper::ArkWebNWebWebviewBridgeHelper()
{
    InitFuncMemberMaps(ARK_WEB_NWEB_INTERFACE_INIT, ARK_WEB_NWEB_INTERFACE_BUTT);
}

bool ArkWebNWebWebviewBridgeHelper::Init(bool runMode, const std::string& bundlePath)
{
    std::string libDirPath;
    if (runMode) {
        libDirPath = bundlePath + "/" + WEBVIEW_RELATIVE_PATH_FOR_BUNDLE;
    } else {
        libDirPath = bundlePath + "/" + WEBVIEW_RELATIVE_PATH_FOR_MOCK;
    }

#ifdef __MUSL__
    if (!LoadLibFile(RTLD_NOW | RTLD_GLOBAL, "nweb_ns", libDirPath, NWEB_LIB_FILE_NAME)) {
#else
    if (!LoadLibFile(RTLD_NOW, libDirPath + "/" + NWEB_LIB_FILE_NAME)) {
#endif
        return false;
    }

    memberCheckFunc_ =
        reinterpret_cast<ArkWebMemberCheckFunc>(LoadFuncSymbol("ark_web_nweb_webcore_check_func_static"));
    if (!memberCheckFunc_) {
        ARK_WEB_BRIDGE_INFO_LOG("failed to load func ark_web_nweb_webcore_check_func_static");
    }
    return true;
}

ArkWebNWebWebviewBridgeHelper& ArkWebNWebWebviewBridgeHelper::GetInstance()
{
    static ArkWebNWebWebviewBridgeHelper helper;
    return helper;
}

void ArkWebNWebWebviewBridgeHelper::PreloadLibFile(bool runMode, const std::string& bundlePath)
{
    std::string libFilePath;
    if (runMode) {
        libFilePath = bundlePath + "/" + WEBVIEW_RELATIVE_PATH_FOR_BUNDLE + "/" + NWEB_LIB_FILE_NAME;
    } else {
        libFilePath = bundlePath + "/" + WEBVIEW_RELATIVE_PATH_FOR_MOCK + "/" + NWEB_LIB_FILE_NAME;
    }

    ArkWebBridgeHelper::PrereadLibFile(libFilePath);
}

void ArkWebNWebWebviewBridgeHelper::PreDlopenLibFile(const std::string& bundlePath)
{
    std::string libDirPath = bundlePath + "/" + WEBVIEW_RELATIVE_PATH_FOR_BUNDLE;
#ifdef __MUSL__
    LoadLibFile(RTLD_NOW | RTLD_GLOBAL, "nweb_ns", libDirPath, NWEB_LIB_FILE_NAME);
#else
    LoadLibFile(RTLD_NOW, libDirPath + "/" + NWEB_LIB_FILE_NAME);
#endif
}

} // namespace OHOS::ArkWeb

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ARK_WEB_EXPORT
void* ark_web_nweb_webview_check_func_static(ArkWebBridgeType bridgeType, const ArkWebString* funcName)
{
    if (!funcName) {
        ARK_WEB_BRIDGE_INFO_LOG("func name is nullptr");
        return nullptr;
    }

    return OHOS::ArkWeb::ArkWebNWebWebviewBridgeHelper::GetInstance().CheckFuncMemberForCalled(
        bridgeType, ArkWebStringStructToClass(*funcName));
}

#ifdef __cplusplus
}
#endif // __cplusplus

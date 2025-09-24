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
#include "arkweb_utils.h"

namespace OHOS::ArkWeb {

const std::string NWEB_LIB_FILE_NAME = "libarkweb_engine.so";

ArkWebNWebWebviewBridgeHelper::ArkWebNWebWebviewBridgeHelper()
{
    InitFuncMemberMaps(ARK_WEB_NWEB_INTERFACE_INIT, ARK_WEB_NWEB_INTERFACE_BUTT);
}

bool ArkWebNWebWebviewBridgeHelper::Init(bool runMode, const std::string& bundlePath)
{
    ARK_WEB_BRIDGE_INFO_LOG("init load resources bundle path %{public}s", bundlePath.c_str());
    if (libFileHandler_) {
        ARK_WEB_BRIDGE_INFO_LOG("library resources have been loaded");
        return true;
    }

    void* libFileHandler = ArkWebBridgeHelperSharedInit(runMode, bundlePath);
    if (!libFileHandler) {
        ARK_WEB_BRIDGE_ERROR_LOG("library resources loaded failed");
        return false;
    }

    libFileHandler_ = libFileHandler;

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
        libFilePath = bundlePath + "/" + GetArkwebRelativePathForBundle() + "/" + NWEB_LIB_FILE_NAME;
    } else {
        libFilePath = bundlePath + "/" + GetArkwebRelativePathForMock() + "/" + NWEB_LIB_FILE_NAME;
    }

    ArkWebBridgeHelper::PrereadLibFile(libFilePath);
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

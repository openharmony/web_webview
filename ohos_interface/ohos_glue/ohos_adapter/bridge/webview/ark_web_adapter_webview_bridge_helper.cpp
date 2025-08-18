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

#include "ohos_adapter/bridge/ark_web_adapter_webview_bridge_helper.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "arkweb_utils.h"

namespace OHOS::ArkWeb {

const std::string ADAPTER_LIB_FILE_NAME = "libarkweb_engine.so";

ArkWebAdapterWebviewBridgeHelper::ArkWebAdapterWebviewBridgeHelper()
{
    Init();

    InitFuncMemberMaps(ARK_WEB_ADAPTER_INTERFACE_INIT, ARK_WEB_ADAPTER_INTERFACE_BUTT);
}

void ArkWebAdapterWebviewBridgeHelper::Init()
{
    if (libFileHandler_) {
        ARK_WEB_BRIDGE_INFO_LOG("library resources have been loaded");
        return;
    }

    void* libFileHandler = ArkWebBridgeHelperSharedInit(false);
    if (!libFileHandler) {
        ARK_WEB_BRIDGE_ERROR_LOG("library resources loaded failed");
        return;
    }

    libFileHandler_ = libFileHandler;

    memberCheckFunc_ =
        reinterpret_cast<ArkWebMemberCheckFunc>(LoadFuncSymbol("ark_web_adapter_webcore_check_func_static"));
    if (!memberCheckFunc_) {
        ARK_WEB_BRIDGE_INFO_LOG("failed to load func ark_web_adapter_webcore_check_func_static");
    }
    return;
}

ArkWebAdapterWebviewBridgeHelper& ArkWebAdapterWebviewBridgeHelper::GetInstance()
{
    static ArkWebAdapterWebviewBridgeHelper helper;
    return helper;
}

} // namespace OHOS::ArkWeb

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ARK_WEB_EXPORT
void* ark_web_adapter_webview_check_func_static(ArkWebBridgeType bridgeType, const ArkWebString* funcName)
{
    if (!funcName) {
        ARK_WEB_BRIDGE_INFO_LOG("func name is nullptr");
        return nullptr;
    }

    return OHOS::ArkWeb::ArkWebAdapterWebviewBridgeHelper::GetInstance().CheckFuncMemberForCalled(
        bridgeType, ArkWebStringStructToClass(*funcName));
}

#ifdef __cplusplus
}
#endif // __cplusplus

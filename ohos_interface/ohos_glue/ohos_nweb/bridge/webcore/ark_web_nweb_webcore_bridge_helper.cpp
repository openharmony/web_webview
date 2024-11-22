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

#include "ohos_nweb/bridge/ark_web_nweb_webcore_bridge_helper.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "base/check.h"

namespace OHOS::ArkWeb {

const std::string NWEB_LIB_FILE_NAME = "libarkweb_core_loader_glue.z.so";

ArkWebNWebWebcoreBridgeHelper::ArkWebNWebWebcoreBridgeHelper()
{
    Init();

    InitFuncMemberMaps(ARK_WEB_NWEB_INTERFACE_INIT, ARK_WEB_NWEB_INTERFACE_BUTT, false);
}

void ArkWebNWebWebcoreBridgeHelper::Init()
{
    CHECK(LoadLibFile(RTLD_LAZY, NWEB_LIB_FILE_NAME, false));

    memberCheckFunc_ =
        reinterpret_cast<ArkWebMemberCheckFunc>(LoadFuncSymbol("ark_web_nweb_webview_check_func_static", false));
}

ArkWebNWebWebcoreBridgeHelper& ArkWebNWebWebcoreBridgeHelper::GetInstance()
{
    static ArkWebNWebWebcoreBridgeHelper helper;
    return helper;
}

} // namespace OHOS::ArkWeb

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

ARK_WEB_EXPORT
void* ark_web_nweb_webcore_check_func_static(ArkWebBridgeType bridgeType, const ArkWebString* funcName)
{
    if (!funcName) {
        ARK_WEB_BRIDGE_INFO_LOG("func name is nullptr");
        return nullptr;
    }

    return OHOS::ArkWeb::ArkWebNWebWebcoreBridgeHelper::GetInstance().CheckFuncMemberForCalled(
        bridgeType, ArkWebStringStructToClass(*funcName));
}

#ifdef __cplusplus
}
#endif // __cplusplus

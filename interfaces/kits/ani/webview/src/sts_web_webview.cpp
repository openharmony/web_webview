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

#include <ani.h>
#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "ani_native_media_player_handler.h"
#include "ani_webview_controller.h"
#include "ani_proxy_config.h"
#include "ani_web_storage.h"
#include "ani_web_data_base.h"
#include "ani_webview_createpdf_execute_callback.h"
#include "nweb_log.h"
#include "ani_geolocation_permission.h"
#include "ani_web_adsblock_manager.h"
#include "webview_javascript_execute_callback.h"
#include "ani_webview_function.h"
#include "ani_web_scheme_handler.h"
#include "ani_web_scheme_handler_resource.h"
#include "ani_web_scheme_handler_request.h"

namespace OHOS {
namespace NWeb {

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    WVLOG_I("Webview ANI_Constructor");
    ani_env *env = nullptr;
    ani_status status = vm->GetEnv(ANI_VERSION_1, &env);
    if (status != ANI_OK || !env) {
        WVLOG_E("ANI_Constructor GetEnv status: %{public}d", status);
        return ANI_NOT_FOUND;
    }

    StsWebviewControllerInit(env);
    StsWebMessageExtInit(env);
    StsBackForwardListInit(env);
    StsWebMessagePortInit(env);
    StsWebSchemeHandlerResponseInit(env);
    StsWebDownloadDelegateInit(env);
    StsWebDownLoadItemInit(env);
    StsWebDownloadManagerInit(env);
    StsWebCookieManagerInit(env);
    StsWebProxyConfigInit(env);
    StsCleanerInit(env);
    StsWebStorageInit(env);
    StsNativeMediaPlayerHandlerinnerInit(env);
    StsWebAdsBlockManagerInit(env);
    StsWebDataBaseInit(env);
    StsPdfDataInit(env);
    StsGeolocationPermissionInit(env);
    StsJsMessageExtInit(env);
    StsWebviewFunctionInit(env);
    StsWebSchemeHandlerResourceInit(env);
    StsWebSchemeHandlerRequestInit(env);
    StsWebSchemeHandlerInit(env);
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}

} // namespace NWeb
} // namespace OHOS
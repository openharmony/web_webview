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

#ifndef OHOS_NWEB_ANI_WEBVIEW_CONTROLLER_H
#define OHOS_NWEB_ANI_WEBVIEW_CONTROLLER_H

#include <ani.h>
#include "uv.h"
#include "nweb_value_callback.h"

namespace OHOS {
namespace NWeb {
struct OfflineResourceValueAni {
    ani_ref urlListArray;
    ani_ref resourceArrayBuffer;
    ani_ref responseHeadersArray;
    ani_ref typeRef;
};

ani_status StsWebviewControllerInit(ani_env *env);
ani_status StsWebMessageExtInit(ani_env *env);
ani_status StsCleanerInit(ani_env *env);
ani_status StsBackForwardListInit(ani_env *env);
ani_status StsWebMessagePortInit(ani_env *env);
ani_status StsWebSchemeHandlerResponseInit(ani_env *env);
ani_status StsWebDownloadDelegateInit(ani_env *env);
ani_status StsWebDownLoadItemInit(ani_env *env);
ani_status StsWebDownloadManagerInit(ani_env *env);
ani_status StsWebCookieManagerInit(ani_env *env);
ani_status StsNativeMediaPlayerHandlerinnerInit(ani_env *env);
ani_status StsWebSchemeHandlerResourceInit(ani_env *env);
ani_status StsWebSchemeHandlerRequestInit(ani_env *env);
ani_status StsWebSchemeHandlerInit(ani_env *env);
} // namespace NWeb
} // namespace OHOS
#endif // OHOS_NWEB_ANI_WEBVIEW_CONTROLLER_H
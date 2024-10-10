/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_NAPI_WEB_DOWNLOAD_DELEGATE_H
#define NWEB_NAPI_WEB_DOWNLOAD_DELEGATE_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "web_download_delegate.h"

namespace OHOS {
namespace NWeb {

class NapiWebDownloadDelegate {
public:
    static napi_value Init(napi_env env, napi_value exports);

    NapiWebDownloadDelegate() = default;
    ~NapiWebDownloadDelegate() = default;

    static napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);

    static napi_value JS_DownloadBeforeStart(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_DownloadDidUpdate(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_DownloadDidFinish(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_DownloadDidFail(napi_env env, napi_callback_info cbinfo);
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEB_DOWNLOAD_DELEGATE_H
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

#ifndef NWEB_NAPI_WEB_ASYNC_CONTROLLER_H
#define NWEB_NAPI_WEB_ASYNC_CONTROLLER_H

#include "hilog/log.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
// The asynchronous interface of NWeb is implemented through this interface.
class NapiWebAsyncController {
public:
    NapiWebAsyncController(napi_env env, napi_value this_var, int32_t nweb_id);
    ~NapiWebAsyncController() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JS_NapiWebAsyncController(napi_env env, napi_callback_info info);
    static napi_value JS_StoreWebArchive(napi_env env, napi_callback_info cbinfo);

    static napi_value StoreWebArchiveInternal(napi_env env, napi_callback_info cbinfo, const std::string &baseName,
        bool autoName);

    // Saves the current view as a web archive by way of callback.
    void StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env, napi_ref callback);
    // Saves the current view as a web archive by way of promise.
    void StoreWebArchivePromise(const std::string &baseName, bool autoName, napi_env env, napi_deferred deferred);

    int32_t nweb_id_ { -1 };
};
} // namespace OHOS

#endif // NWEB_NAPI_WEB_ASYNC_CONTROLLER_H

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_NAPI_WEB_STORAGE_H
#define NWEB_NAPI_WEB_STORAGE_H

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "node_api_types.h"

namespace OHOS {
struct GetOriginUsageOrQuotaParam {
    int retValue;
    bool isQuato;
    std::string origin;
    napi_ref jsStringRef;
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callbackRef;
    napi_status status;
    int errCode;
};

struct NapiWebStorageOrigin {
    std::string origin;
    int64_t quota;
    int64_t usage;
};

struct GetOriginsParam {
    std::vector<NapiWebStorageOrigin> origins;
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callbackRef;
    napi_status status;
    int errCode;
};

class NapiWebStorage {
public:
    NapiWebStorage() {}
    ~NapiWebStorage() = default;
    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsDeleteAllData(napi_env env, napi_callback_info info);
    static napi_value JsDeleteOrigin(napi_env env, napi_callback_info info);
    static napi_value GetErrorCodeValue(napi_env env, int errCode);
    static void ExecuteGetOrigins(napi_env env, void *data);
    static void GetNapiWebStorageOriginForResult(napi_env env,
        const std::vector<NapiWebStorageOrigin> &info, napi_value result);
    static void GetOriginComplete(napi_env env, napi_status status, void *data);
    static void GetOriginsPromiseComplete(napi_env env, napi_status status, void *data);
    static napi_value GetOriginsAsync(napi_env env, napi_value *argv);
    static napi_value GetOriginsPromise(napi_env env);
    static napi_value JsGetOrigins(napi_env env, napi_callback_info info);
    static void ExecuteGetOriginUsageOrQuota(napi_env env, void *data);
    static void GetOriginUsageOrQuotaComplete(napi_env env, napi_status status, void *data);
    static void GetOriginUsageOrQuotaPromiseComplete(napi_env env, napi_status status, void *data);
    static napi_value GetOriginUsageOrQuotaAsync(napi_env env,
        napi_value *argv, const std::string& origin, bool isQuato);
    static napi_value GetOriginUsageOrQuotaPromise(napi_env env,
        napi_value *argv, const std::string& origin, bool isQuato);
    static napi_value JsGetOriginUsageOrQuota(napi_env env, napi_callback_info info, bool isQuato);
    static napi_value JsGetOriginQuota(napi_env env, napi_callback_info info);
    static napi_value JsGetOriginUsage(napi_env env, napi_callback_info info);
    static napi_value JsConstructor(napi_env env, napi_callback_info info);
};
} // namespace OHOS

#endif // NWEB_NAPI_WEB_STORAGE_H

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

#ifndef NWEB_NAPI_WEB_DATA_BASE_H
#define NWEB_NAPI_WEB_DATA_BASE_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
const std::string WEB_DATA_BASE_CLASS_NAME = "WebDataBase";
constexpr int MAX_STRING_LENGTH = 40960;

class NapiWebDataBase {
public:
    NapiWebDataBase() {}

    ~NapiWebDataBase() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsDeleteHttpAuthCredentials(napi_env env, napi_callback_info info);

    static napi_value JsSaveHttpAuthCredentials(napi_env env, napi_callback_info info);

    static napi_value JsGetHttpAuthCredentials(napi_env env, napi_callback_info info);

    static napi_value JsExistHttpAuthCredentials(napi_env env, napi_callback_info info);

    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static bool GetStringPara(napi_env env, napi_value argv, std::string& outValue);

    static bool GetSize(napi_env env, napi_value argv, size_t& outValue);

    static bool GetCharPara(napi_env env, napi_value argv, char* buffer, size_t bufferSize);
};
} // namespace OHOS

#endif // NWEB_NAPI_WEB_DATA_BASE_H
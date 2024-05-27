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
#ifndef BUSINESS_ERROR_H
#define BUSINESS_ERROR_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NWebError {
class BusinessError {
public:
static inline void ThrowError(napi_env env, int32_t err, const std::string& msg)
{
    napi_throw_error(env, std::to_string(err).c_str(), msg.c_str());
}

static napi_value CreateError(napi_env env, int32_t err);

static void ThrowErrorByErrcode(napi_env env, int32_t errCode);

static void ThrowErrorByErrcode(napi_env env, int32_t errCode, const std::string& errorMsg);
};
}
}
#endif
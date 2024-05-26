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

#include "business_error.h"

#include "web_errors.h"

namespace OHOS {
namespace NWebError {
napi_value BusinessError::CreateError(napi_env env, int32_t err)
{
    napi_value businessError = nullptr;
    NAPI_CALL(env, napi_create_object(env, &businessError));
    napi_value errorCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, err, &errorCode));
    napi_value errorMessage = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, GetErrMsgByErrCode(err).c_str(), NAPI_AUTO_LENGTH, &errorMessage));
    NAPI_CALL(env, napi_set_named_property(env, businessError, "code", errorCode));
    NAPI_CALL(env, napi_set_named_property(env, businessError, "message", errorMessage));
    return businessError;
}

void BusinessError::ThrowErrorByErrcode(napi_env env, int32_t errCode)
{
    napi_throw_error(env, std::to_string(errCode).c_str(), GetErrMsgByErrCode(errCode).c_str());
}

void BusinessError::ThrowErrorByErrcode(napi_env env, int32_t errCode, const std::string& errorMsg)
{
    napi_throw_error(env, std::to_string(errCode).c_str(), errorMsg.c_str());
}
}
}
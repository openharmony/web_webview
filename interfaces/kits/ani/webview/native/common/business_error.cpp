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
namespace {
ani_object WrapBusinessError(ani_env *env, const std::string& msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        WVLOG_E("null env");
        return nullptr;
    }

    ani_string aniMsg = nullptr;
    if ((status = env->String_NewUTF8(msg.c_str(), msg.size(), &aniMsg)) != ANI_OK) {
        WVLOG_E("String_NewUTF8 failed %{public}d", status);
        return nullptr;
    }

    ani_ref undefRef;
    if ((status = env->GetUndefined(&undefRef)) != ANI_OK) {
        WVLOG_E("GetUndefined failed %{public}d", status);
        return nullptr;
    }

    if ((status = env->FindClass("escompat.Error", &cls)) != ANI_OK) {
        WVLOG_E("FindClass failed %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "C{std.core.String}C{escompat.ErrorOptions}:", &method)) !=
        ANI_OK) {
        WVLOG_E("Class_FindMethod failed %{public}d", status);
        return nullptr;
    }

    if ((status = env->Object_New(cls, method, &obj, aniMsg, undefRef)) != ANI_OK) {
        WVLOG_E("Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}

ani_object CreateBusinessError(ani_env *env, ani_int code, const std::string& msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        WVLOG_E("null env");
        return nullptr;
    }
    if ((status = env->FindClass("@ohos.base.BusinessError", &cls)) != ANI_OK) {
        WVLOG_E("FindClass failed %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "iC{escompat.Error}:", &method)) != ANI_OK) {
        WVLOG_E("Class_FindMethod failed %{public}d", status);
        return nullptr;
    }
    ani_object error = WrapBusinessError(env, msg);
    if (error == nullptr) {
        WVLOG_E("error null");
        return nullptr;
    }
    ani_int dCode(code);
    if ((status = env->Object_New(cls, method, &obj, dCode, error)) != ANI_OK) {
        WVLOG_E("Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}
}

ani_status AniBusinessErrorError::ThrowError(ani_env *env, int32_t errorCode, const std::string& error_message)
{
    if (env == nullptr) {
        WVLOG_E("null env");
        return ANI_ERROR;
    }
    ani_object err = CreateBusinessError(env, errorCode, error_message);
    if (err == nullptr) {
        WVLOG_E("err null");
        return ANI_ERROR;
    }
    env->ThrowError(static_cast<ani_error>(err));
    return ANI_OK;
}

ani_status AniBusinessErrorError::ThrowErrorByErrCode(ani_env *env, int32_t errorCode)
{
    return ThrowError(env, errorCode, GetErrMsgByErrCode(errorCode));
}

ani_ref AniBusinessErrorError::CreateError(ani_env* env, int32_t err)
{
    std::string errMsg = GetErrMsgByErrCode(err);
    return CreateBusinessError(env, err, errMsg);
}

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
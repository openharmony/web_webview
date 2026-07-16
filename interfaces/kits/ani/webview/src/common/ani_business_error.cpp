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

#include "ani_business_error.h"

#include "web_errors.h"
#include "nweb_log.h"

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
        WVLOG_E("error nulll");
        return nullptr;
    }
    if ((status = env->Object_New(cls, method, &obj, code, error)) != ANI_OK) {
        WVLOG_E("Object_New failed %{public}d", status);
        return nullptr;
    }
    return obj;
}
}

ani_status AniBusinessError::ThrowError(ani_env *env, int32_t errorCode, const std::string& error_message)
{
    ani_object err = CreateBusinessError(env, errorCode, error_message);
    if (err == nullptr) {
        WVLOG_E("err nulll");
        return ANI_ERROR;
    }
    env->ThrowError(static_cast<ani_error>(err));
    return ANI_OK;
}

ani_status AniBusinessError::ThrowErrorByErrCode(ani_env *env, int32_t errorCode)
{
    return ThrowError(env, errorCode, GetErrMsgByErrCode(errorCode));
}

ani_ref AniBusinessError::CreateError(ani_env* env, int32_t err)
{
    std::string errMsg = GetErrMsgByErrCode(err);
    return CreateBusinessError(env, err, errMsg);
}

ani_ref AniBusinessError::CreateError(ani_env* env, int32_t err, const std::string& errorMsg)
{
    return CreateBusinessError(env, err, errorMsg);
}
}
}

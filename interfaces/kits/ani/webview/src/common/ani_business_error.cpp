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
ani_status AniBusinessError::ThrowError(ani_env *env, int errorCode, const std::string& error_message)
{
    ani_status status;
    ani_class errCls;
    const char* className = "L@koalaui/arkts-arkui/generated/ArkBaseInterfaces/BusinessError;";
    if ((status = env->FindClass(className, &errCls)) != ANI_OK) {
        return ANI_ERROR;
    }
    ani_method errCtor;
    if ((status = env->Class_FindMethod(errCls, "<ctor>", ":V", &errCtor)) != ANI_OK) {
        return ANI_ERROR;
    }
    ani_object errObj;
    if ((status = env->Object_New(errCls, errCtor, &errObj)) != ANI_OK) {
        return ANI_ERROR;
    }
    ani_field code_field = nullptr;
    if ((status = env->Class_FindField(errCls, "code", &code_field)) != ANI_OK) {
        return ANI_ERROR;
    }
    if ((status = env->Object_SetField_Double(errObj, code_field, errorCode)) != ANI_OK) {
        return ANI_ERROR;
    }
    ani_field message_field = nullptr;
    if ((status = env->Class_FindField(errCls, "message", &message_field)) != ANI_OK) {
        return ANI_ERROR;
    }
    ani_string ani_str_errorMsg;
    if ((status = env->String_NewUTF8(error_message.c_str(), error_message.size(), &ani_str_errorMsg)) != ANI_OK) {
        return ANI_ERROR;
    }
    if ((status = env->Object_SetField_Ref(errObj, message_field, ani_str_errorMsg)) != ANI_OK) {
        return ANI_ERROR;
    }
    env->ThrowError(static_cast<ani_error>(errObj));
    return ANI_OK;
}

ani_status AniBusinessError::ThrowErrorByErrCode(ani_env *env, int errorCode)
{
    return ThrowError(env, errorCode, GetErrMsgByErrCode(errorCode));
}
}
}

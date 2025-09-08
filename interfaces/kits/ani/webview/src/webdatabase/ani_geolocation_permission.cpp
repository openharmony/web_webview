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

 #include "ani_geolocation_permission.h"

 #include <cstddef>
 #include <cstdint>
 #include <vector>
 
 #include "ani_business_error.h"
 #include "ani_parse_utils.h"
 #include "nweb_data_base.h"
 #include "nweb_helper.h"
 #include "nweb_log.h"
 #include "securec.h"
 #include "web_errors.h"
 
 namespace {
 constexpr int32_t ALLOW_PERMISSION_OPERATION = 1;
 constexpr int32_t DELETE_PERMISSION_OPERATION = 2;
 } // namespace
 
 namespace OHOS {
 namespace NWeb {
 using namespace NWebError;
 using NWebError::NO_ERROR;
 
 namespace {
 const char* GEOLOCATION_PERMISSSION_CLASS_NAME = "L@ohos/web/webview/webview/GeolocationPermissions;";
 } // namespace

 static bool GetStringPara(ani_env* env, ani_string dataStr)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return false;
     }
     constexpr int32_t MAX_STRING_LENGTH = 40960;
     ani_size bufferSize = 0;
     env->String_GetUTF8Size(dataStr, &bufferSize);
     if (bufferSize > MAX_STRING_LENGTH) {
         WVLOG_E("buffer exceed MAX_STRING_LENGTH");
         return false;
     }
     return true;
 }

 static void ProcessActionByType(
     ani_env* env, ani_object object, ani_string origin, ani_boolean incognito, int32_t operationType)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return;
     }
     if (!GetStringPara(env, origin)) {
         AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
             NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "parameter", "string"));
         return;
     }
 
     std::string origin_new;
     if (!AniParseUtils::ParseString(env, origin, origin_new)) {
         WVLOG_E("parse origin failed");
         return;
     }
 
     std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
     if (!dataBase) {
         WVLOG_E("getDatabase failed");
         return;
     }
     if (operationType == ALLOW_PERMISSION_OPERATION) {
         if (dataBase->SetPermissionByOrigin(origin_new, OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE,
                 true, incognito) == NWebError::INVALID_ORIGIN) {
             AniBusinessError::ThrowError(env, NWebError::INVALID_ORIGIN, "SetOrigin is wrong");
             return;
         }
     } else if (operationType == DELETE_PERMISSION_OPERATION) {
         if (dataBase->ClearPermissionByOrigin(origin_new,
                 OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE,
                 incognito) == NWebError::INVALID_ORIGIN) {
             AniBusinessError::ThrowError(env, NWebError::INVALID_ORIGIN, "ClearOrigin is wrong");
             return;
         }
     }
     return;
 }
 
 static void JsDeleteGeolocation(ani_env* env, ani_object object, ani_string origin, ani_boolean incognito)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return;
     }
     return ProcessActionByType(env, object, origin, incognito, DELETE_PERMISSION_OPERATION);
 }
 
 static void JsAllowGeolocation(ani_env* env, ani_object object, ani_string origin, ani_boolean incognito)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return;
     }
     return ProcessActionByType(env, object, origin, incognito, ALLOW_PERMISSION_OPERATION);
 }
 
 static void JsDeleteAllGeolocation(ani_env* env, ani_object object, ani_boolean incognito)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return;
     }
     std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
     if (dataBase != nullptr) {
         dataBase->ClearAllPermission(
             OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE, static_cast<bool>(incognito));
     }
     return;
 }
 
 static ani_ref CreateAniStringArray(ani_env* env, const std::vector<std::string>& paths)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return nullptr;
     }

     ani_ref undefinedRef = nullptr;
     if (ANI_OK != env->GetUndefined(&undefinedRef)) {
         WVLOG_E("GetUndefined Failed.");
     }
 
     ani_array array;
     if (ANI_OK != env->Array_New(paths.size(), undefinedRef, &array)) {
         WVLOG_E("new array ref error.");
         return array;
     }
     for (size_t i = 0; i < paths.size(); ++i) {
         ani_string result {};
         if (ANI_OK != env->String_NewUTF8(paths[i].c_str(), paths[i].size(), &result)) {
             continue;
         }
         if (ANI_OK != env->Array_Set(array, i, result)) {
             return array;
         }
     }
     return array;
 }
 
 static ani_ref GetOriginsSync(ani_env* env, ani_object obj, ani_boolean incognitoObj)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return nullptr;
     }
 
     std::vector<std::string> origins;
 
     std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
     if (!dataBase) {
         WVLOG_E("database is nullptr");
         return nullptr;
     }
 
     origins = dataBase->GetOriginsByPermission(
         OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE, static_cast<bool>(incognitoObj));
 
     return CreateAniStringArray(env, origins);
 }
 
 ani_boolean GetAccessibleGeolocationSync(ani_env* env, ani_object obj, ani_string origin, ani_boolean incognito)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return false;
     }
 
     ani_boolean isUndefined = ANI_TRUE;
     if (env->Reference_IsUndefined(origin, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
         WVLOG_E("origin is undefined");
         return false;
     }
     std::string originStr;
     if (!AniParseUtils::ParseString(env, origin, originStr)) {
         WVLOG_E("parse origin failed");
         return false;
     }
     std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
     if (!dataBase) {
         WVLOG_E("dataBase is nullptr");
         return false;
     }
     bool permissionResult = false;
     bool querySuccess = dataBase->GetPermissionResultByOrigin(
         originStr, OHOS::NWeb::NWebDataBase::WebPermissionType::GEOLOCATION_TYPE, permissionResult, incognito);
     if (!querySuccess) {
         WVLOG_E("Failed to get permission for origin: %s", originStr.c_str());
         return false;
     }
 
     return static_cast<ani_boolean>(permissionResult);
 }
 
 ani_status StsGeolocationPermissionInit(ani_env* env)
 {
     if (env == nullptr) {
         WVLOG_E("env is nullptr");
         return ANI_ERROR;
     }
     ani_class geolocationPermissionCls = nullptr;
     ani_status status = env->FindClass(GEOLOCATION_PERMISSSION_CLASS_NAME, &geolocationPermissionCls);
     if (status != ANI_OK || !geolocationPermissionCls) {
         WVLOG_E("find %{public}s class failed, status: %{public}d", GEOLOCATION_PERMISSSION_CLASS_NAME, status);
         return ANI_ERROR;
     }
 
     std::array allMethods = {
         ani_native_function { "deleteAllGeolocation_inner", nullptr, reinterpret_cast<void*>(JsDeleteAllGeolocation) },
         ani_native_function { "deleteGeolocation_inner", nullptr, reinterpret_cast<void*>(JsDeleteGeolocation) },
         ani_native_function { "allowGeolocation_inner", nullptr, reinterpret_cast<void*>(JsAllowGeolocation) },
         ani_native_function { "getOriginsSync", nullptr, reinterpret_cast<void*>(GetOriginsSync) },
         ani_native_function {
             "getAccessibleGeolocationSync", nullptr, reinterpret_cast<void*>(GetAccessibleGeolocationSync) },
     };
 
     status = env->Class_BindStaticNativeMethods(geolocationPermissionCls, allMethods.data(), allMethods.size());
     if (status != ANI_OK) {
         WVLOG_E("Class_BindStaticNativeMethods failed status: %{public}d", status);
         return ANI_ERROR;
     }
     return ANI_OK;
 }
 
 } // namespace NWeb
 } // namespace OHOS
 
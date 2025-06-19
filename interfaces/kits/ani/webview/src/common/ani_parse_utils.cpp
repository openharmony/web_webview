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

#include "ani_parse_utils.h"

#include <sys/mman.h>
#include <unistd.h>
#include <regex>

#include "nweb_log.h"
#include "securec.h"
#include "nweb_cache_options_impl.h"

namespace OHOS {
namespace NWeb {
bool AniParseUtils::ParseString(ani_env *env, ani_ref ref, std::string& outValue)
{
    ani_string str = static_cast<ani_string>(ref);
    ani_size strSize;
    auto status = env->String_GetUTF8Size(str, &strSize);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_ParseString String_GetUTF8Size status: %{public}d", status);
        return false;
    }
    std::vector<char> buffer(strSize + 1);
    char* utfBuffer = buffer.data();
    ani_size bytes_written = 0;
    env->String_GetUTF8(str, utfBuffer, strSize + 1, &bytes_written);
    utfBuffer[bytes_written] = '\0';
    outValue = std::string(utfBuffer);
    return true;
}

bool AniParseUtils::IsString(ani_env *env, const ani_object& object)
{
    ani_class stringCls;
    ani_status status = env->FindClass("Lstd/core/String;", &stringCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsString FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isString = false;
    env->Object_InstanceOf(object, stringCls, &isString);
    return isString;
}

bool AniParseUtils::IsResource(ani_env *env, const ani_object& object)
{
    ani_class resourceCls;
    ani_status status =
        env->FindClass("Lglobal/resource/Resource;", &resourceCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsResource FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isResource = false;
    env->Object_InstanceOf(object, resourceCls, &isResource);
    return isResource;
}

void* AniParseUtils::Unwrap(ani_env *env, const ani_object& object)
{
    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(object, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return nullptr;
    }
    return reinterpret_cast<void *>(thisVar);
}

bool AniParseUtils::Wrap(ani_env *env, const ani_object& object, const char *className, const ani_long& thisVar)
{
    ani_status status;
    ani_class cls;
    if ((status = env->FindClass(className, &cls)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap FindClass status: %{public}d", status);
        return false;
    }
    ani_method innerWrapMethod;
    if ((status = env->Class_FindMethod(cls, "bindNativePtr", "J:V", &innerWrapMethod)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Class_FindMethod status: %{public}d", status);
        return false;
    }
    if ((status = env->Object_CallMethod_Void(object, innerWrapMethod, thisVar)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Object_CallMethod_Void status: %{public}d", status);
        return false;
    }
    return true;
}

bool AniParseUtils::CreateObjectVoid(ani_env *env, const char *className, ani_object& object)
{
    ani_class cls;
    ani_status status = env->FindClass(className, &cls);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", className, status);
        return false;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) != ANI_OK) {
        WVLOG_E("get %{public}s ctor method failed, status: %{public}d", className, status);
        return false;
    }
    if ((status = env->Object_New(cls, ctor, &object)) != ANI_OK) {
        WVLOG_E("new %{public}s failed, status: %{public}d", className, status);
        return false;
    }
    return true;
}

bool AniParseUtils::GetEnumItemByIndex(ani_env *env, const char* enumName, int32_t typeIndex, ani_enum_item& eType)
{
    ani_enum enumType;
    ani_status status = env->FindEnum(enumName, &enumType);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s enum failed, status: %{public}d", enumName, status);
        return false;
    }
    if ((status = env->Enum_GetEnumItemByIndex(enumType, typeIndex, &eType)) != ANI_OK) {
        WVLOG_E("get %{public}s item by index failed, status: %{public}d", enumName, status);
        return false;
    }
    return true;
}

ani_status AniParseUtils::SetPropertyByName_String(ani_env *env, ani_object aniCls,
                                                   const char *keyName, std::string keyValue)
{
    ani_status status;
    ani_ref string_ref;
    status = env->Object_GetPropertyByName_Ref(aniCls, keyName, &string_ref);
    if (status != ANI_OK) {
        WVLOG_E("Object_GetPropertyByName_Ref failed, key=%{public}s, name=%{public}s, status=%{public}d",
                keyName, keyValue.c_str(), status);
        return status;
    }

    ani_string aniResult{};
    status = env->String_NewUTF8(keyValue.c_str(), keyValue.size(), &aniResult);
    if (status != ANI_OK) {
        WVLOG_E("String_GetUTF8 failed, key=%{public}s, name=%{public}s, status=%{public}d",
                keyName, keyValue.c_str(), status);
        return status;
    }

    status = env->Object_SetPropertyByName_Ref(aniCls, keyName, static_cast<ani_ref>(aniResult));
    return status;
}

std::shared_ptr<CacheOptions> AniParseUtils::ParseCacheOptions(ani_env *env, ani_object cacheOptions)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_ref cacheOptionsArray = nullptr;
    std::map<std::string, std::string> responseHeaders;
    auto defaultCacheOptions = std::make_shared<NWebCacheOptionsImpl>(responseHeaders);
    if (env->Object_GetPropertyByName_Ref(cacheOptions, "responseHeaders", &cacheOptionsArray) != ANI_OK) {
        return defaultCacheOptions;
    }
    ParseStringArrayMap(env, static_cast<ani_object>(cacheOptionsArray), responseHeaders);
    return std::make_shared<NWebCacheOptionsImpl>(responseHeaders);
}

bool AniParseUtils::ParseStringArray(ani_env* env, ani_object argv, std::vector<std::string>& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_boolean isArray = ANI_FALSE;
    ani_double arrayLength;
    ani_array_ref arrayRef;
    env->FindClass("Lescompat/Array;", &cls);
    env->Object_InstanceOf(argv, cls, &isArray);
    if (!isArray) {
        WVLOG_E("argv must be array");
        return false;
    }

    arrayRef = static_cast<ani_array_ref>(argv);
    env->Object_GetPropertyByName_Double(argv, "length", &arrayLength);
    for (ani_double i = 0; i < arrayLength; i++) {
        ani_ref arrayItem = nullptr;
        env->Array_Get_Ref(arrayRef, i, &arrayItem);
        std::string str;
        if (ParseString(env, arrayItem, str)) {
            outValue.push_back(str);
        }
    }
    return true;
}

bool AniParseUtils::ParseStringArrayMap(ani_env* env, ani_object argv, std::map<std::string, std::string>& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_boolean isArray = ANI_FALSE;
    ani_array_ref arrayRef;
    ani_double arrayLength;
    env->FindClass("Lescompat/Array;", &cls);
    env->Object_InstanceOf(argv, cls, &isArray);
    if (!isArray) {
        WVLOG_E("argv must be array");
        return false;
    }

    arrayRef = static_cast<ani_array_ref>(argv);
    env->Object_GetPropertyByName_Double(argv, "length", &arrayLength);
    for (ani_double i = 0; i < arrayLength; i++) {
        ani_ref arrayItem = nullptr;
        env->Array_Get_Ref(arrayRef, i, &arrayItem);
        ani_ref keyObj = nullptr;
        ani_ref valueObj = nullptr;
        env->Object_GetPropertyByName_Ref(static_cast<ani_object>(arrayItem), "headerKey", &keyObj);
        env->Object_GetPropertyByName_Ref(static_cast<ani_object>(arrayItem), "headerValue", &valueObj);

        std::string keyString;
        std::string valueString;
        if (!AniParseUtils::ParseString(env, keyObj, keyString)) {
            WVLOG_E("Parse keyString failed.");
            return false;
        }
        if (!AniParseUtils::ParseString(env, valueObj, valueString)) {
            WVLOG_E("Parse valueString failed.");
            return false;
        }
        outValue[keyString] = valueString;
    }

    return true;
}
}
}

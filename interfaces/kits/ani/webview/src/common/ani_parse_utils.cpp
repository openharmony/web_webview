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
#include "ohos_adapter_helper.h"
#include "securec.h"

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

bool IsFormatStringOfLength(const std::string& str)
{
    std::regex pattern("^\\d+(px|vp|%)?$");
    return std::regex_match(str, pattern);
}

bool IsNumberOfLength(const std::string& value)
{
    if (value.empty()) {
        return false;
    }
    return std::all_of(value.begin(), value.end(), [](char i) { return isdigit(i); });
}

bool TransStringToInt(const std::string& str, int32_t& value)
{
    if (str.empty()) {
        return false;
    }
    int64_t tempValue = std::stoll(str);
    if (tempValue < std::numeric_limits<int32_t>::min() || tempValue > std::numeric_limits<int32_t>::max()) {
        WVLOG_E("input trans failed: out of range");
        value = 0;
        return false;
    }
    value = static_cast<int32_t>(tempValue);
    return true;
}

bool AniParseUtils::ParseJsLengthStringToInt(const std::string& input, PixelUnit& type, int32_t& value)
{
        if (input.empty() || input.size() >= MAX_STRING_TO_INT32_LENGTH) {
            return false;
        }
        if (!IsFormatStringOfLength(input)) {
            return false;
        }
        if (IsNumberOfLength(input)) {
            if (TransStringToInt(input, value)) {
                type = PixelUnit::VP;
                return true;
            }
            return false;
        }
        if (input.back() == '%') {
            std::string trans = input.substr(0, input.length() - 1);
            if (IsNumberOfLength(trans)) {
                if (TransStringToInt(trans, value)) {
                    type = PixelUnit::PERCENTAGE;
                    return true;
                }
            }
            return false;
        }
        if (input.length() < PARSE_THREE) {
            return false;
        }
        std::string lastTwo = input.substr(input.length() - PARSE_TWO);
        std::string trans = input.substr(0, input.length() - PARSE_TWO);
        if (!IsNumberOfLength(trans)) {
            return false;
        }
        if (lastTwo == "px") {
            if (TransStringToInt(trans, value)) {
                type = PixelUnit::PX;
                return true;
            }
        } else if (lastTwo == "vp") {
            if (TransStringToInt(trans, value)) {
                type = PixelUnit::VP;
                return true;
            }
        }
        return false;
}

bool AniParseUtils::ParseInt32(ani_env* env, ani_ref ref, int32_t& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_object object = static_cast<ani_object>(ref);
    ani_type type;
    auto statusGetType = env->Object_GetType(object, &type);
    if (statusGetType != ANI_OK) {
        WVLOG_E("ParseInt32 failed - invalid number type");
        return false;
    }
    ani_class doubleClass;
    env->FindClass("Lstd/core/Double;", &doubleClass);
    ani_boolean isDouble;
    env->Object_InstanceOf(object, doubleClass, &isDouble);
    if (!isDouble) {
        WVLOG_E("ParseInt32 failed - invalid number type");
        return false;
    }
    ani_variable variable {};
    env->Variable_SetValue_Ref(variable, ref);
    ani_int number;
    auto status_GetValue_Int = env->Variable_GetValue_Int(variable, &number);
    if (status_GetValue_Int != ANI_OK) {
        WVLOG_E("ParseInt32 failed - get value error: %d", status_GetValue_Int);
        return false;
    }
    outValue = static_cast<int32_t>(number);
    return true;
}

bool AniParseUtils::IsFunction(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class functionCls;
    ani_status status = env->FindClass("Lstd/core/Double;", &functionCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsDouble FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isFunction = false;
    env->Object_InstanceOf(object, functionCls, &isFunction);
    return isFunction;
}

bool AniParseUtils::IsDouble(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class doubleCls;
    ani_status status = env->FindClass("Lstd/core/Double;", &doubleCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsDouble FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isDouble = false;
    env->Object_InstanceOf(object, doubleCls, &isDouble);
    return isDouble;
}

bool AniParseUtils::IsObject(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class objectCls;
    ani_status status = env->FindClass("Lstd/core/Object;", &objectCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsObject FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isObject = false;
    env->Object_InstanceOf(object, objectCls, &isObject);
    return isObject;
}
}
}

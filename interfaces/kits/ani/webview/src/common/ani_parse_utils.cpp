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

#include <arpa/inet.h>
#include <memory>
#include <sys/mman.h>
#include <unistd.h>
#include <regex>

#include "nweb_helper.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "securec.h"
#include "nweb_cache_options_impl.h"

#define MAX_FLOWBUF_DATA_SIZE 52428800 /* 50 MB */

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
    outValue = std::string(utfBuffer, strSize);
    return true;
}

bool AniParseUtils::IsString(ani_env *env, const ani_object& object)
{
    ani_class stringCls;
    ani_status status = env->FindClass("std.core.String", &stringCls);
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
        env->FindClass("global.resource.Resource", &resourceCls);
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
    if ((status = env->Class_FindMethod(cls, "bindNativePtr", "l:", &innerWrapMethod)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Class_FindMethod status: %{public}d", status);
        return false;
    }
    if ((status = env->Object_CallMethod_Void(object, innerWrapMethod, thisVar)) != ANI_OK) {
        WVLOG_E("AniUtils_Wrap Object_CallMethod_Void status: %{public}d", status);
        return false;
    }
    return true;
}

bool AniParseUtils::ParseDouble_t(ani_env* env, ani_ref ref, double& outValue)
{
    ani_class doubleClass;
    if (env->FindClass("std.core.Double", &doubleClass) != ANI_OK) {
        WVLOG_E("ParseDouble failed - invalid FindClass type");
        return false;
    }
    ani_boolean isDouble;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), doubleClass, &isDouble) != ANI_OK ||
        isDouble != ANI_TRUE) {
        WVLOG_E("ParseDouble failed - invalid double type");
        return false;
    }

    ani_double value = 0;
    if (env->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "unboxed", ":d", &value) != ANI_OK) {
        WVLOG_E("ParseDouble failed");
        return false;
    }
    outValue = value;
    return true;
}

bool AniParseUtils::ParseBoolean_t(ani_env* env, ani_ref ref, bool& outValue)
{
    ani_class boolClass;
    if (env->FindClass("std.core.Boolean", &boolClass) != ANI_OK) {
        WVLOG_E("ParseBoolean failed - invalid FindClass type");
        return false;
    }
    ani_boolean isBool;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), boolClass, &isBool) != ANI_OK || isBool != ANI_TRUE) {
        WVLOG_E("ParseBoolean failed - invalid boolean type");
        return false;
    }

    ani_boolean boolValue = false;
    if (env->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "unboxed", ":z", &boolValue) != ANI_OK) {
        WVLOG_E("ParseBoolean failed");
        return false;
    }
    outValue = boolValue ? true : false;
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
    std::map<std::string, std::string> responseHeaders;
    auto defaultCacheOptions = std::make_shared<NWebCacheOptionsImpl>(responseHeaders);

    ani_ref cacheOptionsArray = nullptr;
    if (env->Object_GetPropertyByName_Ref(cacheOptions, "responseHeaders", &cacheOptionsArray) != ANI_OK) {
        WVLOG_E("PrecompileJavaScript defaultCacheOptions");
        return defaultCacheOptions;
    }

    if(!ParseStringArrayMap(env, static_cast<ani_object>(cacheOptionsArray), responseHeaders)){
        WVLOG_E("PrecompileJavaScript defaultCacheOptions");
        return defaultCacheOptions;
    }
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
    ani_int arrayLength;
    ani_array arrayRef;
    env->FindClass("escompat.Array", &cls);
    env->Object_InstanceOf(argv, cls, &isArray);
    if (!isArray) {
        WVLOG_E("argv must be array");
        return false;
    }

    arrayRef = static_cast<ani_array>(argv);
    ani_status status;
    if ((status = env->Object_GetPropertyByName_Int(argv, "length", &arrayLength)) != ANI_OK) {
        WVLOG_E("string array get length error status = %{public}d", status);
        return false;
    }
    WVLOG_I("stringArray size = %{public}d", static_cast<int>(arrayLength));
    for (ani_int i = 0; i < arrayLength; i++) {
        ani_ref arrayItem = nullptr;
        env->Array_Get(arrayRef, i, &arrayItem);
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
    ani_array arrayRef;
    ani_double arrayLength;
    env->FindClass("escompat.Array", &cls);
    env->Object_InstanceOf(argv, cls, &isArray);
    if (!isArray) {
        WVLOG_E("argv must be array");
        return false;
    }

    arrayRef = static_cast<ani_array>(argv);
    env->Object_GetPropertyByName_Double(argv, "length", &arrayLength);
    for (ani_double i = 0; i < arrayLength; i++) {
        ani_ref arrayItem = nullptr;
        env->Array_Get(arrayRef, i, &arrayItem);
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

bool AniParseUtils::EnumParseInt32_t(ani_env* env, ani_enum_item enum_item, int32_t& outValue)
{
    if (env == nullptr) {
        WVLOG_E("null env");
        return false;
    }
    
    ani_class doubleObject;
    if (env->FindClass("std.core.Object", &doubleObject) != ANI_OK) {
        WVLOG_E("EnumParseInt32 failed - invalid FindClass type");
        return false;
    }
    ani_boolean isObject;
    if (env->Object_InstanceOf(static_cast<ani_object>(enum_item), doubleObject, &isObject) != ANI_OK ||
        isObject != ANI_TRUE) {
        WVLOG_E("EnumParseInt32 failed - invalid int type");
        return false;
    }
    ani_int number = 0;
    ani_status status = env->EnumItem_GetValue_Int(enum_item, &number);
    if (status != ANI_OK) {
        WVLOG_E("EnumParseInt32 failed - EnumItem_GetValue_Int failed");
        return false;
    }
    outValue = static_cast<int32_t>(number);
    return true;
}

bool AniParseUtils::GetStringList(ani_env *env, ani_object array, std::vector<std::string>& outValue)
{
    ani_int arrayLength;
    if (env->Object_GetPropertyByName_Int(array, "length", &arrayLength) != ANI_OK) {
        WVLOG_E("Param check failed, unable to get array length.");
        return false;
    }
    for (int32_t i = 0; i < static_cast<int32_t>(arrayLength); ++i) {
        ani_ref elementRef;
        if (env->Object_CallMethodByName_Ref(array, "$_get", "i:C{std.core.Object}", &elementRef, 
            static_cast<ani_int>(i)) != ANI_OK) {
            WVLOG_E("Failed to get element at index %d", i);
            return false;
        }
        std::string strValue;
        if (!AniParseUtils::ParseString(env, elementRef, strValue)) {
            WVLOG_W("Failed to parse string at index %d", i);
            continue;
        }
        outValue.push_back(strValue);
    }
    return true;
}

bool AniParseUtils::ParseIP(ani_env *env, ani_object urlObj, std::string& ip)
{
    if (AniParseUtils::ParseString(env, urlObj, ip)) {
        if (ip == "") {
            WVLOG_E("The IP is null");
            return false;
        }

        unsigned char buf[sizeof(struct in6_addr)];
        if ((inet_pton(AF_INET, ip.c_str(), buf) == 1) || (inet_pton(AF_INET6, ip.c_str(), buf) == 1)) {
            return true;
        }
        WVLOG_E("IP error.");
        return false;
    }

    WVLOG_E("Unable to parse type from ip object.");
    return false;
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
        if (input.empty() || input.size() > MAX_STRING_TO_INT32_LENGTH) {
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
    ani_class intClass;
    if (env->FindClass("std.core.Int", &intClass) != ANI_OK) {
        WVLOG_E("ParseInt32 failed - invalid FindClass type");
        return false;
    }
    ani_boolean isInt;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), intClass, &isInt) != ANI_OK || isInt != ANI_TRUE) {
        WVLOG_E("ParseInt32 failed - invalid int type");
        return false;
    }

    ani_int value = 0;
    if (env->Object_CallMethodByName_Int(static_cast<ani_object>(ref), "unboxed", ":i", &value) != ANI_OK) {
        WVLOG_E("ParseInt32 failed");
        return false;
    }
    outValue = static_cast<int32_t>(value);
    return true;
}

bool AniParseUtils::IsFunction(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class functionCls;
    ani_status status = env->FindClass("std.core.Function", &functionCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsFunction FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isFunction = false;
    auto statusInstanceOf = env->Object_InstanceOf(object, functionCls, &isFunction);
    if (statusInstanceOf != ANI_OK) {
        WVLOG_E("IsFunction failed - Object_InstanceOf error: %d", statusInstanceOf);
        return false;
    }
    return isFunction;
}
 
bool AniParseUtils::IsDouble(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class doubleCls;
    ani_status status = env->FindClass("std.core.Double", &doubleCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsDouble FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isDouble = false;
    auto statusInstanceOf = env->Object_InstanceOf(object, doubleCls, &isDouble);
    if (statusInstanceOf != ANI_OK) {
        WVLOG_E("IsDouble failed - Object_InstanceOf error: %d", statusInstanceOf);
        return false;
    }
    return isDouble;
}
 
bool AniParseUtils::IsObject(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class objectCls;
    ani_status status = env->FindClass("std.core.Object", &objectCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsObject FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isObject = false;
    auto statusInstanceOf = env->Object_InstanceOf(object, objectCls, &isObject);
    if (statusInstanceOf != ANI_OK) {
        WVLOG_E("IsObject failed - Object_InstanceOf error: %d", statusInstanceOf);
        return false;
    }
    return isObject;
}
 
bool AniParseUtils::CreateBoolean(ani_env *env, bool src, ani_object& aniObj)
{   
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    static const char *className = "std.core.Boolean";
    ani_class cls;
    ani_status status = env->FindClass(className, &cls);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", className, status);
        return false;
    }
    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>","z:", &ctor);
    if (status != ANI_OK) {
        WVLOG_E("get %{public}s ctor method failed, status: %{public}d", className, status);
        return false;
    }
    ani_boolean aniBool = src ? ANI_TRUE : ANI_FALSE;
    status = env->Object_New(cls, ctor, &aniObj, aniBool);
    if (status != ANI_OK) {
        WVLOG_E("new %{public}s failed, status: %{public}d", className, status);
        return false;
    }
    return true;
}

ani_object AniParseUtils::CreateDouble(ani_env* env, ani_double val)
{
    static constexpr const char* className = "std.core.Double";
    ani_class doubleCls {};
    env->FindClass(className, &doubleCls);
    ani_method ctor {};
    env->Class_FindMethod(doubleCls, "<ctor>", "d:", &ctor);
    ani_object obj {};
    if (env->Object_New(doubleCls, ctor, &obj, static_cast<ani_double>(val)) != ANI_OK) {
        WVLOG_E("CreateDouble Failed");
        ani_ref undefinedRef;
        env->GetUndefined(&undefinedRef);
        return static_cast<ani_object>(undefinedRef);
    }
    return obj;
}

ani_object AniParseUtils::CreateInt(ani_env *env, ani_int val)
{
    static constexpr const char* className = "std.core.Int";
    ani_class intCls {};
    env->FindClass(className, &intCls);
    ani_method ctor {};
    env->Class_FindMethod(intCls, "<ctor>", "i:", &ctor);
    ani_object obj {};
    if (env->Object_New(intCls, ctor, &obj, static_cast<ani_int>(val)) != ANI_OK) {
        WVLOG_E("CreateInt Failed");
        ani_ref undefinedRef;
        env->GetUndefined(&undefinedRef);
        return static_cast<ani_object>(undefinedRef);
    }
    return obj;
}

ani_string AniParseUtils::StringToAniStr(ani_env* env, const std::string& str)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_string result {};
    if (ANI_OK != env->String_NewUTF8(str.c_str(), str.size(), &result)) {
        return nullptr;
    }
    return result;
}

ani_ref AniParseUtils::CreateAniStringArray(ani_env* env, const std::vector<std::string>& paths)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefined Failed.");
        return nullptr;
    }
    ani_array array;
    if (ANI_OK != env->Array_New(paths.size(), undefinedRef, &array)) {
        WVLOG_E("new array ref error.");
        return nullptr;
    }
    for (size_t i = 0; i < paths.size(); ++i) {
        auto item = AniParseUtils::StringToAniStr(env, paths[i]);
        if (ANI_OK != env->Array_Set(array, i, item)) {
            return nullptr;
        }
    }
    return array;
}

bool AniParseUtils::ParseBoolean(ani_env* env, ani_ref ref, bool& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class booleanClass;
    if (env->FindClass("std.core.Boolean", &booleanClass) != ANI_OK) {
        WVLOG_E("ParseBoolean failed - invalid FindClass type");
        return false;
    }
    ani_boolean isBoolean;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), booleanClass, &isBoolean) != ANI_OK ||
        isBoolean != ANI_TRUE) {
        WVLOG_E("ParseBoolean failed - invalid boolean type");
        return false;
    }

    ani_boolean boolValue;
    env->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "unboxed", ":z", &boolValue);
    outValue = static_cast<bool>(boolValue);
    return true;
}

bool AniParseUtils::ParseInt64(ani_env* env, ani_ref ref, int64_t& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class longClass;
    if (env->FindClass("std.core.Long", &longClass) != ANI_OK) {
        WVLOG_E("ParseInt64 failed - invalid FindClass type");
        return false;
    }
    ani_boolean isLong;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), longClass, &isLong) != ANI_OK || isLong != ANI_TRUE) {
        WVLOG_E("ParseInt64 failed - invalid long type");
        return false;
    }

    ani_long value;
    env->Object_CallMethodByName_Long(static_cast<ani_object>(ref), "unboxed", ":l", &value);
    outValue = static_cast<int64_t>(value);
    return true;
}

bool AniParseUtils::ParseDouble(ani_env* env, ani_ref ref, double& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class doubleClass;
    if (env->FindClass("std.core.Double", &doubleClass) != ANI_OK) {
        WVLOG_E("ParseDouble failed - invalid FindClass type");
        return false;
    }
    ani_boolean isDouble;
    if (env->Object_InstanceOf(static_cast<ani_object>(ref), doubleClass, &isDouble) != ANI_OK ||
        isDouble != ANI_TRUE) {
        WVLOG_E("ParseDouble failed - invalid double type");
        return false;
    }

    ani_double value;
    env->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "unboxed", ":d", &value);
    outValue = static_cast<double>(value);
    return true;
}

bool AniParseUtils::IsBoolean(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class booleanCls;
    ani_status status = env->FindClass("std.core.Boolean", &booleanCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsBoolean FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isBoolean = false;
    auto statusInstanceOf = env->Object_InstanceOf(object, booleanCls, &isBoolean);
    if (statusInstanceOf != ANI_OK) {
        WVLOG_E("AniUtils_IsBoolean failed - Object_InstanceOf error: %d", statusInstanceOf);
        return false;
    }
    return isBoolean;
}

bool AniParseUtils::IsInteger(ani_env* env, const ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class longCls;
    ani_status status = env->FindClass("std.core.Long", &longCls);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_IsINTEGER FindClass status: %{public}d", status);
        return false;
    }
    ani_boolean isLong = false;
    auto statusInstanceOf = env->Object_InstanceOf(object, longCls, &isLong);
    if (statusInstanceOf != ANI_OK) {
        WVLOG_E("AniUtils_IsINTEGER failed - Object_InstanceOf error: %d", statusInstanceOf);
        return false;
    }
    return isLong;
}

bool AniParseUtils::ParseInt64Array(ani_env* env, ani_object argv, std::vector<int64_t>& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_boolean isArray = ANI_FALSE;
    ani_int arrayLength;
    ani_array arrayRef;
    env->FindClass("escompat.Array", &cls);
    env->Object_InstanceOf(argv, cls, &isArray);
    if (!isArray) {
        WVLOG_E("argv must be array");
        return false;
    }

    arrayRef = static_cast<ani_array>(argv);
    env->Object_GetPropertyByName_Int(argv, "length", &arrayLength);
    for (ani_int i = 0; i < arrayLength; i++) {
        ani_ref arrayItem = nullptr;
        env->Array_Get(arrayRef, i, &arrayItem);
        int64_t value;
        if (ParseInt64(env, arrayItem, value)) {
            outValue.push_back(value);
        }
    }
    return true;
}

bool AniParseUtils::ParseBooleanArray(ani_env* env, ani_object argv, std::vector<bool>& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_boolean isArray = ANI_FALSE;
    ani_int arrayLength;
    ani_array arrayRef;
    env->FindClass("escompat.Array", &cls);
    env->Object_InstanceOf(argv, cls, &isArray);
    if (!isArray) {
        WVLOG_E("argv must be array");
        return false;
    }

    arrayRef = static_cast<ani_array>(argv);
    env->Object_GetPropertyByName_Int(argv, "length", &arrayLength);
    for (ani_int i = 0; i < arrayLength; i++) {
        ani_ref arrayItem = nullptr;
        env->Array_Get(arrayRef, i, &arrayItem);
        bool value;
        if (ParseBoolean(env, arrayItem, value)) {
            outValue.push_back(value);
        }
    }
    return true;
}

bool AniParseUtils::ParseDoubleArray(ani_env* env, ani_object argv, std::vector<double>& outValue)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_boolean isArray = ANI_FALSE;
    ani_int arrayLength;
    ani_array arrayRef;
    env->FindClass("escompat.Array", &cls);
    env->Object_InstanceOf(argv, cls, &isArray);
    if (!isArray) {
        WVLOG_E("argv must be array");
        return false;
    }

    arrayRef = static_cast<ani_array>(argv);
    env->Object_GetPropertyByName_Int(argv, "length", &arrayLength);
    for (ani_int i = 0; i < arrayLength; i++) {
        ani_ref arrayItem = nullptr;
        env->Array_Get(arrayRef, i, &arrayItem);
        double value;
        if (ParseDouble(env, arrayItem, value)) {
            outValue.push_back(value);
        }
    }
    return true;
}

ErrCode AniParseUtils::ConstructStringFlowbuf(ani_env *env, const std::string script, int& fd, size_t& scriptLength)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return NWebError::PARAM_CHECK_ERROR;
    }

    scriptLength = script.size();
    if (scriptLength + 1 > MAX_FLOWBUF_DATA_SIZE) {
        WVLOG_E("String length is too long");
        return NWebError::PARAM_CHECK_ERROR;
    }

    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        WVLOG_E("Create flowbuffer adapter failed");
        return NWebError::NEW_OOM;
    }

    auto ashmem = flowbufferAdapter->CreateAshmem(scriptLength + 1, PROT_READ | PROT_WRITE, fd);
    if (!ashmem) {
        return NWebError::NEW_OOM;
    }

    ani_string string = nullptr;
    env->String_NewUTF8(script.c_str(), scriptLength, &string);
    ani_size result = 0U;
    env->String_GetUTF8Size(string, &result);
    if (result != scriptLength) {
        close(fd);
        WVLOG_E("Write js string failed, the length values are different");
        return NWebError::PARAM_CHECK_ERROR;
    }
    return NWebError::NO_ERROR;
}

ErrCode AniParseUtils::ConstructArrayBufFlowbuf(ani_env* env, const ani_object script, int& fd, size_t& scriptLength)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return NWebError::PARAM_CHECK_ERROR;
    }
    ani_boolean isArrayBuffer = false;
    ani_class arrayBufferClass;
    ani_status status;
    if ((status = env->FindClass("escompat.ArrayBuffer", &arrayBufferClass)) != ANI_OK) {
        WVLOG_E("find buffer class error status : %{public}d", status);
        return NWebError::PARAM_CHECK_ERROR;
    }
    if ((env->Object_InstanceOf(script, arrayBufferClass, &isArrayBuffer) != ANI_OK) || !isArrayBuffer) {
        WVLOG_E("Object_InstanceOf error status : %{public}d", status);
        return NWebError::PARAM_CHECK_ERROR;
    }

    char* arrBuf = nullptr;
    if ((status = env->ArrayBuffer_GetInfo(
             static_cast<ani_arraybuffer>(script), reinterpret_cast<void**>(&arrBuf), &scriptLength)) != ANI_OK) {
        WVLOG_E("ArrayBuffer_GetInfo error status : %{public}d", status);
        return NWebError::PARAM_CHECK_ERROR;
    }

    // get ashmem
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        WVLOG_E("Create flowbuffer adapter failed");
        return NWebError::NEW_OOM;
    }
    auto ashmem = flowbufferAdapter->CreateAshmem(scriptLength + 1, PROT_READ | PROT_WRITE, fd);
    if (!ashmem) {
        return NWebError::NEW_OOM;
    }

    // write to ashmem
    if (memcpy_s(ashmem, scriptLength + 1, arrBuf, scriptLength) != EOK) {
        WVLOG_E("ConstructArrayBufFlowbuf, memory copy failed");
        return NWebError::NEW_OOM;
    }
    static_cast<char*>(ashmem)[scriptLength] = '\0';
    return NWebError::NO_ERROR;
}

ani_ref ConvertToAniHandlerOfString(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToNapiHandlerOfString");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::string msgStr = src->GetString();
    ani_string dstTemp;
    env->String_NewUTF8(msgStr.c_str(), msgStr.size(), &dstTemp);
    return static_cast<ani_ref>(dstTemp);
}

ani_ref ConvertToAniHandlerOfBinary(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToNapiHandlerOfBinary");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::vector<uint8_t> msgArr = src->GetBinary();
    void* arrayData = nullptr;
    ani_arraybuffer arraybuffer;
    ani_status status = env->CreateArrayBuffer(msgArr.size(), &arrayData, &arraybuffer);
    if (status != ANI_OK) {
        WVLOG_E("Create arraybuffer failed status : %{public}d", status);
        return nullptr;
    }
    for (size_t i = 0; i < msgArr.size(); ++i) {
        *(uint8_t*)((uint8_t*)arrayData + i) = msgArr[i];
    }
    WVLOG_I("ConvertToNapiHandlerOfBinary successful!");
    return static_cast<ani_ref>(arraybuffer);
}

ani_ref ConvertToAniHandlerOfBoolean(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfBoolean");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    ani_class bool_cls;
    ani_status status;
    if ((status = env->FindClass("std.core.Boolean", &bool_cls)) != ANI_OK) {
        WVLOG_E("error in FindClass status : %{public}d", status);
        return nullptr;
    }

    ani_method boolInfoCtor;
    if ((status = env->Class_FindMethod(bool_cls, "<ctor>", "z:", &boolInfoCtor)) != ANI_OK) {
        WVLOG_E("error in FindMethod status : %{public}d", status);
        return nullptr;
    }

    ani_object boolInfoObj;
    env->Object_New(bool_cls, boolInfoCtor, &boolInfoObj, static_cast<ani_boolean>(src->GetBoolean()));
    return static_cast<ani_ref>(boolInfoObj);
}

ani_ref ConvertToAniHandlerOfInteger(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfInteger");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    ani_class integer_cls;
    ani_status status;
    if ((status = env->FindClass("std.core.Long", &integer_cls)) != ANI_OK) {
        WVLOG_E("error in FindClass status : %{public}d", status);
        return nullptr;
    }

    ani_method integerInfoCtor;
    if ((status = env->Class_FindMethod(integer_cls, "<ctor>", "l:", &integerInfoCtor)) != ANI_OK) {
        WVLOG_E("error in FindMethod status : %{public}d", status);
        return nullptr;
    }

    ani_object integerInfoObj;
    env->Object_New(integer_cls, integerInfoCtor, &integerInfoObj, static_cast<ani_long>(src->GetInt64()));
    return static_cast<ani_ref>(integerInfoObj);
}

ani_ref ConvertToAniHandlerOfDouble(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfDouble");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    ani_class double_cls;
    ani_status status;
    if ((status = env->FindClass("std.core.Double", &double_cls)) != ANI_OK) {
        WVLOG_E("error in FindClass status : %{public}d", status);
        return nullptr;
    }

    ani_method doubleInfoCtor;
    if ((status = env->Class_FindMethod(double_cls, "<ctor>", "d:", &doubleInfoCtor)) != ANI_OK) {
        WVLOG_E("error in FindMethod status : %{public}d", status);
        return nullptr;
    }

    ani_object doubleInfoObj;
    env->Object_New(double_cls, doubleInfoCtor, &doubleInfoObj, static_cast<ani_long>(src->GetDouble()));
    return static_cast<ani_ref>(doubleInfoObj);
}

ani_ref ConvertToAniHandlerOfError(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfError");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::string errorMsg = src->GetErrName() + ": " + src->GetErrMsg();
    ani_string message = nullptr;
    if (env->String_NewUTF8(errorMsg.c_str(), errorMsg.length(), &message) != ANI_OK) {
        WVLOG_E("error create ani_string object");
        return nullptr;
    }

    ani_class errCls;
    ani_status status;
    if ((status = env->FindClass("std.core.NullPointerError", &errCls)) != ANI_OK) {
        WVLOG_E("error in FindClass status : %{public}d", status);
        return nullptr;
    }

    ani_method errInfoCtor;
    if ((status = env->Class_FindMethod(errCls, "<ctor>", nullptr, &errInfoCtor)) != ANI_OK) {
        WVLOG_E("error in FindMethod status : %{public}d", status);
        return nullptr;
    }

    ani_object errObj;
    env->Object_New(errCls, errInfoCtor, &errObj, message);
    return static_cast<ani_ref>(errObj);
}

ani_ref ConvertToAniHandlerOfStringArr(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfStringArray");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::vector<std::string> values = src->GetStringArray();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < values.size(); ++i) {
        ani_string result {};
        if (ANI_OK != env->String_NewUTF8(values[i].c_str(), values[i].size(), &result)) {
            continue;
        }
        if (ANI_OK != env->Array_Set(array, i, result)) {
            return array;
        }
    }
    return static_cast<ani_ref>(array);
}

ani_ref ConvertToAniHandlerOfBooleanArr(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfBooleanArr");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::vector<bool> values = src->GetBooleanArray();
    size_t valueSize = values.size();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < valueSize; i++) {
        ani_boolean item = static_cast<ani_boolean>(values[i]);
        ani_class booleanCls {};
        if (ANI_OK != env->FindClass("std.core.Boolean", &booleanCls)) {
            return nullptr;
        }
        ani_method ctor {};
        if (ANI_OK != env->Class_FindMethod(booleanCls, "<ctor>", "z:", &ctor)) {
            return nullptr;
        }
        ani_object obj {};
        if (env->Object_New(booleanCls, ctor, &obj, item) != ANI_OK) {
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(array, i, obj)) {
            return array;
        }
    }
    return static_cast<ani_ref>(array);
}

ani_ref ConvertToAniHandlerOfDoubleArr(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfDoubleArr");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::vector<double> values = src->GetDoubleArray();
    size_t valueSize = values.size();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < valueSize; i++) {
        ani_double item = static_cast<ani_double>(values[i]);
        ani_class cls {};
        if (ANI_OK != env->FindClass("std.core.Double", &cls)) {
            return nullptr;
        }
        ani_method ctor {};
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "d:", &ctor)) {
            return nullptr;
        }
        ani_object obj {};
        if (env->Object_New(cls, ctor, &obj, item) != ANI_OK) {
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(array, i, obj)) {
            return array;
        }
    }
    return static_cast<ani_ref>(array);
}

ani_ref ConvertToAniHandlerOfInt64Arr(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertToAniHandlerOfInt64Arr");
    if (!env || !src) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::vector<int64_t> values = src->GetInt64Array();
    size_t valueSize = values.size();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < valueSize; i++) {
        ani_long item = static_cast<ani_long>(values[i]);
        ani_class cls {};
        if (ANI_OK != env->FindClass("std.core.Long", &cls)) {
            return nullptr;
        }
        ani_method ctor {};
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "l:", &ctor)) {
            return nullptr;
        }
        ani_object obj {};
        if (env->Object_New(cls, ctor, &obj, item) != ANI_OK) {
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(array, i, obj)) {
            return array;
        }
    }
    return static_cast<ani_ref>(array);
}

ani_ref AniParseUtils::ConvertNWebToAniValue(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    WVLOG_I("enter ConvertNWebToNapiValue");
    if (!src || !env) {
        WVLOG_E("src of env is nullptr");
        return nullptr;
    }
    NWebValue::Type type = src->GetType();
    using ConvertNWebToAniValueHandler = std::function<ani_ref(ani_env*, std::shared_ptr<NWebMessage>)>;
    static const std::unordered_map<NWebValue::Type, ConvertNWebToAniValueHandler> functionMap = {
        { NWebValue::Type::STRING, ConvertToAniHandlerOfString },
        { NWebValue::Type::BINARY, ConvertToAniHandlerOfBinary },
        { NWebValue::Type::BOOLEAN, ConvertToAniHandlerOfBoolean },
        { NWebValue::Type::INTEGER, ConvertToAniHandlerOfInteger },
        { NWebValue::Type::DOUBLE, ConvertToAniHandlerOfDouble },
        { NWebValue::Type::ERROR, ConvertToAniHandlerOfError },
        { NWebValue::Type::STRINGARRAY, ConvertToAniHandlerOfStringArr },
        { NWebValue::Type::BOOLEANARRAY, ConvertToAniHandlerOfBooleanArr },
        { NWebValue::Type::DOUBLEARRAY, ConvertToAniHandlerOfDoubleArr },
        { NWebValue::Type::INT64ARRAY, ConvertToAniHandlerOfInt64Arr }
    };
    auto it = functionMap.find(type);
    if (it == functionMap.end()) {
        WVLOG_E("This type not support");
        std::string msgStr = "This type not support";
        ani_string dstTemp;
        env->String_NewUTF8(msgStr.c_str(), msgStr.size(), &dstTemp);
        return static_cast<ani_ref>(dstTemp);
    }
    return it->second(env, src);
}

bool AniParseUtils::ParseArrayBuffer(ani_env* env, ani_object script, std::string& outValue)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_boolean isArrayBuffer = false;
    ani_class arrayBufferClass;
    ani_status status;
    if ((status = env->FindClass("escompat.ArrayBuffer", &arrayBufferClass)) != ANI_OK) {
        WVLOG_E("find buffer class error status : %{public}d", status);
        return false;
    }
    if ((env->Object_InstanceOf(script, arrayBufferClass, &isArrayBuffer) != ANI_OK) || !isArrayBuffer) {
        WVLOG_E("Object_InstanceOf error status : %{public}d", status);
        return false;
    }

    char* arrBuf = nullptr;
    size_t byteLength = 0;
    if ((status = env->ArrayBuffer_GetInfo(
             static_cast<ani_arraybuffer>(script), reinterpret_cast<void**>(&arrBuf), &byteLength)) != ANI_OK) {
        WVLOG_E("ArrayBuffer_GetInfo error status : %{public}d", status);
        return false;
    }

    outValue = std::string(arrBuf, byteLength);
    return true;
}

bool AniParseUtils::GetRefProperty(ani_env* env, ani_object param, const char* name, ani_ref& value)
{
    if (env == nullptr) {
        WVLOG_E("null env");
        return false;
    }

    ani_status status;
    ani_boolean isUndefined = true;

    if ((status = env->Object_GetPropertyByName_Ref(param, name, &value)) != ANI_OK) {
        WVLOG_E("status: %{public}d, name : %{public}s", status, name);
        return false;
    }
    if ((status = env->Reference_IsUndefined(value, &isUndefined)) != ANI_OK) {
        WVLOG_E("status: %{public}d, name : %{public}s", status, name);
        return false;
    }
    if (isUndefined) {
        return false;
    }
    return true;
}
}
}

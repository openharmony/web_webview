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

#ifndef OHOS_NWEB_ANI_PARSE_UTILS_H
#define OHOS_NWEB_ANI_PARSE_UTILS_H

#include <ani.h>
#include <arpa/inet.h>
#include <string>
#include "nweb.h"
#include "web_errors.h"
#include "nweb_web_message.h"

namespace OHOS {
namespace NWeb {
constexpr int PARSE_ZERO = 0;
constexpr int PARSE_ONE = 1;
constexpr int PARSE_TWO = 2;
constexpr int PARSE_THREE = 3;
constexpr int PARSE_FOUR = 4;
constexpr int PARSE_FIVE = 5;
constexpr int MAX_STRING_TO_INT32_LENGTH = 10;
ani_ref ConvertToAniHandlerOfStringArr(ani_env* env, std::shared_ptr<NWebMessage> src);
ani_ref ConvertToAniHandlerOfBooleanArr(ani_env* env, std::shared_ptr<NWebMessage> src);
ani_ref ConvertToAniHandlerOfDoubleArr(ani_env* env, std::shared_ptr<NWebMessage> src);
ani_ref ConvertToAniHandlerOfInt64Arr(ani_env* env, std::shared_ptr<NWebMessage> src);
class AniParseUtils {
public:
    static bool ParseString(ani_env *env, ani_ref ref, std::string& outValue);
    static bool IsString(ani_env *env, const ani_object& object);
    static bool IsResource(ani_env *env, const ani_object& object);
    static void* Unwrap(ani_env *env, const ani_object& object);
    static bool Wrap(ani_env *env, const ani_object& object, const char *className, const ani_long& thisVar);
    static bool CreateObjectVoid(ani_env *env, const char *className, ani_object& object);
    static bool GetEnumItemByIndex(ani_env *env, const char* enumName, int32_t typeIndex, ani_enum_item& eType);
    static ani_status SetPropertyByName_String(ani_env *env, ani_object aniCls,
                                               const char *keyName, std::string keyValue);
    static std::shared_ptr<CacheOptions> ParseCacheOptions(ani_env* env, ani_object cacheOptions);
    static bool ParseStringArray(ani_env* env, ani_object argv, std::vector<std::string>& outValue);
    static bool ParseStringArrayMap(ani_env* env, ani_object argv, std::map<std::string, std::string>& outValue);
    static bool GetStringList(ani_env *env, ani_object array, std::vector<std::string>& outValue);
    static bool ParseIP(ani_env *env, ani_object urlObj, std::string& ip);
    static bool ParseJsLengthStringToInt(const std::string &input, PixelUnit &type, int32_t &value);
    static bool ParseInt32(ani_env *env, ani_ref ref, int32_t& outValue);
    static bool IsFunction(ani_env *env, const ani_object& object);
    static bool IsDouble(ani_env *env, const ani_object& object);
    static bool IsObject(ani_env *env, const ani_object& object);
    static ani_string StringToAniStr(ani_env* env, const std::string& str);
    static bool CreateBoolean(ani_env *env, bool src, ani_object& aniObj);
    static ani_object CreateDouble(ani_env *env, ani_double val); 
    static ani_object CreateInt(ani_env *env, ani_int val);
    static ani_string StringToAniStr(ani_env* env, const std::string& str);
    static ani_ref CreateAniStringArray(ani_env *env, const std::vector<std::string> &paths);
    static bool ParseBoolean(ani_env* env, ani_ref ref, bool& outValue);
    static bool ParseInt64(ani_env* env, ani_ref ref, int64_t& outValue);
    static bool ParseDouble(ani_env* env, ani_ref ref, double& outValue);
    static bool IsBoolean(ani_env* env, const ani_object& object);
    static bool IsInteger(ani_env* env, const ani_object& object);
    static bool ParseInt64Array(ani_env* env, ani_object argv, std::vector<int64_t>& outValue);
    static bool ParseBooleanArray(ani_env* env, ani_object argv, std::vector<bool>& outValue);
    static bool ParseDoubleArray(ani_env* env, ani_object argv, std::vector<double>& outValue);
    static bool ParseDouble_t(ani_env* env, ani_ref ref, double& outValue);
    static bool ParseBoolean_t(ani_env* env, ani_ref ref, bool& outValue);
    static ErrCode ConstructStringFlowbuf(ani_env *env, const std::string script,
                                          int& fd, size_t& scriptLength);
    static ErrCode ConstructArrayBufFlowbuf(ani_env *env, const ani_object script,
                                            int& fd, size_t& scriptLength);
    static ani_ref ConvertNWebToAniValue(ani_env *env, std::shared_ptr<NWebMessage> src);
    static bool ParseArrayBuffer(ani_env *env, ani_object script, std::string& outValue);
};
} // namespace NWeb
} // namespace OHOS
#endif
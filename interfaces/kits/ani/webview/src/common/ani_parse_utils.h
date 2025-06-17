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
#include <string>
#include <vector>

#include "nweb.h"
#include "nweb_web_message.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
    constexpr int PARSE_ZERO = 0;
    constexpr int PARSE_ONE = 1;
    constexpr int PARSE_TWO = 2;
    constexpr int PARSE_THREE = 3;
    constexpr int PARSE_FOUR = 4;
    constexpr int PARSE_FIVE = 5;
    constexpr int MAX_STRING_TO_INT32_LENGTH = 10;
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
    static bool ParseJsLengthStringToInt(const std::string &input, PixelUnit &type, int32_t &value);
    static bool ParseInt32(ani_env *env, ani_ref ref, int32_t& outValue);
    static bool IsFunction(ani_env *env, const ani_object& object);
    static bool IsDouble(ani_env *env, const ani_object& object);
    static bool IsObject(ani_env *env, const ani_object& object);
};
} // namespace NWeb
} // namespace OHOS
#endif
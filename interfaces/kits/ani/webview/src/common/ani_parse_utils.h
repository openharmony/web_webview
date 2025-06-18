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

namespace OHOS {
namespace NWeb {
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
};
} // namespace NWeb
} // namespace OHOS
#endif
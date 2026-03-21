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

#ifndef TEST_MOCK_ANI_PARSE_UTILS_H
#define TEST_MOCK_ANI_PARSE_UTILS_H

#include <ani.h>
#include <string>
#include <vector>
#include <map>
#include "nweb.h"
#include "web_errors.h"
#include "nweb_web_message.h"

namespace OHOS {
namespace NWeb {

// Forward declaration
class CacheOptions;

class AniParseUtils {
public:
    // AniParseUtils Mock Object Pool Index
    enum AniMockObjectPoolIndex {
        ANI_MOCK_OBJECT_VOID_OBJ = 0,      // CreateObjectVoid function index
        ANI_MOCK_OBJECT_MAX_SIZE = 10      // Object pool size
    };

    // General Mock Object Pool
    static char g_aniMockObjectPool[ANI_MOCK_OBJECT_MAX_SIZE];
    // Method required for current test
    static void* Unwrap(ani_env* env, const ani_object& object)
    {
        return g_testControllerPtr;
    }

    // Mock implementation for other methods (return default values)
    static bool ParseString(ani_env* env, ani_ref ref, std::string& outValue) { return true; }
    static bool IsString(ani_env* env, const ani_object& object) { return false; }
    static bool IsResource(ani_env* env, const ani_object& object) { return false; }
    static bool Wrap(ani_env* env, const ani_object& object, const char* className, const ani_long& thisVar)
    {
        return true;
    }
    static bool CreateObjectVoid(ani_env* env, const char* className, ani_object& object)
    {
        if (g_createObjectVoidShouldFail) {
            return false;
        }

        object = reinterpret_cast<ani_object>(&g_aniMockObjectPool[ANI_MOCK_OBJECT_VOID_OBJ]);
        return true;
    }
    static bool GetEnumItemByIndex(ani_env* env, const char* enumName, int32_t typeIndex, ani_enum_item& eType)
    {
        return true;
    }
    static ani_status SetPropertyByName_String(ani_env* env, ani_object aniCls, const char* keyName,
        std::string keyValue)
    {
        return ANI_OK;
    }
    static std::shared_ptr<CacheOptions> ParseCacheOptions(ani_env* env, ani_object cacheOptions) { return nullptr; }
    static bool ParseStringArray(ani_env* env, ani_object argv, std::vector<std::string>& outValue) { return true; }
    static bool EnumParseInt32_t(ani_env* env, ani_enum_item enum_item, int32_t& outValue) { return true; }
    static bool ParseStringArrayMap(ani_env* env, ani_object argv, std::map<std::string, std::string>& outValue)
    {
        return true;
    }
    static bool GetStringList(ani_env* env, ani_object array, std::vector<std::string>& outValue) { return true; }
    static bool ParseIP(ani_env* env, ani_object urlObj, std::string& ip) { return true; }
    static bool ParseJsLengthStringToInt(const std::string& input, PixelUnit& type, int32_t& value) { return true; }
    static bool ParseInt32(ani_env* env, ani_ref ref, int32_t& outValue) { return true; }
    static bool IsFunction(ani_env* env, const ani_object& object) { return g_isFunctionResult; }
    static bool IsDouble(ani_env* env, const ani_object& object) { return false; }
    static bool IsObject(ani_env* env, const ani_object& object) { return false; }
    static ani_string StringToAniStr(ani_env* env, const std::string& str) { return nullptr; }
    static bool CreateBoolean(ani_env* env, bool src, ani_object& aniObj) { return true; }
    static ani_object CreateDouble(ani_env* env, ani_double val) { return nullptr; }
    static ani_object CreateInt(ani_env* env, ani_int val) { return nullptr; }
    static ani_ref CreateAniStringArray(ani_env* env, const std::vector<std::string>& paths) { return nullptr; }
    static bool ParseBoolean(ani_env* env, ani_ref ref, bool& outValue) { return true; }
    static bool ParseInt64(ani_env* env, ani_ref ref, int64_t& outValue) { return true; }
    static bool ParseDouble(ani_env* env, ani_ref ref, double& outValue) { return true; }
    static bool IsBoolean(ani_env* env, const ani_object& object) { return false; }
    static bool IsInteger(ani_env* env, const ani_object& object) { return false; }
    static bool ParseInt64Array(ani_env* env, ani_object argv, std::vector<int64_t>& outValue) { return true; }
    static bool ParseBooleanArray(ani_env* env, ani_object argv, std::vector<bool>& outValue) { return true; }
    static bool ParseDoubleArray(ani_env* env, ani_object argv, std::vector<double>& outValue) { return true; }
    static bool ParseDouble_t(ani_env* env, ani_ref ref, double& outValue) { return true; }
    static bool ParseBoolean_t(ani_env* env, ani_ref ref, bool& outValue) { return true; }
    static ErrCode ConstructStringFlowbuf(ani_env* env, const std::string script, int& fd, size_t& scriptLength)
    {
        return 0;
    }
    static ErrCode ConstructArrayBufFlowbuf(ani_env* env, const ani_object script, int& fd, size_t& scriptLength)
    {
        return 0;
    }
    static ani_ref ConvertNWebToAniValue(ani_env* env, std::shared_ptr<NWebMessage> src) { return nullptr; }
    static bool ParseArrayBuffer(ani_env* env, ani_object script, std::string& outValue) { return true; }
    static bool GetRefProperty(ani_env* env, ani_object param, const char* name, ani_ref& value) { return true; }

    // Points to Mock WebviewController object (void* type, use reinterpret_cast)
    static void* g_testControllerPtr;

    // Control IsFunction return value
    static bool g_isFunctionResult;

    // Control EnumItem_GetValue_Int behavior
    static ani_status g_enumGetIntStatus;
    static ani_int g_enumGetIntResult;

    // Control CreateObjectVoid behavior
    static bool g_createObjectVoidShouldFail;

    // Control Object_SetPropertyByName_Double behavior
    static bool g_setPropertyByNameDoubleShouldFail;
};

void* AniParseUtils::g_testControllerPtr = nullptr;
bool AniParseUtils::g_isFunctionResult = true;
ani_status AniParseUtils::g_enumGetIntStatus = ANI_OK;
ani_int AniParseUtils::g_enumGetIntResult = 0;
bool AniParseUtils::g_createObjectVoidShouldFail = false;
bool AniParseUtils::g_setPropertyByNameDoubleShouldFail = false;
char AniParseUtils::g_aniMockObjectPool[ANI_MOCK_OBJECT_MAX_SIZE] = {0};

} // namespace NWeb
} // namespace OHOS

#endif // TEST_MOCK_ANI_PARSE_UTILS_H

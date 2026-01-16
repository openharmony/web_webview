/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <array>
#include <iostream>
#include <mutex>
#include <vector>

#include "ani_user_agent_metadata.h"
#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "ani_user_agent_brand_version.h"
#include "nweb_log.h"
#include "nweb_user_agent_brand_version_impl.h"
#include "nweb_user_agent_metadata_impl.h"
#include "web_errors.h"
#include "webview_controller.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
std::recursive_mutex g_metadataLock;
bool g_metadataMode = true;
const char* ANI_USER_AGENT_METADATA_CLASS_NAME = "@ohos.web.webview.webview.UserAgentMetadata";
const char* ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME = "@ohos.web.webview.webview.UserAgentBrandVersion";
const char* ANI_USER_AGENT_FORM_FACTOR_ENUM_NAME = "@ohos.web.webview.webview.UserAgentFormFactor";
} // namespace

static bool CheckArrayParams(ani_env* env, ani_object arrayObject)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_boolean isArray = ANI_FALSE;
    if (env->FindClass("escompat.Array", &cls) != ANI_OK) {
        WVLOG_E("no array class");
        return false;
    }
    env->Object_InstanceOf(arrayObject, cls, &isArray);
    if (!isArray) {
        WVLOG_E("arrayObject must be array");
        return false;
    }
    return true;
}

static bool GetBrandVersionVector(
    ani_env* env, ani_object arrayObj, std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>>& brandVersionVector)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if (!CheckArrayParams(env, arrayObj)) {
        return false;
    }
    ani_array arrayRef {};
    arrayRef = static_cast<ani_array>(arrayObj);
    ani_size arrayLen = 0;
    if (env->Array_GetLength(arrayRef, &arrayLen) != ANI_OK || arrayLen > SIZE_MAX) {
        WVLOG_E("Invalid array length");
        return false;
    }
    ani_class versionClass;
    if (env->FindClass(ANI_USER_AGENT_BRAND_VERSION_CLASS_NAME, &versionClass) != ANI_OK) {
        WVLOG_E("FindClass failed");
        return false;
    }
    for (uint32_t i = 0; i < static_cast<uint32_t>(arrayLen); i++) {
        ani_ref element = nullptr;
        if (env->Array_Get(arrayRef, i, &element) != ANI_OK || element == nullptr) {
            WVLOG_E(" GetBrandVersionVector get element from array error");
            return false;
        }
        ani_boolean isInstance = ANI_FALSE;
        ani_object elementObj = static_cast<ani_object>(element);
        if (env->Object_InstanceOf(elementObj, versionClass, &isInstance) != ANI_OK || !isInstance) {
            WVLOG_E("Array element is no UserAgentBrandVerson");
            return false;
        }
        NWebUserAgentBrandVersionImpl* brandVersion =
            reinterpret_cast<NWebUserAgentBrandVersionImpl*>(AniParseUtils::Unwrap(env, elementObj));
        if ((!brandVersion)) {
            WVLOG_E("Unwrap UserAgentBrandVersion failed");
            return false;
        }
        auto ptrBrandVersion = std::make_shared<OHOS::NWeb::NWebUserAgentBrandVersionImpl>(
            brandVersion->GetBrand(), brandVersion->GetMajorVersion(), brandVersion->GetFullVersion());
        brandVersionVector.push_back(ptrBrandVersion);
    }
    return true;
}

static void SetBrandVersionList(ani_env* env, ani_object object, ani_object uaArrayObject)
{
    WVLOG_D(" SetBrandVersionList");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>> brandVersionVector;
    bool ret = GetBrandVersionVector(env, uaArrayObject, brandVersionVector);
    if (!ret) {
        return;
    }

    metadata->SetBrandVersionListPtr(brandVersionVector);
}

static void SetArchitecture(ani_env* env, ani_object object, ani_string archObject)
{
    WVLOG_D(" SetArchitecture");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, archObject, value)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "arch", "string"));
        return;
    }
    WVLOG_D(" SetArchitecture %{public}s.", value.c_str());
    metadata->SetArchitecture(value);
}

static void SetBitness(ani_env* env, ani_object object, ani_string bitnessObject)
{
    WVLOG_D(" SetBitness");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, bitnessObject, value)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "bitness", "string"));
        return;
    }
    WVLOG_D(" SetBitness %{public}s.", value.c_str());
    metadata->SetBitness(value);
}

static void SetFormFactors(ani_env* env, ani_object object, ani_object formFactorsObject)
{
    if (!CheckArrayParams(env, formFactorsObject)) {
        return;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::vector<std::string> formFactors;
    ani_array arrayRef {};
    arrayRef = static_cast<ani_array>(formFactorsObject);
    ani_size arrayLen = 0;
    env->Array_GetLength(arrayRef, &arrayLen);
    ani_class enumClass;
    if (env->FindClass(ANI_USER_AGENT_FORM_FACTOR_ENUM_NAME, &enumClass) != ANI_OK) {
        WVLOG_E("SetFormFactors no enum");
        return;
    }
    for (uint32_t i = 0; i < static_cast<uint32_t>(arrayLen); i++) {
        ani_ref element = nullptr;
        if (env->Array_Get(arrayRef, i, &element) != ANI_OK) {
            WVLOG_E(" get element from array error");
            return;
        }
        ani_object elementObj = static_cast<ani_object>(element);

        ani_boolean isEnum;
        auto statusInstanceOf = env->Object_InstanceOf(elementObj, enumClass, &isEnum);
        if (statusInstanceOf != ANI_OK) {
            WVLOG_E("enumObject failed - Object_InstanceOf error: %d", statusInstanceOf);
            return;
        }

        ani_string resultStr;
        ani_status status = env->EnumItem_GetValue_String(static_cast<ani_enum_item>(elementObj), &resultStr);
        if (status != ANI_OK) {
            WVLOG_E("get enum strimg error");
            return;
        }
        std::string value;
        if (!AniParseUtils::ParseString(env, resultStr, value)) {
            WVLOG_E("SetFormFactors ParseString failed");
            return;
        }
        formFactors.push_back(value);
    }
    metadata->SetFormFactors(formFactors);
}

static void SetFullVersion(ani_env* env, ani_object object, ani_string fullVersionObject)
{
    WVLOG_D(" SetFullVersion");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, fullVersionObject, value)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "fullVersion", "string"));
        return;
    }
    WVLOG_D(" SetFullVersion %{public}s.", value.c_str());
    metadata->SetFullVersion(value);
}

static void SetMobile(ani_env* env, ani_object object, ani_boolean isMobile)
{
    WVLOG_D(" SetMobile");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    WVLOG_D(" SetMobile %{public}d.", isMobile);
    metadata->SetMobile(static_cast<bool>(isMobile));
}

static void SetModel(ani_env* env, ani_object object, ani_string modelObject)
{
    WVLOG_D(" SetModel");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, modelObject, value)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "model", "string"));
        return;
    }
    WVLOG_D(" SetModel %{public}s.", value.c_str());
    metadata->SetModel(value);
}

static void SetPlatform(ani_env* env, ani_object object, ani_string platformObject)
{
    WVLOG_D(" SetPlatform");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, platformObject, value)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "platform", "string"));
        return;
    }
    WVLOG_D(" SetPlatform %{public}s.", value.c_str());
    metadata->SetPlatform(value);
}

static void SetPlatformVersion(ani_env* env, ani_object object, ani_string platformVersionObject)
{
    WVLOG_D(" SetPlatformVersion");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, platformVersionObject, value)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "platformVersion", "string"));
        return;
    }
    WVLOG_D(" SetPlatformVersion %{public}s.", value.c_str());
    metadata->SetPlatformVersion(value);
}

static void SetWow64(ani_env* env, ani_object object, ani_boolean isWow64)
{
    WVLOG_D(" SetWow64");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return;
    }
    WVLOG_D(" SetWow64 %{public}d.", isWow64);
    metadata->SetWow64(static_cast<bool>(isWow64));
}

static ani_ref GetBrandVersionsInternal(ani_env* env, NWebUserAgentMetadataImpl* metadata)
{
    if (!metadata) {
        WVLOG_E(" metadata is null");
        return nullptr;
    }
    ani_object arrayObj = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_class arrayCls = nullptr;
    if (env->FindClass("std.core.Array", &arrayCls) != ANI_OK) {
        WVLOG_E("find class std.core.Array failed");
        return nullptr;
    }
    ani_method arrayCtor;
    if (env->Class_FindMethod(arrayCls, "<ctor>", "i:", &arrayCtor) != ANI_OK) {
        WVLOG_E("find class ctor failed");
        return nullptr;
    }
    std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>> versions = metadata->GetBrandVersionListPtr();
    if (env->Object_New(arrayCls, arrayCtor, &arrayObj, versions.size()) != ANI_OK) {
        WVLOG_E("Object_New Array failed");
        return nullptr;
    }
    for (size_t i = 0; i < versions.size(); i++) {
        ani_object obj = {};
        obj = CreateUserAgentBrandVersionObject(env, versions[i]);
        if (obj == nullptr) {
            WVLOG_E("Create brand object failed");
            return nullptr;
        }
        if (env->Object_CallMethodByName_Void(arrayObj, "$_set", "iC{std.core.Object}:", i, obj) != ANI_OK) {
            WVLOG_E("Set object failed");
            return nullptr;
        }
    }
    return arrayObj;
}

static ani_ref GetBrandVersionList(ani_env* env, ani_object object)
{
    WVLOG_D("GetBrandVersionList.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E("Unwrap metadata failed");
        return nullptr;
    }

    return GetBrandVersionsInternal(env, metadata);
}

static ani_string GetArchitecture(ani_env* env, ani_object object)
{
    ani_string value = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return value;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return value;
    }

    std::string result = metadata->GetArchitecture();
    env->String_NewUTF8(result.c_str(), result.size(), &value);

    return value;
}

static ani_string GetBitness(ani_env* env, ani_object object)
{
    ani_string value = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return value;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return value;
    }

    std::string result = metadata->GetBitness();
    env->String_NewUTF8(result.c_str(), result.size(), &value);

    return value;
}

static ani_object CreateAniStringArray(ani_env* env, const std::vector<std::string>& arr)
{
    WVLOG_D(" CreateAniStringArray.");
    if (env == nullptr) {
        WVLOG_E(" env is nullptr");
        return nullptr;
    }

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E(" GetUndefined Failed.");
        return nullptr;
    }

    ani_array array;
    if (ANI_OK != env->Array_New(arr.size(), undefinedRef, &array)) {
        WVLOG_E(" new array ref error.");
        return nullptr;
    }
    ani_enum enumType;
    if (ANI_OK != env->FindEnum(ANI_USER_AGENT_FORM_FACTOR_ENUM_NAME, &enumType)) {
        WVLOG_E("find enumType failed");
        return nullptr;
    }
    for (size_t i = 0; i < arr.size(); ++i) {
        ani_enum_item enumItem = nullptr;
        std::map<std::string, int> formFactorMap { { "Automotive", 0 }, { "Desktop", 1 }, { "Mobile", 2 },
            { "EInk", 3 }, { "Tablet", 4 }, { "Watch", 5 }, { "XR", 6 } };
        auto it = formFactorMap.find(arr[i]);
        if (it == formFactorMap.end()) {
            WVLOG_E(" no find item");
            return nullptr;
        }
        if (ANI_OK != env->Enum_GetEnumItemByIndex(enumType, it->second, &enumItem)) {
            WVLOG_E("get %{public}s item by index failed", arr[i].c_str());
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(array, i, enumItem)) {
            return nullptr;
        }
    }
    return array;
}

static ani_object GetFormFactors(ani_env* env, ani_object object)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return nullptr;
    }

    std::vector<std::string> formFactors = metadata->GetFormFactors();
    return CreateAniStringArray(env, formFactors);
}

static ani_string GetFullVersion(ani_env* env, ani_object object)
{
    ani_string value = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return value;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return value;
    }

    std::string result = metadata->GetFullVersion();
    env->String_NewUTF8(result.c_str(), result.size(), &value);

    return value;
}

static ani_boolean GetMobile(ani_env* env, ani_object object, ani_boolean enable)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return ANI_FALSE;
    }

    if (!metadata->GetMobile()) {
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

static ani_string GetModel(ani_env* env, ani_object object)
{
    ani_string value = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return value;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return value;
    }

    std::string result = metadata->GetModel();
    env->String_NewUTF8(result.c_str(), result.size(), &value);

    return value;
}

static ani_string GetPlatform(ani_env* env, ani_object object)
{
    ani_string value = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return value;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return value;
    }

    std::string result = metadata->GetPlatform();
    env->String_NewUTF8(result.c_str(), result.size(), &value);

    return value;
}

static ani_string GetPlatformVersion(ani_env* env, ani_object object)
{
    ani_string value = nullptr;

    if (!env) {
        WVLOG_E("env is nullptr");
        return value;
    }

    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return value;
    }

    std::string result = metadata->GetPlatformVersion();
    env->String_NewUTF8(result.c_str(), result.size(), &value);

    return value;
}

static ani_boolean GetWow64(ani_env* env, ani_object object)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* metadata = reinterpret_cast<NWebUserAgentMetadataImpl*>(AniParseUtils::Unwrap(env, object));
    if (!metadata) {
        WVLOG_E(" UserAgentMetadata is null");
        return ANI_FALSE;
    }

    if (!metadata->GetWow64()) {
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

ani_object CreateUserAgentMetadataObject(ani_env* env, std::shared_ptr<NWebUserAgentMetadata> metadata)
{
    if (!metadata) {
        return nullptr;
    }
    std::lock_guard<std::recursive_mutex> lock(g_metadataLock);
    g_metadataMode = false;
    std::vector<std::string> strBrand;
    std::vector<std::string> strMajorVersion;
    std::vector<std::string> strFullVersion;
    metadata->GetBrandVersionList(strBrand, strMajorVersion, strFullVersion);
    UserAgentMetadataInfo metadataInfo { .arch = metadata->GetArchitecture(),
        .bitness = metadata->GetBitness(),
        .formFactors = metadata->GetFormFactors(),
        .fullVersion = metadata->GetFullVersion(),
        .isMobile = metadata->GetMobile(),
        .model = metadata->GetModel(),
        .platform = metadata->GetPlatform(),
        .platformVersion = metadata->GetPlatformVersion(),
        .isWow64 = metadata->GetWow64() };
    auto* webMetadata = new (std::nothrow)
        NWebUserAgentMetadataImpl(strBrand, strMajorVersion, strFullVersion, metadataInfo);
    if (!webMetadata) {
        WVLOG_E("[UserAgentMetata] new NWebUserAgentMetadataImpl failed");
        g_metadataMode = true;
        return nullptr;
    }
    ani_object metadataObj = {};
    if (AniParseUtils::CreateObjectVoid(env, ANI_USER_AGENT_METADATA_CLASS_NAME, metadataObj) == false) {
        WVLOG_E("[UserAgentMetata] CreateObjectVoid failed");
        delete webMetadata;
        webMetadata = nullptr;
        g_metadataMode = true;
        return nullptr;
    }
    if (!AniParseUtils::Wrap(
        env, metadataObj, ANI_USER_AGENT_METADATA_CLASS_NAME, reinterpret_cast<ani_long>(webMetadata))) {
        WVLOG_E("[UserAgentMetata] UserAgentMetata wrap failed");
        delete webMetadata;
        webMetadata = nullptr;
        g_metadataMode = true;
        return nullptr;
    }
    g_metadataMode = true;
    return metadataObj;
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_D(" AniUserAgentMetadata native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    {
        std::lock_guard<std::recursive_mutex> lock(g_metadataLock);
        if (g_metadataMode) {
            WVLOG_D(" AniUserAgentMetadata mode true");
            NWebUserAgentMetadataImpl* metadata = new (std::nothrow) NWebUserAgentMetadataImpl();
            if (metadata == nullptr) {
                WVLOG_E("new UserAgentMetadata failed");
                return;
            }
            if (!AniParseUtils::Wrap(
                env, object, ANI_USER_AGENT_METADATA_CLASS_NAME, reinterpret_cast<ani_long>(metadata))) {
                WVLOG_E("UserAgentMetadata wrap failed");
                delete metadata;
                metadata = nullptr;
            }
        }
    }
}

ani_status StsUserAgentMetadataInit(ani_env* env)
{
    WVLOG_D(" StsUserAgentMetadataInit");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class UserAgentMetadataCls = nullptr;
    ani_status status = env->FindClass(ANI_USER_AGENT_METADATA_CLASS_NAME, &UserAgentMetadataCls);
    if (status != ANI_OK || !UserAgentMetadataCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d",
            ANI_USER_AGENT_METADATA_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "setBrandVersionList", nullptr, reinterpret_cast<void*>(SetBrandVersionList) },
        ani_native_function { "setArchitecture", nullptr, reinterpret_cast<void*>(SetArchitecture) },
        ani_native_function { "setBitness", nullptr, reinterpret_cast<void*>(SetBitness) },
        ani_native_function { "setFormFactors", nullptr, reinterpret_cast<void*>(SetFormFactors) },
        ani_native_function { "setFullVersion", nullptr, reinterpret_cast<void*>(SetFullVersion) },
        ani_native_function { "setMobile", nullptr, reinterpret_cast<void*>(SetMobile) },
        ani_native_function { "setModel", nullptr, reinterpret_cast<void*>(SetModel) },
        ani_native_function { "setPlatform", nullptr, reinterpret_cast<void*>(SetPlatform) },
        ani_native_function { "setPlatformVersion", nullptr, reinterpret_cast<void*>(SetPlatformVersion) },
        ani_native_function { "setWow64", nullptr, reinterpret_cast<void*>(SetWow64) },
        ani_native_function { "getBrandVersionList", nullptr, reinterpret_cast<void*>(GetBrandVersionList) },
        ani_native_function { "getArchitecture", nullptr, reinterpret_cast<void*>(GetArchitecture) },
        ani_native_function { "getBitness", nullptr, reinterpret_cast<void*>(GetBitness) },
        ani_native_function { "getFormFactors", nullptr, reinterpret_cast<void*>(GetFormFactors) },
        ani_native_function { "getFullVersion", nullptr, reinterpret_cast<void*>(GetFullVersion) },
        ani_native_function { "getMobile", nullptr, reinterpret_cast<void*>(GetMobile) },
        ani_native_function { "getModel", nullptr, reinterpret_cast<void*>(GetModel) },
        ani_native_function { "getPlatform", nullptr, reinterpret_cast<void*>(GetPlatform) },
        ani_native_function { "getPlatformVersion", nullptr, reinterpret_cast<void*>(GetPlatformVersion) },
        ani_native_function { "getWow64", nullptr, reinterpret_cast<void*>(GetWow64) },
    };

    status = env->Class_BindNativeMethods(UserAgentMetadataCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
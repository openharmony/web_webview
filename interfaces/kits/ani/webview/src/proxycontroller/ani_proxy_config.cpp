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

#include <array>
#include <iostream>

#include "webview_controller.h"
#include "proxy_config.h"
#include "proxy_rule.h"

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_PROXY_CONFIG_CLASS_NAME = "@ohos.web.webview.webview.ProxyConfig";
const char* WEB_PROXY_RULE_CLASS_NAME = "@ohos.web.webview.webview.ProxyRule";
const char* WEB_PROXY_SCHEME_FILTER_ENUM_NAME = "@ohos.web.webview.webview.ProxySchemeFilter";
static const char* CLASS_NAME_STRING = "Lstd/core/String;";
}

static void JsInsertProxyRule(ani_env *env, ani_object object, ani_string url, ani_enum_item schemeFilter)
{
    WVLOG_D("[PROXYCONTROLLER] JsInsertProxyRule.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(url, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        WVLOG_E("[PROXYCONTROLLER] url is undefined");
        return;
    }
    std::string proxyUrl;
    if (!AniParseUtils::ParseString(env, url, proxyUrl)) {
        WVLOG_E("[PROXYCONTROLLER] Parse url failed.");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return;
    }

    auto* proxyConfig = reinterpret_cast<ProxyConfig *>(AniParseUtils::Unwrap(env, object));
    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return;
    }

    int32_t proxySchemeFilter = static_cast<int32_t>(ProxySchemeFilter::MATCH_ALL_SCHEMES);
    isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(schemeFilter, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        ani_int enumValue;
        if (env->EnumItem_GetValue_Int(schemeFilter, &enumValue) != ANI_OK) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "schemeFilter", "ProxySchemeFilter"));
            return;
        }
        proxySchemeFilter = static_cast<int32_t>(enumValue);
    }
    WVLOG_D("[PROXYCONTROLLER] insert proxy rule %{public}s : %{public}d.", proxyUrl.c_str(), proxySchemeFilter);
    proxyConfig->InsertProxyRule(proxyUrl, proxySchemeFilter);
}

static ani_ref GetProxyRulesInternal(ani_env *env, ProxyConfig* proxyConfig,
                                     ani_class arrayCls, ani_class itemCls, ani_enum enumType)
{
    ani_status status;
    std::vector<ProxyRule> proxyRules = proxyConfig->GetProxyRules();
    size_t rulesSize = proxyRules.size();

    ani_method arrayCtor;
    if (env->Class_FindMethod(arrayCls, "<ctor>", "I:V", &arrayCtor) != ANI_OK) {
        WVLOG_E("[PROXYCONTROLLER] FindClass Lescompat/Array; Failed.");
        return nullptr;
    }

    ani_object arrayObj;
    if (env->Object_New(arrayCls, arrayCtor, &arrayObj, rulesSize) != ANI_OK) {
        WVLOG_E("[PROXYCONTROLLER] Object_New Array Faild.");
        return arrayObj;
    }

    for (size_t i = 0; i < rulesSize; i++) {
        ani_method ctor;
        if ((status = env->Class_FindMethod(itemCls, "<ctor>",
            "Lstd/core/String;L@ohos/web/webview/webview/ProxySchemeFilter;:V", &ctor)) != ANI_OK) {
            WVLOG_E("[PROXYCONTROLLER] Class_FindMethod status: %{public}d", status);
            return arrayObj;
        }
        ani_string itemString{};
        if ((status = env->String_NewUTF8(proxyRules[i].GetUrl().c_str(),
                                          proxyRules[i].GetUrl().size(), &itemString)) != ANI_OK) {
            WVLOG_E("[PROXYCONTROLLER] String_NewUTF8 status: %{public}d", status);
            return arrayObj;
        }

        ani_enum_item itemEnum;
        if ((status = env->Enum_GetEnumItemByIndex(enumType, ani_size(proxyRules[i].GetSchemeFilter()), &itemEnum))
            != ANI_OK) {
            WVLOG_E("[PROXYCONTROLLER] Enum_GetEnumItemByIndex status: %{public}d", status);
            return arrayObj;
        }

        ani_object inputObject = nullptr;
        if ((status = env->Object_New(itemCls, ctor, &inputObject, itemString, itemEnum)) != ANI_OK ||
            inputObject == nullptr) {
            WVLOG_E("[PROXYCONTROLLER] Object_New status: %{public}d", status);
            return arrayObj;
        }

        if (env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", i, inputObject) != ANI_OK) {
            WVLOG_E("[PROXYCONTROLLER] Object_New status: %{public}d", status);
            return arrayObj;
        }
    }
    return arrayObj;
}

static ani_ref JsGetProxyRule(ani_env *env, ani_object object)
{
    WVLOG_D("[PROXYCONTROLLER] JsGetProxyRule.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return nullptr;
    }

    auto* proxyConfig = reinterpret_cast<ProxyConfig *>(AniParseUtils::Unwrap(env, object));
    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return nullptr;
    }

    ani_class itemCls;
    ani_status status;
    if ((status = env->FindClass(WEB_PROXY_RULE_CLASS_NAME, &itemCls)) != ANI_OK) {
        WVLOG_E("[PROXYCONTROLLER] FindClass status: %{public}d", status);
        return nullptr;
    }

    ani_enum enumType;
    if ((status = env->FindEnum(WEB_PROXY_SCHEME_FILTER_ENUM_NAME, &enumType)) != ANI_OK) {
        WVLOG_E("[PROXYCONTROLLER] FindEnum status: %{public}d", status);
        return nullptr;
    }

    ani_class arrayCls = nullptr;
    if (env->FindClass("escompat.Array", &arrayCls) != ANI_OK) {
        WVLOG_E("[PROXYCONTROLLER] FindClass Lescompat/Array; Failed.");
        return nullptr;
    }
    return GetProxyRulesInternal(env, proxyConfig, arrayCls, itemCls, enumType);
}

static void JsInsertBypassRule(ani_env* env, ani_object object, ani_string bypassRule)
{
    WVLOG_D("[PROXYCONTROLLER] JsInsertBypassRule.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return;
    }
    ProxyConfig* proxyConfig = reinterpret_cast<ProxyConfig*>(AniParseUtils::Unwrap(env, object));
    if (proxyConfig == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return;
    }
    std::string bypass;
    if (!AniParseUtils::ParseString(env, bypassRule, bypass)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "bypassRule", "string"));
        return;
    }
    WVLOG_D("[PROXYCONTROLLER] insert bypass rule %{public}s.", bypass.c_str());
    proxyConfig->InsertBypassRule(bypass);
    return;
}

static void JsInsertDirectRule(ani_env* env, ani_object object, ani_enum_item schemeFilter)
{
    WVLOG_D("[PROXYCONTROLLER] JsInsertDirectRule.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return;
    }
    ProxyConfig* proxyConfig = reinterpret_cast<ProxyConfig*>(AniParseUtils::Unwrap(env, object));
    if (proxyConfig == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return;
    }

    ani_boolean isUndefined = ANI_TRUE;
    int32_t proxySchemeFilter = static_cast<int32_t>(ProxySchemeFilter::MATCH_ALL_SCHEMES);
    env->Reference_IsUndefined(schemeFilter, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        ani_int enumValue;
        if (env->EnumItem_GetValue_Int(schemeFilter, &enumValue) != ANI_OK) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "schemeFilter", "ProxySchemeFilter"));
            return;
        }
        proxySchemeFilter = static_cast<int32_t>(enumValue);
    }
    WVLOG_D("[PROXYCONTROLLER] insert direct rule %{public}d.", proxySchemeFilter);
    proxyConfig->InsertDirectRule(proxySchemeFilter);
    return;
}

static void JsBypassHostnamesWithoutPeriod(ani_env* env, ani_object object)
{
    WVLOG_D("[PROXYCONTROLLER] JsBypassHostnamesWithoutPeriod.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return;
    }
    ProxyConfig* proxyConfig = reinterpret_cast<ProxyConfig*>(AniParseUtils::Unwrap(env, object));
    if (proxyConfig == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return;
    }
    WVLOG_D("[PROXYCONTROLLER] bypass host names without period.");
    proxyConfig->BypassHostnamesWithoutPeriod();
    return;
}

static void JsClearImplicitRules(ani_env* env, ani_object object)
{
    WVLOG_D("[PROXYCONTROLLER] JsClearImplicitRules.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return;
    }
    ProxyConfig* proxyConfig = reinterpret_cast<ProxyConfig*>(AniParseUtils::Unwrap(env, object));
    if (proxyConfig == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return;
    }
    WVLOG_D("[PROXYCONTROLLER] clear implicit rules.");
    proxyConfig->ClearImplicitRules();
    return;
}

static void JsEnableReverseBypass(ani_env* env, ani_object object, ani_boolean reverse)
{
    WVLOG_D("[PROXYCONTROLLER] JsEnableReverseBypass.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return;
    }
    ProxyConfig* proxyConfig = reinterpret_cast<ProxyConfig*>(AniParseUtils::Unwrap(env, object));
    if (proxyConfig == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return;
    }
    bool enableReverseBypass = static_cast<bool>(reverse);
    WVLOG_D("[PROXYCONTROLLER] enable reverse bypass %{public}d.", enableReverseBypass);
    proxyConfig->EnableReverseBypass(enableReverseBypass);
    return;
}

static ani_object CreateAniStringArray(ani_env* env, const std::vector<std::string>& arr)
{
    WVLOG_D("[PROXYCONTROLLER] CreateAniStringArray.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return nullptr;
    }

    ani_class stringCls = nullptr;
    if (ANI_OK != env->FindClass(CLASS_NAME_STRING, &stringCls)) {
        WVLOG_E("find class %{public}s failed", CLASS_NAME_STRING);
        return nullptr;
    }

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefined Failed.");
        return nullptr;
    }

    ani_array_ref array;
    if (ANI_OK != env->Array_New_Ref(stringCls, arr.size(), undefinedRef, &array)) {
        WVLOG_E("new array ref error.");
        return array;
    }
    for (size_t i = 0; i < arr.size(); ++i) {
        ani_string result {};
        if (ANI_OK != env->String_NewUTF8(arr[i].c_str(), arr[i].size(), &result)) {
            continue;
        }
        if (ANI_OK != env->Array_Set_Ref(array, i, result)) {
            return array;
        }
    }
    WVLOG_D("[PROXYCONTROLLER] CreateAniStringArray Exit");
    return array;
}
static ani_object JsGetBypassRules(ani_env* env, ani_object object)
{
    WVLOG_D("[PROXYCONTROLLER] JsGetBypassRules.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return nullptr;
    }
    ProxyConfig* proxyConfig = reinterpret_cast<ProxyConfig*>(AniParseUtils::Unwrap(env, object));
    if (proxyConfig == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return nullptr;
    }
    std::vector<std::string> bypassRules = proxyConfig->GetBypassRules();
    return CreateAniStringArray(env, bypassRules);
}

static ani_boolean JsIsReverseBypassEnabled(ani_env* env, ani_object object)
{
    WVLOG_D("[PROXYCONTROLLER] JsIsReverseBypassEnabled.");
    if (env == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] env is nullptr");
        return ANI_FALSE;
    }
    ProxyConfig* proxyConfig = reinterpret_cast<ProxyConfig*>(AniParseUtils::Unwrap(env, object));
    if (proxyConfig == nullptr) {
        WVLOG_E("[PROXYCONTROLLER] proxyConfig is null");
        return ANI_FALSE;
    }

    ani_boolean result = ANI_FALSE;
    bool enabled = proxyConfig->IsReverseBypassEnabled();
    WVLOG_D("[PROXYCONTROLLER] is reverse bypass enabled %{public}d", enabled);
    result = static_cast<ani_boolean>(enabled);
    return result;
}

static void Constructor(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    ProxyConfig* proxyConfig = new (std::nothrow) ProxyConfig();
    if (proxyConfig == nullptr) {
        WVLOG_E("new ProxyConfig failed");
        return;
    }
    if (!AniParseUtils::Wrap(env, object, WEB_PROXY_CONFIG_CLASS_NAME,
                             reinterpret_cast<ani_long>(proxyConfig))) {
        WVLOG_E("ProxyConfig wrap failed");
        delete proxyConfig;
        proxyConfig = nullptr;
    }
}

ani_status StsWebProxyConfigInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class proxyConfigCls = nullptr;
    ani_status status = env->FindClass(WEB_PROXY_CONFIG_CLASS_NAME, &proxyConfigCls);
    if (status != ANI_OK || !proxyConfigCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_PROXY_CONFIG_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "insertProxyRule", nullptr, reinterpret_cast<void*>(JsInsertProxyRule) },
        ani_native_function { "getProxyRules", nullptr, reinterpret_cast<void*>(JsGetProxyRule) },
        ani_native_function { "insertBypassRule", nullptr, reinterpret_cast<void*>(JsInsertBypassRule) },
        ani_native_function { "insertDirectRule", nullptr, reinterpret_cast<void*>(JsInsertDirectRule) },
        ani_native_function {
            "bypassHostnamesWithoutPeriod", nullptr, reinterpret_cast<void*>(JsBypassHostnamesWithoutPeriod) },
        ani_native_function { "clearImplicitRules", nullptr, reinterpret_cast<void*>(JsClearImplicitRules) },
        ani_native_function { "enableReverseBypass", nullptr, reinterpret_cast<void*>(JsEnableReverseBypass) },
        ani_native_function { "getBypassRules", nullptr, reinterpret_cast<void*>(JsGetBypassRules) },
        ani_native_function { "isReverseBypassEnabled", nullptr, reinterpret_cast<void*>(JsIsReverseBypassEnabled) },
    };

    status = env->Class_BindNativeMethods(proxyConfigCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
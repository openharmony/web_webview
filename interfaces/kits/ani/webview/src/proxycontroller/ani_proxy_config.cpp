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
const char* WEB_PROXY_CONFIG_CLASS_NAME = "L@ohos/web/webview/webview/ProxyConfig;";
const char* WEB_PROXY_RULE_CLASS_NAME = "L@ohos/web/webview/webview/ProxyRule;";
const char* WEB_PROXY_SCHEME_FILTER_ENUM_NAME = "L@ohos/web/webview/webview/ProxySchemeFilter;";
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
    WVLOG_I("[PROXYCONTROLLER] insert proxy rule %{public}s : %{public}d.", proxyUrl.c_str(), proxySchemeFilter);
    proxyConfig->InsertProxyRule(proxyUrl, proxySchemeFilter);
}

static ani_ref GetProxyRulesInternal(ani_env *env, ProxyConfig* proxyConfig,
                                     ani_class arrayCls, ani_class itemCls, ani_enum enumType)
{
    ani_status status;
    std::vector<ProxyRule> proxyRules = proxyConfig->GetProxyRules();
    size_t rulesSize = proxyRules.size();

    ani_method arrayCtor;
    if(env->Class_FindMethod(arrayCls, "<ctor>", "I:V", &arrayCtor) != ANI_OK){
        WVLOG_E("[PROXYCONTROLLER] FindClass Lescompat/Array; Failed.");
        return nullptr;
    }

    ani_object arrayObj;
    if(env->Object_New(arrayCls, arrayCtor, &arrayObj, rulesSize) != ANI_OK){
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
        if ((status = env->String_NewUTF8(proxyRules[i].GetUrl().c_str(), proxyRules[i].GetUrl().size(),
             &itemString)) != ANI_OK) {
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

        if(env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", i, inputObject) != ANI_OK){
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
    if (env->FindClass("Lescompat/Array;", &arrayCls) != ANI_OK){
        WVLOG_E("[PROXYCONTROLLER] FindClass Lescompat/Array; Failed.");
        return nullptr;
    }
    return GetProxyRulesInternal(env, proxyConfig, arrayCls, itemCls, enumType);
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
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void *>(Constructor) },
        ani_native_function { "insertProxyRule", nullptr, reinterpret_cast<void *>(JsInsertProxyRule) },
        ani_native_function { "getProxyRules", nullptr, reinterpret_cast<void *>(JsGetProxyRule) },
    };

    status = env->Class_BindNativeMethods(proxyConfigCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
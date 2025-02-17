/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi_proxy_rule.h"

#include <cstring>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include "napi_parse_utils.h"
#include <securec.h>

#include "nweb_log.h"
#include "proxy_rule.h"

namespace OHOS {
namespace NWeb {
napi_value NapiProxyRule::JS_GetSchemeFilter(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[PROXYCONTROLLER] NapiProxyRule::JS_GetSchemeFilter");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    ProxyRule *proxyRule = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, &data);

    napi_unwrap(env, thisVar, (void **)&proxyRule);
    if (!proxyRule) {
        WVLOG_E("[PROXYCONTROLLER] NapiProxyRule::JS_GetSchemeFilter proxyRule is null.");
        return nullptr;
    }

    napi_value schemeFilter = nullptr;
    napi_status status = napi_create_int32(env, proxyRule->GetSchemeFilter(), &schemeFilter);
    if (status != napi_ok) {
        WVLOG_E("[PROXYCONTROLLER] NapiProxyRule::JS_SchemeFilter create string failed.");
        return nullptr;
    }
    return schemeFilter;
}

napi_value NapiProxyRule::JS_GetUrl(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[PROXYCONTROLLER] NapiProxyRule::JS_GetUrl");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    ProxyRule *proxyRule = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, &data);

    napi_unwrap(env, thisVar, (void **)&proxyRule);
    if (!proxyRule) {
        WVLOG_E("[PROXYCONTROLLER] NapiProxyRule::JS_GetUrl ProxyRule is null.");
        return nullptr;
    }

    napi_value urlValue = nullptr;
    napi_status status = napi_create_string_utf8(env, proxyRule->GetUrl().c_str(), NAPI_AUTO_LENGTH, &urlValue);
    if (status != napi_ok) {
        WVLOG_E("[PROXYCONTROLLER] NapiProxyRule::JS_GetUrl create string failed.");
        return nullptr;
    }
    return urlValue;
}

napi_value NapiProxyRule::JS_Constructor(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[PROXYCONTROLLER] NapiProxyRule::JS_Constructor is called");
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, &data);

    ProxyRule *proxyRule = new ProxyRule();

    napi_wrap(
        env, thisVar, proxyRule,
        [](napi_env /* env */, void *data, void * /* hint */) {
            ProxyRule *proxyRule = (ProxyRule *)data;
            delete proxyRule;
        },
        nullptr, nullptr);

    return thisVar;
}

napi_value NapiProxyRule::Init(napi_env env, napi_value exports)
{
    WVLOG_D("[PROXYCONTROLLER] NapiProxyRule::Init");
    /* export ProxyRule class */

    ExportProxyRuleClass(env, &exports);

    /* export ProxySchemeFilter enum */

    ExportProxySchemeFilterEnum(env, &exports);

    return exports;
}

void NapiProxyRule::ExportProxyRuleClass(napi_env env, napi_value* exportsPointer)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getUrl", JS_GetUrl),
        DECLARE_NAPI_FUNCTION("getSchemeFilter", JS_GetSchemeFilter),
    };
    napi_value proxyClass = nullptr;
    napi_define_class(env, PROXY_RULE_CLASS_NAME.c_str(), PROXY_RULE_CLASS_NAME.length(), JS_Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &proxyClass);
    napi_set_named_property(env, *exportsPointer, PROXY_RULE_CLASS_NAME.c_str(), proxyClass);
}

void NapiProxyRule::ExportProxySchemeFilterEnum(napi_env env, napi_value* exportsPointer)
{
    napi_value proxySchemeFilterTypeEnum = nullptr;
    napi_property_descriptor proxySchemeFilterProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "MATCH_ALL_SCHEMES",
            NapiParseUtils::ToInt32Value(
                env, static_cast<int32_t>(ProxySchemeFilter::MATCH_ALL_SCHEMES))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MATCH_HTTP",
            NapiParseUtils::ToInt32Value(
                env, static_cast<int32_t>(ProxySchemeFilter::MATCH_HTTP))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MATCH_HTTPS",
            NapiParseUtils::ToInt32Value(
                env, static_cast<int32_t>(ProxySchemeFilter::MATCH_HTTPS))),
    };
    napi_define_class(env, PROXY_SCHEME_FILTER_ENUM_NAME.c_str(), PROXY_SCHEME_FILTER_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr,
        sizeof(proxySchemeFilterProperties) / sizeof(proxySchemeFilterProperties[0]),
        proxySchemeFilterProperties, &proxySchemeFilterTypeEnum);
    napi_set_named_property(env, *exportsPointer, PROXY_SCHEME_FILTER_ENUM_NAME.c_str(), proxySchemeFilterTypeEnum);
}

napi_status NapiProxyRule::DefineProperties(napi_env env, napi_value *object)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getUrl", JS_GetUrl),
        DECLARE_NAPI_FUNCTION("getSchemeFilter", JS_GetSchemeFilter),
    };
    return napi_define_properties(env, *object, sizeof(properties) / sizeof(properties[0]), properties);
}
} // namespace NWeb
} // namespace OHOS
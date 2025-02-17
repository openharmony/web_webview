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

#include "napi_proxy_config.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <securec.h>

#include "business_error.h"
#include "nweb_log.h"
#include "napi_parse_utils.h"
#include "napi/native_node_api.h"
#include "web_errors.h"

#include "proxy_config.h"
#include "proxy_rule.h"
#include "napi_proxy_rule.h"

using namespace OHOS::NWebError;

namespace OHOS {
namespace NWeb {

napi_value NapiProxyConfig::JS_InsertBypassRule(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }

    std::string bypassRule;
    if (!NapiParseUtils::ParseString(env, argv[0], bypassRule)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "bypassRule", "string"));
        return nullptr;
    }

    WVLOG_I("[PROXYCONTROLLER] insert bypass rule %{public}s.", bypassRule.c_str());
    proxyConfig->InsertBypassRule(bypassRule);
    return nullptr;
}

napi_value NapiProxyConfig::JS_InsertDirectRule(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }
    
    if (argc > 0) {
        int32_t schemeFilter = 0;
        if (!NapiParseUtils::ParseInt32(env, argv[0], schemeFilter)) {
            WVLOG_E("[PROXYCONTROLLER] parse int32 failed");
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
                "BusinessError 401: Parameter error. The type of 'schemeFilter' must be int.");
            return nullptr;
        }

        WVLOG_I("[PROXYCONTROLLER] insert direct rule %{public}d.", schemeFilter);
        proxyConfig->InsertDirectRule(schemeFilter);
    } else {
        WVLOG_I("[PROXYCONTROLLER] insert default direct rule.");
        proxyConfig->InsertDirectRule();
    }
    
    return nullptr;
}

napi_value NapiProxyConfig::JS_InsertProxyRule(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }

    std::string proxyUrl;
    if (!NapiParseUtils::ParseString(env, argv[0], proxyUrl)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "proxyUrl", "string"));
        return nullptr;
    }
    if (argc > 1) {
        int32_t schemeFilter = 0;
        if (!NapiParseUtils::ParseInt32(env, argv[1], schemeFilter)) {
            WVLOG_E("[PROXYCONTROLLER] parse int32 failed");
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
                "BusinessError 401: Parameter error. The type of 'schemeFilter' must be int.");
            return nullptr;
        }

        WVLOG_I("[PROXYCONTROLLER] insert proxy rule %{public}s : %{public}d.", proxyUrl.c_str(), schemeFilter);
        proxyConfig->InsertProxyRule(proxyUrl, schemeFilter);
    } else {
        WVLOG_I("[PROXYCONTROLLER] insert proxy rule, the schemeFilter is default.");
        proxyConfig->InsertProxyRule(proxyUrl);
    }
    
    return nullptr;
}

napi_value NapiProxyConfig::JS_BypassHostnamesWithoutPeriod(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }

    WVLOG_I("[PROXYCONTROLLER] bypass host names without period.");
    proxyConfig->BypassHostnamesWithoutPeriod();
    return nullptr;
}

napi_value NapiProxyConfig::JS_ClearImplicitRules(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }

    WVLOG_I("[PROXYCONTROLLER] clear implicit rules.");
    proxyConfig->ClearImplicitRules();
    return nullptr;
}

napi_value NapiProxyConfig::JS_EnableReverseBypass(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }

    bool enableReverseBypass = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], enableReverseBypass)) {
        WVLOG_E("[PROXYCONTROLLER] parse boolean failed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "enableReverseBypass", "boolean"));
        return nullptr;
    }

    WVLOG_I("[PROXYCONTROLLER] enable reverse bypass %{public}d.", enableReverseBypass);
    proxyConfig->EnableReverseBypass(enableReverseBypass);
    return nullptr;
}

napi_value NapiProxyConfig::JS_GetBypassRules(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }

    napi_value result;
    napi_create_array(env, &result);
    std::vector<std::string> bypassRules = proxyConfig->GetBypassRules();
    
    for (size_t index = 0; index < bypassRules.size(); index++) {
        WVLOG_I("[PROXYCONTROLLER] get bypass rules %{public}s.", bypassRules[index].c_str());
        napi_value jsBypassRule = nullptr;
        napi_status status = napi_create_string_utf8(env, bypassRules[index].c_str(),
                                                     bypassRules[index].length(), &jsBypassRule);
        if (status != napi_ok) {
            WVLOG_I("[PROXYCONTROLLER] create js string error");
            continue;
        }
        NAPI_CALL(env, napi_set_element(env, result, index, jsBypassRule));
    }
    
    return result;
}

napi_value NapiProxyConfig::JS_GetProxyRules(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed");
        return nullptr;
    }

    napi_value result = nullptr;
    napi_create_array(env, &result);
    std::vector<ProxyRule> proxyRules = proxyConfig->GetProxyRules();
    for (size_t index = 0; index < proxyRules.size(); index++) {
        WVLOG_I("[PROXYCONTROLLER] get proxy rules %{public}s %{public}d",
                proxyRules[index].GetUrl().c_str(), proxyRules[index].GetSchemeFilter());
        napi_value jsProxyRule = nullptr;
        napi_create_object(env, &jsProxyRule);
        napi_wrap(
            env, jsProxyRule, new ProxyRule(proxyRules[index].GetUrl().c_str(), proxyRules[index].GetSchemeFilter()),
            [](napi_env /* env */, void *data, void * /* hint */) {
                if (data) {
                    ProxyRule *proxyRule = static_cast<ProxyRule *>(data);
                    delete proxyRule;
                }
            },
            nullptr, nullptr);
        NapiProxyRule::DefineProperties(env, &jsProxyRule);
        NAPI_CALL(env, napi_set_element(env, result, index, jsProxyRule));
    }

    return result;
}

napi_value NapiProxyConfig::JS_IsReverseBypassEnabled(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, thisVar, (void **)&proxyConfig);

    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed");
        return nullptr;
    }

    napi_value result = nullptr;
    bool enabled = proxyConfig->IsReverseBypassEnabled();
    WVLOG_I("[PROXYCONTROLLER] is reverse bypass enabled %{public}d", enabled);
    NAPI_CALL(env, napi_get_boolean(env, enabled, &result));
    return result;
}

napi_value NapiProxyConfig::Init(napi_env env, napi_value exports)
{
    WVLOG_I("[PROXYCONTROLLER] NapiProxyConfig::Init is called");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("insertBypassRule", NapiProxyConfig::JS_InsertBypassRule),
        DECLARE_NAPI_FUNCTION("insertDirectRule", NapiProxyConfig::JS_InsertDirectRule),
        DECLARE_NAPI_FUNCTION("insertProxyRule", NapiProxyConfig::JS_InsertProxyRule),
        DECLARE_NAPI_FUNCTION("bypassHostnamesWithoutPeriod", NapiProxyConfig::JS_BypassHostnamesWithoutPeriod),
        DECLARE_NAPI_FUNCTION("clearImplicitRules", NapiProxyConfig::JS_ClearImplicitRules),
        DECLARE_NAPI_FUNCTION("enableReverseBypass", NapiProxyConfig::JS_EnableReverseBypass),
        DECLARE_NAPI_FUNCTION("getBypassRules", NapiProxyConfig::JS_GetBypassRules),
        DECLARE_NAPI_FUNCTION("getProxyRules", NapiProxyConfig::JS_GetProxyRules),
        DECLARE_NAPI_FUNCTION("isReverseBypassEnabled", NapiProxyConfig::JS_IsReverseBypassEnabled),
    };

    napi_value constructor = nullptr;
    napi_define_class(env, PROXY_CONFIG_CLASS_NAME.c_str(), PROXY_CONFIG_CLASS_NAME.length(),
        JS_Constructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "NapiProxyConfig define js class failed.");
    napi_status status = napi_set_named_property(env, exports, PROXY_CONFIG_CLASS_NAME.c_str(), constructor);
    NAPI_ASSERT(env, status == napi_ok, "NapiProxyConfig set property failed.");
    
    return exports;
}

napi_value NapiProxyConfig::JS_Constructor(napi_env env, napi_callback_info info)
{
    WVLOG_I("[PROXYCONTROLLER] NapiProxyConfig::JS_Constructor is called");
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    ProxyConfig *nativeProxyConfig = new ProxyConfig();

    napi_wrap(
        env, thisVar, nativeProxyConfig,
        [](napi_env /* env */, void *data, void * /* hint */) {
            ProxyConfig *proxyConfig = (ProxyConfig *)data;
            delete proxyConfig;
        },
        nullptr, nullptr);

    return thisVar;
}

}
}
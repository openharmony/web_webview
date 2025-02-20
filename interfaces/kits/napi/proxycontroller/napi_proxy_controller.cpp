/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "napi_proxy_controller.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <cstring>
#include "napi_parse_utils.h"

#include "nweb_helper.h"
#include "nweb_log.h"
#include "business_error.h"
#include "web_errors.h"

#include "proxy_config.h"
#include "proxy_rule.h"


namespace OHOS {
namespace NWeb {

napi_value NapiProxyController::JS_Constructor(napi_env env, napi_callback_info info)
{
    WVLOG_I("[PROXYCONTROLLER] NapiProxyController::JS_Constructor");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    return thisVar;
}

napi_value NapiProxyController::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("applyProxyOverride", NapiProxyController::JS_ApplyProxyOverride),
        DECLARE_NAPI_STATIC_FUNCTION("removeProxyOverride", NapiProxyController::JS_RemoveProxyOverride),
    };

    napi_value proxyController = nullptr;
    napi_define_class(env, PROXY_CONTROLLER_CLASS_NAME.c_str(), PROXY_CONTROLLER_CLASS_NAME.length(),
        JS_Constructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &proxyController);
    napi_status status = napi_set_named_property(env, exports, "ProxyController", proxyController);
    NAPI_ASSERT(env, status == napi_ok, "NapiProxyController set property failed.");

    return exports;
}

void NapiProxyController::InnerApplyProxyOverride(ProxyConfig *proxyConfig, napi_env env, napi_ref jsCallback)
{
    std::vector<std::string> proxyUrls;
    std::vector<std::string> proxySchemeFilters;
    std::vector<std::string> bypassRules;
    bool reverseBypass;
    for (auto proxyRule : proxyConfig->GetProxyRules()) {
        WVLOG_I("[PROXYCONTROLLER] add proxy rule %{public}s : %{public}d",
                proxyRule.GetUrl().c_str(), proxyRule.GetSchemeFilter());
        proxyUrls.push_back(proxyRule.GetUrl());
        std::string proxySchemeFilter = "*";
        switch (proxyRule.GetSchemeFilter()) {
            case static_cast<int32_t>(ProxySchemeFilter::MATCH_ALL_SCHEMES):
                break;
            case static_cast<int32_t>(ProxySchemeFilter::MATCH_HTTP):
                proxySchemeFilter = "http";
                break;
            case static_cast<int32_t>(ProxySchemeFilter::MATCH_HTTPS):
                proxySchemeFilter = "https";
                break;
        }
        proxySchemeFilters.push_back(proxySchemeFilter);
    }
    reverseBypass = proxyConfig->IsReverseBypassEnabled();
    WVLOG_I("[PROXYCONTROLLER] reverse bypass  %{public}d", reverseBypass);
    for (auto bypassRule :proxyConfig->GetBypassRules()) {
        WVLOG_I("[PROXYCONTROLLER] add bypass rule %{public}s", bypassRule.c_str());
        bypassRules.push_back(bypassRule);
    }

    auto resultCallback = std::make_shared<ProxyChangedCallbackImpl>(env, jsCallback);
    NWebHelper::Instance().SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules,
                                            reverseBypass, resultCallback);
}

void ProxyChangedCallbackImpl::OnChanged()
{
    napi_value callback = nullptr;
    napi_get_reference_value(env_, callback_, &callback);

    napi_call_function(env_, nullptr, callback, INTEGER_ZERO, nullptr, nullptr);
    napi_delete_reference(env_, callback_);
}

napi_value NapiProxyController::JS_ApplyProxyOverride(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_TWO;
    size_t argcCallback = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = {0};

    WVLOG_I("[PROXYCONTROLLER] apply proxy override.");
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_TWO) {
        WVLOG_E("[PROXYCONTROLLER] param check error.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_valuetype valueType = napi_null;
    napi_typeof(env, argv[argcCallback - 1], &valueType);
    if (valueType != napi_function) {
        WVLOG_E("[PROXYCONTROLLER] last arg is not function %{public}d.", valueType);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
        return nullptr;
    }

    napi_ref jsCallback = nullptr;
    napi_create_reference(env, argv[argcCallback - 1], INTEGER_ONE, &jsCallback);
    if (!jsCallback) {
        WVLOG_E("[PROXYCONTROLLER] generate reference for callback failed.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
        return nullptr;
    }

    ProxyConfig *proxyConfig = nullptr;
    napi_unwrap(env, argv[0], (void **)&proxyConfig);
 
    if (!proxyConfig) {
        WVLOG_E("[PROXYCONTROLLER] unwrap ProxyConfig failed.");
        return nullptr;
    }

    InnerApplyProxyOverride(proxyConfig, env, jsCallback);

    return nullptr;
}

napi_value NapiProxyController::JS_RemoveProxyOverride(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcCallback = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_valuetype valueType = napi_null;
    napi_typeof(env, argv[argcCallback - 1], &valueType);
    if (valueType != napi_function) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
        return nullptr;
    }

    napi_ref jsCallback = nullptr;
    napi_create_reference(env, argv[argcCallback - 1], INTEGER_ONE, &jsCallback);
    if (!jsCallback) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
        return nullptr;
    }

    auto resultCallback = std::make_shared<ProxyChangedCallbackImpl>(env, jsCallback);
    NWebHelper::Instance().RemoveProxyOverride(resultCallback);

    return nullptr;
}
} // namespace NWeb
} // namespace OHOS
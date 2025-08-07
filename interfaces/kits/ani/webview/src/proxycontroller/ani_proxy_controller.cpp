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
#include "ani_proxy_controller.h"

#include "ani.h"
#include "webview_controller.h"
#include "proxy_config.h"
#include "proxy_rule.h"

#include "nweb_helper.h"
#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;

namespace {
const char* WEB_PROXY_CONTROLLER_CLASS_NAME = "L@ohos/web/webview/webview/ProxyController;";
const int ZERO = 0;
} // namespace

static void JsConstructor(ani_env* env, ani_object object)
{
    WVLOG_D("ProxyConfig Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
}

void ProxyChangedCallbackImpl::OnChanged()
{
    ani_ref jsCallback = nullptr;
    ani_ref result;
    if (env_ == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    env_->GlobalReference_Create(callback_, &jsCallback);
    auto status = env_->FunctionalObject_Call(static_cast<ani_fn_object>(jsCallback), ZERO, nullptr, &result);
    if (status != ANI_OK) {
        WVLOG_E("onChanged functionalObject_Call status: %{public}d", status);
        return;
    }
    env_->GlobalReference_Delete(jsCallback);
}

void InnerApplyProxyOverride(ProxyConfig *proxyConfig, ani_env *env, ani_ref jsCallback)
{
    std::vector<std::string> proxyUrls;
    std::vector<std::string> proxySchemeFilters;
    std::vector<std::string> bypassRules;
    bool reverseBypass;
    if (!proxyConfig) {
        WVLOG_E("proxyConfig is nullptr");
        return;
    }
    for (auto proxyRule : proxyConfig->GetProxyRules()) {
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
    WVLOG_I("ProxyConfig reverse bypass  %{public}d", reverseBypass);
    for (auto bypassRule :proxyConfig->GetBypassRules()) {
        WVLOG_I("ProxyConfig add bypass rule %{public}s", bypassRule.c_str());
        bypassRules.push_back(bypassRule);
    }
    auto resultCallback = std::make_shared<ProxyChangedCallbackImpl>(env, jsCallback);
    NWebHelper::Instance().SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules,
                                            reverseBypass, resultCallback);
}

static void JsApplyProxyOverride(ani_env* env, ani_object object, ani_object proxyConfigObject, ani_fn_object callback)
{
    WVLOG_D("ProxyConfig JsApplyProxyOverride Start.");
    if (env == nullptr) {
        WVLOG_E("ProxyConfig env is nullptr");
        return;
    }
    ani_ref jsCallback = nullptr;
    env->GlobalReference_Create(callback, &jsCallback);
    if (!jsCallback) {
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    ProxyConfig* proxyConfig = nullptr;
    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(proxyConfigObject, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }
    proxyConfig = reinterpret_cast<ProxyConfig*>(thisVar);
    if (!proxyConfig) {
        WVLOG_E("ProxyConfig JsApplyProxyOverride proxyConfig is null");
        return;
    }
    InnerApplyProxyOverride(proxyConfig, env, jsCallback);
    return;
}

static void JsRemoveProxyOverride(ani_env *env, ani_object object, ani_fn_object callback)
{
    WVLOG_D("ProxyConfig JsRemoveProxyOverride.");
    if (env == nullptr) {
        WVLOG_E("ProxyConfig env is nullptr");
        return;
    }
    
    ani_ref jsCallback = nullptr;
    env->GlobalReference_Create(callback, &jsCallback);
    if (!jsCallback) {
        NWebError::AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }

    auto resultCallback = std::make_shared<ProxyChangedCallbackImpl>(env, jsCallback);
    NWebHelper::Instance().RemoveProxyOverride(resultCallback);
    return;
}

ani_status StsProxyControllerInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class proxyControllerCls = nullptr;
    ani_status status = env->FindClass(WEB_PROXY_CONTROLLER_CLASS_NAME, &proxyControllerCls);
    if (status != ANI_OK || !proxyControllerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_PROXY_CONTROLLER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(JsConstructor) },
        ani_native_function { "applyProxyOverride", nullptr, reinterpret_cast<void *>(JsApplyProxyOverride) },
        ani_native_function { "removeProxyOverride", nullptr, reinterpret_cast<void *>(JsRemoveProxyOverride) },
    };

    status = env->Class_BindNativeMethods(proxyControllerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}


} // namespace NWeb
} // namespace OHOS
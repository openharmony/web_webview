/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "napi_user_agent_brand_version.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <securec.h>

#include "business_error.h"
#include "napi/native_node_api.h"
#include "napi_parse_utils.h"
#include "napi_proxy_rule.h"
#include "nweb_log.h"
#include "proxy_rule.h"
#include "nweb_user_agent_brand_version_impl.h"
#include "web_errors.h"

using namespace OHOS::NWebError;

namespace OHOS {
namespace NWeb {
namespace {
const std::string USER_AGENT_FORM_FACTOR_ENUM_NAME = "UserAgentFormFactor";
NWebUserAgentBrandVersionImpl* GetBrandVersion(napi_env env, napi_callback_info info, napi_value* argv)
{
    size_t argc = 1;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    NWebUserAgentBrandVersionImpl* brandVersion = nullptr;
    napi_unwrap(env, thisVar, (void**)&brandVersion);

    if (!brandVersion) {
        WVLOG_E("unwrap UserAgentBrandVersion failed.");
        return nullptr;
    }
    return brandVersion;
}
}

napi_value NapiUserAgentBrandVersion::JS_SetBrand(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentBrandVersionImpl* brandVersion = GetBrandVersion(env, info, argv);
    if (!brandVersion) {
        WVLOG_E("UserAgentBrandVersion failed.");
        return nullptr;
    }

    std::string brand;
    if (!NapiParseUtils::ParseString(env, argv[0], brand)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "brand", "string"));
        return nullptr;
    }
    if (brand.empty()) {
        WVLOG_E("brand is empty");
        return nullptr;
    }
    WVLOG_D("brand %{public}s.", brand.c_str());
    brandVersion->SetBrand(brand);
    return nullptr;
}

napi_value NapiUserAgentBrandVersion::JS_SetMajorVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentBrandVersionImpl* brandVersion = GetBrandVersion(env, info, argv);
    if (!brandVersion) {
        WVLOG_E("UserAgentBrandVersion failed.");
        return nullptr;
    }

    std::string majorVersion;
    if (!NapiParseUtils::ParseString(env, argv[0], majorVersion)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "majorVersion", "string"));
        return nullptr;
    }
    if (majorVersion.empty()) {
        WVLOG_E("majorVersion is empty");
        return nullptr;
    }
    WVLOG_D("majorVersion %{public}s.", majorVersion.c_str());
    brandVersion->SetMajorVersion(majorVersion);
    return nullptr;
}

napi_value NapiUserAgentBrandVersion::JS_SetFullVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentBrandVersionImpl* brandVersion = GetBrandVersion(env, info, argv);
    if (!brandVersion) {
        WVLOG_E("UserAgentBrandVersion failed.");
        return nullptr;
    }

    std::string fullVersion;
    if (!NapiParseUtils::ParseString(env, argv[0], fullVersion)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "fullVersion", "string"));
        return nullptr;
    }
    if (fullVersion.empty()) {
        WVLOG_E("fullVersion is empty");
        return nullptr;
    }
    WVLOG_D("FullVersion %{public}s.", fullVersion.c_str());
    brandVersion->SetFullVersion(fullVersion);
    return nullptr;
}

napi_value NapiUserAgentBrandVersion::JS_GetBrand(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentBrandVersionImpl* brandVersion = GetBrandVersion(env, info, argv);
    if (!brandVersion) {
        WVLOG_E("UserAgentBrandVersion failed.");
        return nullptr;
    }
    napi_value result = nullptr;
    std::string brand = brandVersion->GetBrand();
    WVLOG_D("get brand %{public}s.", brand.c_str());
    napi_create_string_utf8(env, brand.c_str(), brand.length(), &result);

    return result;
}

napi_value NapiUserAgentBrandVersion::JS_GetMajorVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentBrandVersionImpl* brandVersion = GetBrandVersion(env, info, argv);
    if (!brandVersion) {
        WVLOG_E("UserAgentBrandVersion failed.");
        return nullptr;
    }
    napi_value result = nullptr;
    std::string majorVersion = brandVersion->GetMajorVersion();
    WVLOG_D("get majorVersion %{public}s.", majorVersion.c_str());
    napi_create_string_utf8(env, majorVersion.c_str(), majorVersion.length(), &result);

    return result;
}

napi_value NapiUserAgentBrandVersion::JS_GetFullVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentBrandVersionImpl* brandVersion = GetBrandVersion(env, info, argv);
    if (!brandVersion) {
        WVLOG_E("UserAgentBrandVersion failed.");
        return nullptr;
    }
    napi_value result = nullptr;
    std::string fullVersion = brandVersion->GetFullVersion();
    WVLOG_D("get fullVersion %{public}s.", fullVersion.c_str());
    napi_create_string_utf8(env, fullVersion.c_str(), fullVersion.length(), &result);

    return result;
}

napi_value NapiUserAgentBrandVersion::Init(napi_env env, napi_value exports)
{
    WVLOG_I("NapiUserAgentBrandVersion::Init is called");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setBrand", NapiUserAgentBrandVersion::JS_SetBrand),
        DECLARE_NAPI_FUNCTION("setMajorVersion", NapiUserAgentBrandVersion::JS_SetMajorVersion),
        DECLARE_NAPI_FUNCTION("setFullVersion", NapiUserAgentBrandVersion::JS_SetFullVersion),
        DECLARE_NAPI_FUNCTION("getBrand", NapiUserAgentBrandVersion::JS_GetBrand),
        DECLARE_NAPI_FUNCTION("getMajorVersion", NapiUserAgentBrandVersion::JS_GetMajorVersion),
        DECLARE_NAPI_FUNCTION("getFullVersion", NapiUserAgentBrandVersion::JS_GetFullVersion),
    };

    napi_value constructor = nullptr;
    napi_define_class(env, USER_AGENT_BRAND_VERSION_CLASS_NAME.c_str(), USER_AGENT_BRAND_VERSION_CLASS_NAME.length(),
        JS_Constructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "NapiUserAgentBrandVersion define js class failed.");
    napi_status status =
        napi_set_named_property(env, exports, USER_AGENT_BRAND_VERSION_CLASS_NAME.c_str(), constructor);
    NAPI_ASSERT(env, status == napi_ok, "NapiUserAgentBrandVersion set property failed.");

    return exports;
}

napi_value NapiUserAgentBrandVersion::JS_Constructor(napi_env env, napi_callback_info info)
{
    WVLOG_I("NapiUserAgentBrandVersion::JS_Constructor is called");
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    NWebUserAgentBrandVersionImpl* nativeUserAgentBrandVersion = new NWebUserAgentBrandVersionImpl();

    napi_wrap(
        env, thisVar, nativeUserAgentBrandVersion,
        [](napi_env /* env */, void* data, void* /* hint */) {
            NWebUserAgentBrandVersionImpl* brandVersion = (NWebUserAgentBrandVersionImpl*)data;
            delete brandVersion;
        },
        nullptr, nullptr);

    return thisVar;
}

napi_status NapiUserAgentBrandVersion::DefineProperties(napi_env env, napi_value *object)
{
    WVLOG_I("NapiUserAgentBrandVersion::DefineProperties is called");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setBrand", NapiUserAgentBrandVersion::JS_SetBrand),
        DECLARE_NAPI_FUNCTION("setMajorVersion", NapiUserAgentBrandVersion::JS_SetMajorVersion),
        DECLARE_NAPI_FUNCTION("setFullVersion", NapiUserAgentBrandVersion::JS_SetFullVersion),
        DECLARE_NAPI_FUNCTION("getBrand", NapiUserAgentBrandVersion::JS_GetBrand),
        DECLARE_NAPI_FUNCTION("getMajorVersion", NapiUserAgentBrandVersion::JS_GetMajorVersion),
        DECLARE_NAPI_FUNCTION("getFullVersion", NapiUserAgentBrandVersion::JS_GetFullVersion),
    };
    return napi_define_properties(env, *object, sizeof(properties) / sizeof(properties[0]), properties);
}

} // namespace NWeb
} // namespace OHOS
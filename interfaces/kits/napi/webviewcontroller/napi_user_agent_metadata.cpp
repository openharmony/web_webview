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

#include "napi_user_agent_metadata.h"

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
#include "napi_user_agent_brand_version.h"
#include "nweb_user_agent_brand_version_impl.h"
#include "nweb_user_agent_metadata_impl.h"
#include "web_errors.h"

using namespace OHOS::NWebError;

namespace OHOS {
namespace NWeb {
namespace {
const std::string USER_AGENT_FORM_FACTOR_ENUM_NAME = "UserAgentFormFactor";
NWebUserAgentMetadataImpl* GetMetadata(napi_env env, napi_callback_info info, napi_value* argv)
{
    size_t argc = 1;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NWebUserAgentMetadataImpl* metadata = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    napi_unwrap(env, thisVar, (void**)&metadata);
    if (!metadata) {
        WVLOG_E("unwrap UserAgentMetadata failed.");
        return nullptr;
    }
    return metadata;
}

napi_value GetValue(napi_env env, std::string strValue)
{
    napi_value value;
    napi_status status = napi_create_string_utf8(env, strValue.c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        WVLOG_E("create string failed.");
        return nullptr;
    }
    return value;
}
}

napi_value NapiUserAgentMetadata::JS_SetBrandVersionList(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    napi_value array = argv[INTEGER_ZERO];
    bool isArray = false;
    napi_is_array(env, array, &isArray);
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "brandVersion", "array"));
        return nullptr;
    }
    uint32_t arrayLength = 0;
    napi_get_array_length(env, array, &arrayLength);
    std::vector<std::shared_ptr<NWebUserAgentBrandVersionImpl>> brandVersionVector;
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_value obj = nullptr;
        napi_get_element(env, array, i, &obj);
        NWebUserAgentBrandVersionImpl* brandVersion = nullptr;
        napi_unwrap(env, obj, (void**)&brandVersion);
        if (!brandVersion) {
            WVLOG_E("SetBrandVersionListArray brandVersion is null");
            return nullptr;
        }
        auto ptrBrandVersion = std::make_shared<OHOS::NWeb::NWebUserAgentBrandVersionImpl>(
            brandVersion->GetBrand(), brandVersion->GetMajorVersion(), brandVersion->GetFullVersion());
        brandVersionVector.push_back(ptrBrandVersion);
    }
    metadata->SetBrandVersionListPtr(brandVersionVector);

    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetArchitecture(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    std::string arch;
    if (!NapiParseUtils::ParseString(env, argv[0], arch)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "arch", "string"));
        return nullptr;
    }

    WVLOG_D("arch %{public}s.", arch.c_str());
    metadata->SetArchitecture(arch);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetBitness(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }
    std::string bitness;
    if (!NapiParseUtils::ParseString(env, argv[0], bitness)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "bitness", "string"));
        return nullptr;
    }

    WVLOG_D("bitness %{public}s.", bitness.c_str());
    metadata->SetBitness(bitness);
    return nullptr;
}


napi_value NapiUserAgentMetadata::JS_SetFormFactors(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    napi_value array = argv[INTEGER_ZERO];
    bool isArray = false;
    napi_is_array(env, array, &isArray);
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "formFactors", "array"));
        return nullptr;
    }
    uint32_t arrayLength = 0;
    napi_get_array_length(env, array, &arrayLength);
    std::vector<std::string> formFactorsVector;
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value formFactors = nullptr;
        napi_get_element(env, array, i, &formFactors);
        std::string formFactorsStr;
        if (!NapiParseUtils::ParseString(env, formFactors, formFactorsStr)) {
            WVLOG_E("[USER_AGENT_METADATA]Unable to parse string from formFactors array object.");
            return nullptr;
        }
        formFactorsVector.push_back(formFactorsStr);
    }
    metadata->SetFormFactors(formFactorsVector);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetFullVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    std::string fullVersion;
    if (!NapiParseUtils::ParseString(env, argv[0], fullVersion)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "fullVersion", "string"));
        return nullptr;
    }

    WVLOG_D("fullVersion %{public}s.", fullVersion.c_str());
    metadata->SetFullVersion(fullVersion);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetMobile(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    bool isMobile = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], isMobile)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "isMobile", "boolean"));
        return nullptr;
    }
    WVLOG_D("isMobile %{public}d.", isMobile);
    metadata->SetMobile(isMobile);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetModel(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    std::string model;
    if (!NapiParseUtils::ParseString(env, argv[0], model)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "model", "string"));
        return nullptr;
    }

    WVLOG_D("model %{public}s.", model.c_str());
    metadata->SetModel(model);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetPlatform(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    std::string platform;
    if (!NapiParseUtils::ParseString(env, argv[0], platform)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "platform", "string"));
        return nullptr;
    }

    WVLOG_D("platform %{public}s.", platform.c_str());
    metadata->SetPlatform(platform);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetPlatformVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    std::string platformVersion;
    if (!NapiParseUtils::ParseString(env, argv[0], platformVersion)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "platformVersion", "string"));
        return nullptr;
    }

    WVLOG_D("platformVersion %{public}s.", platformVersion.c_str());
    metadata->SetPlatformVersion(platformVersion);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_SetWow64(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    bool isWow64 = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], isWow64)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "isWow64", "boolean"));
        return nullptr;
    }
    WVLOG_D("isWow64 %{public}d.", isWow64);
    metadata->SetWow64(isWow64);
    return nullptr;
}

napi_value NapiUserAgentMetadata::JS_GetBrandVersionList(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }
    auto brandVersions = metadata->GetBrandVersionListPtr();
    napi_value result = nullptr;
    napi_create_array(env, &result);
    for (size_t index = 0; index < brandVersions.size(); index++) {
        WVLOG_I("get brandVersion %{public}s %{public}s %{public}s",
            brandVersions[index]->GetBrand().c_str(), brandVersions[index]->GetMajorVersion().c_str(),
            brandVersions[index]->GetFullVersion().c_str());
        napi_value jsBrandVersion = nullptr;
        napi_create_object(env, &jsBrandVersion);
        napi_wrap(
            env, jsBrandVersion,
            new NWebUserAgentBrandVersionImpl(brandVersions[index]->GetBrand(), brandVersions[index]->GetMajorVersion(),
                brandVersions[index]->GetFullVersion()),
            [](napi_env /* env */, void* data, void* /* hint */) {
                if (data) {
                    NWebUserAgentBrandVersionImpl* brandVersion = static_cast<NWebUserAgentBrandVersionImpl*>(data);
                    delete brandVersion;
                }
            },
            nullptr, nullptr);
        NapiUserAgentBrandVersion::DefineProperties(env, &jsBrandVersion);
        NAPI_CALL(env, napi_set_element(env, result, index, jsBrandVersion));
    }

    return result;
}

napi_value NapiUserAgentMetadata::JS_GetArchitecture(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }
    return GetValue(env, metadata->GetArchitecture());
}

napi_value NapiUserAgentMetadata::JS_GetBitness(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }
    return GetValue(env, metadata->GetBitness());
}

napi_value NapiUserAgentMetadata::JS_GetFormFactors(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    napi_value result;
    napi_create_array(env, &result);
    std::vector<std::string> formFactors = metadata->GetFormFactors();
    for (size_t index = 0; index < formFactors.size(); index++) {
        WVLOG_I("get formFactors %{public}s.", formFactors[index].c_str());
        napi_value jsFormFactors = nullptr;
        napi_status status = napi_create_string_utf8(env, formFactors[index].c_str(),
                                                     formFactors[index].length(), &jsFormFactors);
        if (status != napi_ok) {
            WVLOG_I("create js string error");
            continue;
        }
        NAPI_CALL(env, napi_set_element(env, result, index, jsFormFactors));
    }
    return result;
}

napi_value NapiUserAgentMetadata::JS_GetFullVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }
    return GetValue(env, metadata->GetFullVersion());
}

napi_value NapiUserAgentMetadata::JS_GetMobile(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    napi_value result = nullptr;
    bool isMobile = metadata->GetMobile();
    NAPI_CALL(env, napi_get_boolean(env, isMobile, &result));

    return result;
}

napi_value NapiUserAgentMetadata::JS_GetModel(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    return GetValue(env, metadata->GetModel());
}

napi_value NapiUserAgentMetadata::JS_GetPlatform(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    napi_value value = nullptr;
    napi_status status = napi_create_string_utf8(env, metadata->GetPlatform().c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        WVLOG_E("NapiProxyRule::JS_GetPlatform create string failed.");
        return nullptr;
    }
    return value;
}

napi_value NapiUserAgentMetadata::JS_GetPlatformVersion(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    napi_value value = nullptr;
    napi_status status = napi_create_string_utf8(env, metadata->GetPlatformVersion().c_str(), NAPI_AUTO_LENGTH, &value);
    if (status != napi_ok) {
        WVLOG_E("NapiProxyRule::JS_GetPlatformVersion create string failed.");
        return nullptr;
    }
    return value;
}

napi_value NapiUserAgentMetadata::JS_GetWow64(napi_env env, napi_callback_info info)
{
    napi_value argv[1] = { 0 };
    NWebUserAgentMetadataImpl* metadata = GetMetadata(env, info, argv);
    if (!metadata) {
        WVLOG_E("UserAgentMetadata failed.");
        return nullptr;
    }

    napi_value result = nullptr;
    bool isWow64 = metadata->GetWow64();
    NAPI_CALL(env, napi_get_boolean(env, isWow64, &result));
    return result;
}

void NapiUserAgentMetadata::ExportUserAgentFormFactorEnum(napi_env env, napi_value* exports)
{
    napi_value strAutomotive;
    napi_value strDesktop;
    napi_value strMobile;
    napi_value strEInk;
    napi_value strTablet;
    napi_value strWatch;
    napi_value strXR;
    napi_create_string_utf8(env, "Automotive", NAPI_AUTO_LENGTH, &strAutomotive);
    napi_create_string_utf8(env, "Desktop", NAPI_AUTO_LENGTH, &strDesktop);
    napi_create_string_utf8(env, "Mobile", NAPI_AUTO_LENGTH, &strMobile);
    napi_create_string_utf8(env, "EInk", NAPI_AUTO_LENGTH, &strEInk);
    napi_create_string_utf8(env, "Tablet", NAPI_AUTO_LENGTH, &strTablet);
    napi_create_string_utf8(env, "Watch", NAPI_AUTO_LENGTH, &strWatch);
    napi_create_string_utf8(env, "XR", NAPI_AUTO_LENGTH, &strXR);
    napi_value userAgentFormFactorTypeEnum = nullptr;
    napi_property_descriptor userAgentFormFactorProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("AUTOMOTIVE", strAutomotive),
        DECLARE_NAPI_STATIC_PROPERTY("DESKTOP", strDesktop),
        DECLARE_NAPI_STATIC_PROPERTY("MOBILE", strMobile),
        DECLARE_NAPI_STATIC_PROPERTY("EINK", strEInk),
        DECLARE_NAPI_STATIC_PROPERTY("TABLET", strTablet),
        DECLARE_NAPI_STATIC_PROPERTY("WATCH", strWatch),
        DECLARE_NAPI_STATIC_PROPERTY("XR", strXR),
    };
    napi_define_class(env, USER_AGENT_FORM_FACTOR_ENUM_NAME.c_str(), USER_AGENT_FORM_FACTOR_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr,
        sizeof(userAgentFormFactorProperties) / sizeof(userAgentFormFactorProperties[0]), userAgentFormFactorProperties,
        &userAgentFormFactorTypeEnum);
    napi_set_named_property(env, *exports, USER_AGENT_FORM_FACTOR_ENUM_NAME.c_str(), userAgentFormFactorTypeEnum);
}

napi_value NapiUserAgentMetadata::Init(napi_env env, napi_value exports)
{
    WVLOG_I("NapiUserAgentMetadata::Init is called");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setBrandVersionList", NapiUserAgentMetadata::JS_SetBrandVersionList),
        DECLARE_NAPI_FUNCTION("setArchitecture", NapiUserAgentMetadata::JS_SetArchitecture),
        DECLARE_NAPI_FUNCTION("setBitness", NapiUserAgentMetadata::JS_SetBitness),
        DECLARE_NAPI_FUNCTION("setFormFactors", NapiUserAgentMetadata::JS_SetFormFactors),
        DECLARE_NAPI_FUNCTION("setFullVersion", NapiUserAgentMetadata::JS_SetFullVersion),
        DECLARE_NAPI_FUNCTION("setMobile", NapiUserAgentMetadata::JS_SetMobile),
        DECLARE_NAPI_FUNCTION("setModel", NapiUserAgentMetadata::JS_SetModel),
        DECLARE_NAPI_FUNCTION("setPlatform", NapiUserAgentMetadata::JS_SetPlatform),
        DECLARE_NAPI_FUNCTION("setPlatformVersion", NapiUserAgentMetadata::JS_SetPlatformVersion),
        DECLARE_NAPI_FUNCTION("setWow64", NapiUserAgentMetadata::JS_SetWow64),
        DECLARE_NAPI_FUNCTION("getBrandVersionList", NapiUserAgentMetadata::JS_GetBrandVersionList),
        DECLARE_NAPI_FUNCTION("getArchitecture", NapiUserAgentMetadata::JS_GetArchitecture),
        DECLARE_NAPI_FUNCTION("getBitness", NapiUserAgentMetadata::JS_GetBitness),
        DECLARE_NAPI_FUNCTION("getFormFactors", NapiUserAgentMetadata::JS_GetFormFactors),
        DECLARE_NAPI_FUNCTION("getFullVersion", NapiUserAgentMetadata::JS_GetFullVersion),
        DECLARE_NAPI_FUNCTION("getMobile", NapiUserAgentMetadata::JS_GetMobile),
        DECLARE_NAPI_FUNCTION("getModel", NapiUserAgentMetadata::JS_GetModel),
        DECLARE_NAPI_FUNCTION("getPlatform", NapiUserAgentMetadata::JS_GetPlatform),
        DECLARE_NAPI_FUNCTION("getPlatformVersion", NapiUserAgentMetadata::JS_GetPlatformVersion),
        DECLARE_NAPI_FUNCTION("getWow64", NapiUserAgentMetadata::JS_GetWow64),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, USER_AGENT_METADATA_CLASS_NAME.c_str(), USER_AGENT_METADATA_CLASS_NAME.length(),
        JS_Constructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "NapiUserAgentMetadata define js class failed.");
    napi_status status = napi_set_named_property(env, exports, USER_AGENT_METADATA_CLASS_NAME.c_str(), constructor);
    NAPI_ASSERT(env, status == napi_ok, "NapiUserAgentMetadata set property failed.");

    ExportUserAgentFormFactorEnum(env, &exports);
    return exports;
}

napi_value NapiUserAgentMetadata::JS_Constructor(napi_env env, napi_callback_info info)
{
    WVLOG_I("NapiUserAgentMetadata::JS_Constructor is called");
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, &data);

    NWebUserAgentMetadataImpl *metadata = new NWebUserAgentMetadataImpl();

    napi_wrap(
        env, thisVar, metadata,
        [](napi_env /* env */, void *data, void * /* hint */) {
            NWebUserAgentMetadataImpl *metadata = (NWebUserAgentMetadataImpl *)data;
            delete metadata;
        },
        nullptr, nullptr);

    return thisVar;
}

napi_status NapiUserAgentMetadata::DefineProperties(napi_env env, napi_value *object)
{
    WVLOG_I("NapiUserAgentMetadata::DefineProperties is called");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setBrandVersionList", NapiUserAgentMetadata::JS_SetBrandVersionList),
        DECLARE_NAPI_FUNCTION("setArchitecture", NapiUserAgentMetadata::JS_SetArchitecture),
        DECLARE_NAPI_FUNCTION("setBitness", NapiUserAgentMetadata::JS_SetBitness),
        DECLARE_NAPI_FUNCTION("setFormFactors", NapiUserAgentMetadata::JS_SetFormFactors),
        DECLARE_NAPI_FUNCTION("setFullVersion", NapiUserAgentMetadata::JS_SetFullVersion),
        DECLARE_NAPI_FUNCTION("setMobile", NapiUserAgentMetadata::JS_SetMobile),
        DECLARE_NAPI_FUNCTION("setModel", NapiUserAgentMetadata::JS_SetModel),
        DECLARE_NAPI_FUNCTION("setPlatform", NapiUserAgentMetadata::JS_SetPlatform),
        DECLARE_NAPI_FUNCTION("setPlatformVersion", NapiUserAgentMetadata::JS_SetPlatformVersion),
        DECLARE_NAPI_FUNCTION("setWow64", NapiUserAgentMetadata::JS_SetWow64),
        DECLARE_NAPI_FUNCTION("getBrandVersionList", NapiUserAgentMetadata::JS_GetBrandVersionList),
        DECLARE_NAPI_FUNCTION("getArchitecture", NapiUserAgentMetadata::JS_GetArchitecture),
        DECLARE_NAPI_FUNCTION("getBitness", NapiUserAgentMetadata::JS_GetBitness),
        DECLARE_NAPI_FUNCTION("getFormFactors", NapiUserAgentMetadata::JS_GetFormFactors),
        DECLARE_NAPI_FUNCTION("getFullVersion", NapiUserAgentMetadata::JS_GetFullVersion),
        DECLARE_NAPI_FUNCTION("getMobile", NapiUserAgentMetadata::JS_GetMobile),
        DECLARE_NAPI_FUNCTION("getModel", NapiUserAgentMetadata::JS_GetModel),
        DECLARE_NAPI_FUNCTION("getPlatform", NapiUserAgentMetadata::JS_GetPlatform),
        DECLARE_NAPI_FUNCTION("getPlatformVersion", NapiUserAgentMetadata::JS_GetPlatformVersion),
        DECLARE_NAPI_FUNCTION("getWow64", NapiUserAgentMetadata::JS_GetWow64),
    };
    return napi_define_properties(env, *object, sizeof(properties) / sizeof(properties[0]), properties);
}
} // namespace NWeb
} // namespace OHOS
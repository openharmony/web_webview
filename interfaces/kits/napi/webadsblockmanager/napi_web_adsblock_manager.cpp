/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "napi_web_adsblock_manager.h"

#include <cstdint>
#include <uv.h>
#include <vector>

#include "business_error.h"
#include "napi/native_common.h"
#include "nweb_adsblock_manager.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "napi_parse_utils.h"
#include "web_errors.h"
#include "securec.h"
#include "system_properties_adapter_impl.h"

namespace OHOS {
namespace NWeb {

constexpr int MAX_URL_RULES_FILEPATH_LENGTH = 255;

napi_value NapiWebAdsBlockManager::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("setAdsBlockRules",
                                     NapiWebAdsBlockManager::JsSetAdsBlockRules),
        DECLARE_NAPI_STATIC_FUNCTION("addAdsBlockDisallowedList",
                                     NapiWebAdsBlockManager::JsAddAdsBlockDisallowedList),
        DECLARE_NAPI_STATIC_FUNCTION("removeAdsBlockDisallowedList",
                                     NapiWebAdsBlockManager::JsRemoveAdsBlockDisallowedList),
        DECLARE_NAPI_STATIC_FUNCTION("clearAdsBlockDisallowedList",
                                     NapiWebAdsBlockManager::JsClearAdsBlockDisallowedList),
        DECLARE_NAPI_STATIC_FUNCTION("addAdsBlockAllowedList",
                                     NapiWebAdsBlockManager::JsAddAdsBlockAllowedList),
        DECLARE_NAPI_STATIC_FUNCTION("removeAdsBlockAllowedList",
                                     NapiWebAdsBlockManager::JsRemoveAdsBlockAllowedList),
        DECLARE_NAPI_STATIC_FUNCTION("clearAdsBlockAllowedList",
                                     NapiWebAdsBlockManager::JsClearAdsBlockAllowedList),
    };
    napi_value constructor = nullptr;

    napi_define_class(env, WEB_ADSBLOCK_MANAGER_CLASS_NAME.c_str(), WEB_ADSBLOCK_MANAGER_CLASS_NAME.length(),
        JsConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "NapiWebAdsBlockManager define js class failed");
    napi_status status = napi_set_named_property(env, exports, "AdsBlockManager", constructor);
    NAPI_ASSERT(env, status == napi_ok, "NapiWebAdsBlockManager set property failed");
    return exports;
}

napi_value NapiWebAdsBlockManager::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;

    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiWebAdsBlockManager::JsSetAdsBlockRules(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() == ProductDeviceType::DEVICE_TYPE_WEARABLE) {
        WVLOG_E("SetAdsBlockRules: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }
    napi_value retValue = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_TWO) {
        WVLOG_E("setAdsBlockRules failed: wrong param count");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "two"));
        return nullptr;
    }

    std::string rulesFile;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], rulesFile)) {
        WVLOG_E("setAdsBlockRules failed: first param should be string");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The type of rulesFile must be string.");
        return result;
    }

    if (rulesFile.length() > MAX_URL_RULES_FILEPATH_LENGTH) {
        WVLOG_E("setAdsBlockRules failed: rulesFile path too long > %{public}d", MAX_URL_RULES_FILEPATH_LENGTH);
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. rulesFile path too long > 255.");
        return result;
    }

    bool replace = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[INTEGER_ONE], replace)) {
        WVLOG_E("setAdsBlockRules failed: invalid replace value");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The type of replace must be boolean.");
        return result;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->SetAdsBlockRules(rulesFile, replace);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebAdsBlockManager::JsAddAdsBlockDisallowedList(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() == ProductDeviceType::DEVICE_TYPE_WEARABLE) {
        WVLOG_E("AddAdsBlockDisallowedList: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("AddAdsBlockDisallowedList failed: wrong param count");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    std::vector<std::string> domainSuffixes;
    if (!NapiParseUtils::ParseStringArray(env, argv[INTEGER_ZERO], domainSuffixes)) {
        WVLOG_E("AddAdsBlockDisallowedList failed: domainSuffixes should be an array of string");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The type of domainSuffixes must be an array of string.");
        return result;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->AddAdsBlockDisallowedList(domainSuffixes);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebAdsBlockManager::JsRemoveAdsBlockDisallowedList(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() == ProductDeviceType::DEVICE_TYPE_WEARABLE) {
        WVLOG_E("RemoveAdsBlockDisallowedList: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("removeAdsBlockDisallowedList failed: wrong param count");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    std::vector<std::string> domainSuffixes;
    if (!NapiParseUtils::ParseStringArray(env, argv[INTEGER_ZERO], domainSuffixes)) {
        WVLOG_E("removeAdsBlockDisallowedList failed: domainSuffixes should be an array of string");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The type of domainSuffixes must be an array of string.");
        return result;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->RemoveAdsBlockDisallowedList(domainSuffixes);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebAdsBlockManager::JsClearAdsBlockDisallowedList(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() == ProductDeviceType::DEVICE_TYPE_WEARABLE) {
        WVLOG_E("ClearAdsBlockDisallowedList: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->ClearAdsBlockDisallowedList();
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebAdsBlockManager::JsAddAdsBlockAllowedList(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() == ProductDeviceType::DEVICE_TYPE_WEARABLE) {
        WVLOG_E("AddAdsBlockAllowedList: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("AddAdsBlockAllowedList failed: wrong param count");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    std::vector<std::string> domainSuffixes;
    if (!NapiParseUtils::ParseStringArray(env, argv[INTEGER_ZERO], domainSuffixes)) {
        WVLOG_E("AddAdsBlockAllowedList failed: domainSuffixes should be an array of string");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The type of domainSuffixes must be an array of string.");
        return result;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->AddAdsBlockAllowedList(domainSuffixes);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebAdsBlockManager::JsRemoveAdsBlockAllowedList(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() == ProductDeviceType::DEVICE_TYPE_WEARABLE) {
        WVLOG_E("RemoveAdsBlockAllowedList: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }
    napi_value retValue = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("removeAdsBlockAllowedList failed: wrong param count");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    std::vector<std::string> domainSuffixes;
    if (!NapiParseUtils::ParseStringArray(env, argv[INTEGER_ZERO], domainSuffixes)) {
        WVLOG_E("removeAdsBlockAllowedList failed: domainSuffixes should be an array of string");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The type of domainSuffixes must be an array of string.");
        return result;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->RemoveAdsBlockAllowedList(domainSuffixes);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebAdsBlockManager::JsClearAdsBlockAllowedList(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() == ProductDeviceType::DEVICE_TYPE_WEARABLE) {
        WVLOG_E("ClearAdsBlockAllowedList: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->ClearAdsBlockAllowedList();
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

} // namespace NWeb
} // namespace OHOS

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

#include "ani_web_adsblock_manager.h"
#include "nweb_adsblock_manager.h"

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "nweb_helper.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
static const char* WEB_ADS_BLOCK_MANAGER_CLASS_NAME = "L@ohos/web/webview/webview/AdsBlockManager;";
constexpr int MAX_URL_RULES_FILEPATH_LENGTH = 255;
}

void ClearAdsBlockAllowedList(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("clear ads_block allowed list failed, env is nullptr.");
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager = 
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->ClearAdsBlockAllowedList();
    }
}

void ClearAdsBlockDisallowedList(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("clear ads_block disallowed list failed, env is nullptr.");
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager = 
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->ClearAdsBlockAllowedList();
    }
}

void RemoveAdsBlockDisallowedList(ani_env *env, ani_object object, ani_object domainSuffixesArray)
{
    if (env == nullptr) {
        WVLOG_E("remove ads_block disallowed list failed, env is nullptr.");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(domainSuffixesArray, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("domainSuffixes is undefined");
         AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    std::vector<std::string> domainSuffixes;
    if (!AniParseUtils::GetStringList(env, domainSuffixesArray, domainSuffixes)) {
        WVLOG_E("domainSuffixes should be an array of string");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager = 
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->RemoveAdsBlockDisallowedList(domainSuffixes);
    }
}

void AddAdsBlockDisallowedList(ani_env *env, ani_object object, ani_object domainSuffixesArray)
{
    if (env == nullptr) {
        WVLOG_E("add ads_block disallowed list failed, env is nullptr.");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(domainSuffixesArray, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("domainSuffixes is undefined");
         AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    std::vector<std::string> domainSuffixes;
    if (!AniParseUtils::GetStringList(env, domainSuffixesArray, domainSuffixes)) {
        WVLOG_E("domainSuffixes should be an array of string");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager = 
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->AddAdsBlockDisallowedList(domainSuffixes);
    }
}

void AddAdsBlockAllowedList(ani_env *env, ani_object object, ani_object domainSuffixesArray)
{
    if (env == nullptr) {
        WVLOG_E("add ads_block allowed list failed, env is nullptr.");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(domainSuffixesArray, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("domainSuffixes is undefined");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    std::vector<std::string> domainSuffixes;
    if (!AniParseUtils::GetStringList(env, domainSuffixesArray, domainSuffixes)) {
        WVLOG_E("domainSuffixes should be an array of string");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->AddAdsBlockAllowedList(domainSuffixes);
    }
}

void RemoveAdsBlockAllowedList(ani_env *env, ani_object object, ani_object domainSuffixesArray)
{
    if (env == nullptr) {
        WVLOG_E("remove ads_block allowed list failed, env is nullptr.");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(domainSuffixesArray, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("domainSuffixes is undefined");
         AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }
    std::vector<std::string> domainSuffixes;
    if (!AniParseUtils::GetStringList(env, domainSuffixesArray, domainSuffixes)) {
        WVLOG_E("domainSuffixes should be an array of string");
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
        return;
    }

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager = 
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->RemoveAdsBlockAllowedList(domainSuffixes);
    }
}

static void JsSetAdsBlockRules(ani_env* env, ani_object aniClass, ani_object rulesFile, ani_boolean replace)
{
    WVLOG_I("JsSetAdsBlockRules begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string rulesFileStr;
    bool replaceMode = false;
    if (!AniParseUtils::IsString(env, rulesFile)) {
        WVLOG_E("JsSetAdsBlockRules FindClass Fail");
        return;
    }
    if (!AniParseUtils::ParseString(env, static_cast<ani_ref>(rulesFile), rulesFileStr)) {
        WVLOG_E("JsSetAdsBlockRules ParseString Fail");
        return;
    }
    if (rulesFileStr.length() > MAX_URL_RULES_FILEPATH_LENGTH) {
        WVLOG_E("setAdsBlockRules failed: rulesFile path too long > %{public}d", MAX_URL_RULES_FILEPATH_LENGTH);
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    replaceMode = static_cast<bool>(replace);
    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> adsBlockManager =
        OHOS::NWeb::NWebHelper::Instance().GetAdsBlockManager();
    if (adsBlockManager != nullptr) {
        adsBlockManager->SetAdsBlockRules(rulesFileStr, replaceMode);
    }
    return;
}

ani_status StsWebAdsBlockManagerInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("WebAdsBlockManager init failed, env is nullptr.");
        return ANI_ERROR;
    }
    ani_class webAdsBlockManagerCls = nullptr;
    ani_status status = env->FindClass(WEB_ADS_BLOCK_MANAGER_CLASS_NAME, &webAdsBlockManagerCls);
    if (status != ANI_OK || !webAdsBlockManagerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_ADS_BLOCK_MANAGER_CLASS_NAME, status);
        return ANI_ERROR;
    }

    std::array managerMethods = {
        ani_native_function { "clearAdsBlockAllowedList", 
            nullptr, reinterpret_cast<void *>(ClearAdsBlockAllowedList) },
        ani_native_function { "clearAdsBlockDisallowedList", 
            nullptr, reinterpret_cast<void *>(ClearAdsBlockDisallowedList)},
        ani_native_function { "removeAdsBlockDisallowedList", 
            nullptr, reinterpret_cast<void *>(RemoveAdsBlockDisallowedList)},
        ani_native_function { "addAdsBlockDisallowedList", 
            nullptr, reinterpret_cast<void *>(AddAdsBlockDisallowedList)},
        ani_native_function { "removeAdsBlockAllowedList", 
            nullptr, reinterpret_cast<void *>(RemoveAdsBlockAllowedList)},
        ani_native_function { "addAdsBlockAllowedList",
            nullptr, reinterpret_cast<void *>(AddAdsBlockAllowedList)},
        ani_native_function { "setAdsBlockRules",
            nullptr, reinterpret_cast<void *>(JsSetAdsBlockRules)},
    };

    status = env->Class_BindNativeMethods(webAdsBlockManagerCls, managerMethods.data(), managerMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("WebAdsBlockManager Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS
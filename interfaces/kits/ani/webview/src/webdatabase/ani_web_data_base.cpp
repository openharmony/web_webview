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

 #include "ani_web_data_base.h"

#include <array>
#include <iostream>
#include <string>

#include "ani_business_error.h"
#include "ani_parse_utils.h"

#include "nweb_helper.h"
#include "nweb_log.h"
#include "securec.h"
#include "web_errors.h"

namespace {
constexpr int32_t MAX_STRING_LENGTH = 40960;
constexpr int32_t MAX_PWD_LENGTH = 256;
}  // namespace
namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_DATA_BASE_CLASS_NAME = "@ohos.web.webview.webview.WebDataBase";
}  // namespace

bool GetStringPara(ani_env* env, ani_string dataStr)
{
    ani_size bufferSize = 0;
    env->String_GetUTF8Size(dataStr, &bufferSize);
    if (bufferSize > MAX_STRING_LENGTH) {
        return false;
    }
    return true;
}

bool GetSize(ani_env* env, ani_string pwd, ani_size& outValue)
{
    ani_size bufferSize = 0;
    env->String_GetUTF8Size(pwd, &bufferSize);
    if (bufferSize > MAX_PWD_LENGTH) {
        return false;
    }
    outValue = bufferSize;
    return true;
}

bool GetCharPara(ani_env* env, ani_string pwd, ani_size bufferSize)
{
    ani_size jsStringLength = 0;
    env->String_GetUTF8Size(pwd, &jsStringLength);
    if (jsStringLength != bufferSize) {
        return false;
    }
    return true;
}

static void JsSaveHttpAuthCredentials(
    ani_env* env, ani_object object, ani_string host, ani_string realm, ani_string username, ani_string password)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!GetStringPara(env, host) || !GetStringPara(env, realm) || !GetStringPara(env, username)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "parameter", "string"));
        return;
    }
    std::string hostStr;
    std::string usernameStr;
    std::string realmStr;
    std::string passwordStr;
    if (!AniParseUtils::ParseString(env, host, hostStr) || !AniParseUtils::ParseString(env, username, usernameStr) ||
        !AniParseUtils::ParseString(env, realm, realmStr) || !AniParseUtils::ParseString(env, password, passwordStr)) {
        return;
    }
    if (hostStr.empty() || usernameStr.empty()) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NOT_NULL_TWO, "username", "string"));
        return;
    }
    ani_size bufferSize = 0;
    if (!GetSize(env, password, bufferSize)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The length of 'password' must be between 0 and 256.");
        return;
    }
    if (bufferSize > 0) {
        char passwordBox[bufferSize + 1];
        if (memcpy_s(passwordBox, bufferSize + 1, passwordStr.c_str(), bufferSize) != EOK) {
            WVLOG_E("Webdatabase JsSaveHttpAuthCredentials fail: memcpy fail");
            return;
        }
        if (!GetCharPara(env, password, bufferSize)) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                "BusinessError 401: Parameter error. The length of 'password' obtained twice are different");
            return;
        }
        std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
        if (dataBase != nullptr) {
            dataBase->SaveHttpAuthCredentials(hostStr, realmStr, usernameStr, passwordBox);
        }
        (void)memset_s(passwordBox, sizeof(passwordBox), 0, sizeof(passwordBox));
    }
    return;
}

bool JsExistHttpAuthCredentials(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    bool result = false;
    std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (dataBase != nullptr) {
        result = dataBase->ExistHttpAuthCredentials();
    }
    return result;
}

static void JsDeleteHttpAuthCredentials(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (dataBase != nullptr) {
        dataBase->DeleteHttpAuthCredentials();
    }
    return;
}

static ani_ref JsGetHttpAuthCredentials(ani_env* env, ani_object object, ani_string host, ani_string realm)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    if (!GetStringPara(env, host)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "host", "string"));
        return nullptr;
    }
    if (!GetStringPara(env, realm)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "realm", "string"));
        return nullptr;
    }
    std::string hostStr;
    std::string realmStr;
    if (!AniParseUtils::ParseString(env, host, hostStr) || !AniParseUtils::ParseString(env, realm, realmStr)) {
        return nullptr;
    }
    if (hostStr.empty()) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NOT_NULL, "host"));
        return nullptr;
    }
    std::string username;
    char password[MAX_PWD_LENGTH + 1] = { 0 };
    std::vector<std::string> httpAuthCredentials;
    std::shared_ptr<OHOS::NWeb::NWebDataBase> dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (dataBase != nullptr) {
        dataBase->GetHttpAuthCredentials(hostStr, realmStr, username, password, MAX_PWD_LENGTH + 1);
    }
    if (!username.empty() && strlen(password) > 0) {
        httpAuthCredentials.emplace_back(username);
        httpAuthCredentials.emplace_back(password);
    }
    (void)memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1);
    return AniParseUtils::CreateAniStringArray(env, httpAuthCredentials);
}

ani_status StsWebDataBaseInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webDataBaseCls = nullptr;
    ani_status status = env->FindClass(WEB_DATA_BASE_CLASS_NAME, &webDataBaseCls);
    if (status != ANI_OK || !webDataBaseCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_DATA_BASE_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "saveHttpAuthCredentials", nullptr, reinterpret_cast<void*>(JsSaveHttpAuthCredentials) },
        ani_native_function {
            "deleteHttpAuthCredentials", nullptr, reinterpret_cast<void*>(JsDeleteHttpAuthCredentials) },
        ani_native_function {
            "existHttpAuthCredentials", nullptr, reinterpret_cast<void*>(JsExistHttpAuthCredentials) },
        ani_native_function { "getHttpAuthCredentials", nullptr, reinterpret_cast<void*>(JsGetHttpAuthCredentials) },
    };

    status = env->Class_BindStaticNativeMethods(webDataBaseCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindStaticNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_web_data_base.h"

#include <cstdint>
#include <cstring>
#include <vector>

#include "business_error.h"
#include "napi/native_common.h"
#include "nweb_data_base.h"
#include "nweb_helper.h"
#include "web_errors.h"
#include "securec.h"

namespace {
constexpr int32_t MAX_STRING_LENGTH = 40960;
constexpr int32_t MAX_PWD_LENGTH = 256;
constexpr int32_t PARAMZERO = 0;
constexpr int32_t PARAMONE = 1;
constexpr int32_t PARAMTWO = 2;
constexpr int32_t PARAMTHREE = 3;
constexpr int32_t PARAMFOUR = 4;
}

namespace OHOS {
namespace NWeb {
napi_value NapiWebDataBase::Init(napi_env env, napi_value exports)
{
    const std::string WEB_DATA_BASE_CLASS_NAME = "WebDataBase";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("deleteHttpAuthCredentials", NapiWebDataBase::JsDeleteHttpAuthCredentials),
        DECLARE_NAPI_STATIC_FUNCTION("saveHttpAuthCredentials", NapiWebDataBase::JsSaveHttpAuthCredentials),
        DECLARE_NAPI_STATIC_FUNCTION("getHttpAuthCredentials", NapiWebDataBase::JsGetHttpAuthCredentials),
        DECLARE_NAPI_STATIC_FUNCTION("existHttpAuthCredentials", NapiWebDataBase::JsExistHttpAuthCredentials),
    };
    napi_value constructor = nullptr;

    napi_define_class(env, WEB_DATA_BASE_CLASS_NAME.c_str(), WEB_DATA_BASE_CLASS_NAME.length(), JsConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "NapiWebDataBase define js class failed");
    napi_status status = napi_set_named_property(env, exports, "WebDataBase", constructor);
    NAPI_ASSERT(env, status == napi_ok, "NapiWebDataBase set property failed");
    return exports;
}

napi_value NapiWebDataBase::JsDeleteHttpAuthCredentials(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    OHOS::NWeb::NWebDataBase* dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (dataBase != nullptr) {
        dataBase->DeleteHttpAuthCredentials();
    }
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiWebDataBase::JsExistHttpAuthCredentials(napi_env env, napi_callback_info info)
{
    bool isExist = false;
    napi_value result = nullptr;

    OHOS::NWeb::NWebDataBase* dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (dataBase != nullptr) {
        isExist = dataBase->ExistHttpAuthCredentials();
    }
    NAPI_CALL(env, napi_get_boolean(env, isExist, &result));
    return result;
}

bool NapiWebDataBase::GetStringPara(napi_env env, napi_value argv, std::string& outValue)
{
    size_t bufferSize = 0;
    napi_valuetype valueType = napi_null;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_string) {
        return false;
    }
    napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
    if (bufferSize > MAX_STRING_LENGTH) {
        return false;
    }
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv, stringValue, bufferSize + 1, &jsStringLength);
    if (jsStringLength != bufferSize) {
        return false;
    }
    outValue = stringValue;
    return true;
}

bool NapiWebDataBase::GetSize(napi_env env, napi_value argv, size_t& outValue)
{
    size_t bufferSize = 0;
    napi_valuetype valueType = napi_null;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_string) {
        return false;
    }
    napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
    if (bufferSize > MAX_STRING_LENGTH) {
        return false;
    }
    outValue = bufferSize;
    return true;
}

bool NapiWebDataBase::GetCharPara(napi_env env, napi_value argv, char* buffer, size_t bufferSize)
{
    if (bufferSize == 0) {
        return false;
    }
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv, buffer, bufferSize + 1, &jsStringLength);
    if (jsStringLength != bufferSize) {
        return false;
    }
    return true;
}

napi_value NapiWebDataBase::JsSaveHttpAuthCredentials(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 4;
    napi_value argv[4] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != PARAMFOUR) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string host;
    if (!GetStringPara(env, argv[PARAMZERO], host)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string realm;
    if (!GetStringPara(env, argv[PARAMONE], realm)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string username;
    if (!GetStringPara(env, argv[PARAMTWO], username)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if (host.empty() || realm.empty() || username.empty()) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    size_t bufferSize = 0;
    if (!GetSize(env, argv[PARAMTHREE], bufferSize) || bufferSize > MAX_PWD_LENGTH) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }
    if (bufferSize > 0) {
        char password[bufferSize + 1];
        if (!GetCharPara(env, argv[PARAMTHREE], password, bufferSize)) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
            return nullptr;
        }

        OHOS::NWeb::NWebDataBase* dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
        if (dataBase != nullptr) {
            dataBase->SaveHttpAuthCredentials(host, realm, username, password);
        }
        (void)memset_s(password, sizeof(password), 0, sizeof(password));
    }

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiWebDataBase::JsGetHttpAuthCredentials(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 2;
    napi_value argv[2] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    if (argc != PARAMTWO) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string host;
    if (!GetStringPara(env, argv[PARAMZERO], host)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string realm;
    if (!GetStringPara(env, argv[PARAMONE], realm)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    if (host.empty() || realm.empty()) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string username;
    char password[MAX_PWD_LENGTH + 1] = {0};
    napi_value result = nullptr;
    napi_create_array(env, &result);

    OHOS::NWeb::NWebDataBase* dataBase = OHOS::NWeb::NWebHelper::Instance().GetDataBase();
    if (dataBase != nullptr) {
        dataBase->GetHttpAuthCredentials(host, realm, username, password, MAX_PWD_LENGTH + 1);
    }
    if (!username.empty() && strlen(password) > 0) {
        napi_value nameVal = nullptr;
        napi_value pwdVal = nullptr;
        napi_create_string_utf8(env, username.c_str(), username.length(), &nameVal);
        napi_set_element(env, result, PARAMZERO, nameVal);
        napi_create_string_utf8(env, password, strlen(password), &pwdVal);
        napi_set_element(env, result, PARAMONE, pwdVal);
    }
    (void)memset_s(password, MAX_PWD_LENGTH + 1, 0, MAX_PWD_LENGTH + 1);
    return result;
}

napi_value NapiWebDataBase::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    return thisVar;
}
} // namespace NWeb
} // namespace OHOS

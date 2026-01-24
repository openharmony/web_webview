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
#include <cstdint>
#include <iostream>

#include "webview_controller.h"
#include "web_scheme_handler_response.h"
#include "ani_web_net_error_list.h"

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME = "@ohos.web.webview.webview.WebSchemeHandlerResponse";
}

static void JsSetUrl(ani_env *env, ani_object object, ani_object urlObject)
{
    WVLOG_D("WebSchemeHandlerResponse JsSetUrl.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse *>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(urlObject, &isUndefined) != ANI_OK || isUndefined) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return;
    }
    if (!AniParseUtils::IsString(env, urlObject)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return;
    }
    std::string url;
    if (!AniParseUtils::ParseString(env, urlObject, url)) {
         WVLOG_E("ParseString fail");
        return;
    }
    schemeHandler->SetUrl(url.c_str());
}

static ani_string JsGetUrl(ani_env *env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse JsGetUrl.");
    ani_string url = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return url;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse *>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return url;
    }
    char* result = schemeHandler->GetUrl();
    if (result == nullptr) {
        return url;
    }
    if (env->String_NewUTF8(result, strlen(result), &url) != ANI_OK) {
        WVLOG_E("create string object failed");
        OH_ArkWeb_ReleaseString(result);
        return url;
    }
    OH_ArkWeb_ReleaseString(result);
    return url;
}

static void JsSetMimeType(ani_env* env, ani_object object, ani_object mimeTypeObject)
{
    WVLOG_D("WebSchemeHandlerResponse JsSetMimeType.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(mimeTypeObject, &isUndefined) != ANI_OK || isUndefined) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return;
    }
    if (!AniParseUtils::IsString(env, mimeTypeObject)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "type", "string"));
        return;
    }
    std::string mimeType;
    if (!AniParseUtils::ParseString(env, mimeTypeObject, mimeType)) {
        WVLOG_E("ParseString fail");
        return;
    }
    schemeHandler->SetMimeType(mimeType.c_str());
}

static ani_string JsGetMimeType(ani_env* env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse JsGetMimeType.");
    ani_string mimeType = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return mimeType;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return mimeType;
    }
    char* result = schemeHandler->GetMimeType();
    if (result == nullptr) {
        return mimeType;
    }
    if (env->String_NewUTF8(result, strlen(result), &mimeType) != ANI_OK) {
        WVLOG_E("create string object failed");
        OH_ArkWeb_ReleaseString(result);
        return mimeType;
    }
    OH_ArkWeb_ReleaseString(result);
    return mimeType;
}

static void JsSetEncoding(ani_env* env, ani_object object, ani_object encodingObject)
{
    WVLOG_D("WebSchemeHandlerResponse JsSetEncoding.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(encodingObject, &isUndefined) != ANI_OK || isUndefined) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return;
    }
    if (!AniParseUtils::IsString(env, encodingObject)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "encoding", "string"));
        return;
    }
    std::string encoding;
    if (!AniParseUtils::ParseString(env, encodingObject, encoding)) {
        WVLOG_E("ParseString fail");
        return;
    }
    schemeHandler->SetEncoding(encoding.c_str());
}

static ani_string JsGetEncoding(ani_env* env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse JsGetEncoding.");
    ani_string encoding = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return encoding;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return encoding;
    }
    char* result = schemeHandler->GetEncoding();
    if (result == nullptr) {
        return encoding;
    }
    if (env->String_NewUTF8(result, strlen(result), &encoding) != ANI_OK) {
        WVLOG_E("create string object failed");
        OH_ArkWeb_ReleaseString(result);
        return encoding;
    }
    OH_ArkWeb_ReleaseString(result);
    return encoding;
}

static void JsSetStatusText(ani_env* env, ani_object object, ani_object statusTextObject)
{
    WVLOG_D("WebSchemeHandlerResponse JsSetStatusText.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(statusTextObject, &isUndefined) != ANI_OK || isUndefined) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return;
    }
    if (!AniParseUtils::IsString(env, statusTextObject)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "text", "string"));
        return;
    }
    std::string statusText;
    if (!AniParseUtils::ParseString(env, statusTextObject, statusText)) {
        WVLOG_E("ParseString fail");
        return;
    }
    schemeHandler->SetStatusText(statusText.c_str());
}

static ani_string JsGetStatusText(ani_env* env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse JsGetStatusText.");
    ani_string statusText = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return statusText;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return statusText;
    }
    char* result = schemeHandler->GetStatusText();
    if (result == nullptr) {
        return statusText;
    }
    env->String_NewUTF8(result, strlen(result), &statusText);
    OH_ArkWeb_ReleaseString(result);
    return statusText;
}

static void JsSetStatus(ani_env* env, ani_object object, ani_int statusValue)
{
    WVLOG_D("WebSchemeHandlerResponse JsSetStatus.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    schemeHandler->SetStatus(static_cast<int32_t>(statusValue));
}

static ani_int JsGetStatus(ani_env* env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse JsGetStatus.");
    ani_int result = 0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }

    result = static_cast<ani_int>(schemeHandler->GetStatus());
    return result;
}

static void JsSetHeaderByName(
    ani_env* env, ani_object object, ani_object nameObject, ani_object valueObject, ani_boolean overWriteObject)
{
    WVLOG_D("WebSchemeHandlerResponse JsSetHeaderByName.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    if (!AniParseUtils::IsString(env, nameObject)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "name", "string"));
        return;
    }
    if (!AniParseUtils::IsString(env, valueObject)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "value", "string"));
        return;
    }
    std::string name;
    std::string value;
    if (!AniParseUtils::ParseString(env, nameObject, name)) {
        WVLOG_E("ParseString fail");
        return;
    }
    if (!AniParseUtils::ParseString(env, valueObject, value)) {
        WVLOG_E("ParseString fail");
        return;
    }
    schemeHandler->SetHeaderByName(name.c_str(), value.c_str(), static_cast<bool>(overWriteObject));
}

static ani_string JsGetHeaderByName(ani_env* env, ani_object object, ani_object nameObject)
{
    WVLOG_D("WebSchemeHandlerResponse JsGetHeaderByName.");
    ani_string headerValue = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return headerValue;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return headerValue;
    }
    std::string name;
    if (!AniParseUtils::ParseString(env, nameObject, name)) {
        return nullptr;
    }
    char* result = schemeHandler->GetHeaderByName(name.c_str());
    if (result == nullptr) {
        return headerValue;
    }
    if (env->String_NewUTF8(result, strlen(result), &headerValue) != ANI_OK) {
        WVLOG_E("create string object failed");
        OH_ArkWeb_ReleaseString(result);
        return headerValue;
    }
    OH_ArkWeb_ReleaseString(result);
    return headerValue;
}

static ani_enum_item JsGetNetErrorCode(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        WVLOG_E("Unwrap WebSchemeHandler failed");
        return nullptr;
    }
    ani_int result = 0;
    ani_enum enumType;
    ani_status status = env->FindEnum("@ohos.web.netErrorList.WebNetErrorList", &enumType);
    if (status != ANI_OK) {
        WVLOG_E("JsGetNetErrorCode FindEnum error");
        return nullptr;
    }

    result = schemeHandler->GetErrorCode();
    ani_int index = static_cast<ani_int>(GetErrorIndex(result));
    ani_enum_item code;
    status = env->Enum_GetEnumItemByIndex(enumType, index, &code);
    if (status != ANI_OK) {
        WVLOG_E("Enum_GetEnumItemByIndex status: %{public}d", status);
        return nullptr;
    }
    return code;
}

static void JsSetNetErrorCode(ani_env* env, ani_object object, ani_enum_item code)
{
    WVLOG_I("JsSetNetErrorCode begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* schemeHandler = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, object));
    if (!schemeHandler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        WVLOG_E("Unwrap WebSchemeHandler failed");
        return;
    }
    ani_int iCode;
    if (env->EnumItem_GetValue_Int(code, &iCode) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    int32_t codeInt = static_cast<int32_t>(iCode);
    schemeHandler->SetErrorCode(codeInt);
    return;
}

static void Constructor(ani_env *env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerResponse native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    WebSchemeHandlerResponse* schemeHandler = new (std::nothrow) WebSchemeHandlerResponse();
    if (schemeHandler == nullptr) {
        WVLOG_E("new webview controller failed");
        return;
    }
    if (!AniParseUtils::Wrap(env, object, WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME,
                             reinterpret_cast<ani_long>(schemeHandler))) {
        WVLOG_E("WebSchemeHandlerResponse wrap failed");
        delete schemeHandler;
        schemeHandler = nullptr;
    }
}

ani_status StsWebSchemeHandlerResponseInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webSchemeHandlerResponseCls = nullptr;
    ani_status status = env->FindClass(WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME, &webSchemeHandlerResponseCls);
    if (status != ANI_OK || !webSchemeHandlerResponseCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_SCHEME_HANDLER_RESPONSE_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void *>(Constructor) },
        ani_native_function { "setUrl", nullptr, reinterpret_cast<void *>(JsSetUrl) },
        ani_native_function { "getUrl", nullptr, reinterpret_cast<void *>(JsGetUrl) },
        ani_native_function { "setMimeType", nullptr, reinterpret_cast<void*>(JsSetMimeType) },
        ani_native_function { "getMimeType", nullptr, reinterpret_cast<void*>(JsGetMimeType) },
        ani_native_function { "setEncoding", nullptr, reinterpret_cast<void*>(JsSetEncoding) },
        ani_native_function { "getEncoding", nullptr, reinterpret_cast<void*>(JsGetEncoding) },
        ani_native_function { "setStatusText", nullptr, reinterpret_cast<void*>(JsSetStatusText) },
        ani_native_function { "getStatusText", nullptr, reinterpret_cast<void*>(JsGetStatusText) },
        ani_native_function { "setStatus", nullptr, reinterpret_cast<void*>(JsSetStatus) },
        ani_native_function { "getStatus", nullptr, reinterpret_cast<void*>(JsGetStatus) },
        ani_native_function { "setHeaderByName", nullptr, reinterpret_cast<void*>(JsSetHeaderByName) },
        ani_native_function { "getHeaderByName", nullptr, reinterpret_cast<void*>(JsGetHeaderByName) },
        ani_native_function { "getNetErrorCode", nullptr, reinterpret_cast<void*>(JsGetNetErrorCode) },
        ani_native_function { "setNetErrorCode", nullptr, reinterpret_cast<void*>(JsSetNetErrorCode) },
    };

    status = env->Class_BindNativeMethods(webSchemeHandlerResponseCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
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
#include <string>

#include "ani_business_error.h"
#include "ani_native_media_player_handler.h"
#include "ani_parse_utils.h"
#include "ani_webview_controller.h"
#include "native_media_player_impl.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "securec.h"
#include "web_errors.h"
#include "web_scheme_handler_request.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
const char* WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME = "L@ohos/web/webview/webview/WebSchemeHandlerRequest;";
namespace {
const char* ANI_CLASS_WEB_RESOURCETYPE = "L@ohos/web/webview/webview/WebResourceType;";
} // namespace

static ani_boolean JSHasGesture(ani_env* env, ani_object object)
{
    WVLOG_I("jSHasGesture start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* stream = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!stream) {
        WVLOG_E("stream is null");
        return ANI_FALSE;
    }
    return stream->HasGesture() ? ANI_TRUE : ANI_FALSE;
}

static ani_string GetRequestMethod(ani_env* env, ani_object object)
{
    WVLOG_I("getRequestMethod start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    char* referrer = request->GetMethod();
    if (referrer == nullptr) {
        WVLOG_E("getRequestMethod: referrer is nullptr");
        return nullptr;
    }
    ani_string value;
    if (env->String_NewUTF8(referrer, strlen(referrer), &value) != ANI_OK) {
        WVLOG_E("getRequestMethod: failed to create ani_string value");
        return nullptr;
    }
    return value;
}

static ani_boolean IsMainFrame(ani_env* env, ani_object object)
{
    WVLOG_I("isMainFrame start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_TRUE;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_TRUE;
    }
    ani_boolean value = (request->IsMainFrame() ? 1 : 0);
    return value;
}

static ani_string GetReferrer(ani_env* env, ani_object object)
{
    WVLOG_I("getReferrer start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    char* referrer = request->GetReferrer();
    if (referrer == nullptr) {
        WVLOG_E("GetReferrer: referrer is nullptr");
        return nullptr;
    }
    ani_string value;
    if (env->String_NewUTF8(referrer, strlen(referrer), &value) != ANI_OK) {
        WVLOG_E("GetReferrer: failed to create ani_string value");
        return nullptr;
    }
    return value;
}

static ani_string GetFrameUrl(ani_env* env, ani_object object)
{
    WVLOG_I("GetFrameUrl start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    ani_string url = nullptr;
    char* result = request->GetFrameUrl();
    env->String_NewUTF8(result, strlen(result), &url);
    OH_ArkWeb_ReleaseString(result);
    return url;
}

static ani_object GetRequestResourceType(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_int resourceType = 0;
    ani_enum enumType;
    env->FindEnum(ANI_CLASS_WEB_RESOURCETYPE, &enumType);
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    resourceType = static_cast<ani_int>(request->GetRequestResourceType());
    ani_enum_item state;
    env->Enum_GetEnumItemByIndex(enumType, resourceType, &state);
    return state;
}

static ani_string GetRequestUrl(ani_env* env, ani_object object)
{
    WVLOG_I("getRequestUrl start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    char* referrer = request->GetRequestUrl();
    if (referrer == nullptr) {
        WVLOG_E("getRequestUrl: referrer is nullptr");
        return nullptr;
    }
    ani_string value;
    if (env->String_NewUTF8(referrer, strlen(referrer), &value) != ANI_OK) {
        WVLOG_E("getRequestUrl: failed to create ani_string value");
        return nullptr;
    }
    return value;
}

bool GetHeaderProcessItems(ani_env* env, std::vector<std::pair<std::string, std::string>> values, ani_array_ref& array)
{
    WVLOG_I("GetHeaderProcessItems begin");
    ani_class stringCls;
    if (ANI_OK != env->FindClass("L@ohos/web/webview/webview/GetHeaderWebHeader;", &stringCls)) {
        WVLOG_E("getHeader find class failed.");
        return false;
    }
    ani_method personInfoCtor;
    if (ANI_OK != env->Class_FindMethod(stringCls, "<ctor>", nullptr, &personInfoCtor)) {
        WVLOG_E("getHeader GetUndefined Failed.");
        return false;
    }
    for (size_t i = 0; i < values.size(); ++i) {
        ani_string headerKey {};
        ani_string headerValue {};
        ani_object webHeaderObj = {};
        ani_status status = env->Object_New(stringCls, personInfoCtor, &webHeaderObj);
        if (status != ANI_OK) {
            WVLOG_E("getHeader new object error.");
        }
        env->String_NewUTF8(values[i].first.c_str(), values[i].first.size(), &headerKey);
        env->String_NewUTF8(values[i].second.c_str(), values[i].second.size(), &headerValue);

        if ((status = env->Object_SetPropertyByName_Ref(webHeaderObj, "headerKey", static_cast<ani_ref>(headerKey))) !=
            ANI_OK) {
            WVLOG_E("getHeader Set headerKey failed status = %{public}d", status);
            return false;
        }
        if (ANI_OK != env->Object_SetPropertyByName_Ref(webHeaderObj, "headerValue", headerValue)) {
            WVLOG_E("getHeader Get headerValue failed");
            return false;
        }
        env->Array_Set_Ref(array, i, webHeaderObj);
    }
    return true;
}

static ani_ref GetHeader(ani_env* env, ani_object object)
{
    WVLOG_I("getHeader start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }

    std::vector<std::pair<std::string, std::string>> values = request->GetHeader();
    ani_class stringCls;
    if (ANI_OK != env->FindClass("L@ohos/web/webview/webview/GetHeaderWebHeader;", &stringCls)) {
        WVLOG_E("getHeader find class failed.");
        return nullptr;
    }
    ani_method personInfoCtor;
    if (ANI_OK != env->Class_FindMethod(stringCls, "<ctor>", nullptr, &personInfoCtor)) {
        WVLOG_E("getHeader GetUndefined Failed.");
        return nullptr;
    }

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("getHeader GetUndefined Failed.");
        return nullptr;
    }
    ani_array_ref array = nullptr;
    if (ANI_OK != env->Array_New_Ref(stringCls, values.size(), undefinedRef, &array)) {
        WVLOG_E("getHeader new array ref error.");
        return nullptr;
    }

    if (!GetHeaderProcessItems(env, values, array)) {
        WVLOG_E("GetHeaderProcessItems failed");
        return nullptr;
    }
    return array;
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_I("WebSchemeHandlerRequest native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
}

ani_status StsWebSchemeHandlerRequestInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }

    ani_class WebSchemeHandlerRequestCls = nullptr;
    ani_status status = env->FindClass(WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME, &WebSchemeHandlerRequestCls);
    if (status != ANI_OK || !WebSchemeHandlerRequestCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "hasGesture", nullptr, reinterpret_cast<void*>(JSHasGesture) },
        ani_native_function { "getReferrer", nullptr, reinterpret_cast<void*>(GetReferrer) },
        ani_native_function { "getFrameUrl", nullptr, reinterpret_cast<void*>(GetFrameUrl) },
        ani_native_function { "getHeader", nullptr, reinterpret_cast<void*>(GetHeader) },
        ani_native_function { "getRequestResourceType", nullptr, reinterpret_cast<void*>(GetRequestResourceType) },
        ani_native_function { "getRequestUrl", nullptr, reinterpret_cast<void*>(GetRequestUrl) },
        ani_native_function { "isMainFrame", nullptr, reinterpret_cast<void*>(IsMainFrame) },
        ani_native_function { "getRequestMethod", nullptr, reinterpret_cast<void*>(GetRequestMethod) },
    };

    status = env->Class_BindNativeMethods(WebSchemeHandlerRequestCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
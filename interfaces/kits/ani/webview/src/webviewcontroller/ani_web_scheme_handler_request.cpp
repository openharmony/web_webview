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
#include "web_scheme_handler.h"
#include "ani_class_name.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;

static ani_boolean JSHasGesture(ani_env* env, ani_object object)
{
    WVLOG_D("jSHasGesture start");
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
    WVLOG_D("getRequestMethod start");
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
    WVLOG_D("isMainFrame start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }
    ani_boolean value = (request->IsMainFrame() ? 1 : 0);
    return value;
}

static ani_string GetReferrer(ani_env* env, ani_object object)
{
    WVLOG_D("getReferrer start");
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
    WVLOG_D("GetFrameUrl start");
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
    if (result == nullptr) {
        WVLOG_E("GetFrameUrl: url is nullptr");
        return nullptr;
    }
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
    if (env->FindEnum(ANI_CLASS_WEB_RESOURCETYPE, &enumType) != ANI_OK) {
        WVLOG_E("find enum object failed");
        return nullptr;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest*>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    resourceType = static_cast<ani_int>(request->GetRequestResourceType());
    ani_enum_item state;
    if (env->Enum_GetEnumItemByIndex(enumType, resourceType, &state) != ANI_OK) {
        WVLOG_E("find enumItem failed");
        return nullptr;
    }
    return state;
}

static ani_string GetRequestUrl(ani_env* env, ani_object object)
{
    WVLOG_D("getRequestUrl start");
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

bool GetHeaderProcessItems(ani_env* env, std::vector<std::pair<std::string, std::string>> values, ani_array& array)
{
    WVLOG_D("GetHeaderProcessItems begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class stringCls;
    if (ANI_OK != env->FindClass("@ohos.web.webview.webview.GetHeaderWebHeader", &stringCls)) {
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
            return false;
        }
        if ((status = env->String_NewUTF8(values[i].first.c_str(), values[i].first.size(), &headerKey)) != ANI_OK) {
            WVLOG_E("get string object headerKey failed");
            return false;
        }
        
        if ((env->String_NewUTF8(values[i].second.c_str(), values[i].second.size(), &headerValue)) != ANI_OK) {
            WVLOG_E("get string object headerValue failed");
            return false;
        }

        if ((status = env->Object_SetPropertyByName_Ref(webHeaderObj, "headerKey", static_cast<ani_ref>(headerKey))) !=
            ANI_OK) {
            WVLOG_E("getHeader Set headerKey failed status = %{public}d", status);
            return false;
        }
        if (ANI_OK != env->Object_SetPropertyByName_Ref(webHeaderObj, "headerValue", headerValue)) {
            WVLOG_E("getHeader Get headerValue failed");
            return false;
        }
        env->Array_Set(array, i, webHeaderObj);
    }
    return true;
}

static ani_ref GetHeader(ani_env* env, ani_object object)
{
    WVLOG_D("getHeader start");
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

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("getHeader GetUndefined Failed.");
        return nullptr;
    }
    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("getHeader new array ref error.");
        return nullptr;
    }

    if (!GetHeaderProcessItems(env, values, array)) {
        WVLOG_E("GetHeaderProcessItems failed");
        return nullptr;
    }
    return array;
}

static ani_object GetHttpBodyStream(ani_env *env, ani_object object)
{
    WVLOG_I("getHttpBodyStream start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* request = reinterpret_cast<WebSchemeHandlerRequest *>(AniParseUtils::Unwrap(env, object));
    if (!request) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }

    ArkWeb_HttpBodyStream* arkWebPostStream = request->GetHttpBodyStream();
    if (!arkWebPostStream) {
        WVLOG_E("getHttpBodyStream: arkWebPostStream is nullptr");
        return nullptr;
    }
    ani_object httpBodyStreamObject;
    WebHttpBodyStream* stream = new (std::nothrow) WebHttpBodyStream(env, arkWebPostStream);
    if (stream == nullptr) {
        WVLOG_E("stream is nullptr");
        return nullptr;
    }

    ani_class cls;
    ani_method ctor;
    if (env->FindClass(ANI_HTTP_BODY_STREAM, &cls) != ANI_OK) {
        delete stream;
        stream = nullptr;
        return nullptr;
    }
    if (env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor) != ANI_OK) {
        delete stream;
        stream = nullptr;
        return nullptr;
    }
    if (env->Object_New(cls, ctor, &httpBodyStreamObject) != ANI_OK) {
        WVLOG_E("Object_New failed");
        delete stream;
        stream = nullptr;
        return nullptr;
    }
    if (!AniParseUtils::Wrap(env, httpBodyStreamObject, ANI_HTTP_BODY_STREAM, reinterpret_cast<ani_long>(stream))) {
        WVLOG_E("webview controller wrap failed");
        delete stream;
        stream = nullptr;
    }
    WVLOG_I("getHttpBodyStream: arkWebPostStream success");
    return httpBodyStreamObject;
}

static ani_boolean TransferWebSchemeHandlerRequestToStaticInner(
    ani_env* env, ani_class aniClass, ani_object output, ani_object input)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    void *nativePtr = nullptr;
    if (!arkts_esvalue_unwrap(env, input, &nativePtr) || nativePtr == nullptr) {
        WVLOG_E("[TRANSFER] arkts_esvalue_unwrap failed");
        return ANI_FALSE;
    }
    WebSchemeHandlerRequest *schemeHandlerRequest = reinterpret_cast<WebSchemeHandlerRequest *>(nativePtr);
    if (!AniParseUtils::Wrap(env, output, ANI_WEB_WEBSCHEME_HANDLER_REQUEST_CLASS_NAME,
                             reinterpret_cast<ani_long>(schemeHandlerRequest))) {
        WVLOG_E("[TRANSFER] WebSchemeHandlerRequest wrap failed");
        return ANI_FALSE;
    }
    schemeHandlerRequest->IncStrongRef(nullptr);
    return ANI_TRUE;
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_D("WebSchemeHandlerRequest native Constructor");
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
    ani_status status = env->FindClass(ANI_CLASS_WEB_REQUESTTYPE, &WebSchemeHandlerRequestCls);
    if (status != ANI_OK || !WebSchemeHandlerRequestCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_CLASS_WEB_REQUESTTYPE, status);
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
        ani_native_function { "getHttpBodyStream", nullptr, reinterpret_cast<void *>(GetHttpBodyStream) },
        ani_native_function { "getRequestMethod", nullptr, reinterpret_cast<void*>(GetRequestMethod) },
    };

    status = env->Class_BindNativeMethods(WebSchemeHandlerRequestCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return status;
    }

    std::array allStaticMethods = {
        ani_native_function { "transferWebSchemeHandlerRequestToStaticInner",
                              nullptr, reinterpret_cast<void*>(TransferWebSchemeHandlerRequestToStaticInner) },
    };
    status = env->Class_BindStaticNativeMethods(WebSchemeHandlerRequestCls,
        allStaticMethods.data(), allStaticMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindStaticNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
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
#include "native_media_player_impl.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "securec.h"
#include "web_errors.h"
#include "web_scheme_handler_request.h"
#include "web_scheme_handler_response.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
const char* ANI_WEB_RESOURCE_HANDLER_CLASS_NAME = "@ohos.web.webview.webview.WebResourceHandler";
static void JSDidReceiveResponse(ani_env* env, ani_object object, ani_object response)
{
    WVLOG_I("Enter aniwebResourceHandler JSDidReceiveResponse");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* resourceHandler = reinterpret_cast<WebResourceHandler*>(AniParseUtils::Unwrap(env, object));
    if (!resourceHandler) {
        WVLOG_E("stream is nullptr");
        return;
    }

    auto* receiveResponse = reinterpret_cast<WebSchemeHandlerResponse*>(AniParseUtils::Unwrap(env, response));
    if (!receiveResponse) {
        WVLOG_E("receiveResponse is nullptr");
        return;
    }

    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(response, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }

    int32_t ret = resourceHandler->DidReceiveResponse(receiveResponse->GetArkWebResponse());
    WVLOG_I("aniwebResourceHandler JSDidReceiveResponse ret = %{public}d", ret);
    if (ret != 0) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
    }
    return;
}

static void JsDidFinish(ani_env* env, ani_object object)
{
    WVLOG_I("Enter aniwebResourceHandler JsDidFinish");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* resourceHandler = reinterpret_cast<WebResourceHandler*>(AniParseUtils::Unwrap(env, object));
    if (!resourceHandler) {
        WVLOG_E("stream is nullptr");
        return;
    }
    int32_t ret = resourceHandler->DidFinish();
    if (ret != 0) {
        AniBusinessError::ThrowErrorByErrCode(env, RESOURCE_HANDLER_INVALID);
        WVLOG_E("JsDidFinish ret=%{public}d", ret);
    }
    return;
}

static void JsDidReceiveResponseBody(ani_env* env, ani_object object, ani_object arrayBufferObj)
{
    WVLOG_I("Enter aniwebResourceHandler JsDidReceiveResponseBody");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* resourceHandler = reinterpret_cast<WebResourceHandler*>(AniParseUtils::Unwrap(env, object));
    if (!resourceHandler) {
        WVLOG_E("resourceHandler is nullPtr");
        return;
    }

    uint8_t* arrayBuffer = nullptr;
    size_t byteLength = 0;
    if (env->ArrayBuffer_GetInfo(reinterpret_cast<ani_arraybuffer>(arrayBufferObj),
            reinterpret_cast<void**>(&arrayBuffer), &byteLength) != ANI_OK) {
        WVLOG_E("ArrayBuffer_GetInfo failed");
        return;
    }
    int32_t ret = resourceHandler->DidReceiveResponseBody(arrayBuffer, static_cast<int64_t>(byteLength));
    if (ret != 0) {
        WVLOG_E("JSDidReceiveResponseBody ret=%{public}d", ret);
        AniBusinessError::ThrowErrorByErrCode(env, RESOURCE_HANDLER_INVALID);
        return;
    }
    return;
}

static void JsDidFailWithError(ani_env* env, ani_object object, ani_enum_item errorCode)
{
    WVLOG_I("Enter aniwebResourceHandler JsDidFailWithError");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* rosourceHandler = reinterpret_cast<WebResourceHandler*>(AniParseUtils::Unwrap(env, object));
    if (!rosourceHandler) {
        WVLOG_E("rosourceHandler is nullptr");
        return;
    }
    ani_int iCode;
    if (env->EnumItem_GetValue_Int(errorCode, &iCode) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        WVLOG_E("EnumItem_GetValue_Int failed");
        return;
    }

    int32_t codeInt = static_cast<int32_t>(iCode);
    int32_t ret = rosourceHandler->DidFailWithError(static_cast<ArkWeb_NetError>(codeInt));
    WVLOG_I("JSDidFailWithError ret=%{public}d", ret);
    if (ret != 0) {
        AniBusinessError::ThrowErrorByErrCode(env, RESOURCE_HANDLER_INVALID);
        WVLOG_E("JSDidFailWithError ret=%{public}d", ret);
    }
    return;
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_I("WebResourceHandler WebResourceHandler native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
}

ani_status StsWebSchemeHandlerResourceInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }

    ani_class WebResourceCls = nullptr;
    ani_status status = env->FindClass(ANI_WEB_RESOURCE_HANDLER_CLASS_NAME, &WebResourceCls);
    if (status != ANI_OK || !WebResourceCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_WEB_RESOURCE_HANDLER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "didFinish", nullptr, reinterpret_cast<void*>(JsDidFinish) },
        ani_native_function { "didReceiveResponse", nullptr, reinterpret_cast<void*>(JSDidReceiveResponse) },
        ani_native_function { "didFail", nullptr, reinterpret_cast<void*>(JsDidFailWithError) },
        ani_native_function { "didReceiveResponseBody", nullptr, reinterpret_cast<void*>(JsDidReceiveResponseBody) },
    };

    status = env->Class_BindNativeMethods(WebResourceCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
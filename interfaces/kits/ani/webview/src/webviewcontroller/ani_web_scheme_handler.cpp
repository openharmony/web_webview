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
namespace {
const char* WEB_WEBSCHEME_HANDLER_CLASS_NAME = "L@ohos/web/webview/webview/WebSchemeHandler;";
}
static void OnRequestStart(ani_env* env, ani_object object, ani_fn_object callback)
{
    WVLOG_I("SchemeHandler OnRequestStart");
    if (env == nullptr) {
        WVLOG_E("[WebSchemeHandler] env is nullptr");
        return;
    }

    auto* webSchemeHandler = reinterpret_cast<WebSchemeHandler*>(AniParseUtils::Unwrap(env, object));
    if (!webSchemeHandler) {
        WVLOG_E("[WebSchemeHandler] webSchemeHandler is null");
        return;
    }
    ani_vm* vm = nullptr;
    env->GetVM(&vm);
    webSchemeHandler->PutRequestStart(env, vm, callback);
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_I("WebSchemeHandler native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    WebSchemeHandler* webSchemeHandler = new (std::nothrow) WebSchemeHandler(env);
    if (webSchemeHandler == nullptr) {
        WVLOG_E("new WebSchemeHandler failed");
        return;
    }
    if (!AniParseUtils::Wrap(
            env, object, WEB_WEBSCHEME_HANDLER_CLASS_NAME, reinterpret_cast<ani_long>(webSchemeHandler))) {
        WVLOG_E("WebDownloadDelegate wrap failed");
        delete webSchemeHandler;
        webSchemeHandler = nullptr;
    }
}

static void OnRequestStop(ani_env* env, ani_object object, ani_fn_object callback)
{
    WVLOG_D("enter SchemeHandler OnRequestStop");
    if (env == nullptr) {
        WVLOG_E("[WebSchemeHandler] env is nullptr");
        return;
    }

    auto* webSchemeHandler = reinterpret_cast<WebSchemeHandler*>(AniParseUtils::Unwrap(env, object));
    if (!webSchemeHandler) {
        WVLOG_E("[WebSchemeHandler] webSchemeHandler is null");
        return;
    }
    ani_vm* vm = nullptr;
    env->GetVM(&vm);
    webSchemeHandler->PutRequestStop(env, vm, callback);
}

ani_status StsWebSchemeHandlerInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }

    ani_class WebSchemeHandlerCls = nullptr;
    ani_status status = env->FindClass(WEB_WEBSCHEME_HANDLER_CLASS_NAME, &WebSchemeHandlerCls);
    if (status != ANI_OK || !WebSchemeHandlerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_WEBSCHEME_HANDLER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "onRequestStart", "Lstd/core/Function2;:V", reinterpret_cast<void*>(OnRequestStart) },
        ani_native_function { "onRequestStop", nullptr, reinterpret_cast<void*>(OnRequestStop) },
    };

    status = env->Class_BindNativeMethods(WebSchemeHandlerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS
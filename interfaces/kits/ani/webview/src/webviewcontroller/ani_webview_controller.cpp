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

#include "webview_controller.h"

#include "ani_webview_controller.h"

#include "ani_business_error.h"
#include "ani_parse_utils.h"

#include <cstdlib>
#include <ctime>
#include <memory>
#include <unordered_map>
#include <securec.h>
#include <regex>

#include "application_context.h"
#include "ohos_resource_adapter_impl.h"

#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"

#include "nweb_log.h"
#include "web_errors.h"
#include "webview_createpdf_execute_callback.h"
#include "webview_hasimage_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"

#include "nweb_precompile_callback.h"
#include "nweb_cache_options_impl.h"

#include "bundle_mgr_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEBVIEW_CONTROLLER_CLASS_NAME = "L@ohos/web/webview/webview/WebviewController;";
const char* WEB_HEADER_CLASS_NAME = "L@ohos/web/webview/webview/WebHeader;";

bool ParseResourceRawfileUrl(ani_env *env, const ani_object& object, std::string& fileName)
{
    ani_ref paramsRef;
    if (env->Object_GetPropertyByName_Ref(object, "params", &paramsRef) != ANI_OK) {
        return false;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(paramsRef, &isUndefined) != ANI_OK || isUndefined) {
        return false;
    }
    ani_object paramsObj = reinterpret_cast<ani_object>(paramsRef);
    ani_double paramsLength;
    if (env->Object_GetPropertyByName_Double(paramsObj, "length", &paramsLength) != ANI_OK) {
        return false;
    }
    ani_ref fileNameRef;
    if (env->Object_CallMethodByName_Ref(paramsObj, "$_get", "I:Lstd/core/Object;", &fileNameRef, 0) != ANI_OK) {
        return false;
    }
    if (env->Reference_IsUndefined(fileNameRef, &isUndefined) != ANI_OK || isUndefined) {
        return false;
    }
    if (!AniParseUtils::ParseString(env, fileNameRef, fileName) || fileName.empty()) {
        return false;
    }
    return true;
}

bool ParseResourceUrl(ani_env *env, ani_object urlObject, std::string& url, WebviewController *controller)
{
    ani_ref bundleNameRef;
    if (env->Object_GetPropertyByName_Ref(urlObject, "bundleName", &bundleNameRef) != ANI_OK) {
        return false;
    }
    ani_ref moduleNameRef;
    if (env->Object_GetPropertyByName_Ref(urlObject, "moduleName", &moduleNameRef) != ANI_OK) {
        return false;
    }
    ani_ref typeRef;
    if (env->Object_GetPropertyByName_Ref(urlObject, "type", &typeRef) != ANI_OK) {
        return false;
    }
    double typeDouble;
    if (env->Object_CallMethodByName_Double(static_cast<ani_object>(typeRef), "unboxed", ":D",
                                            &typeDouble) != ANI_OK) {
        return false;
    }
    std::string bundleName;
    std::string moduleName;
    if (!AniParseUtils::ParseString(env, bundleNameRef, bundleName) ||
        !AniParseUtils::ParseString(env, moduleNameRef, moduleName)) {
        return false;
    }
    int type = static_cast<int>(std::round(typeDouble));
    if (type == static_cast<int>(ResourceType::RAWFILE)) {
        std::string fileName;
        if (!ParseResourceRawfileUrl(env, urlObject, fileName)) {
            return false;
        }
        if (!controller->GetRawFileUrl(fileName, bundleName, moduleName, url)) {
            return false;
        }
    } else if (type == static_cast<int>(ResourceType::STRING)) {
        ani_double idProperty;
        if (env->Object_GetPropertyByName_Double(urlObject, "id", &idProperty) != ANI_OK) {
            return false;
        }
        int id = static_cast<int>(std::round(idProperty));
        if (!OhosResourceAdapterImpl::GetResourceString(bundleName, moduleName, id, url)) {
            return false;
        }
    } else {
        return false;
    }
    WVLOG_I("ParseResourceUrl url: %{public}s", url.c_str());
    return true;
}
}

static void InitializeWebEngine(ani_env *env, ani_object aniClass)
{
    WVLOG_D("InitializeWebEngine invoked.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    // obtain bundle path
    std::shared_ptr<AbilityRuntime::ApplicationContext> ctx =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!ctx) {
        WVLOG_E("Failed to init web engine due to nil application context.");
        return;
    }

    // load so
    const std::string& bundlePath = ctx->GetBundleCodeDir();
    NWebHelper::Instance().SetBundlePath(bundlePath);
    if (!NWebHelper::Instance().InitAndRun(true)) {
        WVLOG_E("Failed to init web engine due to NWebHelper failure.");
    }
}

static void SetNWebId(ani_env *env, ani_object object, ani_double nwebId)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("webviewController is nullptr.");
        return;
    }
    auto id = static_cast<int>(std::round(nwebId));
    WVLOG_I("SetNWebId nwebId: %{public}d", id);
    controller->SetWebId(id);
}

static void SetHapPath(ani_env *env, ani_object object, ani_string hapPath)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(hapPath, &isUndefined) != ANI_OK || isUndefined) {
        WVLOG_E("is undefined");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("Wrap webviewController failed. WebviewController must be associated with a Web component.");
        return;
    }
    std::string hapPathStr;
    if (!AniParseUtils::ParseString(env, hapPath, hapPathStr)) {
        WVLOG_E("Parse hap path failed.");
        return;
    }
    WVLOG_I("SetHapPath hapPath: %{public}s", hapPathStr.c_str());
    controller->InnerSetHapPath(hapPathStr);
}

static bool GetUrl(ani_env *env, ani_object urlObject, std::string& url, WebviewController *controller)
{
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(urlObject, &isUndefined) != ANI_OK || isUndefined) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "one", "two"));
        return false;
    }
    if (AniParseUtils::IsString(env, urlObject)) {
        if (!AniParseUtils::ParseString(env, urlObject, url)) {
            return false;
        }
    } else if (AniParseUtils::IsResource(env, urlObject)) {
        if (!ParseResourceUrl(env, urlObject, url, controller)) {
            AniBusinessError::ThrowErrorByErrCode(env, INVALID_URL);
            return false;
        }
    } else {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "url"));
        return false;
    }
    return true;
}

static bool GetWebHeaders(ani_env *env, ani_object headersArrayObj, std::map<std::string, std::string>& webHeaders)
{
    ani_double headersLength;
    if (env->Object_GetPropertyByName_Double(headersArrayObj, "length", &headersLength) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return false;
    }
    ani_class webHeaderClass;
    if (env->FindClass(WEB_HEADER_CLASS_NAME, &webHeaderClass) != ANI_OK) {
        WVLOG_E("Find WebHeader Class failed");
        return false;
    }
    for (int i = 0; i < int(headersLength); i++) {
        ani_ref webHeaderRef;
        if (env->Object_CallMethodByName_Ref(headersArrayObj, "$_get", "I:Lstd/core/Object;",
                                             &webHeaderRef, (ani_int)i) != ANI_OK) {
            return false;
        }
        ani_object webHeaderObj = reinterpret_cast<ani_object>(webHeaderRef);
        ani_boolean isWebHeader = false;
        env->Object_InstanceOf(webHeaderObj, webHeaderClass, &isWebHeader);
        if (!isWebHeader) {
            WVLOG_E("not WebHeader");
            return false;
        }
        ani_ref keyPropertyRef;
        if (env->Object_GetPropertyByName_Ref(webHeaderObj, "headerKey", &keyPropertyRef) != ANI_OK) {
            continue;
        }
        ani_ref valuePropertyRef;
        if (env->Object_GetPropertyByName_Ref(webHeaderObj, "headerValue", &valuePropertyRef) != ANI_OK) {
            continue;
        }
        std::string keyString;
        std::string valueString;
        if (!AniParseUtils::ParseString(env, keyPropertyRef, keyString) ||
            !AniParseUtils::ParseString(env, valuePropertyRef, valueString)) {
            continue;
        }
        webHeaders[keyString] = valueString;
    }
    return true;
}

static void LoadUrl(ani_env *env, ani_object object, ani_object urlObject, ani_object headersArrayObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    std::string url;
    if (!GetUrl(env, urlObject, url, controller)) {
        return;
    }
    ErrCode ret;
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(headersArrayObj, &isUndefined) != ANI_OK || isUndefined) {
        ret = controller->LoadUrl(url);
    } else {
        std::map<std::string, std::string> webHeaders;
        if (!GetWebHeaders(env, headersArrayObj, webHeaders)) {
            return;
        }
        ret = controller->LoadUrl(url, webHeaders);
    }
    WVLOG_I("loadUrl ret: %{public}d", ret);
    if (ret != NO_ERROR && ret != NWEB_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
}

static void Clean(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_long ptr;
    ani_status status = env->Object_GetFieldByName_Long(object, "ptr", &ptr);
    if (status != ANI_OK) {
        WVLOG_E("Clean Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }
    delete reinterpret_cast<WebviewController *>(ptr);
}

static void Constructor(ani_env *env, ani_object object, ani_string webTagObject)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined = true;
    env->Reference_IsUndefined(webTagObject, &isUndefined);
    std::string webTag;
    if (isUndefined) {
        webTag = WebviewController::GenerateWebTag();
    } else {
        if (!AniParseUtils::ParseString(env, webTagObject, webTag)) {
            return;
        }
        WVLOG_I("new webview controller webname:%{public}s", webTag.c_str());
    }
    WebviewController* controller = new (std::nothrow) WebviewController(webTag);
    if (controller == nullptr) {
        WVLOG_E("new webview controller failed");
        return;
    }
    WebviewController::webTagSet_.insert(webTag);
    if (!AniParseUtils::Wrap(env, object, WEBVIEW_CONTROLLER_CLASS_NAME, reinterpret_cast<ani_long>(controller))) {
        WVLOG_E("webview controller wrap failed");
        delete controller;
        controller = nullptr;
    }
}

static void OnActive(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->OnActive();
}

static void OnInactive(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->OnInactive();
}

static ani_double GetWebId(ani_env *env, ani_object object)
{
    int32_t webId = -1;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_double>(webId);
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return static_cast<ani_double>(webId);
    }

    webId = controller->GetWebId();
    return static_cast<ani_double>(webId);
}

static ani_boolean GetScrollable(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return true;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return true;
    }

    return controller->GetScrollable();
}

static void RequestFocus(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->RequestFocus();
}

ani_status StsWebviewControllerInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webviewControllerCls = nullptr;
    ani_status status = env->FindClass(WEBVIEW_CONTROLLER_CLASS_NAME, &webviewControllerCls);
    if (status != ANI_OK || !webviewControllerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEBVIEW_CONTROLLER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array controllerMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void *>(Constructor) },
        ani_native_function { "_setNWebId", nullptr, reinterpret_cast<void *>(SetNWebId) },
        ani_native_function { "_setHapPath", nullptr, reinterpret_cast<void *>(SetHapPath) },
        ani_native_function { "initializeWebEngine", nullptr, reinterpret_cast<void *>(InitializeWebEngine) },
        ani_native_function { "loadUrl", nullptr, reinterpret_cast<void *>(LoadUrl) },
        ani_native_function { "onActive", nullptr, reinterpret_cast<void *>(OnActive) },
        ani_native_function { "onInactive", nullptr, reinterpret_cast<void *>(OnInactive) },
        ani_native_function { "getWebId", nullptr, reinterpret_cast<void *>(GetWebId) },
        ani_native_function { "getScrollable", nullptr, reinterpret_cast<void *>(GetScrollable) },
        ani_native_function { "requestFocus", nullptr, reinterpret_cast<void *>(RequestFocus) },
    };

    status = env->Class_BindNativeMethods(webviewControllerCls, controllerMethods.data(), controllerMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }

    static const char *cleanerName = "L@ohos/web/webview/Cleaner;";
    ani_class cleanerCls = nullptr;
    status = env->FindClass(cleanerName, &cleanerCls);
    if (status != ANI_OK || !cleanerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", cleanerName, status);
        return ANI_ERROR;
    }
    std::array cleanerMethods = {
        ani_native_function { "clean", nullptr, reinterpret_cast<void *>(Clean) },
    };
    status = env->Class_BindNativeMethods(cleanerCls, cleanerMethods.data(), cleanerMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS
